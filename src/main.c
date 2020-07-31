#include "ide.h"
#include "rprintf.h"
#include "fat.h"
#include "term.h"

#define BytesPerSector 512
#define SectorsPerCluster 4

int main(){

    // Welcome
    esp_printf(putc, "FAT 16 file system.\n");
    esp_printf(putc, "---\n\n");

    // Initialize the FAT system.
    fatFSinit();

    // Use open
    struct file *fd;
    fatOpen("FUN", fd);

    esp_printf(putc, "\n\n---\n");
    esp_printf(putc, "Path input: FUN\n");
    esp_printf(putc, "Start reading the data...\n\n");

    // Read the file.
    // Initialize a char array as file buffer.
    int fileSize = get_the_first_sector_of_cluster(fd->rde.file_size);
    
    char fileBuffer[fileSize];
    memset(fileBuffer, 0, sizeof(fileBuffer));

    // Read the file.
    fatRead(fd, fileBuffer, fileSize);

    // Print out the data in the buffer. 
    esp_printf(putc, "data read in: %s\n", fileBuffer);


    // Close the file.
    fatClose(fd);

    return 0;
}



