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

#include "ns_base.h"

#include "blocks.h"
#include "texture.h"

#include "spellbook.h"
#include "gl/gltex.h"

#include "config.h"

#include "widgets/fileselect.h"
#include "widgets/uvedit.h"

#include "NvTriStrip/qtwrapper.h"

#include <QGLPixelBuffer>

#include <QComboBox>
#include <QDialog>
#include <QDirModel>
#include <QImage>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QTreeView>

// Brief description is deliberately not autolinked to class Spell
/*! \file texture.cpp
 * \brief Texturing spells
 *
 * All classes here inherit from the Spell class.
 */

/* XPM */
static char const * tex42_xpm[] = {
	"80 80 43 1",
	" 	c None",
	".	c #29194A","+	c #2D1D37","@	c #2F2528","#	c #333018",
	"$	c #3C2D66","%	c #50521A","&	c #5B4E3C","*	c #5B4286",
	"=	c #544885","-	c #584B70",";	c #64589D",">	c #67710F",
	",	c #7558A4","'	c #6A619A",")	c #68658B","!	c #7264AA",
	"~	c #7E78A5","{	c #8173B8","]	c #8274B1","^	c #8276AD",
	"/	c #8A72C0","(	c #998F0F","_	c #83819C",":	c #908F97",
	"<	c #9F80D1","[	c #9589BD","}	c #9885CE","|	c #9986C7",
	"1	c #9D9E9D","2	c #9F9AB4","3	c #A992DC","4	c #B38FDD",
	"5	c #AC9AD0","6	c #AF97DA","7	c #C2BD04","8	c #C1A1DC",
	"9	c #BEA9DD","0	c #BABBBB","a	c #C1B8D2","b	c #CDB7DC",
	"c	c #D5CAE0","d	c #D6D4D7",
	"                                                                                ",
	"                                                                                ",
	"                                                                                ",
	"                                                                                ",
	"                                                                                ",
	"                                             d   2                              ",
	"                                             aaa ~c d                           ",
	"                            dd              b_~2a!)~0                           ",
	"                           cdcc            a6{'~[]'')0                          ",
	"                          cdada9c         dc5^'^]];''~0                         ",
	"                         53bba05[1        dc5{]|{|'']|20                        ",
	"                        ca5}aaab}~0d     cdc5]{|||''|6~10                       ",
	"                        bb2<50ab|{_0     cdc}}|]|['{[}[10                       ",
	"                        b5b|<a0b|]':0    cda3|{{5{{}53|:1d                      ",
	"                         [b[<5ca5]'~1d  cccb66{^6{{[3}|_10                      ",
	"                         |55/|aa5/'':0  bdca65{|9{{5}||_:0                      ",
	"                         555|<509|!;)1dcbccb93]55{|3}}5_11                      ",
	"                         b}5|<]ab5{!':0[bccb9}]99{35}}|_11                      ",
	"                          <5|</5a5/]!)1]ccbb9{]c3}3}}[^:11                      ",
	"                          5<5|/,b5}!!')'bcbb9{5b|}|}}|{:11                      ",
	"                          a<5|}/[5|}!]''bbb96{a933|}[{~110                      ",
	"                           <|8//!56{;!''9bb8|[c5}5}}]{_1:0                      ",
	"                           9}64/{/8}!!;=59b3}bc3|3{|^^:110                      ",
	"            [{/}333369b    c}}6|!,|}!;]=]883{ab353]{|~1:0d       cad            ",
	"        [2 !!{}<<433}}<}5   96<<]!!6,;!=]68}|c633|{^^:110    c9['{;'~           ",
	"          ^;;{/}}<43<<//{/2 d6||<',]{;'==63]5b536|]]_111d cb|||{{{''-0d         ",
	"           2!!{{}}<<<</{/{{{c9|||'=,];='=|<]99}6|{[~2110c963}{}}{]{')11a        ",
	"           ]';;!{{{}}<//!,!{'_[]/]'=,'**-]]]8653[|^:12566333}}]{{''''^{~        ",
	"            {{!!!!{{/{{/!,;;!'^[]'====$*$!'/6|3||~_:[966333}}{{]]]{'{'~0d       ",
	"             ~{'!!!!{!{!{,;=='=]]]=-=*$=-=*]<6|3|^29b83444<}}{{{'{{{{{]_20      ",
	"             2'{'{!!!!!;!!;'=$==]'=*.*$$$-*'/|66|bcb68643}}{]{{{}}{}{}}}_0      ",
	"              ~''';;!,;!='';==-$='-=...$$+$,<45bcc99883<3}|{{{}3}333}}3|:10     ",
	"              2'''!;!!!;;;='==$$$=$$.+..++*]/8bbbbcb8433}{}3333333333}~:110     ",
	"               '''!{{!!!;;=====$..-$..+$@&*,<888cdc846}/}<33333365965~:111d     ",
	"               '''{{}{;!';;==$$$....+@@+%&$,<<4bbb88<464333333396995_1110d      ",
	"                ^{{}{]{;;;;===-...+++###>#-**<44884488434<<<333699[11110        ",
	"               96}{||{]'';===$$$.+@##%>%>&@**,,4bb88444}}}}366352:11110         ",
	"            [|33363}{{{!'====$$..+#>>>>>>((@-,<4b888444<<3436|_211110           ",
	"         [{{/}34464}}{!;'===$$$$.+#%>>>(((7-*,4888888643}}]!')_1110d            ",
	"      a]{{}}}3368633{/''==$$$...+#%>>((7(%%@'<8884/||<]!*====-=):d              ",
	"     [}}/}/}366633<//!;===$...++#%>>>7777(&&&--]]]=*====;!;;;==='_d             ",
	"     |3}333336334<<{!!;=$$$$....+@#%(777((&---$-*-==;*;=*;{;;;';;;)2            ",
	"    63633333433<}{/!;;'=='==$$...+@%>(77(7&*/4]**,**=*=;!///{!!;';;;)c          ",
	"    6966333<3<<}{{/{{!!!=====*$$.+@##&((&&@-,<48/,]/!!;;,/{/{{{{{{{]!'2         ",
	"    93|}}}3}}//}<{}{/!'!;';;;*$$..++@@((-***,<484<<}<<//,,!!{}{{{}}}|{!2d       ",
	"    5]{}3//{/{}/|//'!]{'!;;;,*$$.....@%&-****488444<////{!!;,;{{{|||66|^2d      ",
	"    2~!!!{/{]{]{{]]]{]!!!//,***$....$+@@*,,</<884444<////{!!!;!;'!{{|||~)1d     ",
	"     2;{'']''!]]]]!]!!!!//,,,*$.$$$*$$+$,/</464894446</!!!/!;,;;!'!'']^2:11d    ",
	"      )'''''''''!]!]!!/////!,*$**$*$$*$$,]4<8b84888444</!!!!!;;;';'''''_~10d    ",
	"        _)-)-)''~''']<<///,;********$***/<6b4cc848863<<</,;;;;;;=';=='-:::ad    ",
	"          11::1:111[63<<<{,==;,;=;=*****{6<bbbcb6<888<<<</;;;=;;=;'''')::0dd    ",
	"                  23}/}}/!;;!!;;;;=;!;;,/8|5c8bc9448844<///'='==';='=):110      ",
	"                 9|3<}}/{;;///!!]=={;;!|<b|]db8bb84/8634<{{{='=)=))):1:11d      ",
	"                933/{}{{;!/<}/{{;=}/!!,|498/)cb4b864}863}/}{^))__:111110d       ",
	"                |}}}{{{'{}}}{{}'='3/{{!8458^'2c888834<663<3}|[11111000          ",
	"               ]]|{{{{'{}33<}}''=36{/!'9689,-_28888866}66333|6:1d               ",
	"              ~{{]{]]'||}3}}}{')]63}//]9689{=_:b868864}}666653910               ",
	"             a]]]]''{|333}}3{]_)58|}}{]886b!--1:8638863}}999955a1d              ",
	"             ~'^'']]3363336|^):[966}}{]9668{'-11a8|}8666}399bb9a10              ",
	"            a')''']|666636]]_1:9b6}|3{|b469/-)11dc6||666}|69bcbc01d             ",
	"            )__)']}636686|^_112c84|63{]b845/=)11d c[|||93|3699ccc10             ",
	"              c']|6999966~:1112c86}8|}|b638{--11d  d[||69|}5898bb11d            ",
	"              ~']5599b95_11110bc84}84}|c935{')11d    a||65|]3665[11d            ",
	"             a~^5959992:1:10d bb83356}|b969!-)11d     c[|6||_2521:10            ",
	"               c53652:11100   a96}3866|c969{')11d       a[[_:1:2200             ",
	"                 a2:1111d     c53|66636cb9b{'-11d        d01110d2               ",
	"                  d0100       d6||58666cbbb{')11d          ddd                  ",
	"                               [||666b6cc9c|')11d                               ",
	"                               [||653c8ccbd5'_11d                               ",
	"                               a||663bbcdbc9')11d                               ",
	"                               c]]|53bbcdcc9^_11d                               ",
	"                                []|3599cccc5~:10d                               ",
	"                                d^]|[^69ccc9_:11                                ",
	"                                 a~||)56bcd[:11d                                ",
	"                                   a[)26bbc:111                                 ",
	"                                    a11[ac2111d                                 ",
	"                                      d[a11110                                  ",
	"                                       a d000                                   ",
	"                                       d  d                                     ",
	"                                                                                "};

