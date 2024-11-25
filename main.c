#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "shell.h"
#include "systemState.h"
#include "FilePath.h"
#include "fat.h"
#include "commands.h"

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

void executeCommand(Command *command, SystemState *sysState)
{
    TokenizedResult tokens = tokenize_string(command->commandString);

    if (tokens.length == 0) {
        free_tokenized_result(tokens);
        return;
    }

    printf("First token: '%s'\n", tokens.tokens[0]); // Add quotes to see trailing spaces
    printf("Amount: %i\n", tokens.length);

    // Check for exact matches
    if (strcmp(tokens.tokens[0], "ls") == 0)
    {
        FilePath *lsPath;
        if (tokens.length > 1)
            lsPath = initFilePath(tokens.tokens[1]);
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
        }

        FilePath *mkdirPath;
        mkdirPath = initFilePath(tokens.tokens[1]);
        mkdirPath->pathSize--;

        create_directory(mkdirPath, mkdirPath->pathTokens[mkdirPath->pathSize]);

        if (tokens.length > 1)
            free(mkdirPath);
    }
    else if (strcmp(tokens.tokens[0], "exit") == 0)
    {
        sysState->hasEnded = 1;
        printf("Exiting!\n");
        return;
    }
    else /* default: */
    {
        printf("Command not recognized: '%s'\n", tokens.tokens[0]);
    }

    free_tokenized_result(tokens);

    return;
}

int main() {

    initialize_file_system();

    FilePath *rootPath = initFilePath("");
    create_directory(rootPath, "home");

    FilePath *homePath = initFilePath("home");
    create_directory(homePath, "documents");

    const char *filename = "exampleNew.txt";  // Nome do arquivo a ser criado
    uint8_t data[] = "This is the content of the file!";
    uint32_t size = sizeof(data) - 1;  // Tamanho dos dados

    // Cria o arquivo no diretório especificado
    int result = create_file(homePath, filename, data, size);


    SystemState *state = malloc(sizeof(SystemState));
    FilePath *startingPath = initFilePath("");
    state->currentPath = startingPath;
    state->hasEnded = 0;

    while ( !(state->hasEnded) )
    {
        Command *command = shellCycle(state);
        executeCommand(command, state);
    }
}