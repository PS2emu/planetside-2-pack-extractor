ECO files seem to be used for storing information regarding the automated flora and clutter generation for the specified materials.
  Examples:
* indar_ocean_redsand.eco
* indar_road_highlands.eco

## Structure
### Texturepart
    *TEXTUREPART
	*SOURCE_COLOR_BLEND_MAP	[Filename.dds]
	*SOURCE_NORMAL_MAP	[Filename.dds]
	*SOURCE_SPEC_MAP	[Filename.dds]
	*RENDER_COLOR_NX_MAP	[Filename.dds]
	*RENDER_SPEC_BLEND_NY_MAP	[Filename.dds]
	*DETAILREPEAT	[Integer]
	*BLENDSTRENGTH	[Float]
	*SPECMIN	[Float]
	*SPECMAX	[Float]
	*SPECSMOOTHNESSMIN	[Float]
	*SPECSMOOTHNESSMAX	[Float]
	*PHYSICS_MATERIAL	[String]
    *END_TEXTUREPART   
### Layerpart (can be empty)
    *LAYERPART
    *LAYER	[Layername]
		*FLORA	[Name(String)]
		*TEXTURELAYER	[Filename w/o .eco]
		*DENSITY	[Float]
		*MINSCALE	[Float]
		*MAXSCALE	[Float]
		*SLOPEPEAK	[Float]
		*SLOPEEXTENT	[Float]
		*MINELEVATION	[Float]
		*MAXELEVATION	[Float]
		*MINALPHA	[Integer]
		*CLUMPMASK	[String e.g. "spotty02.bmp"]
		*CLUMPMASKPATH	[Filepath in relation to the studio Fileserver: e.g."X:\Planetside2\Main\Runtime\TerrainEditor\Resources\flora_alpha\"]
		*CLUMPMASKSCALE	[Float]
		*CLUMPMASKUOFFSET	[Integer]
		*CLUMPMASKVOFFSET	[Integer]
		*TINT	[Integer]	[Integer]
		[additional tints]
	*END_LAYER
        [Additional layers]
    *END_LAYERPART