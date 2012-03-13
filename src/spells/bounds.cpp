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

#include "spellbook.h"
#include "widgets/nifeditors.h"

#include <QDebug>

// Brief description is deliberately not autolinked to class Spell
/*! \file bounds.cpp
 * \brief Bounding box editing spells (spEditBounds)
 *
 * All classes here inherit from the Spell class.
 */

//! Edit a bounding box
class spEditBounds : public Spell
{
public:
	QString name() const { return Spell::tr("Edit"); }
	QString page() const { return Spell::tr("Bounds"); }

	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return (nif->itemName( index ) == "BSBound")
			|| (nif->itemName( index.parent() ) == "BSBound" )
			|| (nif->get<bool>( index, "Has Bounding Box" ) == true)
			|| (nif->itemName( index ) == "Bounding Box")
			|| (nif->itemName( index.parent() ) == "Bounding Box");
	}

	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		NifBlockEditor * edit = new NifBlockEditor( nif, nif->getBlock( index ) );
		if ( nif->get<bool>( index, "Has Bounding Box" ) == true || nif->itemName( index ) == "Bounding Box" || nif->itemName( index.parent() ) == "Bounding Box" )
		{
			QModelIndex iBound;
			if ( nif->itemName( index ) == "Bounding Box" )
			{
				iBound = index;
			}
			else if ( nif->itemName( index.parent() ) == "Bounding Box" )
			{
				iBound = index.parent();
			}
			else
			{
				iBound = nif->getIndex( index, "Bounding Box" );
			}
			
			edit->add( new NifVectorEdit( nif, nif->getIndex( iBound, TA_TRANSLATION ) ) );
			edit->add( new NifRotationEdit( nif, nif->getIndex( iBound, TA_ROTATION ) ) );
			edit->add( new NifVectorEdit( nif, nif->getIndex( iBound, TA_RADIUS ) ) );
		}
		else if ( nif->itemName( index ) == "BSBound" || nif->itemName( index.parent() ) == "BSBound" )
		{
			QModelIndex iBound;
			if ( nif->itemName( index ) == "BSBound" )
			{
				iBound = index;
			}
			else if ( nif->itemName( index.parent() ) == "BSBound" )
			{
				iBound = index.parent();
			}

			edit->add( new NifVectorEdit( nif, nif->getIndex( iBound, TA_CENTER ) ) );
			edit->add( new NifVectorEdit( nif, nif->getIndex( iBound, "Dimensions" ) ) );
		}
		edit->show();
		return index;
	}
};

REGISTER_SPELL( spEditBounds )
