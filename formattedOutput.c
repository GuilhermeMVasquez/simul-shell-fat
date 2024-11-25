#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "colors.h"
#include "commands.h"
#include "systemState.h"
#include "shellState.h"

// ##### DEFAULT LINE START #################################################################################

void printManLine(char *commandName, char *arg1, char *arg2, char *whatDoes, char *example)
{
    colorBrightCyan();
    printf(" - ");
    colorGreen();
    printf("%s", commandName);

    if (arg1 != NULL) {
        colorHighIntensityYellow();
        printf(" %s", arg1);
    }

    if (arg2 != NULL) {
        colorHighIntensityMagenta();
        printf(" %s", arg2);
    }

    colorHighIntensityBlue();
    printf("\n   What it does: ");
    resetColor();
    printf("%s\n", whatDoes);

    colorItalicBrightWhite();
    printf("   Example: ");
    resetColor();
    printf("%s\n", example);
    resetColor();
}

void printMan()
{
    colorWhite();
    printf("Default Commands:\n");
    printManLine("init", NULL, NULL, "Formats the FAT, restarting it to it's default start (empty).", "init");
    printManLine("load", NULL, NULL, "Uses the FAT that is on Disk.", "load");
    printManLine("ls", "[/path/directory]", NULL, "List all the directories in the given path.", "ls /users/sergio");
    printManLine("mkdir", "[/path/directory]", NULL, "Creates an empty directory in the given path.", "mkdir Desktop/PUCRS/5SEM");
    printManLine("create", "[/path/file]", NULL, "Creates an empty file in the given path.", "create Desktop/PUCRS/5SEM/main.c");
    printManLine("unlink", "[/path/file]", NULL, "Deletes a file or an empty directory with the given path.", "unlink Desktop/PUCRS/5SEM/main.c");
    printManLine("write", "\"string\"[rep]", "[/path/file]", "Writes data into a file \e[3mrep\e[0m times. (overwriting data, flushing file initially)", "write \"abc\"[5] Desktop/PUCRS/5SEM/main.c");
    printManLine("append", "\"string\"[rep]", "[/path/file]", "Appends data into a file \e[3mrep\e[0m times.", "append \"abc\"[5] Desktop/PUCRS/5SEM/main.c");
    printManLine("read", "[/path/file]", NULL, "Reads the content of a file.", "read Desktop/PUCRS/5SEM/main.c");

    colorWhite();
    printf("\nExtra Commands:\n");
    printManLine("cd", "[/path/directory]", NULL, "Enters the directory in the given path. When inside directories, all paths will be \n   built on top of the path of the directory you are inside. For creating a path from the root,\n   start it with \"/\". If no path is specified to cd, it goes back to the root directory.", "cd Desktop/PUCRS/5SEM");
    printManLine("clear", NULL, NULL, "Clears the screen.", "clear");
    printManLine("exit", NULL, NULL, "Exits the program", "exit");
    printManLine("man", NULL, NULL, "Prints this manual!", "man");
}

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
    if (sysState->hasFAT == 0)
        return;
    colorCyan();    
    printf("/");

    for (int i = 0; i < sysState->currentPath->pathSize; i++) {
        colorBlue();    
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
