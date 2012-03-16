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

#include <QBuffer>
#include <QDebug>
#include <QMessageBox>

#include "blocks.h"
#include "mesh.h"
#include "tangentspace.h"
#include "transform.h"

// Brief description is deliberately not autolinked to class Spell
/*! \file optimize.cpp
 * \brief Optimization spells
 *
 * All classes here inherit from the Spell class.
 */

//! Combines properties
/*!
 * This has a tendency to fail due to supposedly boolean values in many NIFs
 * having values apart from 0 and 1.
 *
 * \sa spCombiTris
 * \sa spUniqueProps
 */
class spCombiProps : public Spell
{
public:
	QString name() const { return Spell::tr( "Combine Properties" ); }
	QString page() const { return Spell::tr( "Optimize" ); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return nif && ! index.isValid();
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & )
	{
		int numRemoved = 0;
		QMap<qint32,QByteArray> props;
		QMap<qint32,qint32> map;
		do
		{
			props.clear();
			map.clear();
			
			for ( qint32 b = 0; b < nif->getBlockCount(); b++ )
			{
				QModelIndex iBlock = nif->getBlock( b );
				if ( nif->isNiBlock( iBlock, T_NIMATERIALPROPERTY ) )
				{
					if ( nif->get<QString>( iBlock, TA_NAME ).contains( TA_MATERIAL ) )
						nif->set<QString>( iBlock, TA_NAME, TA_MATERIAL );
					else if ( nif->get<QString>( iBlock, TA_NAME ).contains( "Default" ) )
						nif->set<QString>( iBlock, TA_NAME, "Default" );
				}
				if ( nif->inherits( iBlock, T_BSSHADERPROPERTY ) || nif->isNiBlock( iBlock, T_BSSHADERTEXTURESET) )
				{
					// these need to be unique
					continue;
				}
				if ( nif->inherits( iBlock, T_NIPROPERTY ) || nif->inherits( iBlock, T_NISOURCETEXTURE ) )
				{
					QBuffer data;
					data.open( QBuffer::WriteOnly );
					data.write( nif->itemName( iBlock ).toAscii() );
					nif->save( data, iBlock );
					props.insert( b, data.buffer() );
				}
			}
			
			foreach ( qint32 x, props.keys() )
			{
				foreach ( qint32 y, props.keys() )
				{
					if ( x < y && ( ! map.contains( y ) ) && props[x].size() == props[y].size() )
					{
						int c = 0;
						while ( c < props[x].size() )
							if ( props[x][c] == props[y][c] )
								c++;
							else
								break;
						if ( c == props[x].size() )
							map.insert( y, x );
					}
				}
			}
			
			if ( ! map.isEmpty() )
			{
				numRemoved += map.count();
				nif->mapLinks( map );
				QList<qint32> l = map.keys();
				qSort( l.begin(), l.end(), qGreater<qint32>() );
				foreach ( qint32 b, l )
					nif->removeNiBlock( b );
			}
			
		} while ( ! map.isEmpty() );
		
		QMessageBox::information( 0 , APP, QString("removed %1 properties").arg(numRemoved) );
		return QModelIndex();
	}
};

REGISTER_SPELL( spCombiProps )

//! Creates unique properties from shared ones
/*!
 * \sa spDuplicateBlock
 * \sa spCombiProps
 */
class spUniqueProps : public Spell
{
public:
	QString name() const { return Spell::tr( "Split Properties" ); }
	QString page() const { return Spell::tr( "Optimize" ); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return nif && ! index.isValid();
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		for ( int b = 0; b < nif->getBlockCount(); b++ )
		{
			QModelIndex iAVObj = nif->getBlock( b, T_NIAVOBJECT );
			if ( iAVObj.isValid() )
			{
				QVector<qint32> props = nif->getLinkArray( iAVObj, TA_PROPERTIES );
				QMutableVectorIterator<qint32> it( props );
				while ( it.hasNext() )
				{
					qint32 & l = it.next();
					QModelIndex iProp = nif->getBlock( l, T_NIPROPERTY );
					if ( iProp.isValid() && nif->getParent( l ) != b )
					{
						QMap<qint32,qint32> map;
						if ( nif->isNiBlock( iProp, T_NITEXTURINGPROPERTY ) )
						{
							foreach ( qint32 sl, nif->getChildLinks( nif->getBlockNumber( iProp ) ) )
							{
								QModelIndex iSrc = nif->getBlock( sl, T_NISOURCETEXTURE );
								if ( iSrc.isValid() && ! map.contains( sl ) )
								{
									QModelIndex iSrc2 = nif->insertNiBlock( T_NISOURCETEXTURE, nif->getBlockCount() + 1 );
									QBuffer buffer;
									buffer.open( QBuffer::WriteOnly );
									nif->save( buffer, iSrc );
									buffer.close();
									buffer.open( QBuffer::ReadOnly );
									nif->load( buffer, iSrc2 );
									map[ sl ] = nif->getBlockNumber( iSrc2 );
								}
							}
						}
						
						QModelIndex iProp2 = nif->insertNiBlock( nif->itemName( iProp ), nif->getBlockCount() + 1 );
						QBuffer buffer;
						buffer.open( QBuffer::WriteOnly );
						nif->save( buffer, iProp );
						buffer.close();
						buffer.open( QBuffer::ReadOnly );
						nif->loadAndMapLinks( buffer, iProp2, map );
						l = nif->getBlockNumber( iProp2 );
					}
				}
				nif->setLinkArray( iAVObj, TA_PROPERTIES, props );
			}
		}
		return index;
	}
};

