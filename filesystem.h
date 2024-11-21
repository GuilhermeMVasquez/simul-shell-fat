#define BLOCK_SIZE		1024
#define BLOCKS			2048
#define FAT_SIZE		(BLOCKS * 2)
#define FAT_BLOCKS		(FAT_SIZE / BLOCK_SIZE)
#define ROOT_BLOCK		FAT_BLOCKS
#define DIR_ENTRY_SIZE	32
#define DIR_ENTRIES		(BLOCK_SIZE / DIR_ENTRY_SIZE)

/* FAT data structure */
uint16_t fat[BLOCKS];
/* data block */
uint8_t data_block[BLOCK_SIZE];

/* directory entry */
struct dir_entry_s {
	int8_t filename[25];
	uint8_t attributes;
	uint16_t first_block;
	uint32_t size;
};
