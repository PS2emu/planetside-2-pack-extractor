#PACK 

`PACK` files are uncompressed files archives.

## Format

`PACK` files are split into `CHUNK`s.

## CHUNK

`CHUNK` blocks define a chunk of files within this `PACK` file.  There can be multiple `CHUNK`s in a `PACK` file.

```cpp
struct chunk_t
{
    unsigned int next_chunk_offset;
    unsigned int file_count;
    file_t* files;
};
```

`next_chunk_offset` defines the absolute offset to the beginning of the next `CHUNK` block.  If the value is `0`, then this is the last `CHUNK` in this file.

`file_count` defines the number of files contained in this block.

`files` is an array of `file_t`s of size `chunk_t::file_count`.

##FILE
```cpp
struct file_t
{
    unsigned int name_length;
    char* name;
    unsigned int offset;
    unsigned int length;
    unsigned int crc32;
};
```

`name_length` defines the string length of `file_t::name`.

`name` defines the name of the file of length `file_t::name_length`.

`offset` defines the absolute file offset of the beginning of this file.

`length` defines the length in bytes of this file.

`crc32` defines the result of performing a CRC-32 algorithm on all the bytes of this file.