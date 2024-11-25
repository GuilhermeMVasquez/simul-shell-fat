#include <stdlib.h>
#include <string.h>
#include "FilePath.h"

FilePath *initFilePath(char *pathAsStr)
{
    // Create a FilePath structure
    FilePath *filePath = malloc(sizeof(FilePath));
    if (!filePath) {
        return NULL; // Allocation failed
    }

    // Initialize members
    filePath->pathTokens = NULL;
    filePath->pathSize = 0;

    // Count tokens to determine the number of path components
    char *tempStr = strdup(pathAsStr); // Duplicate the string to avoid modifying the original
    if (!tempStr) {
        free(filePath);
        return NULL; // Allocation failed
    }

    char *token = strtok(tempStr, "/");
    int count = 0;
    while (token) {
        count++;
        token = strtok(NULL, "/");
    }
    free(tempStr); // Free the temporary string

    // Allocate memory for the array of path components
    filePath->pathTokens = malloc(sizeof(char *) * count);
    if (!filePath->pathTokens) {
        free(filePath);
        return NULL; // Allocation failed
    }

    // Tokenize again to fill the path components
    tempStr = strdup(pathAsStr);
    if (!tempStr) {
        free(filePath->pathTokens);
        free(filePath);
        return NULL; // Allocation failed
    }

    token = strtok(tempStr, "/");
    int index = 0;
    while (token) {
        filePath->pathTokens[index] = strdup(token); // Duplicate each token
        if (!filePath->pathTokens[index]) {
            // Free previously allocated memory on failure
            for (int i = 0; i < index; i++) {
                free(filePath->pathTokens[i]);
            }
            free(filePath->pathTokens);
            free(filePath);
            free(tempStr);
            return NULL;
        }
        index++;
        token = strtok(NULL, "/");
    }
    filePath->pathSize = count;
    free(tempStr);

    return filePath;
}