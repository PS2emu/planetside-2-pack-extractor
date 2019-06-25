#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

// Indicates header section lengths
#define CHUNK_HEADER_LENGTH 8
#define CHUNK_OFFSET_LENGTH 4
#define CHUNK_FILECOUNT_LEN 4
#define FILE_NAME_LENGTH    4
#define FILE_OFFSET_LENGTH  4
#define FILE_SIZE_LENGTH    4
#define FILE_CRC32_LENGTH   4

struct file_t
{
    unsigned char *name;
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

struct pack_t
{
    struct chunk_t *chunk;          // Array of chunk_t's
    unsigned int numChunks;         // Number of chunks in pack file
};



unsigned int GetCharArrayNumeric(unsigned char *header, unsigned int size);
//uint64_t bytes_to_u64(unsigned char* bytes, size_t len);
void SearchFileName(struct pack_t pack, FILE *fp);
void Extract(struct chunk_t *chk, unsigned int chunkCount, unsigned int fileCount, FILE *fp);
void FreePackPointers(struct pack_t *pack);
void FreeChunkPointers(struct chunk_t *chunk);

// Changes: standardized pointer's as: <type> *<var_name>

// Collects chunk header into memory
struct chunk_t *GetChunkHeader(unsigned int chkoff, FILE *fp){

    unsigned char *nextSubpackOffset = (unsigned char *)malloc(CHUNK_OFFSET_LENGTH);
    unsigned char *curSubpackFileCount = (unsigned char *)malloc(CHUNK_FILECOUNT_LEN);

    // fseek to correct location of chunk
    fseek(fp, chkoff, SEEK_SET);

    // Assign header to chunk_t params
    fread(nextSubpackOffset, 1, CHUNK_OFFSET_LENGTH, fp);
    fread(curSubpackFileCount, 1, CHUNK_FILECOUNT_LEN, fp);

    unsigned int nextChunkOffest = GetCharArrayNumeric(nextSubpackOffset, CHUNK_OFFSET_LENGTH);

    unsigned int chunkFileCount = GetCharArrayNumeric(curSubpackFileCount, CHUNK_FILECOUNT_LEN);

    printf("Next chunk: %08x, Chunk FC: %x (%d)\n", nextChunkOffest, chunkFileCount, chunkFileCount);

    struct chunk_t *curChunk = calloc(1, sizeof(struct chunk_t));
    curChunk->next_chunk_offset = nextChunkOffest;
    curChunk->file_count = chunkFileCount;
    curChunk->files = (struct file_t*)calloc(curChunk->file_count, sizeof(struct file_t));
    if(curChunk->files == NULL) {
        printf("NULL POINTER (curChunk.files)\n");
    }

    free(nextSubpackOffset);
    free(curSubpackFileCount);
    return curChunk;
}

// Collects file header into memory
struct file_t *GetFileHeader(FILE *fp){
    struct file_t *fileHeader = calloc(1, sizeof(struct file_t));

    // Reads in 4 bytes/name length as char's
    unsigned int numericNameLength = 0;
    unsigned char *fileNameLength = calloc(FILE_NAME_LENGTH, 1);
    if(fileNameLength == NULL) {
        printf("NULL POINTER (fileNameLength)\n");
    }
    fread(fileNameLength, 1, FILE_NAME_LENGTH, fp);
    numericNameLength = GetCharArrayNumeric(fileNameLength, FILE_NAME_LENGTH);
    fileHeader->name_length = numericNameLength;
    free(fileNameLength);

    // Reads file name as string
    unsigned char *fileString = calloc(numericNameLength + 1, 1);
    if(fileString == NULL) {
        printf("NULL POINTER (fileString)\n");
    }
    fread(fileString, 1, numericNameLength, fp);
    printf("%s!\n", fileString);
    fileHeader->name = fileString;

    // Collects file offset
    unsigned int numericOffset = 0;
    unsigned char *fileOffset = calloc(FILE_OFFSET_LENGTH, 1);
    if(fileOffset == NULL) {
        printf("NULL POINTER (fileOffset)\n");
    }
    fread(fileOffset, 1, FILE_OFFSET_LENGTH, fp);
    numericOffset = GetCharArrayNumeric(fileOffset, FILE_OFFSET_LENGTH);
    fileHeader->offset = numericOffset;
    free(fileOffset);

    // Collects file length
    unsigned int numericLength = 0;
    unsigned char *fileLength = calloc(FILE_SIZE_LENGTH, 1);
    if(fileLength == NULL) {
        printf("NULL POINTER (fileLength)\n");
    }
    fread(fileLength, 1, FILE_SIZE_LENGTH, fp);
    numericLength = GetCharArrayNumeric(fileLength, FILE_SIZE_LENGTH);
    fileHeader->length = numericLength;
    free(fileLength);

    // Collects file CRC32 hash
    unsigned int numericCRC = 0;
    unsigned char *fileCRC = calloc(FILE_CRC32_LENGTH, 1);
    if(fileCRC == NULL) {
        printf("NULL POINTER (fileCRC)\n");
    }
    fread(fileCRC, 1, FILE_CRC32_LENGTH, fp);
    numericCRC = GetCharArrayNumeric(fileCRC, FILE_CRC32_LENGTH);
    fileHeader->crc32 = numericCRC;
    free(fileCRC);

