#ifndef SHELLSTATE_H
#define SHELLSTATE_H


typedef struct ShellState {
    int cursorPosition;
    int reuseCommandsPointer;
    char *str;
} ShellState;

#endif // SHELLSTATE_H