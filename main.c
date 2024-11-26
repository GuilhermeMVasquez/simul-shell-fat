#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "shell.h"
#include "systemState.h"
#include "FilePath.h"
#include "fat.h"
#include "commands.h"
#include "formattedOutput.h"

void clearScreen()
{
  const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
  write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
}

// Define a struct to hold tokens and their count
typedef struct {
    char** tokens;
    int length;
} TokenizedResult;

char* trim_whitespace(const char* str) {
    while (isspace((unsigned char)*str)) str++; // Skip leading whitespace

    if (*str == 0) return strdup(""); // All spaces, return empty string

    const char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--; // Skip trailing whitespace

    size_t len = end - str + 1;
    char* trimmed = malloc(len + 1);
    if (trimmed) {
        strncpy(trimmed, str, len);
        trimmed[len] = '\0';
    }
    return trimmed;
}

// Function to tokenize a string into a TokenizedResult
TokenizedResult tokenize_string(const char* str) {
    TokenizedResult result = {NULL, 0};
    if (!str) return result;
    char *trimmed_str = trim_whitespace(str);

    // Count the number of tokens (words)
    int token_count = 0;
    const char* temp = trimmed_str;
    while (*temp) {
        while (*temp == ' ') temp++; // Skip spaces
        if (*temp) {
            token_count++;
            while (*temp && *temp != ' ') temp++; // Move to the end of the word
        }
    }

    // Allocate memory for the array of strings
    char** tokens = malloc((token_count + 1) * sizeof(char*)); // +1 for NULL terminator
    if (!tokens) return result;

    int index = 0;
    temp = trimmed_str; // Reset to the start of the string
    while (*temp) {
        while (*temp == ' ') temp++; // Skip spaces
        if (*temp) {
            const char* start = temp;
            while (*temp && *temp != ' ') temp++; // Find the end of the word

            // Allocate and copy the token
            size_t len = temp - start;
            tokens[index] = malloc(len + 1); // +1 for null terminator
            if (!tokens[index]) {
                // Cleanup in case of allocation failure
                for (int i = 0; i < index; i++) free(tokens[i]);
                free(tokens);
                return result;
            }
            strncpy(tokens[index], start, len);
            tokens[index][len] = '\0'; // Null-terminate
            index++;
        }
    }
    tokens[index] = NULL; // Null-terminate the array of strings

    // Fill the result struct
    result.tokens = tokens;
    result.length = token_count;
    return result;
}

// Free the memory allocated by tokenize_string
void free_tokenized_result(TokenizedResult result) {
    if (!result.tokens) return;
    for (int i = 0; i < result.length; i++) {
        free(result.tokens[i]);
    }
    free(result.tokens);
}

// Function to extract the string and repetition count
char *parseAppendArgument(const char *arg, int *repetitions) {
    *repetitions = 1; // Default repetitions
    int len = strlen(arg);

    // Validate starting and ending quote
    if (len < 2 || arg[0] != '"' || strchr(arg + 1, '"') == NULL) {
        return NULL; // Invalid pattern
    }

    // Find the closing quote for the string
    const char *closingQuote = strchr(arg + 1, '"');
    if (!closingQuote) {
        return NULL; // No closing quote
    }

    // Calculate the length of the string part
    int stringLength = closingQuote - (arg + 1);

    // Allocate memory for the extracted string
    char *extractedString = (char *)malloc(stringLength + 1);
    if (!extractedString) {
        perror("malloc failed");
        return NULL; // Memory allocation error
    }
    strncpy(extractedString, arg + 1, stringLength);
    extractedString[stringLength] = '\0'; // Null-terminate the string

    // Check for optional "[rep]" part
    if (*(closingQuote + 1) == '[' && *(arg + len - 1) == ']') {
        const char *repStart = closingQuote + 2; // Start after '['
        const char *repEnd = arg + len - 1;      // End before ']'

        // Verify all characters in "[rep]" are digits
        for (const char *p = repStart; p < repEnd; ++p) {
            if (!isdigit(*p)) {
                free(extractedString);
                return NULL; // Invalid repetition format
            }
        }

        // Parse repetition value
        *repetitions = atoi(repStart);
    }

    return extractedString; // Return the extracted string
}

