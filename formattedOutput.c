#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "colors.h"
#include "commands.h"
#include "systemState.h"
#include "shellState.h"

// ##### DEFAULT LINE START #################################################################################

void printUserAndMachine()
{
    colorBrightGreen();
    printf("sergio@darkstar");
    resetColor();
}

void printSeparator()
{
    colorWhite();
    printf(":");
    resetColor();
}

void printCurrentPath(SystemState* sysState)
{
    colorBlue();
    if (sysState->currentPath->pathSize == 0)
        printf("/");

    for (int i = 0; i < sysState->currentPath->pathSize; i++) {
        
        printf("%s", sysState->currentPath->pathTokens[i]);
        if (i != sysState->currentPath->pathSize-1) {
            colorCyan();
            printf("/");
            colorBlue();
        }
            
    }
        
    resetColor();
}

void printDolarSignAndSpace()
{
    colorWhite();
    printf("$ ");
    resetColor();
}

void printLineStart(SystemState *sysState)
{
    printUserAndMachine();
    printSeparator();
    printCurrentPath(sysState);
    printDolarSignAndSpace();
}

// ##### DEFAULT LINE START #################################################################################

void clearLine() {
    printf("\r\033[K");
}

void moveCursorLeft(int amount)
{
    if (amount > 0)
        printf("\033[%dD", amount);
}

void moveCursorRight(int amount)
{
    printf("\033[%dC", amount);
}

void adjustCursorPosition(ShellState *shellState)
{
    int strSize = strlen(shellState->str);
    if (strSize > shellState->cursorPosition)
        moveCursorLeft(strSize - shellState->cursorPosition);
}

void printTrailingSpaces(char *str) {
    // Get the length of the string
    int len = strlen(str);
    
    // Start from the end of the string and count trailing spaces
    int count = 0;
    while (len > 0 && str[len - 1] == ' ') {
        count++;
        len--;
    }

    // Print the trailing spaces
    for (int i = 0; i < count; i++) {
        printf(" "); // Print each trailing space
    }
}

int getAmountOfTokens(char *str) {
    char *strCopy = malloc(strlen(str) + 1);
    strcpy(strCopy, str);

    // Count total number of tokens to determine the last token
    int totalTokens = 0;

    char *token = strtok(strCopy, " ");
    while (token != NULL) {
        totalTokens++;
        token = strtok(NULL, " ");
    }
    free(strCopy);

    return totalTokens;
}

void printLineShell(SystemState* sysState, ShellState* shellState)
{
    clearLine();
    printLineStart(sysState);

    int totalTokens = getAmountOfTokens(shellState->str);
    
    char *strCopy = malloc(strlen(shellState->str) + 1);
    strcpy(strCopy, shellState->str);

    char *token = strtok(strCopy, " ");
    
    int currentTokenIndex = 0;
    // Iterate over the substrings
    while (token != NULL) {
        if (currentTokenIndex != 0) {
            printf(" "); // Print space between tokens except for the first one
        }

        char *autocomplete = NULL;
        
        autocomplete = getCommandsAutocomplete(token);
        
        if (autocomplete != NULL && currentTokenIndex == totalTokens-1) {
            colorHighIntensityWhite();
            printf("%s", token);
            colorWhite();
            printf("%s", autocomplete + strlen(token));
            moveCursorLeft(strlen(autocomplete) - strlen(token));
        } else {
            colorBrightWhite();
            printf("%s", token);
        }

        // Get the next token (substring)
        token = strtok(NULL, " ");
        currentTokenIndex++;
    }

    // Now print the trailing spaces of the string called str
    printTrailingSpaces(shellState->str);

    adjustCursorPosition(shellState);

    resetColor();
}