QIcon * tex42_xpm_icon = 0;

//! Find the reference to shape data from a model and index
QModelIndex getData( const NifModel * nif, const QModelIndex & index )
{
	if ( nif->isNiBlock( index, "NiTriShape" ) || nif->isNiBlock( index, "NiTriStrips" ) )
		return nif->getBlock( nif->getLink( index, TA_DATA ) );
	else if ( nif->isNiBlock( index, T_NITRISHAPEDATA ) || nif->isNiBlock( index, T_NITRISTRIPSDATA ) )
		return index;
	return QModelIndex();
}

//! Find the reference to UV data from a model and index
QModelIndex getUV( const NifModel * nif, const QModelIndex & index )
{
	QModelIndex iData = getData( nif, index );

	if ( iData.isValid() )
	{
		QModelIndex iUVs = nif->getIndex( iData, TA_UVSETS );
		return iUVs;
	}
	return QModelIndex();
}

//! Selects a texture filename
class spChooseTexture : public Spell
{
public:
	QString name() const { return Spell::tr("Choose"); }
	QString page() const { return Spell::tr("Texture"); }
	bool instant() const { return true; }
	QIcon icon() const
	{
		if ( ! tex42_xpm_icon ) tex42_xpm_icon = new QIcon( tex42_xpm );
		return *tex42_xpm_icon;
	}

	bool isApplicable( const NifModel * nif, const QModelIndex & idx )
	{
		QModelIndex iBlock = nif->getBlock( idx );
		if (  ( nif->isNiBlock( iBlock, T_NISOURCETEXTURE ) || nif->isNiBlock( iBlock, T_NIIMAGE ) )
			&& ( iBlock == idx.sibling( idx.row(), 0 ) || nif->itemName( idx ) == TA_FILENAME ) )
			return true;
		else if ( nif->isNiBlock( iBlock, "BSShaderNoLightingProperty" ) && nif->itemName( idx ) == TA_FILENAME )
			return true;
		else if ( nif->isNiBlock( iBlock, "BSShaderTextureSet" ) && nif->itemName( idx ) == "Textures" )
			return true;
		else if ( nif->isNiBlock( iBlock, "SkyShaderProperty" ) && nif->itemName( idx ) == TA_FILENAME )
			return true;
		else if ( nif->isNiBlock( iBlock, "TileShaderProperty" ) && nif->itemName( idx ) == TA_FILENAME )
			return true;
		return false;
	}

