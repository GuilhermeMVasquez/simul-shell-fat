#include <stdlib.h>

#include "shell.h"
#include "systemState.h"
#include "fat.h"

int main() {
    SystemState *state = malloc(sizeof(SystemState));

    char *startingPath = malloc(sizeof(char) * 2);
    startingPath[0] = '/';
    startingPath[1] = '\0';

    state->currentPath = startingPath;

    char hasEnded = 0;
    while (!hasEnded)
    {
        shellCycle(state, &hasEnded);
    }
}