#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

// Indicates header section lengths
#define CHUNK_HEADER_LENGTH 8
#define CHUNK_OFFSET_LENGTH 4
#define CHUNK_FILECOUNT_LEN 4
#define FILE_NAME_LENGTH    4
#define FILE_OFFSET_LENGTH  4
#define FILE_SIZE_LENGTH    4
#define FILE_CRC32_LENGTH   4

struct chunk_t GetChunkHeader(unsigned int chkoff, FILE *fp);
struct file_t GetFileHeader(FILE *fp);
//struct chunk_t CollectChunkFiles(struct chunk_t chk, FILE *fp);
struct chunk_t* IterateThroughChunks(FILE *fp);
unsigned int GetCharArrayNumeric(unsigned char *header, unsigned int size);
uint64_t bytes_to_u64(unsigned char* bytes, size_t len);

struct file_t
{
    unsigned char* name;
    unsigned int name_length;       // 4 bytes: File name length
    unsigned int offset;            // 4 bytes: Location of file
    unsigned int length;            // 4 bytes: Length/end location of file
    unsigned int crc32;             // 4 bytes: ?Some kind of check
};

// Header is 8 Bytes 00 - 07
struct chunk_t
{
    unsigned int next_chunk_offset; // first 4 bytes
    unsigned int file_count;        // next 4 bytes
    struct file_t *files;           // an array of file headers in memory
};

int main(void){

    // Open file
    // Create function to iterate through all .pack files*************************
    FILE *fp;
    char fileName[] = "Assets_000.pack";

    // Reading file in as binary
    fp = fopen(fileName, "rb");

    // Read file
    if(fp == NULL){
        printf("File not found\n");
        return 0;
    }

    // Iterate over all file headers in chunk
    struct chunk_t *packFile;
    packFile = IterateThroughChunks(fp);
    printf("chnk_t size: %lu\n", sizeof(struct chunk_t));
    printf("file_t size: %lu\n", sizeof(struct file_t));
    printf("Item in memory: %s\n", packFile[16].files[0].name);

    //free(packFile);
//    if(fclose(fp) == 0)
//        printf("Closed file\n");
//    else
//        printf("Couldn't close file\n");
    return 0;
}

struct chunk_t* IterateThroughChunks(FILE *fp){

    int i, j = 0;
    int chkoff = 0;
    unsigned int totalHeaderSize;
    struct chunk_t myChunk;

    struct chunk_t *packFile;
    packFile = calloc(sizeof(struct chunk_t), 16);

    do{
        totalHeaderSize = 0;

        myChunk = GetChunkHeader(chkoff, fp);

        for(i = 0; i < myChunk.file_count; i++){

            printf("%d ", i);
            struct file_t f = GetFileHeader(fp);
            myChunk.files[totalHeaderSize] = f;
            unsigned int prevFileHeaderSize =    sizeof(f.name_length) +
                                    4 +
                                    sizeof(f.offset) +
                                    sizeof(f.length) +
                                    sizeof(f.crc32);
            totalHeaderSize += prevFileHeaderSize;
        }
        chkoff = myChunk.next_chunk_offset;
        packFile[j * sizeof(struct chunk_t)] = myChunk;
        j++;
    }
    while(chkoff != 0);

    return packFile;
}

struct chunk_t GetChunkHeader(unsigned int chkoff, FILE *fp){
    int i = 0;

    unsigned char* subPackHeader = (unsigned char *)malloc(CHUNK_HEADER_LENGTH);

    fseek(fp, chkoff, SEEK_SET);
    fread(subPackHeader, 1, CHUNK_HEADER_LENGTH, fp);

    // Assign header to chunk_t params
    unsigned int nextChunkOffest = 0;
    unsigned int chunkFileCount = 0;
    
    // Converts char values to uint for chunk_t variable assignment
    unsigned char higher;
    unsigned char lower;
    int exp = 0;
    for(i = 0; i < 8; i += 2){
        higher = (((subPackHeader[(i / 2)]) >> 4) & 0x0F);
        lower = ((subPackHeader[(i / 2)]) & 0x0F);
        exp = pow(16, (8 - (i + 1)));
        nextChunkOffest += (unsigned int)(higher) * exp;
        exp = pow(16, (8 - (i + 2)));
        nextChunkOffest += (unsigned int)(lower) * exp;
    }

