#ifndef SHELL_H
#define SHELL_H

#include "commands.h"
#include "systemState.h"

Command* shellCycle(SystemState* systemState, char *hasEnded);

#endif // SHELL_H