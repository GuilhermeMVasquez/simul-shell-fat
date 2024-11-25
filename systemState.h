#ifndef SYSTEMSTATE_H
#define SYSTEMSTATE_H

#include "FilePath.h"

typedef struct SystemState {
    FilePath *currentPath;
} SystemState;

#endif // SYSTEMSTATE_H