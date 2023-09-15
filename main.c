#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include "directory_retriever.h"
#include "file_reader.h"

#define OUTPUT_FILE "result.txt"

int main() {
    DIR* current_dir;
    struct dirent* dir_entry;
    int file_count = 0;

    // Open the current directory
    current_dir = opendir(".");
    if (current_dir == NULL) {
        perror("Unable to open current directory");
        return 1;
    }
    printf("Parent: Scanning the current directory...\n");

    // Count files with qualified extensions
    while ((dir_entry = readdir(current_dir)) != NULL) {
        if (has_usp_extension(dir_entry->d_name) == 1) file_count++;
    }

    if (file_count == 0){
        printf("Parent: No file with the qualified extension was found in the directory. Now Exiting...\n");
        closedir(current_dir);
        return 0;
    }

    rewinddir(current_dir);
    printf("Parent: %d files with the qualified extension found. Creating files names list...\n", file_count);
    char **file_names_list = (char **)malloc(file_count * sizeof(char *));
    if (file_names_list == NULL) {
        perror("Parent: Failed to allocate memory for file names list");
        return 1;
    }

    // Populate the file names list
    int index = 0;
    while ((dir_entry = readdir(current_dir)) != NULL) {
        if (has_usp_extension(dir_entry->d_name) == 1) {
            file_names_list[index] = strdup(dir_entry->d_name);
            if (file_names_list[index] == NULL) {
                perror("Failed to allocate memory for a file name");
                return 1;
            }
            index++;
            if (index == file_count) break;
        }
    }
    closedir(current_dir);

    // Allocate memory for pipes
    printf("Parent: List creation completed. Allocating memory for pipes...\n");
    int (*pipe_array)[2];
    pipe_array = (int (*)[2])malloc(file_count * sizeof(int[2]));
    if (!pipe_array) {
        perror("Failed to allocate memory for pipes");
        return 1;
    }

    // Create pipes
    for (int i = 0; i < file_count; i++) {
        if (pipe(pipe_array[i]) == -1) {
            perror("Failed to create a pipe");
            for (int j = 0; j < i; j++) {
                close(pipe_array[j][0]);
                close(pipe_array[j][1]);
            }
            free(pipe_array);
            return 1;
        }
    }

    // Fork child processes
    printf("Parent: Pipes created. Forking child process...\n");
    int process_index = -1;
    for (int i = 0; i < file_count && process_index == -1; i++) {
        switch (fork()) {
        case -1: 
            perror("Failed to fork a child process");
            exit(1);
        case 0: 
            for (int i = 0; i < file_count; i++) free(file_names_list[i]);
            free(file_names_list);
            process_index = i;
            printf("Child#%d: Process initialized.\n", process_index);
            break;
        default:
            printf("Parent: Child process #%d forked.\n", i);
            break;
        }
    }

    //Child Process logic
    if (process_index != -1) {
        for (int i = 0; i < file_count; i++) {
            if (process_index != i) {
                close(pipe_array[i][1]);
                close(pipe_array[i][0]);
            }
        }

        // Read the file name from the pipe
        size_t file_name_len;
        if (read(pipe_array[process_index][0], &file_name_len, sizeof(size_t)) < 0) {
            perror("Failed to read from pipe in child process");
            exit(1);
        }

        char *received_file_name = (char *)malloc(file_name_len + 1);
        if (read(pipe_array[process_index][0], received_file_name, sizeof(char) * file_name_len) < 0) {
            perror("Failed to read from pipe in child process");
            exit(1);
        }

        received_file_name[file_name_len] = '\0';
        close(pipe_array[process_index][0]);

        // Process the file and write the result to the pipe
        char* result = read_and_calculate_from_file(received_file_name);
        free(received_file_name);
        if (result) {
            size_t result_length = strlen(result);
            if (write(pipe_array[process_index][1], &result_length, sizeof(result_length)) == -1) {
                perror("Failed to write to pipe in child process");
                exit(1);
            }
            if (write(pipe_array[process_index][1], result, sizeof(char) * result_length) == -1) {
                perror("Failed to write to pipe in child process");
                exit(1);
            }
            free(result);
            close(pipe_array[process_index][1]);
        } else {
            perror("An error occurred while processing the file in child process");
            free(result);
            return 1;
        }
        free(pipe_array);
        printf("Child#%d: Task completed and now exiting...\n", process_index);
        exit(0);
    }

    // Parent Process logic

    printf("Sufficient child processes created. Sending file names to child processes...\n");
    // Send file names to child processes
    for (int i = 0; i < file_count; i++) {
        size_t file_name_length = strlen(file_names_list[i]);
        if (write(pipe_array[i][1], &file_name_length, sizeof(file_name_length)) == -1) {
            perror("Failed to write to pipe in parent process");
            return 1;
        }
        if (write(pipe_array[i][1], file_names_list[i], sizeof(char) * file_name_length) == -1) {
            perror("Failed to write to pipe in parent process");
            return 1;
        }
        free(file_names_list[i]);
        close(pipe_array[i][1]);
    }
    free(file_names_list);

    printf("Parent: Successfully sent file names to child processes. Waiting for them to process...\n");
    sleep(1);

    // Open the result file for writing
    int output_fd = open(OUTPUT_FILE, O_WRONLY | O_CREAT | O_APPEND, S_IWUSR | S_IRUSR);
    if (output_fd == -1) {
        perror("Failed to open output file");
        free(pipe_array);
        return 1;
    }
    
    for (int i = 0; i < file_count; i++) {
        size_t result_length;
        // Read results from child processes 
        if (read(pipe_array[i][0], &result_length, sizeof(size_t)) < 0) {
            perror("Failed to read from pipe in parent process");
            close(output_fd);
            free(pipe_array);
            return 1;
        }

        char *received_result = (char *)malloc(result_length + 1);
        if (read(pipe_array[i][0], received_result, sizeof(char) * result_length) < 0) {
            perror("Failed to read from pipe in parent process");
            close(output_fd);
            free(pipe_array);
            return 1;
        }
        received_result[result_length] = '\0'; 
        close(pipe_array[i][0]);

        // Write the received result to the result file
        ssize_t bytes_written = write(output_fd, received_result, sizeof(char) * result_length);
        ssize_t newline_written = write(output_fd, "\n", sizeof(char));
        if (bytes_written == -1 || newline_written == -1) {
            close(output_fd);
            free(pipe_array);
            perror("Failed to write to output file");
            return 1;
        }
        printf("Parent: Wrote the result \"%s\" to the output file\n", received_result);
        free(received_result);
    }
    close(output_fd);
    free(pipe_array);

    // Wait for all child processes to complete
    while (wait(NULL) != -1 || errno != ECHILD);// for (int i = 0; i < file_count; i++) wait(NULL);
    printf("Parent: Confirmed that all child processes have completed. Exiting main process...\n");
    
    return 0;
}