#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#include "shell.h"
#include "systemState.h"
#include "FilePath.h"
#include "commands.h"
#include "fat.h"
#include "formattedOutput.h"
#include "tokenize.h"
#include "executeCommand.h"

void clearScreen()
{
  const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
  write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
    if (*(closingQuote + 1) == '[') {
        const char *repStart = closingQuote + 2; // Start after '['
        const char *repEnd = strchr(repStart, ']'); // Find the closing ']'

        if (repEnd == NULL) {
            free(extractedString);
            return NULL; // Invalid pattern, missing closing ']'
        }

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
    else if (strcmp(tokens.tokens[0], "unlink") == 0)
    {
        if (tokens.length == 1) {
            printf("usage: unlink [path/directory]");
            return;
        }

        FilePath *unlinkPath;
        unlinkPath = initFilePathFromOtherPath(sysState->currentPath, tokens.tokens[1]);
        unlinkPath->pathSize--;

        unlink_file(unlinkPath, unlinkPath->pathTokens[unlinkPath->pathSize]);

        free(unlinkPath);
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