	QModelIndex cast( NifModel * nif, const QModelIndex & idx )
	{
		QModelIndex iBlock = nif->getBlock( idx );
		QModelIndex iFile;
		bool setExternal = false;

		if (  ( nif->isNiBlock( iBlock, T_NISOURCETEXTURE ) || nif->isNiBlock( iBlock, T_NIIMAGE ) )
			&& ( iBlock == idx.sibling( idx.row(), 0 ) || nif->itemName( idx ) == TA_FILENAME ) )
		{
			iFile = nif->getIndex( iBlock, TA_FILENAME );
			setExternal = true;
		}
		else if ( nif->isNiBlock( iBlock, "BSShaderTextureSet" ) && nif->itemName( idx ) == "Textures" )
			iFile = idx;
		else if ( nif->isNiBlock( iBlock, "BSShaderNoLightingProperty" ) && nif->itemName( idx ) == TA_FILENAME )
			iFile = idx;
		else if ( nif->isNiBlock( iBlock, "SkyShaderProperty" ) && nif->itemName( idx ) == TA_FILENAME )
			iFile = idx;
		else if ( nif->isNiBlock( iBlock, "TileShaderProperty" ) && nif->itemName( idx ) == TA_FILENAME )
			iFile = idx;

		if (!iFile.isValid())
			return idx;

		QString file = TexCache::find( nif->get<QString>( iFile ), nif->getFolder() );

		if ( ! QFile::exists(file) )
		{
			// if file not found in cache, use last texture path
			NIFSKOPE_QSETTINGS(cfg);
			QString defaulttexpath(cfg.value("last texture path", QVariant(QDir::homePath())).toString());
			//file = QDir(defaulttexpath).filePath(file);
			file = defaulttexpath;
		}


		// to avoid shortcut resolve bug take *.* as text filter
		file = QFileDialog::getOpenFileName( 0, "Select a texture file", file, "*.*" );

		if ( ! file.isEmpty() )
		{
			// save path for future
			NIFSKOPE_QSETTINGS(cfg);
			cfg.setValue("last texture path", QVariant(QDir(file).absolutePath()));

			file = TexCache::stripPath( file, nif->getFolder() );
			if (setExternal)
			{
				nif->set<int>( iBlock, "Use External", 1 );
				// update the TA_FILENAME block reference, since it changes when we set Use External
				if ( nif->checkVersion( NF_V10010000, 0 ) && nif->isNiBlock( iBlock, T_NISOURCETEXTURE )  )
				{
					iFile = nif->getIndex( iBlock, TA_FILENAME );
				}
			}
			nif->set<QString>( iFile, file.replace( "/", "\\" ) );
		}
		return idx;
	}
};

REGISTER_SPELL( spChooseTexture )

//! Opens a UVWidget to edit texture coordinates
class spEditTexCoords : public Spell
{
public:
	QString name() const { return Spell::tr("Edit UV"); }
	QString page() const { return Spell::tr("Texture"); }

	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return ( nif->itemName(index) == "NiTriShape" || nif->itemName(index) == "NiTriStrips" );

		//QModelIndex iUVs = getUV( nif, index );
		//return iUVs.isValid() && nif->rowCount( iUVs ) >= 1;
	}

	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		UVWidget::createEditor( nif, index );
		return index;
	}
};

REGISTER_SPELL( spEditTexCoords )

//! Add a texture to the specified texture slot
/*!
 * \param nif The model
 * \param index The index of the mesh
 * \param name The name of the texture slot
 */
QModelIndex addTexture( NifModel * nif, const QModelIndex & index, const QString & name )
{
	QModelIndex iTexProp = nif->getBlock( index, "NiTexturingProperty" );
	if ( ! iTexProp.isValid() )	return index;
	if ( nif->get<int>( iTexProp, "Texture Count" ) < 7 )
		nif->set<int>( iTexProp, "Texture Count", 7 );

	nif->set<int>( iTexProp, QString( "Has %1" ).arg( name ), 1 );
	QPersistentModelIndex iTex = nif->getIndex( iTexProp, name );
	if ( ! iTex.isValid() ) return index;

	nif->set<int>( iTex, "Clamp Mode", 3 );
	nif->set<int>( iTex, "Filter Mode", 3 );
	nif->set<int>( iTex, "PS2 K", -75 );
	nif->set<int>( iTex, "Unknown1", 257 );

	QModelIndex iSrcTex = nif->insertNiBlock( T_NISOURCETEXTURE, nif->getBlockNumber( iTexProp ) + 1 );
	nif->setLink( iTex, "Source", nif->getBlockNumber( iSrcTex ) );

	nif->set<int>( iSrcTex, "Pixel Layout", ( nif->getVersion() == "20.0.0.5" && name == "Base Texture" ? 6 : 5 ) );
	nif->set<int>( iSrcTex, "Use Mipmaps", 2 );
	nif->set<int>( iSrcTex, "Alpha Format", 3 );
	nif->set<int>( iSrcTex, "Unknown Byte", 1 );
	nif->set<int>( iSrcTex, "Unknown Byte 2", 1 );
	nif->set<int>( iSrcTex, "Use External", 1 );

	spChooseTexture * chooser = new spChooseTexture();
	return chooser->cast( nif, iSrcTex );
}

//! Adds a Base texture
class spAddBaseMap : public Spell
{
public:
	QString name() const { return Spell::tr("Add Base Texture"); }
	QString page() const { return Spell::tr("Texture"); }

	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		QModelIndex block = nif->getBlock( index, "NiTexturingProperty" );
		return ( block.isValid() && nif->get<int>( block, "Has Base Texture" ) == 0 ); 
	}

	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		return addTexture( nif, index, "Base Texture" );
	}
};

