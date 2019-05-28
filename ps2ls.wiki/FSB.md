###FSB 

FSBs are Fmod Sound Banks, holding the sound data for Forgelight

###FSB Format

Binary format in big endian order.


SOE has a propietary tag in the least signifiant bit of one set of flags in the header.  Set the LSB to 0 and the sounds can be played with Fmod.


WIP Header work:
```
Header = {
    4 bytes         Signature "FSB5"
    4 bytes         Unknown0 (0x00000001)
    4 bytes         NumSounds
    4 bytes         HeaderSize
    4 bytes         NameSize
    4 bytes         DataSize
    4 bytes         DataFormat (Vorbis = 0x0000000F)
    4 bytes         Unknown1
    4 bytes         Unknown2
    8 bytes         NameHash(?)
    16 bytes        DataHash(?)
    <HeaderSize>    SoundHeader
    <NameSize>      SoundName
    <DataSize>      SoundData
}

SoundHeader = {
    4 bytes         Flags(?) often 0x00000033, 0x00000031 or 0x0000000D
    4 bytes         Unknown3
    <HeaderSize-8>  [HdrData]
}

HdrData = {
    1 bit           NoClueButMustBeZeroed
    23 bits         SomeDataLen
    1 byte          DataType?
    <SomeDataLen>   SomeData
}
```