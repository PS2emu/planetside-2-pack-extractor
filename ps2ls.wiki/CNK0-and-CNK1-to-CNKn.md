Believed to be world data

It's just heightmap + texturing info + some textures
````cpp
typedef struct _SCNKHeader{
  UINT8 FourCC[4];
  UINT32 Version; //?
  UINT32 UncompressedSize;
  UINT32 CompressedSize;
} SCNKHeader, *lpSCNKHeader;
````
 Compression is http://code.google.com/p/lzham/

CChunk1 should also load cnk2, cnk3, etc. with a modified FourCC check.
````cpp
class CLodTile {
public:
	INT32				X;
	INT32				Y;
	INT32				Unk0;
	INT32				Unk1;
	INT32				EcoCount;
	INT32				Index;
	INT32				Unk2;
	UINT32				ImageSize;
	CDynMemoryBuffer	ImageData;
	UINT32				LayerTextureCount;
	UINT8*				LayerTextures;
public:
	CLodTile() : ImageData(8192), X(0), Y(0), Unk0(0), Unk1(0), EcoCount(0), Index(0), Unk2(0), ImageSize(0),
		LayerTextureCount(0), LayerTextures(NULL){
	}
	~CLodTile(){
		if (LayerTextures)
			delete [] LayerTextures;
	}
	bool Load(CDynMemoryBuffer::CDynMemoryReader &r){
		UINT32 ev, fc, fv, i, j;
		void* p;
		if (!r.Get(&X))
			return false;
		if (!r.Get(&Y))
			return false;
		if (!r.Get(&Unk0))
			return false;
		if (!r.Get(&Unk1))
			return false;
		if (!r.Get(&EcoCount))
			return false;
		for (i = 0; i < EcoCount; i++){
			if (!r.Get(&ev))
				return false;
			if (!r.Get(&fc))
				return false;
			for (j = 0; j < fc; j++){
				if (!r.Get(&fv))
					return false;
				if (!r.SeekCur(fv*8))
					return false;
			}
		}
		if (!r.Get(&Index))
			return false;
		if (!r.Get(&Unk2))
			return false;
		if (Unk2 > 0){
			if (!r.Get(&ImageSize))
				return false;
			p = r.GetPtr(ImageSize);
			if (!p)
				return false;
			if (!ImageData.Alloc(ImageSize))
				return false;
			memcpy(ImageData.GetBytePtr(), p, ImageSize);
		} else
			ImageSize = 0;
		if (!r.Get(&LayerTextureCount))
			return false;
		p = r.GetPtr(LayerTextureCount);
		if (!p)
			return false;
		LayerTextures = new UINT8[LayerTextureCount];
		if (!LayerTextures)
			return false;
		memcpy(LayerTextures, p, LayerTextureCount);
		return true;
	}
};

template<typename T> class TTypedArray {
protected:
	T*		m_pArray;
	UINT32	m_Size;
public:
	TTypedArray() : m_Size(0), m_pArray(NULL){
	}
	~TTypedArray(){
		Clear();
	}
	bool Alloc(UINT32 count){
		if (m_Size == count)
			return true;
		if (m_pArray)
			delete [] m_pArray;
		m_pArray = new T[m_Size = count];
		return m_pArray != NULL;
	}
	operator T* (){
		return m_pArray;
	}
	T& operator[](UINT32 index){
		return m_pArray && index < m_Size ? *(m_pArray+index) : *(T*)NULL;
	}
	T* Array(){
		return m_pArray;
	}
	UINT32 Size(){
		return m_Size;
	}
	void Clear(){
		if (m_pArray){
			delete [] m_pArray;
			m_pArray = NULL;
		}
		m_Size = 0;
	}
};

class CChunk0 {
public:
	#pragma pack(push, 1)
	typedef struct _SCNKHeader{
		UINT8	FourCC[4];
		UINT32	Version; //?
		UINT32	UncompressedSize;
		UINT32	CompressedSize;
	} SCNKHeader, *lpSCNKHeader;
	typedef struct _STerrainLod0Vtx{
		INT16	X;
		INT16	Y;
		INT16	FarHeight;
		INT16	NearHeight;
		UINT8	C0[4];
		UINT8	C1[4];
	} STerrainLod0Vtx, *lpSTerrainLod0Vtx;
	typedef struct _SRenderBatch{
		UINT32	IndexOffset;
		UINT32	IndexCount;
		UINT32	VertOffset;
		UINT32	VertCount;
	} SRenderBatch, *lpSRenderBatch;
	typedef struct _SChunkVec3{
		float	X;
		float	Y;
		float	Z;
	} SChunkVec3, *lpSChunkVec3;
	typedef struct _STileOccluderInfo{
		char Data[64];
	} STileOccluderInfo, *lpSTileOccluderInfo;
	typedef struct _SOptimizedDrawTileInfo{
		char Data[320];
	} SOptimizedDrawTileInfo, *lpSOptimizedDrawTileInfo;
	#pragma pack(pop)

