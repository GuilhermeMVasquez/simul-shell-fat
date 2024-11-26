#ifndef FAT_H
#define FAT_H

#include <stdint.h>
#include "FilePath.h"

/* Número de blocos no sistema */
#define NUM_BLOCKS 2048
#define BLOCK_SIZE		1024
#define BLOCKS			2048
#define FAT_SIZE		(BLOCKS * 2)
#define FAT_BLOCKS		(FAT_SIZE / BLOCK_SIZE)
#define ROOT_BLOCK		FAT_BLOCKS
#define DIR_ENTRY_SIZE	32
#define DIR_ENTRIES		(BLOCK_SIZE / DIR_ENTRY_SIZE)
uint16_t fat[NUM_BLOCKS];
uint8_t data_block[BLOCK_SIZE];

/* Estrutura para uma entrada de diretório */
typedef struct dir_entry_s {
    int8_t filename[25];
    uint8_t attributes;
    uint16_t first_block;
    uint32_t size;
} Dir_Entry;

/* save the fat in disk */
void save_fat_to_disk();

/* load fat from disk */
void load_fat_from_disk();

/* search for a free block in fat */
int find_free_block();

/* allocate a block in fat */
int allocate_block();

void free_blocks(int initial_block);

int create_file( FilePath *filepath, const char *name, const uint8_t *data, uint32_t size );


void read_block(char *file, uint32_t block, uint8_t *record);

/* writes a data block to disk */
void write_block(char *file, uint32_t block, uint8_t *record);

/* initialize the file system */
void initialize_file_system();

int create_directory( FilePath *filepath, const char *dirname );

int list_directory( FilePath *filepath );

char check_if_dir_exists( FilePath *filepath );

int append_file( FilePath *filepath, const char *filename, const uint8_t *data, int repetions );

int overwrite_file( FilePath *filepath, const char *filename, const uint8_t *data, int repetitions );

int read_file( FilePath *filepath, const char *filename );

#endif // FAT_H
