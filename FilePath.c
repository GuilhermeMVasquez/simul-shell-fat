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

FilePath *initFilePathFromOtherPath(FilePath *start, char *pathToAppend)
{
    // Check if the new path is absolute
    if (pathToAppend[0] == '/') {
        return initFilePath(pathToAppend);
    }

    // Calculate the total number of tokens in the new path
    char *tempStr = strdup(pathToAppend);
    if (!tempStr) {
        return NULL; // Allocation failed
    }

    int newTokensCount = 0;
    char *token = strtok(tempStr, "/");
    while (token) {
        newTokensCount++;
        token = strtok(NULL, "/");
    }
    free(tempStr);

    // Create a new FilePath structure
    FilePath *newFilePath = malloc(sizeof(FilePath));
    if (!newFilePath) {
        return NULL; // Allocation failed
    }

    // Calculate total size for pathTokens
    newFilePath->pathSize = start->pathSize + newTokensCount;
    newFilePath->pathTokens = malloc(sizeof(char *) * newFilePath->pathSize);
    if (!newFilePath->pathTokens) {
        free(newFilePath);
        return NULL; // Allocation failed
    }

    // Copy tokens from the start FilePath
    for (int i = 0; i < start->pathSize; i++) {
        newFilePath->pathTokens[i] = strdup(start->pathTokens[i]);
        if (!newFilePath->pathTokens[i]) {
            // Free allocated memory on failure
            for (int j = 0; j < i; j++) {
                free(newFilePath->pathTokens[j]);
            }
            free(newFilePath->pathTokens);
            free(newFilePath);
            return NULL;
        }
    }

    // Tokenize pathToAppend and add tokens to the new FilePath
    tempStr = strdup(pathToAppend);
    if (!tempStr) {
        for (int i = 0; i < start->pathSize; i++) {
            free(newFilePath->pathTokens[i]);
        }
        free(newFilePath->pathTokens);
        free(newFilePath);
        return NULL;
    }

    token = strtok(tempStr, "/");
    int index = start->pathSize;
    while (token) {
        newFilePath->pathTokens[index] = strdup(token);
        if (!newFilePath->pathTokens[index]) {
            // Free allocated memory on failure
            for (int i = 0; i < index; i++) {
                free(newFilePath->pathTokens[i]);
            }
            free(newFilePath->pathTokens);
            free(newFilePath);
            free(tempStr);
            return NULL;
        }
        index++;
        token = strtok(NULL, "/");
    }
    free(tempStr);

    return newFilePath;
}