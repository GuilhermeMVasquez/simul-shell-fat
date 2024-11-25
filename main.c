#include <stdlib.h>

#include "shell.h"
#include "systemState.h"
#include "FilePath.h"
#include "fat.h"
#include "stdio.h"


int main() {

    initialize_file_system();

    FilePath *rootPath = initFilePath("");
    create_directory(rootPath, "home");

    FilePath *homePath = initFilePath("home");
    create_directory(homePath, "documents");

    const char *filename = "exampleNew.txt";  // Nome do arquivo a ser criado
    uint8_t data[] = "This is the content of the file!";
    uint32_t size = sizeof(data) - 1;  // Tamanho dos dados

    // Cria o arquivo no diretório especificado
    int result = create_file(homePath, filename, data, size);

    printf("\nLS: \n");

    list_directory(homePath);
    




    // SystemState *state = malloc(sizeof(SystemState));
    // FilePath *startingPath = initFilePath("home/pedro/teste");
    // state->currentPath = startingPath;

    // char hasEnded = 0;
    // while (!hasEnded)
    // {
    //     shellCycle(state, &hasEnded);
    // }
}