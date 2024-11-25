#ifndef FILEPATH_H
#define FILEPATH_H

typedef struct FilePath {
    char **pathTokens;
    int pathSize;
} FilePath;

FilePath *initFilePath(char *pathAsStr);

#endif // FILEPATH_H