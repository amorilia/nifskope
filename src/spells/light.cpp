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

#include "spellbook.h"

#include "widgets/nifeditors.h"

// Brief description is deliberately not autolinked to class Spell
/*! \file light.cpp
 * \brief Light editing spells (spLightEdit)
 *
 * All classes here inherit from the Spell class.
 */

/* XPM */
static char const * light42_xpm[] = {
"24 24 43 1",
" 	c None",
".	c #000100","+	c #0E0D02","@	c #111401","#	c #151500","$	c #191903",
"%	c #1E1D02","&	c #201E00","*	c #2A2C01","=	c #2D2D00","-	c #2E2F00",
";	c #2F3000",">	c #3B3A00",",	c #3D3C00","'	c #3E3D00",")	c #454300",
"!	c #464800","~	c #494B00","{	c #525200","]	c #565700","^	c #6B6900",
"/	c #6B6D00","(	c #797A00","_	c #7E7F02",":	c #848300","<	c #9D9E03",
"[	c #A6A600","}	c #B3B202","|	c #B8B500","1	c #CFD000","2	c #D7D600",
"3	c #DDDB00","4	c #E4E200","5	c #E9E600","6	c #E8EB00","7	c #ECEF00",
"8	c #F1F300","9	c #F3F504","0	c #F6F800","a	c #F9FA00","b	c #FBFC00",
"c	c #FEFE00","d	c #FFFF01",
"         -,'~'*         ",
"       $[8bbdb5_        ",
"       }bddddddb[       ",
"      :bddddddddb<      ",
"     -2dddddddddda;     ",
"     'addddddddddb{     ",
"     ~bddddddddddd^     ",
"     ]dddddddddddd/     ",
"     ~bddddddddddd{     ",
"     ,addddddddddb;     ",
"      3dddddddddd7      ",
"      :adddddddd9:      ",
"       (8bddddb8)       ",
"        ,}4741|]        ",
"         +@##%&         ",
"         ......         ",
"         ......         ",
"         ......         ",
"         ......         ",
"         ......         ",
"         ......         ",
"         ......         ",
"         ......         ",
"          ....          "};

QIcon * light42_xpm_icon = 0;

//! Edit the parameters of a light object
class spLightEdit : public Spell
{
public:
	QString name() const { return Spell::tr("Light"); }
	QString page() const { return Spell::tr(""); }
	bool instant() const { return true; }
	QIcon icon() const
	{
		if ( ! light42_xpm_icon ) light42_xpm_icon = new QIcon( light42_xpm );
		return *light42_xpm_icon;
	}
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iBlock = nif->getBlock( index );
		QModelIndex sibling = index.sibling( index.row(), 0 );
		return index.isValid() && nif->inherits( iBlock, "NiLight" ) && ( iBlock == sibling || nif->getIndex( iBlock, TA_NAME ) == sibling );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iLight = nif->getBlock( index );
		
		NifBlockEditor * le = new NifBlockEditor( nif, iLight );
		le->pushLayout( new QHBoxLayout() );
		le->add( new NifVectorEdit( nif, nif->getIndex( iLight, TA_TRANSLATION ) ) );
		le->add( new NifRotationEdit( nif, nif->getIndex( iLight, "Rotation" ) ) );
		le->popLayout();
		le->add( new NifFloatSlider( nif, nif->getIndex( iLight, "Dimmer" ), 0, 1.0 ) );
		le->pushLayout( new QHBoxLayout() );
		le->add( new NifColorEdit( nif, nif->getIndex( iLight, TA_AMBIENTCOLOR ) ) );
		le->add( new NifColorEdit( nif, nif->getIndex( iLight, TA_DIFFUSECOLOR ) ) );
		le->add( new NifColorEdit( nif, nif->getIndex( iLight, TA_SPECULARCOLOR ) ) );
		le->popLayout();
		le->pushLayout( new QHBoxLayout(), "Point Light Parameter" );
		le->add( new NifFloatEdit( nif, nif->getIndex( iLight, "Constant Attenuation" ) ) );
		le->add( new NifFloatEdit( nif, nif->getIndex( iLight, "Linear Attenuation" ) ) );
		le->add( new NifFloatEdit( nif, nif->getIndex( iLight, "Quadratic Attenuation" ) ) );
		le->popLayout();
		le->pushLayout( new QHBoxLayout(), "Spot Light Parameters" );
		le->add( new NifFloatEdit( nif, nif->getIndex( iLight, "Cutoff Angle" ), 0, 90 ) );
		le->add( new NifFloatEdit( nif, nif->getIndex( iLight, "Exponent" ), 0, 128 ) );
		le->popLayout();
		le->show();
		
		return index;
	}
};

REGISTER_SPELL( spLightEdit )