	TTypedArray<CLodTile>				Tiles;
	TTypedArray<UINT16>					Indices;
	TTypedArray<STerrainLod0Vtx>		Vertices;
	TTypedArray<SRenderBatch>			RenderBatches;
	TTypedArray<SOptimizedDrawTileInfo>	OptimizedDrawTileInfo;
	TTypedArray<UINT16>					UShorts;
	TTypedArray<SChunkVec3>				Vectors;
	TTypedArray<STileOccluderInfo>		TileOccluderInfo;
	
public:
	CChunk0(){
	}
	~CChunk0(){
	}
	void Clear(){
		Tiles.Clear();
		Indices.Clear();
		Vertices.Clear();
		RenderBatches.Clear();
		OptimizedDrawTileInfo.Clear();
		UShorts.Clear();
		Vectors.Clear();
		TileOccluderInfo.Clear();
	}
	bool Load(const char* pPath){
		Clear();
		lzham_z_stream s;
		int res;
		UINT32 i, j, c;
		CDynMemoryBuffer fbuf(8192);
		CDynMemoryBuffer dbuf(8192);
		SCNKHeader hdr;
		if (!LoadFileToBuffer(pPath, fbuf))
			return false;
		if (fbuf.GetSize() < sizeof(SCNKHeader))
			return false;
		memcpy(&hdr, fbuf.GetPtr(), sizeof(SCNKHeader));
		if (memcmp(hdr.FourCC, "CNK0", 4) != 0)
			return false;
		if (!dbuf.Alloc(hdr.UncompressedSize))
			return false;
		if (fbuf.GetSize() != sizeof(SCNKHeader)+hdr.CompressedSize)
			return false;
		lzham_z_inflateInit2(&s, 20);
		s.avail_in = hdr.CompressedSize;
		s.avail_out = hdr.UncompressedSize;
		s.next_in = (unsigned char*)fbuf.GetBytePtr()+sizeof(SCNKHeader);
		s.next_out = (unsigned char*)dbuf.GetBytePtr();
		res = lzham_z_inflate(&s, LZHAM_Z_FINISH);
		lzham_z_inflateEnd(&s);
		if (res != LZHAM_Z_STREAM_END)
			return false;
		CDynMemoryBuffer::CDynMemoryReader r(&dbuf);
		
		if (!r.Get(&c))
			return Clear(), false;
		if (!Tiles.Alloc(c))
			return false;
		for (i = 0; i < c; i++){
			if (!Tiles[i].Load(r))
				return Clear(), false;
		}
		if (!r.Get(&c))
			return Clear(), false;
		if (!r.Get(&c))
			return Clear(), false;
		if (!r.SeekCur(c*4))
			return Clear(), false;
		//Indices
		if (!r.Get(&c))
			return Clear(), false;
		if (!Indices.Alloc(c))
			return Clear(), false;
		if (!r.GetRaw(Indices.Array(), c*sizeof(UINT16)))
			return Clear(), false;
		//Vertices
		if (!r.Get(&c))
			return Clear(), false;
		if (!Vertices.Alloc(c))
			return Clear(), false;
		if (!r.GetRaw(Vertices.Array(), c*sizeof(STerrainLod0Vtx)))
			return Clear(), false;
		//Render batches
		if (!r.Get(&c))
			return Clear(), false;
		if (!RenderBatches.Alloc(c))
			return Clear(), false;
		if (!r.GetRaw(RenderBatches.Array(), c*sizeof(SRenderBatch)))
			return Clear(), false;
		//Optimized draw
		if (!r.Get(&c))
			return Clear(), false;
		if (!OptimizedDrawTileInfo.Alloc(c))
			return Clear(), false;
		if (!r.GetRaw(OptimizedDrawTileInfo.Array(), c*sizeof(SOptimizedDrawTileInfo)))
			return Clear(), false;
		//USHorts
		if (!r.Get(&c))
			return Clear(), false;
		if (!UShorts.Alloc(c))
			return Clear(), false;
		if (!r.GetRaw(UShorts.Array(), c*sizeof(UINT16)))
			return Clear(), false;
		//Vectors
		if (!r.Get(&c))
			return Clear(), false;
		if (!Vectors.Alloc(c))
			return Clear(), false;
		if (!r.GetRaw(Vectors.Array(), c*sizeof(SChunkVec3)))
			return Clear(), false;
		//Occluders
		if (!r.Get(&c))
			return Clear(), false;
		if (!TileOccluderInfo.Alloc(c))
			return Clear(), false;
		if (!r.GetRaw(TileOccluderInfo.Array(), c*sizeof(STileOccluderInfo)))
			return Clear(), false;
		return true;
	}
};



