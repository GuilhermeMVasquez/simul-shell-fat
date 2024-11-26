#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#include "shell.h"
#include "systemState.h"
#include "FilePath.h"
#include "commands.h"
#include "executeCommand.h"

int main() {
    SystemState *state = malloc(sizeof(SystemState));
    FilePath *startingPath = initFilePath("");
    state->currentPath = startingPath;
    state->hasEnded = 0;
    state->hasFAT = 0;

    while ( !(state->hasEnded) )
    {
        Command *command = shellCycle(state);
        executeCommand(command, state);
    }
}