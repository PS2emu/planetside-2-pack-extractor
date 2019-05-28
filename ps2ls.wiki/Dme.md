# DME 

`DME` files contain model mesh data.

## Format

`DME` files are binary files written in little-endian byte order.

```cpp
typedef unsigned char byte_t;

struct vector3_t
{
    float x;
    float y;
    float z;
};

struct aabb_t
{
    vector3_t min;
    vector3_t max;
};
```

### DMOD
The first block in the file is the `DMOD` block.

```cpp
struct dmod_t
{
    char magic[4];
    unsigned int version;
    unsigned int dmat_length;
};
```

`magic` is a 4-character code that is always `"DMOD"`.

`version` defines the version of the `DME` file.  At the time of this writing, the only values encountered have been `3` and `4`.

`dmat_length` defines the length of the `DMAT` block.

----
### DMAT 
Immediately following the `DMOD` block is an inline [DMA](https://github.com/RoyAwesome/ps2ls/wiki/DMA)  file of length `dmod_t::dmat_length`.

----
### MODEL 
The `MODEL` block begins at the absolute file offset defined by `dmod_t::model_header_offset`.
```cpp
struct model_t
{
    aabb_t aabb;
    unsigned int mesh_count;
};
```

`aabb` defines an [axis-aligned bounding box](http://en.wikipedia.org/wiki/Minimum_bounding_box#Axis-aligned_minimum_bounding_box) that encompasses the model.

`mesh_count` is the number of `MESH` blocks that immediately follow the `MODEL` block.

----
### MESH
Immediately following the `MODEL` block, there are `model_t::mesh_count`  `MESH` entries.

The structure of the `MESH` block is dependent on the value of `dmod_t::version`.

#### Header 

The first part of the `MESH` block is the header.  The structure of the header differs depending on the value of `dmod_t::version`.

##### Version 3 

```cpp
// use when dmod_t::version is 3
struct mesh_t
{
    byte_t unknown_0[16];
    unsigned int bytes_per_vertex;
    unsigned int vertex_count;
    unsigned int index_size;
    unsigned int index_count;
};
```

##### Version 4 

```cpp
// use when dmod_t::version is 4
struct mesh_t
{
    unsigned int draw_call_offset;
    unsigned int draw_call_count;
    unsigned int bone_transform_count;
    unsigned int unknown;
    unsigned int vertex_stream_count;
    unsigned int index_size;
    unsigned int index_count;
    unsigned int vertex_count;
};
```
`draw_call_offset` is the index for the first drawcall for this mesh.
`draw_call_count` is the number of drawcalls for this mesh.
`bone_transform_count` should be the maximum number of bones to bind for this mesh. It's typically 64 for skinned meshes, 0 for unskinned meshes.

`unknown` is always -1/0xFFFFFFFF.

`vertex_stream_count` defines the number of vertex data blocks in this mesh.  When `dmod_t::version` is `3`, this value is assumed to be `1`.

`index_size` defines the byte-length of each index.

`index_count` defines the number of indices in this mesh.

`vertex_count` defines the number of vertices in this mesh.

#### Vertex Streams(s)

Immediately following the header are `mesh_t::vertex_stream_count` vertex streams.

```cpp
struct vertex_stream_t
{
    unsigned int bytes_per_vertex;
    byte_t* vertices;
};
```

`bytes_per_vertex` defines the number of bytes per vertex for the vertex block (stride).

`vertices` is a contiguous block of vertex data.  The length in bytes of this block is `mesh_t::bytes_per_vertex * mesh_t::vertex_count`.

####Index Block 

Immediately following the vertex block(s) is the index block.

```cpp
unsigned short* indices;
```

`indices` is a block of contiguous indices of length `mesh_t::index_count * sizeof(unsigned short)` that index into the vertex array.  Each sequential set of 3 indices define a triangle of the mesh in a clockwise winding order.

----
## Drawcalls

```cpp
unsigned int draw_call_count;
```

```cpp
struct draw_call
{
    unsigned int unknown_0;
    unsigned int bone_start;
    unsigned int bone_count;
    unsigned int delta;
    unsigned int unknown_1;
    unsigned int vertex_offset;
    unsigned int vertex_count;
    unsigned int index_offset;
    unsigned int index_count;
};
```

```cpp
unsigned int bone_map_entry_count;
```

```cpp
struct bone_map_entry
{
    unsigned short bone_index;
    unsigned short global_index;
```