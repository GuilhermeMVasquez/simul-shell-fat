#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#include "formattedOutput.h"
#include "commands.h"
#include "systemState.h"
#include "shellState.h"
#include "tokenize.h"
#include "fat.h"

#define TAB 9
#define DELETE 127
#define TOP 65
#define BOTTOM 66
#define LEFT 68
#define RIGHT 67

int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

void saveCursorPosition()
{
    printf("\033[s");
}

void tabFunction(SystemState *systemState, ShellState *shellState, int *length)
{
    TokenizedResult tokens = tokenize_string(shellState->str);
    
    char *autocomplete;
    if (tokens.length <= 1) {
        autocomplete = getCommandsAutocomplete(shellState->str);
        
        if (autocomplete == NULL)
            return;

        free(shellState->str);
        shellState->str = malloc(strlen(autocomplete) + 1);
        strcpy(shellState->str, autocomplete);
        (*length) = strlen(shellState->str);
        shellState->cursorPosition = (*length);
    }
    else {
        autocomplete = getPathAutocomplete(systemState->currentPath, tokens.tokens[tokens.length - 1]);

        if (autocomplete == NULL)
            return;

        // Find the start of the last token in shellState->str
        int lastTokenBeginning = strlen(shellState->str) - 1;
        char hasFinishedTrailingWhiteSpaces = 0;

        while (lastTokenBeginning > 0) {
            if (shellState->str[lastTokenBeginning] == ' ') { // Fix: use single quotes for character
                if (hasFinishedTrailingWhiteSpaces) {
                    lastTokenBeginning++;
                    break;
                }
            } else {
                hasFinishedTrailingWhiteSpaces = 1;
            }
            lastTokenBeginning--;
        }

        // Create the new string: prefix + autocomplete
        size_t prefixLength = lastTokenBeginning;
        size_t newLength = prefixLength + strlen(autocomplete) + 1;

        char *newStr = malloc(newLength);
        strncpy(newStr, shellState->str, prefixLength); // Copy the prefix
        newStr[prefixLength] = '\0';                   // Null-terminate the prefix
        strcat(newStr, autocomplete);                  // Append the autocomplete

        free(shellState->str);  // Free the old string
        shellState->str = newStr;
        (*length) = strlen(shellState->str);
        shellState->cursorPosition = (*length);
    }
}

Command* shellCycle(SystemState* systemState)
{
    char ch = 'a';
    char *str = malloc(1);
    int length = 0;
    
    ShellState* shellState = malloc(sizeof(ShellState));
    shellState->cursorPosition = 0;
    shellState->str = str;

    printLineShell(systemState, shellState);

    while (ch != 10)
    {
        ch = getch();
        switch (ch) {
            case DELETE:
                if (shellState->cursorPosition > 0)
                    length--;
                    shellState->str[length] = '\0';
                    shellState->cursorPosition --;
                    break;
            case RIGHT:
                if (shellState->cursorPosition < length)
                    shellState->cursorPosition++;
                break;
            case LEFT:
                if (shellState->cursorPosition > 0)
                    shellState->cursorPosition--;
                break;
            
            case TOP:
            case BOTTOM:
                continue;
            
            case TAB:
                tabFunction(systemState, shellState, &length);
                break;
            default:
                shellState->str = realloc(shellState->str, length + 2);
                shellState->str[length++] = ch;
                shellState->str[length] = '\0';
                shellState->cursorPosition++;
        }
        printLineShell(systemState, shellState);
        // printf("\n%d", ch);
    }

    Command *command = malloc(sizeof(Command));
    command->commandString = shellState->str;
    return command;
}