    for(i = 0; i < 8; i += 2){
        higher = (((subPackHeader[(i / 2) + 4]) >> 4) & 0x0F);
        lower = ((subPackHeader[(i / 2) + 4]) & 0x0F);
        exp = pow(16, (8 - (i + 1)));
        chunkFileCount += (unsigned int)(higher) * exp;
        exp = pow(16, (8 - (i + 2)));
        chunkFileCount += (unsigned int)(lower) * exp;
    }

    printf("Next chunk: %08x, Chunk FC: %x (%d)\n", nextChunkOffest & 0xFFFFFFFF, chunkFileCount & 0xFFFFFFFF, chunkFileCount);

    struct chunk_t curChunk;
    curChunk.next_chunk_offset = nextChunkOffest;
    curChunk.file_count = chunkFileCount;
    curChunk.files = malloc(curChunk.file_count * sizeof(struct file_t));    // 

    free(subPackHeader);
    return curChunk;
}

struct file_t GetFileHeader(FILE *fp){
    char nullSentienel = '\0';
    struct file_t fileHeader;

    // Reads in 4 bytes/name length as char's
    unsigned char *fileNameLength = malloc(FILE_NAME_LENGTH);

    fread(fileNameLength, 1, FILE_NAME_LENGTH, fp);

    // Converts char's to uint
    unsigned int numericNameLength = 0;
    numericNameLength = GetCharArrayNumeric(fileNameLength, FILE_NAME_LENGTH);
    fileHeader.name_length = numericNameLength;

    // Reads file name as string
    unsigned char *fileString = malloc(numericNameLength + 1);
    fread(fileString, 1, numericNameLength, fp);
    fileString[numericNameLength] = nullSentienel;
    printf("%s!\n", fileString);
    fileHeader.name = fileString;

    // Collects file offset, length, crc32
    unsigned char *fileOffset = malloc(FILE_OFFSET_LENGTH);
    unsigned int numericOffset = 0;
    fread(fileOffset, 1, FILE_OFFSET_LENGTH, fp);
    numericOffset = GetCharArrayNumeric(fileOffset, FILE_OFFSET_LENGTH);
    fileHeader.offset = numericOffset;

    unsigned int numericLength = 0;
    unsigned char *fileLength = malloc(FILE_SIZE_LENGTH);
    fread(fileLength, 1, FILE_SIZE_LENGTH, fp);
    numericLength = GetCharArrayNumeric(fileLength, FILE_SIZE_LENGTH);
    fileHeader.length = numericLength;

    unsigned int numericCRC = 0;
    unsigned char *fileCRC = malloc(FILE_CRC32_LENGTH);
    fread(fileCRC, 1, FILE_CRC32_LENGTH, fp);
    numericCRC = GetCharArrayNumeric(fileCRC, FILE_CRC32_LENGTH);
    fileHeader.crc32 = numericCRC;

    free(fileNameLength);
    free(fileOffset);
    free(fileLength);
    free(fileCRC);
    return fileHeader;
}

unsigned int GetCharArrayNumeric(unsigned char *header, unsigned int size){
    unsigned int numeric = 0;

    unsigned char higher;
    unsigned char lower;
    int i;
    unsigned int exp;
    for(i = 0; i < 8; i += 2){
        higher = (((header[(i / 2)]) >> 4) & 0x0F);
        lower = ((header[(i / 2)]) & 0x0F);
        exp = pow(16, (8 - (i + 1)));
        numeric += (unsigned int)(higher) * exp;
        exp = pow(16, (8 - (i + 2)));
        numeric += (unsigned int)(lower) * exp;
    }
    //printf("%x ", numeric);

    return numeric;
}

// Better implementation of function above
//uint64_t bytes_to_u64(unsigned char* bytes, size_t len) {
//    if (len > 8) {
//        printf("Bad length\n");
//        return 0;
//    }
//
//    uint64_t accumulator = 0;
//    for (int i = 0; i < len; ++i) {
//        accumulator <<= 8;
//        accumulator += bytes[i];
//    }
//
//    return accumulator;
//}

// struct chunk_t CollectChunkFiles(struct chunk_t chk, FILE *fp){
    
// }