REGISTER_SPELL( spAddBaseMap )

//! Adds a Dark texture
class spAddDarkMap : public Spell
{
public:
	QString name() const { return Spell::tr("Add Dark Map"); }
	QString page() const { return Spell::tr("Texture"); }

	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		QModelIndex block = nif->getBlock( index, "NiTexturingProperty" );
		return ( block.isValid() && nif->get<int>( block, "Has Dark Texture" ) == 0 ); 
	}

	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		return addTexture( nif, index, "Dark Texture" );
	}
};

REGISTER_SPELL( spAddDarkMap )

//! Adds a Detail texture
class spAddDetailMap : public Spell
{
public:
	QString name() const { return Spell::tr("Add Detail Map"); }
	QString page() const { return Spell::tr("Texture"); }

	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		QModelIndex block = nif->getBlock( index, "NiTexturingProperty" );
		return ( block.isValid() && nif->get<int>( block, "Has Detail Texture" ) == 0 ); 
	}

	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		return addTexture( nif, index, "Detail Texture" );
	}
};

REGISTER_SPELL( spAddDetailMap )

//! Adds a Glow texture
class spAddGlowMap : public Spell
{
public:
	QString name() const { return Spell::tr("Add Glow Map"); }
	QString page() const { return Spell::tr("Texture"); }

	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		QModelIndex block = nif->getBlock( index, "NiTexturingProperty" );
		return ( block.isValid() && nif->get<int>( block, "Has Glow Texture" ) == 0 ); 
	}

	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		return addTexture( nif, index, "Glow Texture" );
	}
};

REGISTER_SPELL( spAddGlowMap )

//! Adds a Bump texture
class spAddBumpMap : public Spell
{
public:
	QString name() const { return Spell::tr("Add Bump Map"); }
	QString page() const { return Spell::tr("Texture"); }

	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		QModelIndex block = nif->getBlock( index, "NiTexturingProperty" );
		return ( block.isValid() && nif->get<int>( block, "Has Bump Map Texture" ) == 0 ); 
	}

	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iSrcTex = addTexture( nif, index, "Bump Map Texture" );
		QModelIndex block = nif->getBlock( index, "NiTexturingProperty" );
		nif->set<float>( block, "Bump Map Luma Scale", 1.0 );
		nif->set<float>( block, "Bump Map Luma Offset", 0.0 );
		QModelIndex iMatrix = nif->getIndex( block, "Bump Map Matrix" );
		nif->set<float>( iMatrix, "m11", 1.0 );
		nif->set<float>( iMatrix, "m12", 0.0 );
		nif->set<float>( iMatrix, "m21", 0.0 );
		nif->set<float>( iMatrix, "m22", 1.0 );
		return iSrcTex;
	}
};

REGISTER_SPELL( spAddBumpMap )

//! Adds a Decal 0 texture
class spAddDecal0Map : public Spell
{
public:
	QString name() const { return Spell::tr("Add Decal 0 Map"); }
	QString page() const { return Spell::tr("Texture"); }

	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		QModelIndex block = nif->getBlock( index, "NiTexturingProperty" );
		return ( block.isValid() && nif->get<int>( block, "Has Decal 0 Texture" ) == 0 ); 
	}

	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		return addTexture( nif, index, "Decal 0 Texture" );
	}
};

REGISTER_SPELL( spAddDecal0Map )

//! Adds a Decal 1 texture
class spAddDecal1Map : public Spell
{
public:
	QString name() const { return Spell::tr("Add Decal 1 Map"); }
	QString page() const { return Spell::tr("Texture"); }

	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		QModelIndex block = nif->getBlock( index, "NiTexturingProperty" );
		return ( block.isValid() && nif->get<int>( block, "Has Decal 1 Texture" ) == 0 ); 
	}

	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		return addTexture( nif, index, "Decal 1 Texture" );
	}
};

REGISTER_SPELL( spAddDecal1Map )

//! Adds a Decal 2 texture
class spAddDecal2Map : public Spell
{
public:
	QString name() const { return Spell::tr("Add Decal 2 Map"); }
	QString page() const { return Spell::tr("Texture"); }

	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		QModelIndex block = nif->getBlock( index, "NiTexturingProperty" );
		return ( block.isValid() && nif->get<int>( block, "Has Decal 2 Texture" ) == 0 ); 
	}

	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		return addTexture( nif, index, "Decal 2 Texture" );
	}
};

REGISTER_SPELL( spAddDecal2Map )

//! Adds a Decal 3 texture
class spAddDecal3Map : public Spell
{
public:
	QString name() const { return Spell::tr("Add Decal 3 Map"); }
	QString page() const { return Spell::tr("Texture"); }

	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		QModelIndex block = nif->getBlock( index, "NiTexturingProperty" );
		return ( block.isValid() && nif->get<int>( block, "Has Decal 3 Texture" ) == 0 ); 
	}

	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		return addTexture( nif, index, "Decal 3 Texture" );
	}
};

REGISTER_SPELL( spAddDecal3Map )

//! Wrap a value between 0 and 1
#define wrap01f( X ) ( X > 1 ? X - floor( X ) : X < 0 ? X - floor( X ) : X )