REGISTER_SPELL( spUniqueProps )

//! Removes nodes with no children and singular parents
/*!
 * Note that the user might lose "important" named nodes with this; short of
 * asking for confirmation or simply reporting nodes instead of removing
 * them, there's not much that can be done to prevent a NIF that won't work
 * ingame.
 */
class spRemoveBogusNodes : public Spell
{
public:
	QString name() const { return Spell::tr( "Remove Bogus Nodes" ); }
	QString page() const { return Spell::tr( "Optimize" ); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return nif && ! index.isValid();
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		Q_UNUSED(index);
		bool removed;
		int cnt = 0;
		do
		{
			removed = false;
			for ( int b = 0; b < nif->getBlockCount(); b++ )
			{
				QModelIndex iNode = nif->getBlock( b, T_NINODE );
				if ( iNode.isValid() )
				{
					if ( nif->getChildLinks( b ).isEmpty() && nif->getParentLinks( b ).isEmpty() )
					{
						int x = 0;
						for ( int c = 0; c < nif->getBlockCount(); c++ )
						{
							if ( c != b )
							{
								if ( nif->getChildLinks( c ).contains( b ) )
									x++;
								if ( nif->getParentLinks( c ).contains( b ) )
									x = 2;
								if ( x >= 2 )
									break;
							}
						}
						if ( x < 2 )
						{
							removed = true;
							cnt++;
							nif->removeNiBlock( b );
							break;
						}
					}
				}
			}
		} while ( removed );
		
		if ( cnt > 0 )
			QMessageBox::information(0, APP, QString( Spell::tr( "removed %1 nodes" ) ).arg(cnt));
		
		return QModelIndex();
	}
};

REGISTER_SPELL( spRemoveBogusNodes )

//! Combines geometry data
/*!
 * Can fail for a number of reasons, usually due to mismatched properties (see
 * spCombiProps for why that can fail) or non-geometry children (extra data,
 * skin instance etc.).
 */
class spCombiTris : public Spell
{
public:
	QString name() const { return Spell::tr( "Combine Shapes" ); }
	QString page() const { return Spell::tr( "Optimize" ); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return nif && nif->isNiBlock( index, T_NINODE );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		// join meshes which share properties and parent
		// ( animated ones are left untouched )
		
		QPersistentModelIndex iParent( index );
		
		// populate a list of possible candidates
		
		QList<qint32> lTris;
		
		foreach ( qint32 lChild, nif->getLinkArray( iParent, TA_CHILDREN ) )
		{
			if ( nif->getParent( lChild ) == nif->getBlockNumber( iParent ) )
			{
				QModelIndex iChild = nif->getBlock( lChild );
				if ( nif->isNiBlock( iChild, T_NITRISHAPE ) || nif->isNiBlock( iChild, T_NITRISTRIPS ) )
					lTris << lChild;
			}
		}
		
		// detect matches
		
		QMap< qint32, QList< qint32 > > match;
		QList<qint32> found;
		
		foreach ( qint32 lTriA, lTris )
		{
			if ( found.contains( lTriA ) )
				continue;
			foreach ( qint32 lTriB, lTris )
			{	
				if ( matches( nif, nif->getBlock( lTriA ), nif->getBlock( lTriB ) ) )
				{
					match[ lTriA ] << lTriB;
					found << lTriB;
				}
			}
		}
		
		// combine the matches
		
		spApplyTransformation ApplyTransform;
		spTangentSpace TSpace;
		
		QList<QPersistentModelIndex> remove;
		
		foreach ( qint32 lTriA, match.keys() )
		{
			ApplyTransform.cast( nif, nif->getBlock( lTriA ) );
			
			foreach ( qint32 lTriB, match[ lTriA ] )
			{
				ApplyTransform.cast( nif, nif->getBlock( lTriB ) );
				combine( nif, nif->getBlock( lTriA ), nif->getBlock( lTriB ) );
				remove << nif->getBlock( lTriB );
			}
			
			TSpace.castIfApplicable( nif, nif->getBlock( lTriA ) );
		}
		
		// remove the now obsolete shapes
		
		spRemoveBranch BranchRemover;
		
		foreach ( QModelIndex rem, remove )
		{
			BranchRemover.cast( nif, rem );
		}
		
		return iParent;
	}
	
