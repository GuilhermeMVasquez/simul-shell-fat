#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include "shell.h"
#include "systemState.h"
#include "FilePath.h"
#include "commands.h"
#include "executeCommand.h"
#include "dynamic_list.h"

int main() {
    SystemState *state = malloc(sizeof(SystemState));

    DynamicList *usedCommands = malloc(sizeof(DynamicList));
    init_list(usedCommands, 10);
    state->stackOfUsedCommands = usedCommands;

    FilePath *startingPath = initFilePath("");
    state->currentPath = startingPath;

    state->hasEnded = 0;
    state->hasFAT = 0;

    while ( !(state->hasEnded) )
    {
        Command *command = shellCycle(state);
        if (strlen(command->commandString) != 0) {
            char *copyString = malloc(sizeof(char *) * (strlen(command->commandString) + 1));
            strcpy(copyString, command->commandString);
            append(state->stackOfUsedCommands, copyString);
        }
        executeCommand(command, state);
    }
}