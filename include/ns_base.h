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

// XML tags
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

// NiBlocks - artificial blocks
#define B_NIHEADER "NiHeader"
#define B_NIFOOTER "NiFooter"
#define B_NIBLOCK "NiBlock"

// XML type names
#define TN_STRING "string"

#endif /* __NS_BASE_H__ */