//! Saves the UV layout as a TGA
class spTextureTemplate : public Spell
{
	QString name() const { return Spell::tr("Export Template"); }
	QString page() const { return Spell::tr("Texture"); }

	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iUVs = getUV( nif, index );
		return iUVs.isValid() && nif->rowCount( iUVs ) >= 1;
	}

	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iUVs = getUV( nif, index );
		if ( nif->rowCount( iUVs ) <= 0 )
			return index;

		// fire up a dialog to set the user parameters
		QDialog dlg;
		QGridLayout * lay = new QGridLayout;
		dlg.setLayout( lay );

		FileSelector * file = new FileSelector( FileSelector::SaveFile, "File", QBoxLayout::RightToLeft );
		file->setFilter( QStringList() << FMASK_TGA );
		lay->addWidget( file, 0, 0, 1, 2 );

		lay->addWidget( new QLabel( "Size" ), 1, 0 );
		QComboBox * size = new QComboBox;
		lay->addWidget( size, 1, 1 );
		for ( int i = 6; i < 12; i++ )
			size->addItem( QString::number( 2 << i ) );

		lay->addWidget( new QLabel( "Coord Set" ), 2, 0 );
		QComboBox * set = new QComboBox;
		lay->addWidget( set, 2, 1 );
		for ( int i = 0; i < nif->rowCount( iUVs ); i++ )
			set->addItem( QString( "set %1" ).arg( i ) );

		lay->addWidget( new QLabel( "Wrap Mode" ), 3, 0 );
		QComboBox * wrap = new QComboBox;
		lay->addWidget( wrap, 3, 1 );
		wrap->addItem( "wrap" );
		wrap->addItem( "clamp" );

		QPushButton * ok = new QPushButton( "Ok" );
		QObject::connect( ok, SIGNAL( clicked() ), &dlg, SLOT( accept() ) );
		lay->addWidget( ok, 4, 0, 1, 2 );

		NIFSKOPE_QSETTINGS(settings);
		settings.beginGroup( "spells" );
		settings.beginGroup( page() );
		settings.beginGroup( name() );

		wrap->setCurrentIndex( settings.value( "Wrap Mode", 0 ).toInt() );
		size->setCurrentIndex( settings.value( "Image Size", 2 ).toInt() );
		file->setText( settings.value( TA_FILENAME, "" ).toString() );

		if ( dlg.exec() != QDialog::Accepted )
			return index;

		settings.setValue( "Wrap Mode", wrap->currentIndex() );
		settings.setValue( "Image Size", size->currentIndex() );
		settings.setValue( TA_FILENAME, file->text() );

		// get the selected coord set
		QModelIndex iSet = iUVs.child( set->currentIndex(), 0 );

		QVector<Vector2> uv = nif->getArray<Vector2>( iSet );
		QVector<Triangle> tri;

		// get the triangles
		QModelIndex iData = getData( nif, index );
		QModelIndex iPoints = nif->getIndex( iData, TA_POINTS );
		if ( iPoints.isValid() )
		{
			QList< QVector< quint16 > > strips;
			for ( int r = 0; r < nif->rowCount( iPoints ); r++ )
				strips.append( nif->getArray<quint16>( iPoints.child( r, 0 ) ) );
			tri = triangulate( strips );
		}
		else
		{
			tri = nif->getArray<Triangle>( nif->getIndex( getData( nif, index ), TA_TRIANGLES ) );
		}

		// render the template image
		quint16 s = size->currentText().toInt();

		QImage img( s, s, QImage::Format_RGB32 );
		QPainter pntr( &img );

		pntr.fillRect( img.rect(), QColor( 0xff, 0xff, 0xff ) );
		pntr.scale( s, s );
		pntr.setPen( QColor( 0x10, 0x20, 0x30 ) );

		bool wrp = wrap->currentIndex() == 0;

		foreach ( Triangle t, tri )
		{
			Vector2 v2[3];
			for ( int i = 0; i < 3; i++ )
			{
				v2[i] = uv.value( t[i] );
				if ( wrp )
				{
					v2[i][0] = wrap01f( v2[i][0] );
					v2[i][1] = wrap01f( v2[i][1] );
				}
			}

			pntr.drawLine( QPointF( v2[0][0], v2[0][1] ), QPointF( v2[1][0], v2[1][1] ) );
			pntr.drawLine( QPointF( v2[1][0], v2[1][1] ), QPointF( v2[2][0], v2[2][1] ) );
			pntr.drawLine( QPointF( v2[2][0], v2[2][1] ), QPointF( v2[0][0], v2[0][1] ) );
		}

		// write the file
		QString filename = file->text();
		if ( ! filename.endsWith( ".tga", Qt::CaseInsensitive ) )
			filename.append( ".tga" );

		quint8 hdr[18];
		for ( int o = 0; o < 18; o++ ) hdr[o] = 0;
		hdr[02] = 2; // TGA_COLOR
		hdr[12] = s % 256;
		hdr[13] = s / 256;
		hdr[14] = s % 256;
		hdr[15] = s / 256;
		hdr[16] = 32; // bpp
		hdr[17] = 32; // flipV

		QFile f( filename );
		if ( ! f.open( QIODevice::WriteOnly ) || f.write( (char *) hdr, 18 ) != 18 || f.write( (char *) img.bits(), s * s * 4 ) != s * s * 4 )
			qWarning() << "exportTemplate(" << filename << ") : could not write file";

		return index;
	}
};

REGISTER_SPELL( spTextureTemplate )

//! Global search and replace of texturing apply modes
class spMultiApplyMode : public Spell
{
public:
	QString name() const { return Spell::tr("Multi Apply Mode"); }
	QString page() const { return Spell::tr("Batch"); } 
	
	bool isApplicable( const NifModel * nif, const QModelIndex &index )
	{
		// Apply Mode field is defined in nifs up to version 20.0.0.5
		return nif->checkVersion( 0, NF_V20000005 ) && ! index.isValid();
	}
	
