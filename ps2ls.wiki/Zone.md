Map data.
```
typedef struct _SZoneHdr{
	char	FourCC[4]; //ZONE
	char	Pad0[0x24-8];
	UINT32	QuadsPerTile;
	float	TileSize;
	UINT32	Unk1;
	UINT32	VertsPerTile; //?
	UINT32	TilesPerChunk; //8
	INT32	StartX; //-64, etc
	INT32	StartY;
	UINT32	ChunksX; //128
	UINT32	ChunksY;
	UINT32	EcoCount;
} SZoneHdr, *lpSZoneHdr;

void ZoneTest(){
	UINT32 i, j, c, k, c2, tmp;
	CDynMemoryBuffer buf(8192);
	LoadFileToBuffer("D:\\PS2GU9\\Indar.zone", buf);
	CDynMemoryBuffer::CDynMemoryReader r(&buf);
	SZoneHdr hdr;
	r.Get(&hdr);
	float f;
	const char* s;
	bool b;
	UINT8 u8;
	D3DXVECTOR3 v;
	D3DXVECTOR4 trans, rot, scale;
	for (i = 0; i < hdr.EcoCount; i++){
		r.Get(&tmp);
		s = r.GetZTString(); //Name
		s = r.GetZTString(); //ColorNxMap
		s = r.GetZTString(); //SpecNyMap
		r.Get(&c); //Detail repeat

		r.Get(&f); //BlendStrength
		r.Get(&f); //SpecMin
		r.Get(&f); //SpecMax
		r.Get(&f); //SpecSmoothnessMin
		r.Get(&f); //SpecSmoothnessMax
		s = r.GetZTString(); //PhysicsMaterial
		r.Get(&c); //LayerCount
		for (j = 0; j < c; j++){
			r.Get(&f); //Density
			r.Get(&f); //MinScale
			r.Get(&f); //MaxScale
			r.Get(&f); //SlopePeak
			r.Get(&f); //SlopeExtent
			r.Get(&f); //MinElevation
			r.Get(&f); //MaxElevation
			r.GetZTString(); //Flora
			r.Get(&c2); //TintCount
			for (k = 0; k < c2; k++){
				r.Get(&tmp); //Unknown
				r.Get(&tmp); //Unknown
			}
		}
	}
	//Flora
	r.Get(&c); //Flora count
	for (i = 0; i < c; i++){
		s = r.GetZTString(); //Name
		s = r.GetZTString(); //Texture?
		s = r.GetZTString(); //Model file
		r.Get(&b); //Unknown
		r.Get(&f); //unknown
		r.Get(&f); //Unknown

	}
	//Invis walls
	r.Get(&c); 
	for (i = 0; i < c; i++){
		r.Get(&c);
		r.SeekCur(12*c); //Possibly three floats
	}
	//Runtime objects
	r.Get(&c); //Count
	for (i = 0; i < c; i++){
		s = r.GetZTString(); //.adr file name
		r.Get(&f); //Unknown
		r.Get(&c2); //Placement count
		for (j = 0; j < c2; j++){
			//Translation (in first three components)
			for (k = 0; k < 4; k++){
				r.Get(&f);
			}
			//Rotation
			printf("Rotation = ");
			for (k = 0; k < 4; k++){
				r.Get(&f);
				printf("%f ", f);
			}
			printf("\n");
			//Scale (in first three components)
			for (k = 0; k < 4; k++){
				r.Get(&f);
			}
			r.Get(&tmp); //Unknown
			r.Get(&u8); //Unknown
			r.Get(&f); //Unknown
		}
	}
	//Lights
	r.Get(&c);
	for (i = 0; i < c; i++){
		s = r.GetZTString(); //Name
		s = r.GetZTString(); //Color/texture
		//r.Get(&tmp);
		//r.Get(&u8);
		/*for (k = 0; k < 4; k++){
			r.Get(&f);
		}
		for (k = 0; k < 4; k++){
			r.Get(&f);
		}*/
		r.GetPtr(70); //70 bytes of unknown data, 32 bytes starting at 5 might be translation and rotation
		s = r.GetZTString(); //Unknown
		r.Get(&tmp);
	}
	//Unknown data
	r.Get(&c);
	_getch();
}
```