#ifndef FILEPATH_H
#define FILEPATH_H

typedef struct FilePath {
    char **pathTokens;
    int pathSize;
} FilePath;

FilePath *initFilePath(char *pathAsStr);

FilePath *initFilePathFromOtherPath(FilePath *start, char *pathToAppend);

#endif // FILEPATH_H