	QModelIndex cast( NifModel *nif, const QModelIndex &index )
	{
		QStringList modes;
		modes << "Replace" <<  "Decal" << "Modulate" << "Hilight" << "Hilight2";
		
		QDialog dlg;
		dlg.resize( 300, 60 );
		QComboBox *cbRep = new QComboBox( &dlg );
		QComboBox *cbBy = new QComboBox( &dlg );
		QPushButton *btnOk = new QPushButton( "OK", &dlg );
		QPushButton *btnCancel = new QPushButton( "Cancel", &dlg );
		cbRep->addItems( modes );
		cbRep->setCurrentIndex( 2 );
		cbBy->addItems( modes );
		cbBy->setCurrentIndex( 2 );
		
		QGridLayout *layout;
		layout = new QGridLayout;
		layout->setSpacing( 20 );
		layout->addWidget( new QLabel( "Replace", &dlg ), 0, 0, Qt::AlignBottom );
		layout->addWidget( new QLabel( "By", &dlg ), 0, 1, Qt::AlignBottom );
		layout->addWidget( cbRep, 1, 0, Qt::AlignTop );
		layout->addWidget( cbBy, 1, 1, Qt::AlignTop );
		layout->addWidget( btnOk, 2, 0 );
		layout->addWidget( btnCancel, 2, 1 );
		dlg.setLayout( layout );
		
		QObject::connect( btnOk, SIGNAL( clicked() ), &dlg, SLOT( accept() ) );
		QObject::connect( btnCancel, SIGNAL( clicked() ), &dlg, SLOT( reject() ) );
		
		if ( dlg.exec() != QDialog::Accepted )
			return QModelIndex();
		
		QList< QPersistentModelIndex > indices;
		
		for ( int n = 0; n < nif->getBlockCount(); n++ )
		{
			QModelIndex idx = nif->getBlock( n );
			indices << idx;
		}
		
		foreach ( QModelIndex idx, indices )
		{
			replaceApplyMode( nif, idx, cbRep->currentIndex(), cbBy->currentIndex() );
		}
		
		return QModelIndex();
	}
	
	//! Recursively replace the Apply Mode
	void replaceApplyMode( NifModel *nif, const QModelIndex &index, int rep, int by )
	{
		if ( !index.isValid() )
			return;
		
		if ( nif->inherits( index, "NiTexturingProperty" ) &&
			nif->get<int>( index, "Apply Mode" ) == rep )
			nif->set<int>( index, "Apply Mode", by );
		
		QModelIndex iChildren = nif->getIndex( index, "Children" );
		QList<qint32> lChildren = nif->getChildLinks( nif->getBlockNumber( index ) );
		if ( iChildren.isValid() )
		{
			for ( int c = 0; c < nif->rowCount( iChildren ); c++ )
			{
				qint32 link = nif->getLink( iChildren.child( c, 0 ) );
				if ( lChildren.contains( link ) )
				{
					QModelIndex iChild = nif->getBlock( link );
					replaceApplyMode( nif, iChild, rep, by );
				}
			}
		}
		
		QModelIndex iProperties = nif->getIndex( index, TA_PROPERTIES );
		if ( iProperties.isValid() )
		{
			for ( int p = 0; p < nif->rowCount( iProperties ); p++ )
			{
				QModelIndex iProp = nif->getBlock( nif->getLink( iProperties.child( p, 0 ) ) );
				replaceApplyMode( nif, iProp, rep, by );
			}
		}
	}
};

REGISTER_SPELL( spMultiApplyMode )

//! Debug function - display information about a texture
class spTexInfo : public Spell
{
public:
	QString name() const { return Spell::tr("Info"); }
	QString page() const { return Spell::tr("Texture"); }

	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iBlock = nif->getBlock( index );
		if ( nif->isNiBlock( iBlock, T_NISOURCETEXTURE ) )
			return true;
		return false;
	}

	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		TexCache * tex = new TexCache();
		tex->setNifFolder( nif->getFolder() );
		int isExternal = nif->get<int>( index, "Use External" );
		if ( isExternal ) {
			QString filename = nif->get<QString>(index, TA_FILENAME);
			tex->bind( filename );
		} else {
			tex->bind( index );
		}
		qWarning() << tex->info( index );
		return QModelIndex();
	}
};

#ifndef QT_NO_DEBUG
REGISTER_SPELL( spTexInfo )
#endif

//! Export a packed NiPixelData texture
class spExportTexture : public Spell
{
public:
	QString name() const { return Spell::tr("Export"); }
	QString page() const { return Spell::tr("Texture"); }

	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iBlock = nif->getBlock( index );
		if ( nif->isNiBlock( iBlock, T_NISOURCETEXTURE ) && nif->get<int>( iBlock, "Use External" ) == 0 )
		{
			QModelIndex iData = nif->getBlock( nif->getLink( index, "Pixel Data" ) );
			if ( iData.isValid() )
			{
				return true;
			}
		}
		else if ( nif->inherits( iBlock, T_ATEXTURERENDERDATA ) )
		{
			int thisBlockNumber = nif->getBlockNumber( index );
			QModelIndex iParent = nif->getBlock( nif->getParent( thisBlockNumber ) );
			if( ! iParent.isValid() )
			{
				return true;
			}
		}
		return false;
	}

	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		TexCache * tex = new TexCache();
		tex->setNifFolder( nif->getFolder() );
		QModelIndex iBlock = nif->getBlock( index );
		if ( nif->inherits( iBlock, T_NISOURCETEXTURE ) )
		{
			tex->bind( index );
			QString file = nif->getFolder();
			if ( nif->checkVersion( NF_V10010000, 0 ) )
			{
				// Qt uses "/" regardless of platform
				file.append( "/" + nif->get<QString>( index, TA_FILENAME ) );
			}
			QModelIndex iData = nif->getBlock( nif->getLink( index, "Pixel Data" ) );
			QString filename = QFileDialog::getSaveFileName( 0, Spell::tr("Export texture"), file, FMASK_DDS" "FMASK_TGA );
			if ( ! filename.isEmpty() )
			{
				if ( tex->exportFile( iData, filename ) )
				{
					nif->set<int>( index, "Use External", 1 );
					filename = TexCache::stripPath( filename, nif->getFolder() );
					nif->set<QString>( index, TA_FILENAME, filename );
					tex->bind( filename );
				}
			}
			return index;
		}
		else if ( nif->inherits( iBlock, T_ATEXTURERENDERDATA ) )
		{
			TexCache * tex = new TexCache();
			tex->setNifFolder( nif->getFolder() );
			QString file = nif->getFolder();
			QString filename = QFileDialog::getSaveFileName( 0, Spell::tr("Export texture"), file, FMASK_DDS" "FMASK_TGA );
			if ( ! filename.isEmpty() )
			{
				tex->exportFile( index, filename );
			}
		}
		return index;
	}
};

