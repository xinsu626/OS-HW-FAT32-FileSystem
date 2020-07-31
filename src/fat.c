#include "ide.h"
#include "rprintf.h"
#include "fat.h"
#include "term.h"


/*
 * Global variables
 * */
unsigned char bootSectorBuffer[512];
char rootDirectoryBuffer[8192];
char fatTableBuffer[16384];
struct boot_sector *bootSector;
struct root_directory_entry *rootDirectory;
int first_data_sector;


/**
 * Remove space in the string.
 * Reference: https://www.youtube.com/watch?v=PkbEY7BiXgA
 * **/
void removeSpace(char *str) {
    if (str == NULL) {
        return;
    }
    int n = 0;
    for (int i=0; i<sizeof(str); ++i) {
        if (str[i] != ' ') {
            str[n++] = str[i];
        }
    }
    str[n] = '\0';
}


/**
 * C implementation for memory set. 
 * Reference: Neil's video.
 * **/
void *memset(void *s, int c, size_t n){
    unsigned char *ptr = (unsigned char*)s;
    unsigned char v = (unsigned char)c;

    for (int i=0; i<n; i++){
        ptr[i] = v;
    }
    return s;
}


/**
 * C implementation for memory set.
 * Reference: Neil's video.
 * **/
void memcpy(void * __restrict dest, const void * __restrict src, size_t num) {
    
    unsigned char *d = (unsigned char*)dest;
    unsigned char *s = (unsigned char*)src;

    for (int k=0; k<num; k++){
        d[k] = s[k];
    }
    return dest;
}


/**
 * Compare two string.
 * Reference: C library.
 * **/
int strcmp(char *s1, char *s2){
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    } 
    return *(const unsigned char*)s1 - *(const unsigned char*)s2; 
}


/**
 * Compute the length of a char [] string.
 * **/
size_t str_len(const char *str){
    for (size_t len=0;;++len) if (str[len]==0) return len;
}


/**
 * Compute the first sector of the cluster.
 * Reference the OS dev.
 * **/
int get_the_first_sector_of_cluster(int cluster) {
    return ((cluster - 2) * bootSector->num_sectors_per_cluster) + first_data_sector;
}


/*
 * Initialize the bootSector, rootDirectory, FAT table.
 * */
int fatFSinit(){
    // Cat the buffers to the struct.
    bootSector = bootSectorBuffer;
    rootDirectory = rootDirectoryBuffer;
    
    // Read the bootSector.
    ata_lba_read(2048, bootSector, 1);

    // Print some info.
    esp_printf(putc, "bytes_per_sector = %d\n", bootSector->bytes_per_sector);
    esp_printf(putc, "num_sectors_per_cluster = %d\n", bootSector->num_sectors_per_cluster);
    esp_printf(putc, "num_reserved_sectors = %d\n", bootSector->num_reserved_sectors);
    esp_printf(putc, "num_fat_tables = %d\n", bootSector->num_fat_tables);
    esp_printf(putc, "num_root_dir_entries = %d\n", bootSector->num_root_dir_entries);

    // Compute the logical block adress of root directory entry.
    int rootDirectoryLba = 2048 + bootSector->num_reserved_sectors + \
                           (bootSector->num_sectors_per_fat * bootSector->num_fat_tables);

    // Read the root directory in.
    ata_lba_read(rootDirectoryLba, rootDirectoryBuffer, 1);

    // Compute the useful information.

    int root_dir_sectors = (bootSector->num_root_dir_entries * 32)/bootSector->bytes_per_sector;
    first_data_sector = bootSector->num_reserved_sectors + \
                       ((bootSector->num_fat_tables * bootSector->num_sectors_per_fat) + \
                       root_dir_sectors) + 2048;

    // Read FAT
    ata_lba_read(2048 + bootSector->num_reserved_sectors, fatTableBuffer, 32);

    return 0;
}


/*
 * Open a file and use struct file to hold the opened file.
 * */
int fatOpen(char *path, struct file *fd) {
    
    // Declare a char array to store file name from disk.
    char fileNamePath[8];
    char fileNameRootDirectory[8];

    memset(fileNamePath, 0, sizeof(fileNamePath)); // Zero out the filename path.
    memcpy(fileNamePath, path, 8);    

    // Search the file at the root level.
    for (int i=0; i < 10; i++){
        memset(fileNameRootDirectory, 0, sizeof(fileNameRootDirectory));
        memcpy(fileNameRootDirectory, rootDirectory[i].file_name, 3);

        // Remove the possible space.
        removeSpace(fileNameRootDirectory);

        if (strcmp(fileNameRootDirectory, fileNamePath) == 0) {
            fd->rde = rootDirectory[i];
            fd->start_cluster = rootDirectory[i].cluster;
            break;
        }
    }
    return 0;
}


/*
 * Get the file size in bytes.
 * */
unsigned int fatGetFileSize(struct file *fd) {
    return (unsigned int) fd->rde.file_size;
}


/*
 * Read the file into a buffer.
 * */
int fatRead(struct file *fd, unsigned char *buf, size_t count){

    // Calcuate the first sector of the cluster.
    int cluster = fd->rde.cluster;
    int sector = get_the_first_sector_of_cluster(cluster);
    ata_lba_read(sector, buf, 4);
    return 0;
}


/*
 * Close the opened file.
 * */

int fatClose(struct file *fd) {
    memset(fd, 0, sizeof(fd));
    return 0;
}


