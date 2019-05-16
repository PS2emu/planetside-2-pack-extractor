#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct chunk_t GetChunkHeader(unsigned int chkoff, FILE *fp);
struct file_t GetFileHeader(FILE *fp);
//struct chunk_t CollectChunkFiles(struct chunk_t chk, FILE *fp);
struct chunk_t* IterateThroughChunks(FILE *fp);

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

    //printf("%08x\n", sizeof(struct file_t));

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
    printf("First chunk: %s\n", packFile[16].files[0].name);

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
    //printf("%d ", packFile[0].file_count);

    return packFile;
}

struct chunk_t GetChunkHeader(unsigned int chkoff, FILE *fp){
    int i = 0;

    //char buff[4096];
    unsigned char* subPackHeader;
    subPackHeader = (unsigned char *)malloc(8);

    fseek(fp, chkoff, SEEK_SET);
    fread(subPackHeader, 1, 8, fp);

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
    // printf("files size %d\n", *curChunk.files);
    // printf("files size location %d\n", &curChunk.files);
    
    free(subPackHeader);
    return curChunk;
}

struct file_t GetFileHeader(FILE *fp){
    unsigned char higher;
    unsigned char lower;
    char nullSentienel = '\0';
    int i;
    int numericNameLength = 0;  // INITIALIZED TO 0 IN CASE NO FUTURE ASSIGNMENT
    int exp;
    struct file_t fileHeader;

    // Reads in 4 bytes/name length as char's
    unsigned char *fileNameLength;
    fileNameLength = malloc(4);

    fread(fileNameLength, 1, 4, fp);

    // Converts char's to uint
    for(i = 0; i < 8; i += 2){
        higher = (((fileNameLength[(i / 2)]) >> 4) & 0x0F);
        lower = ((fileNameLength[(i / 2)]) & 0x0F);
        exp = pow(16, (8 - (i + 1)));
        numericNameLength += (unsigned int)(higher) * exp;
        exp = pow(16, (8 - (i + 2)));
        numericNameLength += (unsigned int)(lower) * exp;
    }
    fileHeader.name_length = numericNameLength;

    // Reads file name as string
    unsigned char *fileString;
    fileString = malloc(numericNameLength + 1);
    fread(fileString, 1, numericNameLength, fp);
    fileString[numericNameLength] = nullSentienel;
    printf("%s!\n", fileString);    // OUTPUT WAS:<mystring><BYTES FROM MEMORY>!
    fileHeader.name = fileString;

    // Collects file offset, length, crc32
    unsigned char *fileOffset;
    unsigned int numericOffset = 0;
    fileOffset = malloc(4);
    fread(fileOffset, 1, 4, fp);
    for(i = 0; i < 8; i += 2){
        higher = (((fileOffset[(i / 2)]) >> 4) & 0x0F);
        lower = ((fileOffset[(i / 2)]) & 0x0F);
        exp = pow(16, (8 - (i + 1)));
        numericOffset += (unsigned int)(higher) * exp;
        exp = pow(16, (8 - (i + 2)));
        numericOffset += (unsigned int)(lower) * exp;
    }
    fileHeader.offset = numericOffset;

    unsigned char *fileLength;
    unsigned int numericLength = 0;
    fileLength = malloc(4);
    fread(fileLength, 1, 4, fp);
    for(i = 0; i < 8; i += 2){
        higher = (((fileLength[(i / 2)]) >> 4) & 0x0F);
        lower = ((fileLength[(i / 2)]) & 0x0F);
        exp = pow(16, (8 - (i + 1)));
        numericLength += (unsigned int)(higher) * exp;
        exp = pow(16, (8 - (i + 2)));
        numericLength += (unsigned int)(lower) * exp;
    }
    fileHeader.length = numericLength;

    unsigned char *fileCRC;
    unsigned int numericCRC = 0;
    fileCRC = malloc(4);
    fread(fileCRC, 1, 4, fp);
    for(i = 0; i < 8; i += 2){
        higher = (((fileCRC[(i / 2)]) >> 4) & 0x0F);
        lower = ((fileCRC[(i / 2)]) & 0x0F);
        exp = pow(16, (8 - (i + 1)));
        numericCRC += (unsigned int)(higher) * exp;
        exp = pow(16, (8 - (i + 2)));
        numericCRC += (unsigned int)(lower) * exp;
    }
    fileHeader.crc32 = numericCRC;

    free(fileNameLength);
    free(fileOffset);
    free(fileLength);
    free(fileCRC);
    return fileHeader;
}

// struct chunk_t CollectChunkFiles(struct chunk_t chk, FILE *fp){
    
// }