void executeCommand(Command *command, SystemState *sysState)
{
    TokenizedResult tokens = tokenize_string(command->commandString);

    if (tokens.length == 0) {
        free_tokenized_result(tokens);
        return;
    }

    if (strcmp(tokens.tokens[0], "init") == 0)
    {
        initialize_file_system();
        sysState->hasFAT = 1;
        FilePath *rootPath = initFilePath("");
        free(sysState->currentPath);
        sysState->currentPath = rootPath;
        return;
    }
    else if (strcmp(tokens.tokens[0], "load") == 0)
    {
        load_fat_from_disk();
        sysState->hasFAT = 1;
        return;
    }
    else if (strcmp(tokens.tokens[0], "exit") == 0)
    {
        sysState->hasEnded = 1;
        return;
    }
    else if (strcmp(tokens.tokens[0], "clear") == 0)
    {
        clearScreen();
        return;
    }
    else if (strcmp(tokens.tokens[0], "man") == 0)
    {
        printMan();
        return;
    }
    else if (sysState->hasFAT == 0)
    {
        printf("Please, first initialize or load a FAT using either \"init\" or \"load\"\n");
        return;
    }

    // Check for exact matches
    if (strcmp(tokens.tokens[0], "ls") == 0)
    {
        FilePath *lsPath;
        if (tokens.length > 1)
            lsPath = initFilePathFromOtherPath(sysState->currentPath, tokens.tokens[1]);
        else
            lsPath = sysState->currentPath;

        list_directory(lsPath);

        if (tokens.length > 1)
            free(lsPath);
    }
    else if (strcmp(tokens.tokens[0], "mkdir") == 0)
    {
        if (tokens.length <= 1) {
            printf("usage: mkdir [/caminho/diretório]");
            return;
        }

        FilePath *mkdirPath;
        mkdirPath = initFilePathFromOtherPath(sysState->currentPath, tokens.tokens[1]);
        mkdirPath->pathSize--;

        create_directory(mkdirPath, mkdirPath->pathTokens[mkdirPath->pathSize]);
    
        free(mkdirPath);
    }
    else if (strcmp(tokens.tokens[0], "create") == 0)
    {
        if (tokens.length <= 1) {
            printf("usage: create [/path/file]");
            return;
        }

        FilePath *createPath;
        createPath = initFilePathFromOtherPath(sysState->currentPath, tokens.tokens[1]);
        createPath->pathSize--;

        create_file(createPath, createPath->pathTokens[createPath->pathSize], 0, 0);
        
        free(createPath);
    }
    else if (strcmp(tokens.tokens[0], "cd") == 0)
    {
        if (tokens.length == 1) {
            FilePath *rootPath = initFilePath("");
            free(sysState->currentPath);
            sysState->currentPath = rootPath;
            return;
        }

        FilePath *cdPath;
        cdPath = initFilePathFromOtherPath(sysState->currentPath, tokens.tokens[1]);

        if (check_if_dir_exists(cdPath)) {
            free(sysState->currentPath);
            sysState->currentPath = cdPath;
        }
        else {
            printf("error: %s directory not found!", tokens.tokens[1]);
            free(cdPath);
        }
    }
    else if (strcmp(tokens.tokens[0], "read") == 0)
    {
        if (tokens.length == 1) {
            printf("usage: read [path/file]");
            return;
        }

        FilePath *readPath;
        readPath = initFilePathFromOtherPath(sysState->currentPath, tokens.tokens[1]);
        readPath->pathSize--;

        read_file(readPath, readPath->pathTokens[readPath->pathSize]);

        free(readPath);
    }
    else if (strcmp(tokens.tokens[0], "append") == 0)
    {
        if (tokens.length <= 2) {
            printf("usage: append \"string\"[rep] [/caminho/arquivo]\n");
            return;
        }

        int repetitions;
        char *stringToAppend = parseAppendArgument(tokens.tokens[1], &repetitions);

        if (stringToAppend == NULL) {
            printf("usage: append \"string\"[rep] [/caminho/arquivo]\n");
            return;
        }

        FilePath *appendPath;
        appendPath = initFilePathFromOtherPath(sysState->currentPath, tokens.tokens[2]);
        appendPath->pathSize--;

        append_file(appendPath, appendPath->pathTokens[appendPath->pathSize], stringToAppend, repetitions);

        free(appendPath);
    }
    else if (strcmp(tokens.tokens[0], "write") == 0)
    {
        if (tokens.length <= 2) {
            printf("usage: write \"string\"[rep] [/caminho/arquivo]\n");
            return;
        }

        int repetitions;
        char *stringToWrite = parseAppendArgument(tokens.tokens[1], &repetitions);

        if (stringToWrite == NULL) {
            printf("usage: write \"string\"[rep] [/caminho/arquivo]\n");
            return;
        }

        FilePath *writePath;
        writePath = initFilePathFromOtherPath(sysState->currentPath, tokens.tokens[2]);
        writePath->pathSize--;

        overwrite_file(writePath, writePath->pathTokens[writePath->pathSize], stringToWrite, repetitions);

        free(writePath);
    }
    else /* command not found */
    {
        printf("command not found: %s\n", tokens.tokens[0]);
    }

    free_tokenized_result(tokens);

    return;
}

int main() {
    SystemState *state = malloc(sizeof(SystemState));
    FilePath *startingPath = initFilePath("");
    state->currentPath = startingPath;
    state->hasEnded = 0;
    state->hasFAT = 0;

    while ( !(state->hasEnded) )
    {
        Command *command = shellCycle(state);
        executeCommand(command, state);
    }
}