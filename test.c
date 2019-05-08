#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// //0xAB
// #define HI_NIBBLE(b) (((b) >> 4) & 0x0F)    //0xA
// #define LO_NIBBLE(b) ((b) & 0x0F)           //0xB

struct file_t
{
    unsigned int name_length;       // 4 bytes: File name length
    char* name;
    unsigned int offset;            // 4 bytes: Location of file
    unsigned int length;            // 4 bytes: Length/end location of file
    unsigned int crc32;             // 4 bytes: ?Some kind of check
};

// Header is 8 Bytes 00 - 07
struct chunk_t
{
    unsigned int next_chunk_offset; // first 4 bytes
    unsigned int file_count;        // next 4 bytes
    // should be array
    struct file_t* files[];
};

int main(int argc, char** argv)
{
    int i = 0;

    // Open file
    FILE *fp;
    char fileName[] = "Assets_000.pack";

    // Reading file in as binary
    fp = fopen(fileName, "rb");
    printf("File opened\n");

    // Read file
    if(fp == NULL){
        printf("File not found\n");
    }

    //char buff[4096];
    unsigned char* subPackHeader;
    subPackHeader = (unsigned char *)malloc(8);

    //printf("%d", sizeof(int));

    fseek(fp, 0, SEEK_SET);
    fread(subPackHeader, 1, 8, fp);
    printf("read from file\n");

    // Assign header to chunk_t params
    unsigned int chunkOffest = 0;
    unsigned int chunkFileCount = 0;

    printf("print header\n");
    for(i = 0; i < 8; i++)
        printf("%02x ", subPackHeader[i] & 0xFF);
    printf("\n");
    
    // Converts char values to uint for chunk_t variable assignment
    printf("assign chunk vals\n");
    unsigned char higher;
    unsigned char lower;
    int exp = 0;
    for(i = 0; i < 8; i += 2){
        higher = (((subPackHeader[(i / 2)]) >> 4) & 0x0F);
        lower = ((subPackHeader[(i / 2)]) & 0x0F);
        exp = pow(16, (8 - (i + 1)));
        chunkOffest += (unsigned int)(higher) * exp;
        exp = pow(16, (8 - (i + 2)));
        chunkOffest += (unsigned int)(lower) * exp;
    }

    for(i = 0; i < 8; i += 2){
        higher = (((subPackHeader[(i / 2) + 4]) >> 4) & 0x0F);
        lower = ((subPackHeader[(i / 2) + 4]) & 0x0F);
        exp = pow(16, (8 - (i + 1)));
        chunkFileCount += (unsigned int)(higher) * exp;
        exp = pow(16, (8 - (i + 2)));
        chunkFileCount += (unsigned int)(lower) * exp;
    }

    printf("Chunk size: %08x, Chunk FC: %08x\n", chunkOffest & 0xFFFFFFFF, chunkFileCount & 0xFFFFFFFF);

    
    free(subPackHeader);
    fclose(fp);
    printf("closed file");
    return 0;
}