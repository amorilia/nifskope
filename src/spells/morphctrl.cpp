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

#include <QDebug>

// Brief description is deliberately not autolinked to class Spell
/*! \file morphctrl.cpp
 * \brief Morph controller spells (spMorphFrameSave)
 *
 * All classes here inherit from the Spell class.
 */

//! Saves the current morph position.
/**
 * Does not seem to work properly? MorphController may need updating.
 */
class spMorphFrameSave : public Spell
{
public:
	QString name() const { return Spell::tr("Save Vertices To Frame"); }
	QString page() const { return Spell::tr("Morph"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return nif->isNiBlock( index, T_NIGEOMMORPHERCONTROLLER ) && nif->checkVersion( NF_V10010000, 0 )
			&& getMeshData( nif, index ).isValid() && listFrames( nif, index ).count() > 0;
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iMeshData = getMeshData( nif, index );
		QModelIndex iMorphData = getMorphData( nif, index );
		
		QMenu menu;
		QStringList frameList = listFrames( nif, index );
		
		if ( nif->get<int>( iMeshData, TA_NUMVERTICES ) != nif->get<int>( iMorphData, TA_NUMVERTICES ) )
			menu.addAction( frameList.first() );
		else
			foreach ( QString f, frameList )
				menu.addAction( f );
		
		QAction * act = menu.exec( QCursor::pos() );
		if ( act )
		{
			QModelIndex iFrames = getFrameArray( nif, index );
			int selFrame = frameList.indexOf( act->text() );
			if ( selFrame == 0 )
			{
				qWarning() << "overriding base key frame, all other frames will be cleared";
				nif->set<int>( iMorphData, TA_NUMVERTICES, nif->get<int>( iMeshData, TA_NUMVERTICES ) );
				QVector<Vector3> verts = nif->getArray<Vector3>( iMeshData, TA_VERTICES );
				nif->updateArray( iFrames.child( 0, 0 ), TA_VECTORS );
				nif->setArray( iFrames.child( 0, 0 ), TA_VECTORS, verts );
				verts.fill( Vector3() );
				for ( int f = 1; f < nif->rowCount( iFrames ); f++ )
				{
					nif->updateArray( iFrames.child( f, 0 ), TA_VECTORS );
					nif->setArray<Vector3>( iFrames.child( f, 0 ), TA_VECTORS, verts );
				}
			}
			else
			{
				QVector<Vector3> verts = nif->getArray<Vector3>( iMeshData, TA_VERTICES );
				QVector<Vector3> base = nif->getArray<Vector3>( iFrames.child( 0, 0 ), TA_VECTORS );
				QVector<Vector3> frame( base.count(), Vector3() );
				for ( int n = 0; n < base.count(); n++ )
					frame[ n ] = verts.value( n ) - base[ n ];
				nif->setArray<Vector3>( iFrames.child( selFrame, 0 ), TA_VECTORS, frame );
			}
		}
		
		return index;
	}
	
	//! Helper function to get the Mesh data
	QModelIndex getMeshData( const NifModel * nif, const QModelIndex & iMorpher )
	{
		QModelIndex iMesh = nif->getBlock( nif->getParent( nif->getBlockNumber( iMorpher ) ) );
		if ( nif->inherits( iMesh, T_NITRIBASEDGEOM ) )
		{
			QModelIndex iData = nif->getBlock( nif->getLink( iMesh, TA_DATA ) );
			if ( nif->inherits( iData, T_NITRIBASEDGEOMDATA ) )
				return iData;
			else
				return QModelIndex();
		}
		else
			return QModelIndex();
	}
	
	//! Helper function to get the morph data
	QModelIndex getMorphData( const NifModel * nif, const QModelIndex & iMorpher )
	{
		return nif->getBlock( nif->getLink( iMorpher, TA_DATA ), "NiMorphData" );
	}
	
	//! Helper function to get the morph frame array
	QModelIndex getFrameArray( const NifModel * nif, const QModelIndex & iMorpher )
	{
		return nif->getIndex( getMorphData( nif, iMorpher ), TA_MORPHS );
	}
	
	//! Helper function to get the list of morph frames
	QStringList listFrames( const NifModel * nif, const QModelIndex & iMorpher )
	{
		QModelIndex iFrames = getFrameArray( nif, iMorpher );
		if ( iFrames.isValid() )
		{
			QStringList list;
			for ( int i = 0; i < nif->rowCount( iFrames ); i++ )
			{
				list << nif->get<QString>( iFrames.child( i, 0 ), "Frame Name" );
			}
			return list;
		}
		else
			return QStringList();
	}
	
};

REGISTER_SPELL( spMorphFrameSave )

