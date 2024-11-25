#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "fat.h"
#include "FilePath.h"

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
    FILE *fs = fopen( "filesystem.dat", "r+b" );
    if ( fs == NULL ) {
        // Attempt to create the file if it doesn't exist
        fs = fopen( "filesystem.dat", "w+b" );
        if ( fs == NULL ) {
            printf( "Error: Could not open or create filesystem.dat.\n" );
            return;
        }
    }

    // this is bug correction ( last version it causes a bug )
    fseek( fs, 0, SEEK_SET );
    size_t written = fwrite( fat, sizeof( uint16_t ), NUM_BLOCKS, fs );
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

    printf("FAT loaded from disk.\n");
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
Dir_Entry find_directory(FilePath *filepath) {
    Dir_Entry entry;
    memset( &entry, 0, sizeof( entry)  );
    uint32_t parent_block = ROOT_BLOCK; 

    for ( int i = 0; i < filepath->pathSize; i++ ) {
        int found = 0;

        // loop through entries in the current directory
        for ( int j = 0; j < DIR_ENTRIES; j++ ) {
            read_dir_entry( parent_block, j, &entry );

            if ( entry.attributes == 0x02 && strncmp(( char * )entry.filename, filepath->pathTokens[ i ], 25 ) == 0 ) {
                // directory found
                parent_block = entry.first_block; 
                found = 1;
                break;
            }
        }

        if ( !found ) {
            printf( "Error: directory '%s' not found.\n", filepath->pathTokens[ i ] );
            memset( &entry, 0, sizeof( entry ) );  // return a clean directory
            return entry;
        }
    }

    return entry;
}

 /* Create regular file */
