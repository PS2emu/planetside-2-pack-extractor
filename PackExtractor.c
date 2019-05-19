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
//uint64_t bytes_to_u64(unsigned char* bytes, size_t len);
void freePackPointers(struct chunk_t *pack, unsigned int count);
void freeChunkPointers(struct chunk_t *chunk, unsigned int size);
void freeFilePointers(struct file_t *file, unsigned int count);

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
    struct chunk_t *packFile = IterateThroughChunks(fp);

    // Single pointer free test
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < packFile[i].file_count; j++){
            free(packFile[i].files[j].name); // Free's file name pointer
        }
        free(packFile[i].files); // Free's all files
    }
    free(packFile); // Free's all chunks/.pack

    if(fclose(fp) == 0)
        printf("Closed file\n");
    else
        printf("Couldn't close file\n");
    return 0;
}

struct chunk_t* IterateThroughChunks(FILE *fp){

    int i, j = 0;
    int chkoff = 0;
    struct chunk_t myChunk;

    // Allocates 16 instances of size chunk_t
    struct chunk_t *packFile = calloc(16, sizeof(struct chunk_t));
    if(packFile == NULL) {
        printf("NULL POINTER (packFile - IterateThroughChunks)\n");
    }

    // while loop iterates through chunks
    // for loop iterates through files is in the chunks
    do{

        // gets the next chunk based on
        // offset give by last chunk
        myChunk = GetChunkHeader(chkoff, fp);

        for(i = 0; i < myChunk.file_count; i++){

            printf("%d ", i);
            struct file_t f = GetFileHeader(fp);

            // assigns file to chunk memory
            myChunk.files[i] = f;
        }

        // Next chunk location defined
        chkoff = myChunk.next_chunk_offset;

        // Current chunk stored
        packFile[j] = myChunk;
        j++;
    }
    // Checks for last chunk, which will have an offset value of 00 00 00 00
    while(chkoff != 0);

    // Returns all chunks in .pack
    return packFile;
}

struct chunk_t GetChunkHeader(unsigned int chkoff, FILE *fp){

    unsigned char* nextSubpackOffset = (unsigned char *)malloc(CHUNK_OFFSET_LENGTH);
    unsigned char* curSubpackFileCount = (unsigned char*)malloc(CHUNK_FILECOUNT_LEN);

    // fseek to correct location of chunk
    fseek(fp, chkoff, SEEK_SET);

    // Assign header to chunk_t params
    fread(nextSubpackOffset, 1, CHUNK_OFFSET_LENGTH, fp);
    fread(curSubpackFileCount, 1, CHUNK_FILECOUNT_LEN, fp);

    unsigned int nextChunkOffest = GetCharArrayNumeric(nextSubpackOffset, CHUNK_OFFSET_LENGTH);

    unsigned int chunkFileCount = GetCharArrayNumeric(curSubpackFileCount, CHUNK_FILECOUNT_LEN);

    printf("Next chunk: %08x, Chunk FC: %x (%d)\n", nextChunkOffest, chunkFileCount, chunkFileCount);

    struct chunk_t curChunk;
    curChunk.next_chunk_offset = nextChunkOffest;
    curChunk.file_count = chunkFileCount;
    curChunk.files = (struct file_t*)calloc(curChunk.file_count, sizeof(struct file_t));
    if(curChunk.files == NULL) {
        printf("NULL POINTER (curChunk.files)\n");
    }

    free(nextSubpackOffset);
    free(curSubpackFileCount);
    return curChunk;
}

struct file_t GetFileHeader(FILE *fp){
    struct file_t fileHeader;

    // Reads in 4 bytes/name length as char's
    unsigned int numericNameLength = 0;
    unsigned char *fileNameLength = calloc(FILE_NAME_LENGTH, 1);
    if(fileNameLength == NULL) {
        printf("NULL POINTER (fileNameLength)\n");
    }
    fread(fileNameLength, 1, FILE_NAME_LENGTH, fp);
    numericNameLength = GetCharArrayNumeric(fileNameLength, FILE_NAME_LENGTH);
    fileHeader.name_length = numericNameLength;
    free(fileNameLength);

    // Reads file name as string
    unsigned char *fileString = calloc(numericNameLength + 1, 1);
    if(fileString == NULL) {
        printf("NULL POINTER (fileString)\n");
    }
    fread(fileString, 1, numericNameLength, fp);
    printf("%s!\n", fileString);
    fileHeader.name = fileString;

    // Collects file offset
    unsigned int numericOffset = 0;
    unsigned char *fileOffset = calloc(FILE_OFFSET_LENGTH, 1);
    if(fileOffset == NULL) {
        printf("NULL POINTER (fileOffset)\n");
    }
    fread(fileOffset, 1, FILE_OFFSET_LENGTH, fp);
    numericOffset = GetCharArrayNumeric(fileOffset, FILE_OFFSET_LENGTH);
    fileHeader.offset = numericOffset;
    free(fileOffset);

    // Collects file length
    unsigned int numericLength = 0;
    unsigned char *fileLength = calloc(FILE_SIZE_LENGTH, 1);
    if(fileLength == NULL) {
        printf("NULL POINTER (fileLength)\n");
    }
    fread(fileLength, 1, FILE_SIZE_LENGTH, fp);
    numericLength = GetCharArrayNumeric(fileLength, FILE_SIZE_LENGTH);
    fileHeader.length = numericLength;
    free(fileLength);

    // Collects file CRC32 hash
    unsigned int numericCRC = 0;
    unsigned char *fileCRC = calloc(FILE_CRC32_LENGTH, 1);
    if(fileCRC == NULL) {
        printf("NULL POINTER (fileCRC)\n");
    }
    fread(fileCRC, 1, FILE_CRC32_LENGTH, fp);
    numericCRC = GetCharArrayNumeric(fileCRC, FILE_CRC32_LENGTH);
    fileHeader.crc32 = numericCRC;
    free(fileCRC);

    return fileHeader;
}

unsigned int GetCharArrayNumeric(unsigned char *header, unsigned int size){
    unsigned int numeric = 0;

    if(size > 4){
        printf("Too many bytes\n");
        return 0;
    }

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

void freePackPointers(struct chunk_t *pack, unsigned int count){
    for(int i = 0; i < count; i++){
        freeChunkPointers(pack, (unsigned int)sizeof(struct chunk_t)); // [sizeof(struct chunk_t) * i]
    }

    free(pack);
}

void freeChunkPointers(struct chunk_t *chunk, unsigned int size){

    freeFilePointers(chunk->files, chunk->file_count);
    free(chunk);
}

void freeFilePointers(struct file_t *file, unsigned int count) {

    for (int i = 0; i < count; i++) {
        printf("Freed %s!\n", file->name);
        free(file->name);
    }
}