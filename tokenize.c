#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>

#include "tokenize.h"

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

TokenizedResult tokenize_string(const char* str) {
    TokenizedResult result = {NULL, 0};
    if (!str) return result;
    char* trimmed_str = trim_whitespace(str);

    // Count the number of tokens (words)
    int token_count = 0;
    const char* temp = trimmed_str;
    bool in_quotes = false;

    while (*temp) {
        while (*temp == ' ' && !in_quotes) temp++; // Skip spaces if not in quotes
        if (*temp) {
            token_count++;
            if (*temp == '"') {
                in_quotes = !in_quotes; // Toggle in_quotes
                temp++;
                while (*temp && (in_quotes || *temp != ' ')) {
                    if (*temp == '"') in_quotes = !in_quotes; // Toggle again if another quote is found
                    temp++;
                }
            } else {
                while (*temp && *temp != ' ') temp++; // Move to the end of the word
            }
        }
    }

    // Allocate memory for the array of strings
    char** tokens = malloc((token_count + 1) * sizeof(char*)); // +1 for NULL terminator
    if (!tokens) return result;

    int index = 0;
    temp = trimmed_str; // Reset to the start of the string
    in_quotes = false;

    while (*temp) {
        while (*temp == ' ' && !in_quotes) temp++; // Skip spaces if not in quotes
        if (*temp) {
            const char* start = temp;
            if (*temp == '"') {
                in_quotes = !in_quotes; // Toggle in_quotes
                temp++;
                while (*temp && (in_quotes || *temp != ' ')) {
                    if (*temp == '"') in_quotes = !in_quotes; // Toggle again if another quote is found
                    temp++;
                }
                if (!in_quotes) temp++; // Include closing quote
            } else {
                while (*temp && *temp != ' ') temp++; // Find the end of the word
            }

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