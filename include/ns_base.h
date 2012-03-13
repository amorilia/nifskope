/***** BEGIN LICENSE BLOCK *****

BSD License

Copyright (c) 2005-2012, NIF File Format Library and Tools
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the NIF File Format Library and Tools project may not be
   used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***** END LICENCE BLOCK *****/

#ifndef __NS_BASE_H__
#define __NS_BASE_H__

#define STR_NOTHING "None"

#define NF_VSEPARATOR "."
#define NF_V03030013 0x0303000D
#define NF_V20000005 0x14000005
#define NF_V10000100 0x0A000100
#define NF_V20020000 0x14020000
#define NF_V20010003 0x14010003
#define NF_V10010000 0x0A010000
#define NF_V10020000 0x0A020000
#define NF_V10000000 0x0A000000
#define NF_V20000004 0x14000004
#define NF_V20050000 0x14050000
#define NF_V20020007 0x14020007
#define NF_V04010012 0x04010012
#define NF_V04000000 0x04000000

#define NF_H1 "NetImmerse File Format, Version "
#define NF_H2 "Gamebryo File Format, Version "
#define NF_V20000005_DEFAULT_UV 11
#define NF_V20000005_DEFAULT_UV2 11
#define NF_V03030013_DEF_CR1 "Numerical Design Limited, Chapel Hill, NC 27514"
#define NF_V03030013_DEF_CR2 "Copyright (c) 1996-2000"
#define NF_V03030013_DEF_CR3 "All Rights Reserved"

#define FMASK_BSA  "*.bsa"
#define FMASK_FRAG "*.frag"
#define FMASK_PROG "*.prog"
#define FMASK_VERT "*.vert"
#define FMASK_KF   "*.kf"
#define FMASK_KFA  "*.kfa"
#define FMASK_KF_A "*.kf(a)"
#define FMASK_KFM  "*.kfm"
#define FMASK_NIF  "*.nif"
#define FMASK_TGA  "*.tga"
#define FMASK_DDS  "*.dds"
#define FMASK_BMP  "*.bmp"
#define FMASK_PNG  "*.png"
#define FMASK_JPG  "*.jpg"
#define FMASK_NIFCACHE "*.nifcache"
#define FMASK_TEXCACHE "*.texcache"
#define FMASK_PCPATCH "*.pcpatch"
#define FMASK_JMI "*.jmi"

// XML attribute values
#define AV_ARG "ARG"
#define AV_TEMPLATE "TEMPLATE"
#define AV_SEMANTICDATA_TEXTCOORD "TEXCOORD"
#define AV_SEMANTICDATA_POSITION "POSITION"
#define AV_SEMANTICDATA_NORMAL "NORMAL"
#define AV_SEMANTICDATA_INDEX "INDEX"
#define AV_FLOAT32_3 "F_FLOAT32_3"
#define AV_MESHPRMITIVETYPE_TRIANGLES "MESH_PRIMITIVE_TRIANGLES"
// T_BSLIGHTINGSHADERPROPERTY, T_BSEFFECTSHADERPROPERTY
//   TA_SHADERFLAGS2
#define AV_VERTEX_ANIMATION 29
#define AV_DOUBLE_SIDED 4

// XML "tag" "tag add"
#define T_ATEXTURERENDERDATA "ATextureRenderData"
#define T_NIDATASTREAM "NiDataStream"
	#define TA_NIDS_USAGE "Usage"
	#define TA_NIDS_ACCESS "Access"
#define T_HEADER "Header"
	#define TA_HVERSION "Version"
	#define TA_HSTRING "Header String"
	#define TA_HUV "User Version"
	#define TA_HUV2	"User Version 2"
	#define TA_HCR "Copyright"
	#define TA_HBLOCKSNUM "Num Blocks"
	#define TA_HBLOCKTYPES "Block Types"
	#define TA_HBLOCTYPEIDX "Block Type Index"
	#define TA_HBLOCKSIZE "Block Size"
	#define TA_HBLOCKTYPESNUM "Num Block Types"
	#define TA_HSTRINGSNUM "Num Strings"
	#define TA_HSTRINGS "Strings"
	#define TA_HSTRINGMAXLEN "Max String Length"
#define T_FOOTER "Footer"
	#define TA_FROOTS "Roots"
	#define TA_FROOTSNUM "Num Roots"