    return fileHeader;
}

// Cycles through every chunk in a .pack file
struct chunk_t *IterateThroughChunks(int *chunkCount, FILE *fp){

    int i, j = 0;
    int chkoff = 0;
    struct chunk_t *myChunk = calloc(1, sizeof(struct chunk_t));

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

        for(i = 0; i < myChunk->file_count; i++){

            printf("%d ", i);
            struct file_t *f = GetFileHeader(fp);

            // assigns file to chunk memory
            myChunk->files[i] = *f;
            free(f);
        }

        // Next chunk location defined
        chkoff = myChunk->next_chunk_offset;

        // Current chunk stored
        packFile[j] = *myChunk;
        j++;
    }
    // Checks for last chunk, which will have an offset value of 00 00 00 00
    while(chkoff != 0);

    *chunkCount = j;

    free(myChunk);
    // Returns all chunks in .pack
    return packFile;
}

// Returns number from array of 4 bytes
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

/*
 * User inputs to search through files
 * Uses the pack_t structure for traversal
 * Searches via exact filename or substring
 */
void SearchFileName(struct pack_t pack, FILE *fp){
    unsigned int chunkLocation = 0;
    unsigned int fileLocation = 0;
    char input = '0';

    unsigned char *searchingName = calloc(64, sizeof(unsigned char));

    printf("*****Search Options*****\n");
    printf("Exact file (1), Substring (2): ");
    scanf(" %c", &input);

    while(input == '1' || input == '2') {
        switch (input) {
            case '1':
                printf("Enter the full file name with extension: ");
                scanf("%s", searchingName);
                for (int i = 0; i < pack.numChunks; i++) {

                    for (int j = 0; j < pack.chunk[i].file_count; j++) {

                        if (strcmp((char *) searchingName, (char *) pack.chunk[i].files[j].name) == 0) {
                            printf("File found, extracting!\n");
                            chunkLocation = i;
                            fileLocation = j;
                            Extract(pack.chunk, chunkLocation, fileLocation, fp);
                        }
                    }
                }

                printf("*****Search Options*****\n");
                printf("Exact file (1), Substring (2): ");
                scanf(" %c", &input);

                memset(searchingName, '\0', 64);
                break;

            case '2':
                printf("Enter the substring: ");
                scanf("%s", searchingName);

                for (int i = 0; i < pack.numChunks; i++) {

                    for (int j = 0; j < pack.chunk[i].file_count; j++) {

                        char extractConfirm = '0';
                        if (strstr((char *) pack.chunk[i].files[j].name, (char *) searchingName) != NULL) {
                            printf("Extract this file? (y/n): %s", pack.chunk[i].files[j].name);
                            scanf(" %c", &extractConfirm);

                            if(extractConfirm == 'y') {
                                chunkLocation = i;
                                fileLocation = j;
                                Extract(pack.chunk, chunkLocation, fileLocation, fp);
                            }

                            if(extractConfirm == 'n'){
                                char confirm = 'n';
                                printf("Continue? (y/n): ");
                                scanf("%c", &confirm);

                                if(confirm == 'y')
                                    printf("Continuing\n");
                                else
                                    break;
                            }
                        }
                    }
                }

                printf("*****Search Options*****\n");
                printf("Exact file (1), Substring (2): ");
                scanf(" %c", &input);

                memset(searchingName, '\0', 64);
                break;

            default:
                printf("Invalid input\n");
                printf("Exact file (1), Substring (2): ");
                scanf(" %c", &input);

                memset(searchingName, '\0', 64);
                break;
        }
    }
}

// Writes file as original filename in working directory
 void Extract(struct chunk_t *chk, unsigned int chunkCount, unsigned int fileCount, FILE *fp){
    fseek(fp, chk[chunkCount].files[fileCount].offset, SEEK_SET);
    unsigned char *fileData = calloc(chk[chunkCount].files[fileCount].length, 1);
    FILE *newFile;

    newFile = fopen((char *)chk[chunkCount].files[fileCount].name, "w+");
    fread(fileData, 1, chk[chunkCount].files[fileCount].length, fp);
    fwrite(fileData, 1, chk[chunkCount].files[fileCount].length, newFile);
 }

 // Free's chunk pointers in pack
void FreePackPointers(struct pack_t *pack){
    for(int i = 0; i < pack->numChunks; i++){
        FreeChunkPointers(&pack->chunk[i]);
    }

    free(pack->chunk);
}

// Free's file name pointers and files pointers in chunks
void FreeChunkPointers(struct chunk_t *chunk){

    for(int i = 0; i < chunk->file_count; i++){
        free(chunk->files[i].name);
    }

    free(chunk->files);
}

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

    // Keep track of the number of chunks in each .pack file
    int numberChunks = 0;

    // Iterate over all file headers in chunk
    struct pack_t pack;
    pack.chunk = IterateThroughChunks(&numberChunks, fp);
    pack.numChunks = numberChunks;

    // Search for user files specified by user
    SearchFileName(pack, fp);

    // Free all 3 levels of pointers: chunks, files, names
    FreePackPointers(&pack);

    if(fclose(fp) == 0)
        printf("Closed file\n");
    else
        printf("Couldn't close file\n");
    return 0;
}