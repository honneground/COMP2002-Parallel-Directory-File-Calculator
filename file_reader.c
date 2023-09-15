#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int get_int_length(int value) {
    int length = 0;
    if (value == 0) return 1;
    while (value) {
        length++;
        value /= 10;
    }
    return length;
}

char* int_to_str(int value) {
    int length = get_int_length(value);
    char* result_str = (char*)malloc(length + 1);  // +1 for the null terminator
    result_str[length] = '\0';  // null-terminate the string
    int index = length - 1;
    do {
        result_str[index--] = '0' + (value % 10);
        value /= 10;
    } while (value > 0);
    return result_str;
}

char* format_result(char* task_id, int calculation_result) {

    char* calculation_result_str = int_to_str(calculation_result);
    char* formatted_str = (char*)malloc(strlen(task_id) + strlen(calculation_result_str) + 3); // +3 for '#' character, ':' character, and null terminator
    formatted_str[0] = '\0';
    strcat(formatted_str, "#");
    strcat(formatted_str, task_id);
    free(task_id);
    strcat(formatted_str, ":");
    strcat(formatted_str, calculation_result_str);
    free(calculation_result_str);
    return formatted_str;
}

char* read_and_calculate_from_file(char* file_path) {
    int file_descriptor = open(file_path, O_RDONLY);
    if (file_descriptor == -1) return NULL;  // Error opening file

    int buffer_size = 1024;
    char* buffer = (char*) malloc(buffer_size);
    int total_bytes_read = 0;
    int bytes_read;

    while ((bytes_read = read(file_descriptor, buffer + total_bytes_read, buffer_size - total_bytes_read)) > 0) {
        total_bytes_read += bytes_read;
        if (total_bytes_read == buffer_size) {  // Buffer is full, need to resize
            buffer_size *= 2;
            buffer = (char*) realloc(buffer, buffer_size);
        }
    }

    if (bytes_read == -1) {  // Error reading file
        close(file_descriptor);
        return NULL;
    }

    buffer[total_bytes_read] = '\0';  // Null-terminate the content
    close(file_descriptor);

    char* task_id;
    char operator_char;
    char* temp_pointer;
    int operand1, operand2, result;

    // Extract ID
    temp_pointer = strchr(buffer, '#') + 1;
    char* end_of_id = strchr(temp_pointer, '\n');
    int task_id_length = end_of_id - temp_pointer;

    task_id = (char*)malloc(task_id_length + 1);  // +1 for the null-terminator
    if(!task_id) { // Check for malloc failure
        free(buffer);
        return NULL;
    }

    strncpy(task_id, temp_pointer, task_id_length);
    task_id[task_id_length] = '\0'; 



    // Parse numbers and operator
    temp_pointer = strchr(temp_pointer, '\n') + 1;
    operand1 = atoi(temp_pointer);

    temp_pointer = strchr(temp_pointer, '\n') + 1;
    operator_char = *temp_pointer;

    temp_pointer = strchr(temp_pointer, '\n') + 1;
    operand2 = atoi(temp_pointer);

    free(buffer);

    // Compute the result
    switch (operator_char) {
        case '+': result = operand1 + operand2; break;
        case '-': result = operand1 - operand2; break;
        case '*': result = operand1 * operand2; break;
        case '/':
            if (operand2 != 0) result = operand1 / operand2;
            else return NULL;   // Handle division by zero
            break;
        default: return NULL;   // Invalid operation
    }

    char* result_str = format_result(task_id, result);
    return result_str;
}

// Debug Code
#ifdef DEBUG
int main() {
    char* file_path = "sample1.usp";
    char* calculation_result = read_and_calculate_from_file(file_path);
    if (calculation_result) {
        printf("%s\n", calculation_result);
        free(calculation_result);
    } else {
        printf("An error occurred.\n");
    }
    return 0;
}
#endif