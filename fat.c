#include "fat.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>


/* FAT e funções associadas */


void initialize_fat() {
    for (int i = 0; i < 4; i++) {
        fat[i] = 0x7FFE;
    }

    fat[ROOT_BLOCK] = 0x7fff; //intiialize the root dir 

    for (int i = ROOT_BLOCK + 1; i < NUM_BLOCKS; i++) {
        fat[i] = 0x0000;
    }
    // for (int i = 0; i < NUM_BLOCKS; i++) {
    //     printf("fat[%d] = 0x%04X\n", i, fat[i]);
    // }
}

/* reads a directory entry from a directory */
void read_dir_entry(uint32_t block, uint32_t entry, Dir_Entry *dir_entry)
{
	read_block("filesystem.dat", block, data_block);
	memcpy(dir_entry, &data_block[entry * sizeof(Dir_Entry)], sizeof(Dir_Entry));
}

/* writes a directory entry in a directory */
void write_dir_entry(uint32_t block, uint32_t entry, Dir_Entry *dir_entry)
{
	read_block("filesystem.dat", block, data_block);
	memcpy(&data_block[entry * sizeof(Dir_Entry)], dir_entry, sizeof(Dir_Entry));
	write_block("filesystem.dat", block, data_block);
}


void initialize_file_system() {
    initialize_fat();
    save_fat_to_disk();

    /* initializing the root dir */
	/* initialize an empty data block */
	for (int i = 0; i < BLOCK_SIZE; i++)
		data_block[i] = 0;
	/* write an empty ROOT directory block */
	write_block("filesystem.dat", ROOT_BLOCK, data_block);


    /* initializing the rest of blocks */
    /* write the remaining data blocks to disk */
	for (int i = ROOT_BLOCK + 1; i < BLOCKS; i++)
		write_block("filesystem.dat", i, data_block);
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
void load_fat_from_disk() {
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
    for (int i = ROOT_BLOCK + 1; i < NUM_BLOCKS; i++) { 
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
        int next_block = fat[current_block]; 
        fat[current_block] = 0x0000;         
        current_block = next_block;          
    }
}

/* find specific directory */
Dir_Entry find_directory( const char *path[], int path_length ) {
    Dir_Entry entry;          
    memset( &entry, 0, sizeof( entry)  );    
    uint32_t parent_block = ROOT_BLOCK; 

    for ( int i = 0; i < path_length; i++ ) {
        int found = 0;

        // loop through entries in the current directory
        for ( int j = 0; j < DIR_ENTRIES; j++ ) {
            read_dir_entry( parent_block, j, &entry );

            if ( entry.attributes == 0x02 && strncmp(( char * )entry.filename, path[ i ], 25 ) == 0 ) {
                // directory found
                parent_block = entry.first_block; 
                found = 1;
                break;
            }
        }

        if ( !found ) {
            printf( "Error: directory '%s' not found.\n", path[ i ] );
            memset( &entry, 0, sizeof( entry ) );  // return a clean directory
            return entry;
        }
    }

    return entry;
}

 /* Create regular file */
int create_file( const char *path[], int path_length, const char *name, const uint8_t *data, uint32_t size ) {
    // Find the destination directory using the find_directory helper function
    Dir_Entry parent_dir = find_directory( path, path_length );

    // look if directory is found
    if ( parent_dir.attributes == 0x00 ) {
        printf( "Error: directory not found.\n" );
        return -1;
    }

    uint32_t parent_block = parent_dir.first_block;  

    //allocating the first block 
    int first_block = allocate_block();
    if (first_block == -1) {
        printf( "Error: There are no free blocks for the file.\n" );
        return -1;
    }

    int current_block = first_block;
    uint32_t remaining_bytes = size;
    uint32_t data_offset = 0;

    //writing 
    FILE *fs = fopen( "filesystem.dat", "r+b" );
    if (fs == NULL) {
        printf( "Error opening file system file.\n" );
        free_blocks( first_block );  // Release allocated blocks
        return -1;
    }

    // write the archive in the blocks
    while ( remaining_bytes > 0 ) {
        fseek( fs, current_block * BLOCK_SIZE, SEEK_SET );
        uint32_t bytes_to_write = ( remaining_bytes > BLOCK_SIZE ) ? BLOCK_SIZE : remaining_bytes;
        fwrite( &data[ data_offset ], 1, bytes_to_write, fs);

        remaining_bytes -= bytes_to_write;
        data_offset += bytes_to_write;

        if ( remaining_bytes > 0 ) {
            int next_block = allocate_block();
            if ( next_block == -1 ) {
                printf( "Error: There is no space to continue writing the file.\n" );
                free_blocks(first_block);
                fclose(fs);
                return -1;
            }
            fat[ current_block ] = next_block;
            current_block = next_block;
        }
    }

    // Mark the last block as the end
    fat[ current_block ] = 0x7FFF;

    // create archive entry in directory
    Dir_Entry new_entry;
    memset( &new_entry, 0, sizeof( Dir_Entry ) );
    strncpy( ( char * )new_entry.filename, name, 25 );
    new_entry.attributes = 0x01;  // regular archive
    new_entry.first_block = first_block;
    new_entry.size = size;

    //Add the entry to the destination directory
    for ( int i = 0; i < DIR_ENTRIES; i++ ) {
        Dir_Entry temp_entry;
        read_dir_entry( parent_block, i, &temp_entry );

        if ( temp_entry.attributes == 0x00 ) {  // Entrada vazia
            write_dir_entry( parent_block, i, &new_entry );
            printf( "File '%s' created in directory with starting block %d.\n", name, first_block );
            fclose(fs);
            return first_block;
        }
    }

    printf( "Error: Destination directory is full.\n" );
    fclose( fs );
    return -1;
}


void read_block(char *file, uint32_t block, uint8_t *record)
{
	FILE *f;

	f = fopen(file, "r+");
	fseek(f, block * BLOCK_SIZE, SEEK_SET);
	fread(record, 1, BLOCK_SIZE, f);
	fclose(f);
}

/* writes a data block to disk */
void write_block(char *file, uint32_t block, uint8_t *record)
{
	FILE *f;

	f = fopen(file, "r+");
	fseek(f, block * BLOCK_SIZE, SEEK_SET);
	fwrite(record, 1, BLOCK_SIZE, f);
	fclose(f);
}


int create_directory( const char *path[], int path_length, const char *dirname ) {
    uint32_t parent_block = ROOT_BLOCK;  // Starts in the root directory
    Dir_Entry entry;

    //Traverse the directories in the path
    for ( int i = 0; i < path_length; i++ ) {
        int found = 0;

    // Cycle through the current directory entries to find the next level of the path
        for ( int j = 0; j < DIR_ENTRIES; j++ ) {
            read_dir_entry( parent_block, j, &entry );

            if ( entry.attributes == 0x02 && strncmp(( char * )entry.filename, path[ i ], 25) == 0 ) {
                // Diretório encontrado
                parent_block = entry.first_block;  // Avança para o próximo bloco
                found = 1;
                break;
            }
        }

        if ( !found ) {
            return -1;
        }
    }

    //Create the final directory in the parent directory
    int new_dir_block = allocate_block();
    if ( new_dir_block == -1 ) {
        printf("Error: There are no free blocks for the final directory.\n");
        return -1;
    }

    // Initialize the final directory block with 32 empty entries
    uint8_t data_block[ BLOCK_SIZE ] = {0};
    write_block( "filesystem.dat", new_dir_block, data_block );

    // Create the final directory entry
    memset( &entry, 0, sizeof( Dir_Entry ) );
    strncpy( ( char * )entry.filename, dirname, 25 );
    entry.attributes = 0x02;  // directory
    entry.first_block = new_dir_block;
    entry.size = 0;

    // Add entry to parent directory    
    for ( int j = 0; j < DIR_ENTRIES; j++ ) {
        Dir_Entry temp_entry;
        read_dir_entry( parent_block, j, &temp_entry );

        if ( temp_entry.attributes == 0x00 ) {  // Entrada vazia
            write_dir_entry(parent_block, j, &entry);
            printf("Directory '%s' created in block %d.\n", dirname, new_dir_block);
            return new_dir_block;  // Retorna o bloco onde o diretório foi criado
        }
    }

    printf( "Error: Parent directory is full.\n" );
    return -1;  // Parent directory is full
}

// Function to list the contents of a specific directory
int list_directory( const char *path[], int path_length ) {
// Find the desired directory using find_directory
    Dir_Entry target_dir = find_directory( path, path_length );

    if ( target_dir.attributes == 0x00 || target_dir.attributes != 0x02 ) {
        printf( "Error: Directory not found or invalid.\n" );
        return -1;
    }

    // Read the block from the found directory
    uint32_t dir_block = target_dir.first_block;

    for ( int i = 0; i < DIR_ENTRIES; i++ ) {
        Dir_Entry entry;
        read_dir_entry(dir_block, i, &entry);

        // Checks if the input is not empty
        if ( entry.attributes != 0x00 ) {
            printf("  - %s type %s, first block: %d, size: %d bytes\n",
                   entry.filename,
                   (entry.attributes == 0x02) ? "Diretório" : "Arquivo",
                   entry.first_block,
                   entry.size);
        }
    }

    return 0;
}




