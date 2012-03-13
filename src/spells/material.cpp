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
/*! \file material.cpp
 * \brief Material editing spells (spMaterialEdit)
 *
 * All classes here inherit from the Spell class.
 */

/* XPM */
static char const * mat42_xpm[] = {
"64 64 43 1",
" 	c None",
".	c #866A36","+	c #856E33","@	c #817333","#	c #836F45","$	c #8B6D3E",
"%	c #8C6F34","&	c #917231","*	c #77812C","=	c #7D765D","-	c #987147",
";	c #6C8D29",">	c #93792F",",	c #7A7B71","'	c #A0734E",")	c #71828B",
"!	c #5DA11E","~	c #9B822B","{	c #56AD16","]	c #6E889D","^	c #AD795F",
"/	c #698DB0","(	c #48BE13","_	c #6691BD",":	c #43C311","<	c #A58D28",
"[	c #6396CE","}	c #1BE30C","|	c #38D700","1	c #BD7F71","2	c #11F003",
"3	c #C78581","4	c #0CFF00","5	c #B7A51C","6	c #DA8C95","7	c #C3B618",
"8	c #ED93AA","9	c #F898B9","0	c #D6CD0A","a	c #FF9BC4","b	c #E6DF00",
"c	c #F6F500","d	c #FFFF00",
"                                                                ",
"                                                                ",
"                                  .......                       ",
"                               ................                 ",
"                             .......................            ",
"                           ...........................          ",
"                          ...............................       ",
"                        .................................       ",
"                       ...................................      ",
"                     ......................................     ",
"                    ........................................    ",
"                   ...........................@***+.........    ",
"                  ................%~5<%......;2444}:*........   ",
"                 ................&0ddd0&....@}4444442;........  ",
"                ................%7ddddd7....;44444444|+.......  ",
"               .................5ddddddb>...{444444442@.......  ",
"              .................%0ddddddc<..+|444444442@.......  ",
"             ..................%bddddddc<..@|444444442@.......  ",
"            ...................%bddddddc<..@|44444444|........  ",
"           ............'^1^-...%bddddddc>..@|44444444!........  ",
"          ............39aaa8'..%bddddddb%..+:4444442!.........  ",
"         ............^9aaaaa6$.%7dddddd0....*}4444};..........  ",
"        .............6aaaaaa9^..<cddddd5.....*(||{@...........  ",
"        ............$8aaaaaaa1...7ddddb>.....................   ",
"       .............-9aaaaaaa1...%7cdb~......................   ",
"      ..............-9aaaaaaa1.....>~&......................    ",
"      ..............-8aaaaaaa1..............................    ",
"     ........#)]],..$8aaaaaa9^..............................    ",
"     .......=_[[[[,..^9aaaaa6..............................     ",
"    .......$/[[[[[]$..^9aaa8'.............................      ",
"    .......)[[[[[[_#...'363'..............................      ",
"    ......#_[[[[[[_#....................................        ",
"    ......=[[[[[[[/#..................................          ",
"    ......,[[[[[[[_#.................................           ",
"    ......,[[[[[[[_#...............................             ",
"    ......,[[[[[[[]$.............................               ",
"    ......=_[[[[[_#............................                 ",
"    .......][[[[_,............................                  ",
"    .......#][[_,...........................                    ",
"    .........==#..........................                      ",
"    .....................................                       ",
"    ....................................                        ",
"     ...................................                        ",
"     ..................................                         ",
"     .................................                          ",
"      ................................                          ",
"      ...............................                           ",
"      ..................  ...........                           ",
"      ................      .........                           ",
"       ..............        ........                           ",
"        .............        ........                           ",
"        ............         .......                            ",
"         ...........         .......                            ",
"         ............        .......                            ",
"          ...........       ........                            ",
"           ...........     .........                            ",
"           ........................                             ",
"            .......................                             ",
"             .....................                              ",
"               .................                                ",
"                .............                                   ",
"                  ........                                      ",
"                                                                ",
"                                                                "};

QIcon * mat42_xpm_icon = 0;

//! Edit a material
class spMaterialEdit : public Spell
{
public:
	QString name() const { return Spell::tr("Material"); }
	QString page() const { return Spell::tr(""); }
	bool instant() const { return true; }
	QIcon icon() const
	{
		if ( ! mat42_xpm_icon ) mat42_xpm_icon = new QIcon( mat42_xpm );
		return *mat42_xpm_icon;
	}
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iBlock = nif->getBlock( index, T_NIMATERIALPROPERTY );
		QModelIndex sibling = index.sibling( index.row(), 0 );
		return index.isValid() && ( iBlock == sibling || nif->getIndex( iBlock, TA_NAME ) == sibling );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iMaterial = nif->getBlock( index );
		NifBlockEditor * me = new NifBlockEditor( nif, iMaterial );
		
		me->pushLayout( new QHBoxLayout );
		me->add( new NifColorEdit( nif, nif->getIndex( iMaterial, TA_AMBIENTCOLOR ) ) );
		me->add( new NifColorEdit( nif, nif->getIndex( iMaterial, TA_DIFFUSECOLOR ) ) );
		me->popLayout();
		me->pushLayout( new QHBoxLayout );
		me->add( new NifColorEdit( nif, nif->getIndex( iMaterial, TA_SPECULARCOLOR ) ) );
		me->add( new NifColorEdit( nif, nif->getIndex( iMaterial, TA_EMISSIVECOLOR ) ) );
		me->popLayout();
		me->add( new NifFloatSlider( nif, nif->getIndex( iMaterial, TA_ALPHA ), 0.0, 1.0 ) );
		me->add( new NifFloatSlider( nif, nif->getIndex( iMaterial, TA_GLOSSINESS ), 0.0, 100.0 ) );
		me->setWindowModality(Qt::ApplicationModal);
		me->show();
		
		return index;
	}
};

REGISTER_SPELL( spMaterialEdit )

