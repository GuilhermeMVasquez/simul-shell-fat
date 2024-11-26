#ifndef SYSTEMSTATE_H
#define SYSTEMSTATE_H

#include "FilePath.h"
#include "dynamic_list.h"

typedef struct SystemState {
    FilePath *currentPath;
    char hasEnded;
    char hasFAT;
    DynamicList *stackOfUsedCommands;
} SystemState;

#endif // SYSTEMSTATE_H