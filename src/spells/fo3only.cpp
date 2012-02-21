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

#include <QDebug>

// Brief description is deliberately not autolinked to class Spell
/*! \file fo3only.cpp
 * \brief Fallout 3 specific spells (spFO3FixShapeDataName)
 *
 * All classes here inherit from the Spell class.
 */

//! Set the name of the NiGeometryData node to parent name or zero
class spFO3FixShapeDataName : public Spell
{
public:
	QString name() const { return Spell::tr("Fix Geometry Data Names"); }
	QString page() const { return Spell::tr("Sanitize"); }
	bool sanity() const { return true; }
	
	//////////////////////////////////////////////////////////////////////////
	// Valid if nothing or NiGeometryData-based node is selected
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		//if ( !index.isValid() )
		//	return false;
		
		if ( !nif->checkVersion( 0x14020007, 0x14020007 ) || (nif->getUserVersion() != 11) )
			return false;
		
		return !index.isValid() || nif->getBlock( index, "NiGeometryData" ).isValid();
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		if ( index.isValid() && nif->getBlock( index, "NiGeometryData" ).isValid() )
		{
			nif->set<int>(index, "Unknown ID", 0);        
		}
		else
		{
			// set all blocks
			for ( int n = 0; n < nif->getBlockCount(); n++ ) {
				QModelIndex iBlock = nif->getBlock( n );
				if ( nif->getBlock( iBlock, "NiGeometryData" ).isValid() ) 
				{
					cast(nif, iBlock);
				}
			}
		}
		return index;
	}
};

REGISTER_SPELL( spFO3FixShapeDataName )

