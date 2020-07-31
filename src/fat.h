
#ifndef __FAT_H__
#define __FAT_H__

#include <stdint.h>
//#include <string.h>
//#include "clibfuncs.h"

#define CLUSTER_SIZE 4096
#define SECTORS_PER_CLUSTER (CLUSTER_SIZE/SECTOR_SIZE)

#define FILE_ATTRIBUTE_SUBDIRECTORY 0x10

/*
 * Data structure definitions.
 *
 */

/*
 * Boot sector structure for FAT FS. This is the structure of the first sector
 * in the filesystem's partition. The drive also has a boot sector called the
 * MBR which is stored in the first sector on the drive.
 *
 */

struct boot_sector {
    char code[3];
    char oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t num_sectors_per_cluster;
    uint16_t num_reserved_sectors;
    uint8_t num_fat_tables;
    uint16_t num_root_dir_entries;
    uint16_t total_sectors;
    uint8_t media_descriptor;
    uint16_t num_sectors_per_fat;
    uint16_t num_sectors_per_track;
    uint16_t num_heads;
    uint32_t num_hidden_sectors;
    uint32_t total_sectors_in_fs;
    uint8_t logical_drive_num;
    uint8_t reserved;
    uint8_t extended_signature;
    uint32_t serial_number;
    char volume_label[11];
    char fs_type[8];
    char boot_code[448];
    uint16_t boot_signature;
}__attribute__((packed));

/*
 * Root directory entry used to store info about a file. These data structures
 * are packed in the root directory.
 *
 */
struct root_directory_entry {
    char file_name[8];
    char file_extension[3];
    uint8_t attribute;
    uint8_t reserved1;
    uint8_t creation_timestamp;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t access_date;
    uint16_t reserved2;
    uint16_t modified_time;
    uint16_t modified_date;
    uint16_t cluster;
    uint32_t file_size;
};

/*
 *
 * Stores info about an open file
 *
 */
struct file {
    struct file *next;
    struct file *prev;
    struct root_directory_entry rde;
    uint32_t start_cluster;
};

/*
 * Below is macros to access the members of the FAT FS boot sector. On
 * compilers that don't supprot struct packing (like bcc), we need to use these 
 * nstead of the struct defined above.
 *
 * NOTE: ASSUMES sizeof(int) = 4. ONLY WORKS IN 16-BIT PROTECTED MODE!!!!
 */
#define FBS_OEM_NAME(fbs)                        ((char*)((unsigned int)fbs+3))
#define FBS_BYTES_PER_SECTOR(fbs)     (*(unsigned int*) ((unsigned int)fbs+11))
#define FBS_SECTORS_PER_CLUSTER(fbs)  (*(unsigned char*)((unsigned int)fbs+13))
#define FBS_NUM_RESERVED_SECTORS(fbs) (*(unsigned int*) ((unsigned int)fbs+14))
#define FBS_NUM_FAT_TABLES(fbs)       (*(unsigned char*)((unsigned int)fbs+16))
#define FBS_NUM_ROOT_DIR_ENTRIES(fbs) (*(unsigned int*) ((unsigned int)fbs+17))
#define FBS_TOTAL_SECTORS(fbs)        (*(unsigned int*) ((unsigned int)fbs+19))
#define FBS_MEDIA_DESCRIPTOR(fbs)     (*(unsigned char*)((unsigned int)fbs+21))
#define FBS_NUM_SECTORS_PER_FAT(fbs)  (*(unsigned int*) ((unsigned int)fbs+22))
#define FBS_NUM_SECTORS_PER_TRACK(fbs)(*(unsigned int*) ((unsigned int)fbs+24))
#define FBS_NUM_HEADS (fbs)           (*(unsigned int*) ((unsigned int)fbs+26))
#define FBS_NUM_HIDDEN_SECTORS(fbs)   (*(unsigned long*)((unsigned int)fbs+28))
#define FBS_TOTAL_SECTORS_IN_FS(fbs)  (*(unsigned long*)((unsigned int)fbs+32))
#define FBS_LOGICAL_DRIVE_NUM(fbs)    (*(unsigned char*)((unsigned int)fbs+36))
#define FBS_EXTENDED_SIGNATURE(fbs)   (*(unsigned char*)((unsigned int)fbs+38))
#define FBS_SERIAL_NUMBER(fbs)        (*(unsigned long*)((unsigned int)fbs+39))
#define FBS_VOLUME_LABEL(fbs)                   ((char*)((unsigned int)fbs+43))
#define FBS_FS_TYPE(fbs)                        ((char*)((unsigned int)fbs+54))


/*
 * Function prototypes
 *
 */
int fatFSInit();
int fatOpen(char *path, struct file *fd);
int fatClose(struct file *fd);
int fatRead(struct file *fd, unsigned char *buf, size_t count);
unsigned int fatGetFileSize(struct file *fd);


#endif
