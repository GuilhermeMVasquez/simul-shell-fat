#ifndef FAT_H
#define FAT_H

#include <stdint.h>

/* Número de blocos no sistema */
#define NUM_BLOCKS 2048

/* Estrutura para uma entrada de diretório */
struct dir_entry_s {
    int8_t filename[25];
    uint8_t attributes;
    uint16_t first_block;
    uint32_t size;
};

/* declarações das funções */
void initialize_fat();

void save_fat_to_disk();

void load_fat_from_disk();

int find_free_block();

int allocate_block();

void free_blocks(int initial_block);

int create_file(const char *name, const uint8_t *data, uint32_t size);
#endif // FAT_H
