#include <stdio.h>
#include <stdlib.h>

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
    struct file_t* files;
};

int main(int argc, char** argv)
{
    printf("Test\n");

    // Open file
    // Read file

    // Traverse subpacks

    /*
    Functions needed:
    
    */

    return 0;
}