int create_file( FilePath *filepath, const char *name, const uint8_t *data, uint32_t size ) {
    uint32_t parent_block;

    if (filepath->pathSize == 0) {
        parent_block = ROOT_BLOCK;
    } else {
        // Find the destination directory using the find_directory helper function
        Dir_Entry parent_dir = find_directory( filepath );

        // look if directory is found
        if ( parent_dir.attributes == 0x00 ) {
            printf( "Error: directory not found.\n" );
            return -1;
        }

        parent_block = parent_dir.first_block;
    }

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

    save_fat_to_disk();

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


int create_directory( FilePath *filepath, const char *dirname ) {
    uint32_t parent_block = ROOT_BLOCK;  // Starts in the root directory
    Dir_Entry entry;

    //Traverse the directories in the path
    for ( int i = 0; i < filepath->pathSize; i++ ) {
        int found = 0;

    // Cycle through the current directory entries to find the next level of the path
        for ( int j = 0; j < DIR_ENTRIES; j++ ) {
            read_dir_entry( parent_block, j, &entry );

            if ( entry.attributes == 0x02 && strncmp(( char * )entry.filename, filepath->pathTokens[ i ], 25) == 0 ) {
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

    save_fat_to_disk();

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
int list_directory( FilePath *filepath ) {
    struct dir_entry_s target_dir;
    uint32_t dir_block;

    // Special case: list the root directory if the path is empty
    if (filepath->pathSize == 0) {
        dir_block = ROOT_BLOCK;
    } else {
        // Find the desired directory using find_directory
        target_dir = find_directory( filepath );

        if (target_dir.attributes == 0x00 || target_dir.attributes != 0x02) {
            printf("Error: Directory not found or invalid.\n");
            return -1;
        }

        // Read the block from the found directory
        dir_block = target_dir.first_block;
    }


    for ( int i = 0; i < DIR_ENTRIES; i++ ) {
        Dir_Entry entry;
        read_dir_entry(dir_block, i, &entry);

        // Checks if the input is not empty
        if ( entry.attributes != 0x00 ) {
            printf("  - %s type %s, first block: %d, size: %d bytes\n",
                   entry.filename,
                   (entry.attributes == 0x02) ? "directory" : "archive",
                   entry.first_block,
                   entry.size);
        }
    }

    return 0;
}

char check_if_dir_exists( FilePath *filepath )
{
    struct dir_entry_s target_dir;

    // Special case: list the root directory if the path is empty
    if (filepath->pathSize == 0) {
        return 1;
    } else {
        // Find the desired directory using find_directory
        target_dir = find_directory( filepath );

        if (target_dir.attributes == 0x00 || target_dir.attributes != 0x02) {
            // Directory not found or invalid.
            return 0;
        }

        return 1;
    }
}

int find_file_in_directory(uint32_t parent_block, const char *filename, struct dir_entry_s *file_entry) {
    for (int i = 0; i < DIR_ENTRIES; i++) {
        read_dir_entry(parent_block, i, file_entry);

        if (file_entry->attributes == 0x01 && strncmp((char *)file_entry->filename, filename, 25) == 0) {
            // File found, return its index
            return i;
        }
    }

    // File not found
    printf("Error: File '%s' not found.\n", filename);
    return -1;
}

int append_file( FilePath *filepath, const char *filename, const uint8_t *data ) {

    if (data == NULL) {
        printf("Error: Trying to append nothing\n");
        return -1;
    }

    // Calculating the data entry size 
    uint32_t data_size = strlen( ( char * ) data );  

    uint32_t parent_block;
    struct dir_entry_s file_entry;
    int found = 0;

    // Handle root case or find the parent directory
    if ( filepath->pathSize == 0 ) {
        // File is in the root directory
        parent_block = ROOT_BLOCK;
    } else {
        // File is in a subdirectory; locate the directory
        struct dir_entry_s target_dir = find_directory( filepath );
        if (target_dir.attributes == 0x00 || target_dir.attributes != 0x02) {
            printf("Error: Directory not found or invalid.\n");
            return -1;
        }
        parent_block = target_dir.first_block;
    }

    int entry_index = find_file_in_directory( parent_block, filename, &file_entry );
    if (entry_index == -1) 
        return -1;  // File not found, error already printed by find_file_in_directory

    //Find the last block of the file
    uint32_t current_block = file_entry.first_block;
    uint32_t last_block = current_block;

    while (fat[ current_block ] != 0x7FFF) {  // 0x7FFF indicates the last block
        last_block = current_block;
        current_block = fat[ current_block ];
    }

    //Check for free space in the last block
    uint8_t buffer[ BLOCK_SIZE ];
    read_block( "filesystem.dat", current_block, buffer );

    uint32_t used_space_in_last_block = file_entry.size % BLOCK_SIZE;
    uint32_t free_space_in_last_block = BLOCK_SIZE - used_space_in_last_block;

    //Write data to the file
    FILE *fs = fopen( "filesystem.dat", "r+b" );
    if (fs == NULL) {
        printf( "Error opening filesystem file.\n" );
        return -1;
    }

    uint32_t data_offset = 0;

    // If there is space in the last block, write data to it
    if ( free_space_in_last_block > 0 ) {
        uint32_t bytes_to_write = ( data_size > free_space_in_last_block ) ? free_space_in_last_block : data_size;
        fseek( fs, last_block * BLOCK_SIZE + used_space_in_last_block, SEEK_SET );
        fwrite( &data[ data_offset ], 1, bytes_to_write, fs );

        data_offset += bytes_to_write;
        data_size -= bytes_to_write;
    }

    //Allocate new blocks if needed
    while ( data_size > 0 ) {
        int new_block = allocate_block();
        if (new_block == -1) {
            printf( "Error: No space left to continue appending to the file.\n" );
            fclose( fs );
            return -1;
        }

        // Update FAT to chain the new block
        fat[ last_block ] = new_block;
        last_block = new_block;

        // Write data to the new block
        uint32_t bytes_to_write = ( data_size > BLOCK_SIZE ) ? BLOCK_SIZE : data_size;
        fseek( fs, last_block * BLOCK_SIZE, SEEK_SET );
        fwrite( &data[ data_offset ], 1, bytes_to_write, fs );

        data_offset += bytes_to_write;
        data_size -= bytes_to_write;
    }

    //Mark the new last block in FAT
    fat[ last_block ] = 0x7FFF;

    save_fat_to_disk();

    // Update the file's size in its directory entry
    file_entry.size += data_offset;
    write_dir_entry( parent_block, found, &file_entry );

    fclose( fs );
    printf( "Appended %d bytes to the file '%s'.\n", data_offset, filename );
    return 0;
}