#define T_NISOURCETEXTURE "NiSourceTexture"
#define T_NIIMAGE "NiImage"
#define T_NISTRINGEXTRADATA "NiStringExtraData"
#define T_CONTROLLERLINK "ControllerLink"
#define T_NIFLOATINTERPOLATOR "NiFloatInterpolator"
#define T_NIFLOATDATA "NiFloatData"
#define T_NITRISHAPEDATA "NiTriShapeData"
#define T_NITRISTRIPSDATA "NiTriStripsData"
#define T_NISKININSTANCE "NiSkinInstance"
#define T_NIGEOMMORPHERCONTROLLER "NiGeomMorpherController"
#define T_NIUVCONTROLLER "NiUVController"
#define T_NITRIBASEDGEOM "NiTriBasedGeom"
#define T_NIBINARYEXTRADATA "NiBinaryExtraData"
#define T_NISKINDATA "NiSkinData"
#define T_NISKINPARTITION "NiSkinPartition"
#define T_BSLIGHTINGSHADERPROPERTY "BSLightingShaderProperty"
#define T_BSEFFECTSHADERPROPERTY "BSEffectShaderProperty"
#define T_NIALPHAPROPERTY "NiAlphaProperty"
#define T_NIZBUFFERPROPERTY "NiZBufferProperty"
#define T_NITEXTURINGPROPERTY "NiTexturingProperty"
#define T_NITEXTUREPROPERTY "NiTextureProperty"
#define T_NIMATERIALPROPERTY "NiMaterialProperty"
#define T_NISPECULARPROPERTY "NiSpecularProperty"
#define T_NIWIREFRAMEPROPERTY "NiWireframeProperty"
#define T_NIVERTEXTCOLORPROPERTY "NiVertexColorProperty"
#define T_NISTENCILPROPERTY "NiStencilProperty"
#define T_BSSHADERLIGHTINGPROPERTY "BSShaderLightingProperty"
#define T_BSSHADERNOLIGHTINGPROPERTY "BSShaderNoLightingProperty"
#define T_BSSHADERPPLIGHINGPROPERTY "BSShaderPPLightingProperty"
#define T_NIFLIPCONTROLLER "NiFlipController"
#define T_NITEXTURETRANSFORMCONTROLLER "NiTextureTransformController"
#define T_NIALPHACONTROLLER "NiAlphaController"
#define T_NIMATERIALCOLORCONTROLLER "NiMaterialColorController"
#define T_BSSHADERTEXTURESET "BSShaderTextureSet"