class CChunk1{
public:
	#pragma pack(push, 1)
	typedef struct _SCNKHeader{
		UINT8	FourCC[4];
		UINT32	Version; //?
		UINT32	UncompressedSize;
		UINT32	CompressedSize;
	} SCNKHeader, *lpSCNKHeader;
	typedef struct _STerrainLod1Vtx{
		INT16	X;
		INT16	Y;
		INT16	FarHeight;
		INT16	NearHeight;
		UINT8	C0[4];
	} STerrainLod1Vtx, *lpSTerrainLod1Vtx;
	typedef struct _SRenderBatch{
		UINT32	IndexOffset;
		UINT32	IndexCount;
		UINT32	VertOffset;
		UINT32	VertCount;
	} SRenderBatch, *lpSRenderBatch;
	typedef struct _SChunkVec3{
		float	X;
		float	Y;
		float	Z;
	} SChunkVec3, *lpSChunkVec3;
	typedef struct _STileOccluderInfo{
		char Data[64];
	} STileOccluderInfo, *lpSTileOccluderInfo;
	typedef struct _SOptimizedDrawTileInfo{
		char Data[320];
	} SOptimizedDrawTileInfo, *lpSOptimizedDrawTileInfo;
	typedef struct _STex1Header{
		UINT32	DDSSize;
	} STex1Header, *lpSTex1Header;