	//! Determine if two shapes are identical
	bool matches( const NifModel * nif, QModelIndex iTriA, QModelIndex iTriB )
	{
		if ( iTriA == iTriB || nif->itemName( iTriA ) != nif->itemName( iTriB )
			|| nif->get<int>( iTriA, TA_FLAGS ) != nif->get<int>( iTriB, TA_FLAGS ) )
			return false;
		
		QVector<qint32> lPrpsA = nif->getLinkArray( iTriA, TA_PROPERTIES );
		QVector<qint32> lPrpsB = nif->getLinkArray( iTriB, TA_PROPERTIES );
		
		qSort( lPrpsA );
		qSort( lPrpsB );
		
		if ( lPrpsA != lPrpsB )
			return false;
		
		foreach ( qint32 l, nif->getChildLinks( nif->getBlockNumber( iTriA ) ) )
		{
			if ( lPrpsA.contains( l ) ) continue;
			QModelIndex iBlock = nif->getBlock( l );
			if ( nif->isNiBlock( iBlock, T_NITRISHAPEDATA ) )
				continue;
			if ( nif->isNiBlock( iBlock, T_NITRISTRIPSDATA ) )
				continue;
			if ( nif->isNiBlock( iBlock, T_NIBINARYEXTRADATA ) && nif->get<QString>( iBlock, TA_NAME ) == STR_TS )
				continue;
			qWarning() << "Attached " << nif->itemName( iBlock ) << " prevents " << nif->get<QString>( iTriA, TA_NAME ) << " and " << nif->get<QString>( iTriB, TA_NAME ) << " from matching.";
			return false;
		}
		
		foreach ( qint32 l, nif->getChildLinks( nif->getBlockNumber( iTriB ) ) )
		{
			if ( lPrpsB.contains( l ) )
				continue;
			QModelIndex iBlock = nif->getBlock( l );
			if ( nif->isNiBlock( iBlock, T_NITRISHAPEDATA ) )
				continue;
			if ( nif->isNiBlock( iBlock, T_NITRISTRIPSDATA ) )
				continue;
			if ( nif->isNiBlock( iBlock, T_NIBINARYEXTRADATA ) && nif->get<QString>( iBlock, TA_NAME ) == STR_TS )
				continue;
			qWarning() << "Attached " << nif->itemName( iBlock ) << " prevents " << nif->get<QString>( iTriA, TA_NAME ) << " and " << nif->get<QString>( iTriB, TA_NAME ) << " from matching.";
			return false;
		}
		
		QModelIndex iDataA = nif->getBlock( nif->getLink( iTriA, TA_DATA ), T_NITRIBASEDGEOMDATA );
		QModelIndex iDataB = nif->getBlock( nif->getLink( iTriB, TA_DATA ), T_NITRIBASEDGEOMDATA );
		
		return dataMatches( nif, iDataA, iDataB );
	}
	
	//! Determines if two sets of shape data are identical
	bool dataMatches( const NifModel * nif, QModelIndex iDataA, QModelIndex iDataB )
	{
		if ( iDataA == iDataB )
			return true;
			
		foreach ( QString id, QStringList() << TA_VERTICES << TA_NORMALS << TA_VERTEXCOLORS << TA_UVSETS )
		{
			QModelIndex iA = nif->getIndex( iDataA, id );
			QModelIndex iB = nif->getIndex( iDataB, id );
			
			if ( iA.isValid() != iB.isValid() )
				return false;
			
			if ( id == TA_UVSETS && nif->rowCount( iA ) != nif->rowCount( iB ) )
				return false;
		}
		return true;
	}
	
