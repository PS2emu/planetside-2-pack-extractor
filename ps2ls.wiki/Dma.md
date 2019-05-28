# DMA 

`DMA` files contain material information.

### DMAT 

```cpp
struct dmat_t
{
    char magic[4];
    unsigned int version;
    unsigned int textures_length;
    char textures[textures_length];
    unsigned int material_count;
    material_t materials[material_count];
};
```
`magic` is a 4-character code that is always "DMAT".

`version` defines the version of the DMA file.  At the time of this writing, the only value encountered has been `1`.

`textures_length` defines the byte length of the `textures` string block.

`textures` is a contiguous block of null-terminated strings.  Each string defines the file name of the texture to be used for the material.

`material_count` defines the amount of `MATERIAL` blocks that immediately follow the `DMAT` block.

`materials` is an array of `material_t` structs of size `dmat_t::material_count`.

----
### MATERIAL

```cpp
struct material_t
{
    unsigned int name_hash;
    unsigned int data_length;
    unsigned int material_definition_hash;
    unsigned int parameter_count;
    material_parameter_t parameters[parameter_count];
};
```

`name_hash` is assumed to be a hash of the material name.

`data_length` defines the length of the `MATERIAL` block.

`material_definition_hash` defines the hashed name of the material definition as defined in the `materials_3.xml` file.

`parameter_count` defines the size of the `material_t::parameters` array.

`parameters` is an array of `material_parameter_t` objects of size `material_t::parameter_count`.

----
### MATERIAL PARAMETER 
```cpp
struct material_parameter_t
{
    unsigned int semantic_hash;
    unsigned int d3dxparameter_class;
    unsigned int d3dxparameter_type;
    unsigned int data_length;
    byte_t data[data_length];
};
```

`semantic_hash` is a case sensitive Jenkins hash of the parameter's semantic.
`d3dxparameter_class` defines the  [D3DXPARAMETER_CLASS](http://msdn.microsoft.com/en-us/library/windows/desktop/bb205378.aspx) of the parameter.

`d3dxparameter_type` defines the [D3DXPARAMTER_TYPE](http://msdn.microsoft.com/en-us/library/windows/desktop/bb205380.aspx)  of the parameter.

`data_length` defines size of the `material_parameter_t::data` array.

`data` is a byte array of size `material_parameter_t::data_length` that defines the parameter data. For texture parameteres it's a 32 bit unsigned int containing the a upper case Jenkins hash of a texture name stored in `textures` or 0 for none.