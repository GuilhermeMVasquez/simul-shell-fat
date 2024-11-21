#ifndef COMMANDS_H
#define COMMANDS_H

#include <string.h>

// Command definitions
#define INIT "init"
#define LOAD "load"
#define LS "ls"
#define MKDIR "mkdir"
#define CREATE "create"
#define UNLINK "unlink"
#define WRITE "write"
#define APPEND "append"
#define READ "read"
#define CD "cd"

typedef struct Command {
    char *commandName;
    char *path;
} Command;

// Function prototype
char *getCommandsAutocomplete(char *prefix);

#endif // COMMANDS_H