	//! Combines meshes a and b ( a += b )
	void combine( NifModel * nif, QModelIndex iTriA, QModelIndex iTriB )
	{
		nif->set<quint32>( iTriB, TA_FLAGS, nif->get<quint32>( iTriB, TA_FLAGS ) | 1 );
		
		QModelIndex iDataA = nif->getBlock( nif->getLink( iTriA, TA_DATA ), T_NITRIBASEDGEOMDATA );
		QModelIndex iDataB = nif->getBlock( nif->getLink( iTriB, TA_DATA ), T_NITRIBASEDGEOMDATA );
		
		int numA = nif->get<int>( iDataA, TA_NUMVERTICES );
		int numB = nif->get<int>( iDataB, TA_NUMVERTICES );
		nif->set<int>( iDataA, TA_NUMVERTICES, numA + numB );
		
		nif->updateArray( iDataA, TA_VERTICES );
		nif->setArray<Vector3>( iDataA, TA_VERTICES, nif->getArray<Vector3>( iDataA, TA_VERTICES ).mid( 0, numA ) + nif->getArray<Vector3>( iDataB, TA_VERTICES ) );
		
		nif->updateArray( iDataA, TA_NORMALS );
		nif->setArray<Vector3>( iDataA, TA_NORMALS, nif->getArray<Vector3>( iDataA, TA_NORMALS ).mid( 0, numA ) + nif->getArray<Vector3>( iDataB, TA_NORMALS ) );
		
		nif->updateArray( iDataA, TA_VERTEXCOLORS );
		nif->setArray<Color4>( iDataA, TA_VERTEXCOLORS, nif->getArray<Color4>( iDataA, TA_VERTEXCOLORS ).mid( 0, numA ) + nif->getArray<Color4>( iDataB, TA_VERTEXCOLORS ) );
		
		QModelIndex iUVa = nif->getIndex( iDataA, TA_UVSETS );
		QModelIndex iUVb = nif->getIndex( iDataB, TA_UVSETS );
		
		for ( int r = 0; r < nif->rowCount( iUVa ); r++ )
		{
			nif->updateArray( iUVa.child( r, 0 ) );
			nif->setArray<Vector2>( iUVa.child( r, 0 ), nif->getArray<Vector2>( iUVa.child( r, 0 ) ).mid( 0, numA ) + nif->getArray<Vector2>( iUVb.child( r, 0 ) ) );
		}
		
		int triCntA = nif->get<int>( iDataA, TA_NUMTRIANGLES );
		int triCntB = nif->get<int>( iDataB, TA_NUMTRIANGLES );
		nif->set<int>( iDataA, TA_NUMTRIANGLES, triCntA + triCntB );
		nif->set<int>( iDataA, TA_NUMTRIANGLEPOINTS, ( triCntA + triCntB ) * 3 );
		
		QVector<Triangle> triangles = nif->getArray<Triangle>( iDataB, TA_TRIANGLES );
		QMutableVectorIterator<Triangle> itTri( triangles );
		while ( itTri.hasNext() )
		{
			Triangle & tri = itTri.next();
			tri[0] += numA;
			tri[1] += numA;
			tri[2] += numA;
		}
		nif->updateArray( iDataA, TA_TRIANGLES );
		nif->setArray<Triangle>( iDataA, TA_TRIANGLES, triangles + nif->getArray<Triangle>( iDataA, TA_TRIANGLES ) );
		
		int stripCntA = nif->get<int>( iDataA, TA_NUMSTRIPS );
		int stripCntB = nif->get<int>( iDataB, TA_NUMSTRIPS );
		nif->set<int>( iDataA, TA_NUMSTRIPS, stripCntA + stripCntB );
		
		nif->updateArray( iDataA, TA_STRIPLENGTHS );
		nif->updateArray( iDataA, TA_POINTS );
		for ( int r = 0; r < stripCntB; r++ )
		{
			QVector<quint16> strip = nif->getArray<quint16>( nif->getIndex( iDataB, TA_POINTS ).child( r, 0 ) );
			QMutableVectorIterator<quint16> it( strip );
			while ( it.hasNext() )
				it.next() += numA;
			nif->set<int>( nif->getIndex( iDataA, TA_STRIPLENGTHS ).child( r + stripCntA, 0 ), strip.size() );
			nif->updateArray( nif->getIndex( iDataA, TA_POINTS ).child( r + stripCntA, 0 ) );
			nif->setArray<quint16>( nif->getIndex( iDataA, TA_POINTS ).child( r + stripCntA, 0 ), strip );
		}
		
		spUpdateCenterRadius CenterRadius;
		CenterRadius.castIfApplicable( nif, iDataA );
	}
};

REGISTER_SPELL( spCombiTris )
