#ifndef SYSTEMSTATE_H
#define SYSTEMSTATE_H

#include "FilePath.h"

typedef struct SystemState {
    FilePath *currentPath;
    char hasEnded;
    char hasFAT;
} SystemState;

#endif // SYSTEMSTATE_H