#include <stdlib.h>

#include "shell.h"
#include "systemState.h"
#include "FilePath.h"
#include "fat.h"
#include "stdio.h"


int main() {

    // initialize_file_system();

    // const char *path[] = {};  // Caminho vazio, diretório será criado no root
    // create_directory(path, 0, "home");

    // const char *path2[] = {"home"};
    // create_directory(path2, 1, "documents");

    // const char *path3[] = {"home"};  // Caminho para o diretório de destino
    // int path_length = 1;  // Número de níveis no caminho

    // const char *filename = "exampleNew.txt";  // Nome do arquivo a ser criado
    // uint8_t data[] = "This is the content of the file!";
    // uint32_t size = sizeof(data) - 1;  // Tamanho dos dados

    // // Cria o arquivo no diretório especificado
    // int result = create_file(path3, path_length, filename, data, size);

    // list_directory(path2, 1 );
    




    SystemState *state = malloc(sizeof(SystemState));
    FilePath *startingPath = initFilePath("home/pedro/teste");
    state->currentPath = startingPath;

    char hasEnded = 0;
    while (!hasEnded)
    {
        shellCycle(state, &hasEnded);
    }
}