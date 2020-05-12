#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DISK_NAME   "disk.bin"
#define SECTOR_SIZE 0x200

#define ROUND_ADDRESS(addr) (addr = (addr & 0x01FF) ? (addr & (~0x01FF) + SECTOR_SIZE) : addr)
#define LAST_SECTOR(addr) (ROUND_ADDRESS(addr) / SECTOR_SIZE)
#define SKIP_LAST_SECTORS 1

typedef struct _sector_range_t {
    unsigned int start_sector;
    unsigned int count;

    unsigned int address_to_load;
} sector_range_t;

sector_range_t range_list[] = {
    {  1, 15, 0x8000 },
    { 16,  8, 0xA000 },
    //{ 24, 59, 0xB000 },
    {  0,  0,      0 }, // end of struct
};

void disable_buffering(void)
{
    setvbuf(stdin,  NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

long read_str(char *str, size_t size)
{
    long retn = read(0, str, size);
    
    if (retn < 0) {
        printf("read error");
        exit(1);
    }

    if (str[retn-1] == '\n')
        str[retn-1] = '\0';

    return retn;
}

int main()
{
    FILE*        fp         = NULL;
    char         choise[4]  = { 0 };
    bool         success    = false;
    unsigned int sector     = 0;
    unsigned int address    = 0;
    struct stat  statistics = { 0 };
    int          file_size  = 0;
    int          max_sector = 0; 
    int          descriptor;

    disable_buffering();

    descriptor = open(DISK_NAME, O_RDONLY);
    if (descriptor == -1) {
        perror("Error while opening the file");
        exit(EXIT_FAILURE);
    }

    fp = fdopen(descriptor, "rb");
    if (fp == NULL) {
        perror("Error while opening descriptor");
        exit(EXIT_FAILURE);
    }

    if (fstat(descriptor, &statistics) == -1) {
        perror("Error while reading statistics");
        exit(EXIT_FAILURE);
    }

    file_size = statistics.st_size;
    max_sector = LAST_SECTOR(file_size) - 1 - SKIP_LAST_SECTORS;
    printf("The number of sectors on the disk: %d-%d\n", 0, max_sector);
    
    // SECTOR
    printf("What sector of disk do you want to read? (decimal)\n");
    read_str(choise, sizeof(choise));
    sector = atoi(choise);

    if (sector > max_sector) {
        perror("Disk is smaller!\n");
        exit(EXIT_FAILURE);
    }

    if (sector != 0) {
        // MEMORY ADDRESS
        printf("Now let's reverse a little! At what memory address will this sector be loaded? (hex with \"0x\")\n");
        scanf("0x%x", &address);
        
        for (sector_range_t* range = range_list;
             range != &range_list[sizeof(range_list) - 1];
             range++) {
            
            if (sector >= range->start_sector
             && sector < (range->start_sector + range->count)) {
                if (address == range->address_to_load + (sector - range->start_sector) * SECTOR_SIZE) {
                    success = true;
                    break;
                }
            }
        }
    }
    else {
        success = true;
    }

    printf("Checking...\n");
    if (success) {

        if (fseek(fp, SECTOR_SIZE * sector, SEEK_SET) != 0) {
            perror("Error while seeking the file");
            exit(EXIT_FAILURE);
        }

        printf("0x%04X:\t", sector * SECTOR_SIZE);

        unsigned int counter = 0;
        unsigned char ch = 0;
        while((ch = fgetc(fp)) != EOF) {
            if (counter == SECTOR_SIZE) {
                break;
            }
            
            printf("%02X ", ch);
            
            counter++;

            if (counter % 32 == 0) {
                printf("\n\t");
            } 
        }
    }
    else {
        printf("You had a mistake. Bye.");
    }

    printf("\n");
    
    fclose(fp);
    return 0;
}