REGISTER_SPELL( spExportTexture )

//! Pack a texture to NiPixelData
class spEmbedTexture : public Spell
{
public:
	QString name() const { return Spell::tr("Embed"); }
	QString page() const { return Spell::tr("Texture"); }

	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		if ( ! ( nif->checkVersion( 0, NF_V10020000 ) || nif->checkVersion( NF_V20000004, 0 ) ) )
		{
			return false;
		}
		QModelIndex iBlock = nif->getBlock( index );
		if ( !( nif->isNiBlock( iBlock, T_NISOURCETEXTURE ) && nif->get<int>( iBlock, "Use External" ) == 1 ))
		{
			return false;
		}
		TexCache * tex = new TexCache();
		tex->setNifFolder( nif->getFolder() );
		if ( tex->bind( nif->get<QString>( iBlock, TA_FILENAME ) ) )
		{
			return true;
		}
		return false;
	}

	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		TexCache * tex = new TexCache();
		tex->setNifFolder( nif->getFolder() );
		if ( tex->bind( index ) )
		{
			//qWarning() << "spEmbedTexture: Embedding texture " << index;

			int blockNum = nif->getBlockNumber( index );
			nif->insertNiBlock( "NiPixelData", blockNum+1 );
			QPersistentModelIndex iSourceTexture = nif->getBlock( blockNum, T_NISOURCETEXTURE );
			QModelIndex iPixelData = nif->getBlock( blockNum+1, "NiPixelData" );

			//qWarning() << "spEmbedTexture: Block number" << blockNum << "holds source" << iSourceTexture << "Pixel data will be stored in" << iPixelData;
			
			// finish writing this function
			if ( tex->importFile( nif, iSourceTexture, iPixelData ) )
			{
				QString tempFileName = nif->get<QString>( iSourceTexture, TA_FILENAME );
				tempFileName = TexCache::stripPath( tempFileName, nif->getFolder() );
				nif->set<int>( iSourceTexture, "Use External", 0 );
				nif->set<int>( iSourceTexture, "Unknown Byte", 1 );
				nif->setLink( iSourceTexture, "Pixel Data", blockNum+1 );
				if( nif->checkVersion( NF_V10010000, 0 ) )
				{
					nif->set<QString>( iSourceTexture, TA_FILENAME, tempFileName );
				}
				else
				{
					nif->set<QString>( index, TA_NAME, tempFileName );
				}
			}
			else
			{
				qWarning() << "Could not save texture";
				// delete block?
				/*
				nif->removeNiBlock( blockNum+1 );
				nif->set<int>( iSourceTexture, "Use External", 1 );
				*/
			}
		}
		return index;
	}
};

REGISTER_SPELL( spEmbedTexture )

