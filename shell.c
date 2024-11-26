#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#include "formattedOutput.h"
#include "commands.h"
#include "systemState.h"
#include "shellState.h"

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
    char *autocomplete = getCommandsAutocomplete(shellState->str);

    if (autocomplete == NULL)
        return;

    free(shellState->str);
    shellState->str = malloc(strlen(autocomplete) + 1);
    strcpy(shellState->str, autocomplete);
    (*length) = strlen(shellState->str);
    shellState->cursorPosition = (*length);
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