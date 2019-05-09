#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// //0xAB
// #define HI_NIBBLE(b) (((b) >> 4) & 0x0F)    //0xA
// #define LO_NIBBLE(b) ((b) & 0x0F)           //0xB

struct chunk_t CollectChunkFiles(struct chunk_t chk, FILE **fp);
struct file_t NextFile(FILE **fp);
void NextChunk(unsigned int chkoff);

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

    struct chunk_t curChunk;
    curChunk.next_chunk_offset = chunkOffest;
    curChunk.file_count = chunkFileCount;
    curChunk.files[chunkFileCount];

    CollectChunkFiles(curChunk, &fp);
    
    free(subPackHeader);
    fclose(fp);
    printf("closed file");
    return 0;
}

struct chunk_t CollectChunkFiles(struct chunk_t chk, FILE **fp){
    int i;
    int numericNameLength = 0;
    struct file_t myFile;

    // Reads in 4 bytes/name length as char's
    unsigned char *fileNameLength;
    fileNameLength = malloc(4);

    fread(fileNameLength, 1, 4, *fp);

    // Converts char's to uint
    unsigned char higher;
    unsigned char lower;
    int exp;
    for(i = 0; i < 8; i += 2){
        higher = (((fileNameLength[(i / 2)]) >> 4) & 0x0F);
        lower = ((fileNameLength[(i / 2)]) & 0x0F);
        exp = pow(16, (8 - (i + 1)));
        numericNameLength += (unsigned int)(higher) * exp;
        exp = pow(16, (8 - (i + 2)));
        numericNameLength += (unsigned int)(lower) * exp;
    }

    if(numericNameLength == 26)
        printf("%d\n", numericNameLength);
    myFile.name_length = numericNameLength;

    // Reads file name as string
    unsigned char *fileString;
    fileString = malloc(numericNameLength);
    fread(fileString, 1, numericNameLength, *fp);

    printf("fileString size == %d ", sizeof(fileString));
    printf("%s!\n", fileString);
    myFile.name = fileString;

    // Collects file offset, length, crc32
    unsigned char *fileOffset;
    unsigned int numericOffset = 0;
    fileOffset = malloc(4);
    fread(fileOffset, 1, 4, *fp);
    for(i = 0; i < 8; i += 2){
        printf("%02x ", fileOffset[i / 2] & 0xFF);
        higher = (((fileOffset[(i / 2)]) >> 4) & 0x0F);
        //printf("%x ", higher & 0xF);
        lower = ((fileOffset[(i / 2)]) & 0x0F);
        //printf("%x ", lower & 0xF);
        exp = pow(16, (8 - (i + 1)));
        //printf("%d ", exp);
        numericOffset += (unsigned int)(higher) * exp;
        exp = pow(16, (8 - (i + 2)));
        //printf("%d ", exp);
        numericOffset += (unsigned int)(lower) * exp;
    }
    printf("%08x\n", numericOffset) & 0xFFFFFFFF;
    myFile.offset = numericOffset;

    unsigned char *fileLength;
    unsigned int numericLength = 0;
    fileLength = malloc(4);
    fread(fileLength, 1, 4, *fp);
    for(i = 0; i < 8; i += 2){
        printf("%02x ", fileLength[i / 2] & 0xFF);
        higher = (((fileLength[(i / 2)]) >> 4) & 0x0F);
        //printf("%x ", higher & 0xF);
        lower = ((fileLength[(i / 2)]) & 0x0F);
        //printf("%x ", lower & 0xF);
        exp = pow(16, (8 - (i + 1)));
        //printf("%d ", exp);
        numericLength += (unsigned int)(higher) * exp;
        exp = pow(16, (8 - (i + 2)));
        //printf("%d ", exp);
        numericLength += (unsigned int)(lower) * exp;
    }
    printf("%08x\n", numericLength) & 0xFFFFFFFF;
    myFile.length = numericLength;

    unsigned char *fileCRC;
    unsigned int numericCRC = 0;
    fileCRC = malloc(4);
    fread(fileCRC, 1, 4, *fp);
    for(i = 0; i < 8; i += 2){
        printf("%02x ", fileCRC[i / 2] & 0xFF);
        higher = (((fileCRC[(i / 2)]) >> 4) & 0x0F);
        lower = ((fileCRC[(i / 2)]) & 0x0F);
        exp = pow(16, (8 - (i + 1)));
        numericCRC += (unsigned int)(higher) * exp;
        exp = pow(16, (8 - (i + 2)));
        numericCRC += (unsigned int)(lower) * exp;
    }
    printf("%08x\n", numericCRC) & 0xFFFFFFFF;
    myFile.crc32 = numericCRC;

    free(fileNameLength);
    free(fileOffset);
    free(fileLength);
    free(fileCRC);
    return chk;
}

struct file_t NextFile(FILE **fp){

}

void NextChunk(unsigned int chkoff){
    printf("Next Chunk\n");
}