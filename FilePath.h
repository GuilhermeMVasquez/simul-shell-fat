#ifndef FILEPATH_H
#define FILEPATH_H

typedef struct FilePath {
    char **currentPath;
    int pathSize;
} FilePath;

FilePath *initFilePath(char *pathAsStr);

#endif // FILEPATH_H