	typedef struct _STex1Footer{
		UINT32	Count;
	} STex1Footer, *lpSTex1Footer;
	#pragma pack(pop)
	CDynMemoryBuffer**	m_ppTileColorNx;
	CDynMemoryBuffer**	m_ppTileSpecNy;
	UINT32				m_TextureCount;
	CDynMemoryBuffer	m_VertexData;
	UINT32				m_NumVerts;
	UINT32				m_VertsPerSide;
	TTypedArray<UINT16>					Indices;
	TTypedArray<STerrainLod1Vtx>		Vertices;
	TTypedArray<SRenderBatch>			RenderBatches;
	TTypedArray<SOptimizedDrawTileInfo>	OptimizedDrawTileInfo;
	TTypedArray<UINT16>					UShorts;
	TTypedArray<SChunkVec3>				Vectors;
	TTypedArray<STileOccluderInfo>		TileOccluderInfo;
public:
	inline CChunk1() : m_VertexData(8192), m_NumVerts(0), m_VertsPerSide(0), m_TextureCount(0), m_ppTileColorNx(NULL), m_ppTileSpecNy(NULL){
	}
	inline ~CChunk1(){
		Clear();
	}
	inline void Clear(){
		UINT32 i;
		for (i = 0; i < m_TextureCount; i++){
			if (m_ppTileColorNx && m_ppTileColorNx[i])
				delete m_ppTileColorNx[i];
			if (m_ppTileSpecNy && m_ppTileSpecNy[i])
				delete m_ppTileSpecNy[i];
		}
		if (m_ppTileColorNx)
			delete [] m_ppTileColorNx;
		if (m_ppTileSpecNy)
			delete [] m_ppTileSpecNy;
		m_ppTileColorNx = m_ppTileSpecNy = NULL;
		m_TextureCount = 0;
		m_VertexData.Clear(true);
		m_NumVerts = 0;
		m_VertsPerSide = 0;
		Indices.Clear();
		Vertices.Clear();
		RenderBatches.Clear();
		OptimizedDrawTileInfo.Clear();
		UShorts.Clear();
		Vectors.Clear();
		TileOccluderInfo.Clear();
	}
	inline CDynMemoryBuffer* GetTileColorNx(UINT32 i){
		return (m_ppTileColorNx && i < m_TextureCount) ? m_ppTileColorNx[i] : NULL;
	}
	inline CDynMemoryBuffer* GetTileSpecNy(UINT32 i){
		return (m_ppTileSpecNy && i < m_TextureCount) ? m_ppTileSpecNy[i] : NULL;
	}
	inline CDynMemoryBuffer *GetVertexData(){
		return &m_VertexData;
	}
	inline UINT32 GetNumVerts(){
		return m_NumVerts;
	}
	inline UINT32 GetVertsPerSide(){
		return m_VertsPerSide;
	}
	inline UINT32 GetNumTextures(){
		return m_TextureCount;
	}
	bool Load(const char* pPath){
		Clear();
		lzham_z_stream s;
		int res;
		UINT32 i, j, c;
		CDynMemoryBuffer fbuf(8192);
		CDynMemoryBuffer dbuf(8192);
		SCNKHeader hdr;
		STex1Header thdr;
		STex1Footer tftr;
		if (!LoadFileToBuffer(pPath, fbuf))
			return false;
		if (fbuf.GetSize() < sizeof(SCNKHeader))
			return false;
		memcpy(&hdr, fbuf.GetPtr(), sizeof(SCNKHeader));
		if (memcmp(hdr.FourCC, "CNK1", 4) != 0)
			return false;
		if (!dbuf.Alloc(hdr.UncompressedSize))
			return false;
		if (fbuf.GetSize() != sizeof(SCNKHeader)+hdr.CompressedSize)
			return false;
		lzham_z_inflateInit2(&s, 20);
		s.avail_in = hdr.CompressedSize;
		s.avail_out = hdr.UncompressedSize;
		s.next_in = (unsigned char*)fbuf.GetBytePtr()+sizeof(SCNKHeader);
		s.next_out = (unsigned char*)dbuf.GetBytePtr();
		res = lzham_z_inflate(&s, LZHAM_Z_FINISH);
		lzham_z_inflateEnd(&s);
		if (res != LZHAM_Z_STREAM_END)
			return false;
		CDynMemoryBuffer::CDynMemoryReader r(&dbuf);
		if (!r.Get(&m_TextureCount))
			return false;
		m_ppTileColorNx = new CDynMemoryBuffer*[m_TextureCount];
		if (!m_ppTileColorNx)
			return false;
		memset(m_ppTileColorNx, 0, sizeof(CDynMemoryBuffer*)*m_TextureCount);
		m_ppTileSpecNy = new CDynMemoryBuffer*[m_TextureCount];
		if (!m_ppTileSpecNy)
			return Clear(), false;
		memset(m_ppTileSpecNy, 0, sizeof(CDynMemoryBuffer*)*m_TextureCount);
		for (i = 0; i < m_TextureCount; i++){
			if (!(m_ppTileColorNx[i] = new CDynMemoryBuffer(8192)))
				return Clear(), false;
			if (!(m_ppTileSpecNy[i] = new CDynMemoryBuffer(8192)))
				return Clear(), false;

			if (!r.Get(&thdr))
				return Clear(), false;
			if (!m_ppTileColorNx[i]->Alloc(thdr.DDSSize))
				return Clear(), false;
			if (!r.GetRaw(m_ppTileColorNx[i]->GetBytePtr(), thdr.DDSSize))
				return Clear(), false;

			if (!r.Get(&thdr))
				return Clear(), false;
			if (!m_ppTileSpecNy[i]->Alloc(thdr.DDSSize))
				return Clear(), false;
			if (!r.GetRaw(m_ppTileSpecNy[i]->GetBytePtr(), thdr.DDSSize))
				return Clear(), false;

			for (j = 0; j < 4; j++){
				if (!r.Get(&tftr))
					return Clear(), false;
				if (!r.GetPtr(tftr.Count))
					return Clear(), false;
			}
		}
		if (!r.Get(&m_VertsPerSide))
			return Clear(), false;
		//Raw height data?
		if (!r.Get(&c))
			return Clear(), false;
		if (!r.SeekCur(c*sizeof(UINT32)))
			return Clear(), false;
		//Indices
		if (!r.Get(&c))
			return Clear(), false;
		if (!Indices.Alloc(c))
			return Clear(), false;
		if (!r.GetRaw(Indices.Array(), c*sizeof(UINT16)))
			return Clear(), false;
		//Vertices
		if (!r.Get(&c))
			return Clear(), false;
		if (!Vertices.Alloc(c))
			return Clear(), false;
		if (!r.GetRaw(Vertices.Array(), c*12))
			return Clear(), false;
		//Render batches
		if (!r.Get(&c))
			return Clear(), false;
		if (!RenderBatches.Alloc(c))
			return Clear(), false;
		if (!r.GetRaw(RenderBatches.Array(), c*sizeof(SRenderBatch)))
			return Clear(), false;
		//Optimized draw
		if (!r.Get(&c))
			return Clear(), false;
		if (!OptimizedDrawTileInfo.Alloc(c))
			return Clear(), false;
		if (!r.GetRaw(OptimizedDrawTileInfo.Array(), c*sizeof(SOptimizedDrawTileInfo)))
			return Clear(), false;
		//USHorts
		if (!r.Get(&c))
			return Clear(), false;
		if (!UShorts.Alloc(c))
			return Clear(), false;
		if (!r.GetRaw(UShorts.Array(), c*sizeof(UINT16)))
			return Clear(), false;
		//Vectors
		if (!r.Get(&c))
			return Clear(), false;
		if (!Vectors.Alloc(c))
			return Clear(), false;
		if (!r.GetRaw(Vectors.Array(), c*sizeof(SChunkVec3)))
			return Clear(), false;
		//Occluders
		if (!r.Get(&c))
			return Clear(), false;
		if (!TileOccluderInfo.Alloc(c))
			return Clear(), false;
		if (!r.GetRaw(TileOccluderInfo.Array(), c*sizeof(STileOccluderInfo)))
			return Clear(), false;
		return true;
	}
};
````