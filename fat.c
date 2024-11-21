#include "fat.h"
#include <stdio.h>

/* FAT e funções associadas */
uint16_t fat[NUM_BLOCKS];

void initialize_fat() {
    for (int i = 0; i < 4; i++) {
        fat[i] = 0x7FFE;
    }
    for (int i = 4; i < NUM_BLOCKS; i++) {
        fat[i] = 0x0000;
    }
    // for (int i = 0; i < NUM_BLOCKS; i++) {
    //     printf("fat[%d] = 0x%04X\n", i, fat[i]);
    // }
}


void save_fat_to_disk() {
    FILE *fs = fopen("filesystem.dat", "wb");
    if (fs == NULL) {
        printf("Erro ao criar o arquivo filesystem.dat\n");
        return;
    }
    fwrite(fat, sizeof(uint16_t), NUM_BLOCKS, fs);
    fclose(fs);
}

// função para carregar FAT de filesystem.dat para a memória
void carregar_fat_do_disco() {
    FILE *fs = fopen("filesystem.dat", "rb");
    if (fs == NULL) {
        printf("Erro ao abrir o arquivo filesystem.dat\n");
        return;
    }
    fread(fat, sizeof(uint16_t), NUM_BLOCKS, fs);
    fclose(fs);

    printf("FAT carregada do disco.\n");
}

// função para encontrar o próximo bloco livre na FAT
int find_free_block() {
    for (int i = 4; i < NUM_BLOCKS; i++) { 
        if (fat[i] == 0x0000) {
            return i;
        }
    }
    return -1; // sem blocos livres
}

// função para alocar um bloco
int allocate_block() {
    for (int i = 4; i < NUM_BLOCKS; i++) { 
        if (fat[i] == 0x0000) {
            fat[i] = 0x7FFF; // ja marca como ultimo bloco
            return i;
        }
    }
    return -1; // Sem blocos livres
}

//  função para liberar blocos ocupados
void free_blocks(int initial_block) {
    int current_block = initial_block;
    while (current_block != 0x7FFF) {
        int next_block = fat[current_block]; // salva o próximo bloco antes de liberar ( o próximo bloco é o conteudo de fat[bloco_atual])
        fat[current_block] = 0x0000;         // marca o bloco atual como livre
        current_block = next_block;          // passa para o próximo bloco
    }
}

int create_file(const char *name, const uint8_t *data, uint32_t size){
    int first_block = allocate_block();
    
    if (first_block == -1) {
        printf("Error: No space to create the file.\n");
        return -1;
    }

    int current_block = first_block;
    uint32_t remaining_bytes = size;

    while (remaining_bytes > 0) {
        
        FILE *fs = fopen("filesystem.dat", "r+b");
        fseek(fs, current_block * 1024, SEEK_SET);
        fwrite(data, 1, remaining_bytes > 1024 ? 1024 : remaining_bytes, fs);
        fclose(fs);

        remaining_bytes -= 1024;

        if (remaining_bytes > 0) {
            // aloca o proximo bloco
            int next_block = allocate_block();
            if (next_block == -1) {
                printf("Error: No space while continuing to create the file.\n");
                free_blocks(first_block);
                return -1;
            }
            fat[current_block] = next_block; // encadeamento
            current_block = next_block;
        }
    }

    fat[current_block] = 0x7FFF; // marca o estado do ultimo bloco
    return first_block;          

}
