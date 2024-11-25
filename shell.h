#ifndef SHELL_H
#define SHELL_H

#include "commands.h"
#include "systemState.h"

Command* shellCycle(SystemState* systemState);

#endif // SHELL_H