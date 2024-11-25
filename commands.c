#include "commands.h"

char *getCommandsAutocomplete(char *prefix) {
    char *commands[] = {LS, CD, MAN, INIT, LOAD, EXIT, READ, CLEAR, MKDIR, WRITE, CREATE, UNLINK, APPEND};
    int numCommands = sizeof(commands) / sizeof(commands[0]);

    for (int i = 0; i < numCommands; i++) {
        if (strncmp(prefix, commands[i], strlen(prefix)) == 0) {
            return commands[i];
        }
    }

    return NULL;
}