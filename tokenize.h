#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <stdlib.h>

// Struct to hold tokens and their count
typedef struct {
    char** tokens;  // Array of token strings
    int length;     // Number of tokens
} TokenizedResult;

char* trim_whitespace(const char* str);

TokenizedResult tokenize_string(const char* str);

void free_tokenized_result(TokenizedResult result);

#endif // TOKENIZE_H