TexFlipDialog::TexFlipDialog( NifModel * nif, QModelIndex & index, QWidget * parent ) : QDialog( parent )
{
	this->nif = nif;
	baseIndex = index;

	grid = new QGridLayout;
	setLayout( grid );
	
	listmodel = new QStringListModel;
	listview = new QListView;
	listview->setModel( listmodel );
	
	// texture action group; see options.cpp
	QButtonGroup * actgrp = new QButtonGroup( this );
	connect( actgrp, SIGNAL( buttonClicked( int ) ), this, SLOT( textureAction( int ) ) );
	int btnid = 0;
	foreach ( QString tfaname, QStringList() << Spell::tr("Add Textures") << Spell::tr("Remove Texture") << Spell::tr("Move Up") << Spell::tr("Move Down") )
	{
		QPushButton * bt = new QPushButton( tfaname );
		textureButtons[btnid] = bt;
		actgrp->addButton( bt, btnid++ );
		grid->addWidget( bt, 0, btnid, 1, 1 );
	}
	
	// add the list view and set up members
	grid->addWidget( listview, 1, 0, 1, 0 );
	listFromNif();

	connect( listview->selectionModel(), SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
			this, SLOT( texIndex( const QModelIndex & ) ) );
	texIndex( listview->currentIndex() );

	QHBoxLayout * hbox1 = new QHBoxLayout();
	hbox1->addWidget( startTime = new NifFloatEdit( nif, nif->getIndex( baseIndex, "Start Time" ), 0 ) );
	hbox1->addWidget( stopTime = new NifFloatEdit( nif, nif->getIndex( baseIndex, "Stop Time" ), 0 ) );
	grid->addLayout( hbox1, 2, 0, 1, 0 );

	startTime->updateData( nif );
	stopTime->updateData( nif );

	QHBoxLayout * hbox2 = new QHBoxLayout();
	QPushButton * ok = new QPushButton( Spell::tr("OK"), this );
	hbox2->addWidget( ok );
	connect( ok, SIGNAL( clicked() ), this, SLOT( accept() ) );
	QPushButton * cancel = new QPushButton( Spell::tr("Cancel"), this );
	hbox2->addWidget( cancel );
	connect( cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	grid->addLayout( hbox2, 3, 0, 1, 0 );
}

void TexFlipDialog::textureAction( int i )
{
	QModelIndex idx = listview->currentIndex();
	switch ( i )
	{
		case 0: // add
			flipnames = QFileDialog::getOpenFileNames( this, Spell::tr("Choose texture file(s)"), nif->getFolder(), FMASK_DDS" "FMASK_TGA" "FMASK_BMP );
			listmodel->setStringList( listmodel->stringList() << flipnames );
			break;
		case 1: // remove
			if ( idx.isValid() )
			{
				listmodel->removeRow( idx.row(), QModelIndex() );
			}
		case 2:
			if ( idx.isValid() && idx.row() > 0 )
			{	// move up
				QModelIndex xdi = idx.sibling( idx.row() - 1, 0 );
				QVariant v = listmodel->data( idx, Qt::EditRole );
				listmodel->setData( idx, listmodel->data( xdi, Qt::EditRole ), Qt::EditRole );
				listmodel->setData( xdi, v, Qt::EditRole );
				listview->setCurrentIndex( xdi );
			}
			break;
		case 3:
			if ( idx.isValid() && idx.row() < listmodel->rowCount() - 1 )
			{	// move down
				QModelIndex xdi = idx.sibling( idx.row() + 1, 0 );
				QVariant v = listmodel->data( idx, Qt::EditRole );
				listmodel->setData( idx, listmodel->data( xdi, Qt::EditRole ), Qt::EditRole );
				listmodel->setData( xdi, v, Qt::EditRole );
				listview->setCurrentIndex( xdi );
			}
			break;
	}
}

void TexFlipDialog::texIndex( const QModelIndex & idx )
{
	textureButtons[0]->setEnabled( true );
	textureButtons[1]->setEnabled( idx.isValid() );
	textureButtons[2]->setEnabled( idx.isValid() && ( idx.row() > 0 ) );
	textureButtons[3]->setEnabled( idx.isValid() && ( idx.row() < listmodel->rowCount() - 1 ) );
}

QStringList TexFlipDialog::flipList()
{
	return listmodel->stringList();
}

void TexFlipDialog::listFromNif()
{
	// update string list
	int numSources = nif->get<int>( baseIndex, "Num Sources" );
	QModelIndex sources = nif->getIndex( baseIndex, "Sources" );
	
	if ( nif->rowCount( sources ) != numSources )
	{
		qWarning() << "Number of sources does not match!";
		return;
	}

	QStringList sourceFiles;
	for ( int i = 0; i < numSources; i++ )
	{
		QModelIndex source = nif->getBlock( nif->getLink( sources.child( i, 0 ) ) );
		sourceFiles << nif->get<QString>( source, TA_FILENAME );
	}
	
	listmodel->setStringList( sourceFiles );
}

//! Edit TexFlipController
/*!
 * TODO: update for version conditions, preserve properties on existing textures
 */
class spEditFlipper : public Spell
{
public:
	QString name() const { return Spell::tr("Edit Flip Controller"); }
	QString page() const { return Spell::tr("Texture"); }
 	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return ( nif->getBlockName( index ) == "NiFlipController" );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QModelIndex flipController = index;
		TexFlipDialog * texFlip = new TexFlipDialog( nif, flipController );
		
		if ( texFlip->exec() != QDialog::Accepted )
		{
			return QModelIndex();
		}
		
		QStringList flipNames = texFlip->flipList();
		
		if ( flipNames.size() == 0 )
		{
			//nif->removeNiBlock( nif->getBlockNumber( flipController ) );
			return index;
		}

		// TODO: use a map here to delete missing textures and preserve existing properties
		
		QModelIndex sources = nif->getIndex( flipController, "Sources" );
		if ( nif->get<int>( flipController, "Num Sources" ) > flipNames.size() )
		{
			// delete blocks
			qWarning() << "Found" << flipNames.size() << "textures, have" << nif->get<int>( flipController, "Num Sources" );
			for ( int i = flipNames.size(); i < nif->get<int>( flipController, "Num Sources" ); i++ )
			{
				qWarning() << "Deleting" << nif->getLink( sources.child( i, 0 ) );
				nif->removeNiBlock( nif->getLink( sources.child( i, 0 ) ) );
			}
		}
		
		nif->set<int>( flipController, "Num Sources", flipNames.size() );
		nif->updateArray( sources );
		
		for( int i = 0; i < flipNames.size(); i++ )
		{
			QString name = TexCache::stripPath( flipNames.at(i), nif->getFolder() );
			QModelIndex sourceTex;
			if( nif->getLink( sources.child( i, 0 ) ) == -1 )
			{
				sourceTex = nif->insertNiBlock( T_NISOURCETEXTURE, nif->getBlockNumber( flipController ) + i + 1 );
				nif->setLink( sources.child( i, 0 ), nif->getBlockNumber( sourceTex ) );
			}
			else
			{
				sourceTex = nif->getBlock( nif->getLink( sources.child( i, 0 ) ) );
			}
			nif->set<QString>( sourceTex, TA_FILENAME, name );
		}
		
		nif->set<float>( flipController, "Frequency", 1 );
		nif->set<quint16>( flipController, "Flags", 8 );
		nif->set<float>( flipController, "Delta", ( nif->get<float>( flipController, "Stop Time" ) - nif->get<float>( flipController, "Start Time" ) ) / flipNames.size() );
		
		return index;
	}
};

REGISTER_SPELL( spEditFlipper )

//! Insert and manage TexFlipController
class spTextureFlipper : public Spell
{
public:
	QString name() const { return Spell::tr("Add Flip Controller"); }
	QString page() const { return Spell::tr("Texture"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		// also check NiTextureProperty?
		QModelIndex block = nif->getBlock( index, "NiTexturingProperty" );
		return block.isValid();
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		// attach a NiFlipController
		QModelIndex flipController = nif->insertNiBlock( "NiFlipController", nif->getBlockNumber( index ) + 1 );
		blockLink( nif, index, flipController );
		
		spEditFlipper flipEdit;

		return flipEdit.cast( nif, flipController );
	}
};

REGISTER_SPELL( spTextureFlipper )