#define TA_FILENAME "File Name"
#define TA_STRINGDATA "String Data"
#define TA_NAME "Name"
#define TA_CONTROLLEDBLOCKS "Controlled Blocks"
#define TA_NODENAME "Node Name"
#define TA_NODENAMEOFFSET "Node Name Offset"
#define TA_STRINGPALETTE "String Palette"
#define TA_PALETTE "Palette"
#define TA_NUMVERTICES "Num Vertices"
#define TA_TEXTURESOURCE "Texture Source"
#define TA_INDEX "Index"
#define TA_STRING "String"
#define TA_MORPHS "Morphs"
#define TA_INTERPOLATOR "Interpolator"
#define TA_INTERPOLATORS "Interpolators"
#define TA_INTERPOLATORWEIGHTS "Interpolator Weights"
#define TA_DATA "Data"
#define TA_VECTORS "Vectors"
#define TA_UVGROUPS "UV Groups"
#define TA_NUMSUBMESHES "Num Submeshes"
#define TA_DATAS "Datas"
#define TA_PRIMITIVETYPE "Primitive Type"
#define TA_NUMREGIONS "Num Regions"
#define TA_REGIONS "Regions"
#define TA_STARTINDEX "Start Index"
#define TA_NUMINDICES "Num Indices"
#define TA_NUMCOMPONENTS "Num Components"
#define TA_COMPONENTFORMATS "Component Formats"
#define TA_PROPERTIES "Properties"
#define TA_SHADERFLAGS2 "Shader Flags 2"
#define TA_VERTICES "Vertices"
#define TA_NORMALS "Normals"
#define TA_VERTEXCOLORS "Vertex Colors"
#define TA_TANGENTS "Tangents"
#define TA_BINORMALS "Binormals"
#define TA_UVSETS "UV Sets"
#define TA_UVSETS2 "UV Sets 2"
#define TA_TRIANGLES "Triangles"
#define TA_POINTS "Points"
#define TA_EXTRADATALIST "Extra Data List"
#define TA_BINARYDATA "Binary Data"
#define TA_SKELETONROOT "Skeleton Root"
#define TA_BONES "Bones"
#define TA_BONELIST "Bone List"
#define TA_HASVERTEXWEIGHTS "Has Vertex Weights"
#define TA_SKINPARTITION "Skin Partition"
#define TA_SKINPARTITIONBLOCKS "Skin Partition Blocks"
#define TA_STRIPS "Strips"
#define TA_STRIPLENGTHS "Strip Lengths"
#define TA_FLAGS "Flags"
#define TA_THRESHOLD "Threshold"
#define TA_FUNCTION "Function"
#define TA_BASETEXTURE "Base Texture"
#define TA_DARKTEXTURE "Dark Texture"
#define TA_DETAILTEXTURE "Detail Texture"
#define TA_GLOSSTEXTURE "Gloss Texture"
#define TA_GLOWTEXTURE "Glow Texture"
#define TA_BUMPMAPTEXTURE "Bump Map Texture"
#define TA_DECAL0TEXTURE "Decal 0 Texture"
#define TA_DECAL1TEXTURE "Decal 1 Texture"
#define TA_DECAL2TEXTURE "Decal 2 Texture"
#define TA_DECAL3TEXTURE "Decal 3 Texture"
#define TA_SOURCE "Source"
#define TA_UVSET "UV Set"
#define TA_FILTERMODE "Filter Mode"
#define TA_CLAMPMODE "Clamp Mode"
#define TA_HASTEXTURETRANSFORM "Has Texture Transform"
#define TA_TRANSLATION "Translation"
#define TA_TILING "Tiling"
#define TA_WROTATION "W Rotation"
#define TA_CENTEROFFSET "Center Offset"
#define TA_DELTA "Delta"
#define TA_TEXTURESLOT "Texture Slot"
#define TA_SOURCES "Sources"
#define TA_IMAGES "Images"
#define TA_OPERATION "Operation"
#define TA_IMAGE "Image"
#define TA_ALPHA "Alpha"
#define TA_AMBIENTCOLOR "Ambient Color"
#define TA_DIFFUSECOLOR "Diffuse Color"
#define TA_SPECULARCOLOR "Specular Color"
#define TA_EMISSIVECOLOR "Emissive Color"
#define TA_GLOSSINESS "Glossiness"
#define TA_TARGETCOLOR "Target Color"
#define TA_VERTEXMODE "Vertex Mode"
#define TA_LIGHTINGMODE "Lighting Mode"
#define TA_DRAWMODE "Draw Mode"
#define TA_TEXTURESET "Texture Set"
#define TA_NUMTEXTURES "Num Textures"
#define TA_TEXTURES "Textures"
#define TA_SOURCETEXTURE "Source Texture"

// NiBlocks - artificial blocks
#define B_NIHEADER "NiHeader"
#define B_NIFOOTER "NiFooter"
#define B_NIBLOCK "NiBlock"

// XML type names
#define TN_STRING "string"
#define TN_MESHPRIMITIVETYPE "MeshPrimitiveType"
#define TN_COMPONENTFORMAT "ComponentFormat"

// display names - column headers
#define DN_NAME "Name"
#define DN_TYPE "Type"
#define DN_VAL "Value"
#define DN_ARG "Argument"
#define DN_ARR1 "Array1"
#define DN_ARR2 "Array2"
#define DN_COND "Condition"
#define DN_VER1 "since"
#define DN_VER2 "until"
#define DN_VERCOND "Version Condition"

// texture names - inetrnal use
#define TX_BASE "base"
#define TX_DARK "dark"
#define TX_DETAIL "detail"
#define TX_GLOSS "gloss"
#define TX_GLOW "glow"
#define TX_BUMPMAP "bumpmap"
#define TX_DECAL0 "decal0"
#define TX_DECAL1 "decal1"
#define TX_DECAL2 "decal2"
#define TX_DECAL3 "decal3"

#endif /* __NS_BASE_H__ */
