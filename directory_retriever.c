#include "directory_retriever.h"

int has_usp_extension(char* filename) {
    size_t len = strlen(filename);

    // Check if the filename is too short to have a ".usp" extension
    if (len < 4) {
        return 0;
    }

    // Check the last 4 characters of the filename
    if (strcmp(&filename[len-4], ".usp") == 0) {
        return 1;
    }

    return 0;
}
