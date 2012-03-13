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

#include "mesh.h"

#include <QDebug>
#include <QDialog>
#include <QGridLayout>

#include <cfloat>

// Brief description is deliberately not autolinked to class Spell
/*! \file mesh.cpp
 * \brief Mesh spells
 *
 * All classes here inherit from the Spell class.
 */

//! Find shape data of triangle geometry
static QModelIndex getShape( const NifModel * nif, const QModelIndex & index )
{
	QModelIndex iShape = nif->getBlock( index );
	if ( nif->isNiBlock( iShape, "NiTriBasedGeomData" ) )
		iShape = nif->getBlock( nif->getParent( nif->getBlockNumber( iShape ) ) );
	if ( nif->isNiBlock( iShape, T_NITRISHAPE ) || nif->isNiBlock( index, T_NITRISTRIPS ) )
		if ( nif->getBlock( nif->getLink( iShape, TA_DATA ), "NiTriBasedGeomData" ).isValid() )
			return iShape;
	return QModelIndex();
}

//! Find triangle geometry
/*!
 * Subtly different to getShape(); that requires
 * <tt>nif->getBlock( nif->getLink( getShape( nif, index ), TA_DATA ) );</tt>
 * to return the same result.
 */
static QModelIndex getTriShapeData( const NifModel * nif, const QModelIndex & index )
{
	QModelIndex iData = nif->getBlock( index );
	if ( nif->isNiBlock( index, T_NITRISHAPE ) )
		iData = nif->getBlock( nif->getLink( index, TA_DATA ) );
	if ( nif->isNiBlock( iData, T_NITRISHAPEDATA ) )
		return iData;
	else return QModelIndex();
}

//! Removes elements of the specified type from an array
template <typename T> static void removeFromArray( QVector<T> & array, QMap<quint16, bool> map )
{
	for ( int x = array.count() - 1; x >= 0; x-- )
	{
		if ( ! map.contains( x ) )
			array.remove( x );
	}
}

//! Removes waste vertices from the specified data and shape
static void removeWasteVertices( NifModel * nif, const QModelIndex & iData, const QModelIndex & iShape )
{
	try
	{
		// read the data
		
		QVector<Vector3> verts = nif->getArray<Vector3>( iData, TA_VERTICES );
		if ( ! verts.count() ) {
			throw QString( Spell::tr("no vertices?") );
		}
		QVector<Vector3> norms = nif->getArray<Vector3>( iData, TA_NORMALS );
		QVector<Color4> colors = nif->getArray<Color4>( iData, TA_VERTEXCOLORS );
		QList< QVector<Vector2> > texco;
		QModelIndex iUVSets = nif->getIndex( iData, TA_UVSETS );
		for ( int r = 0; r < nif->rowCount( iUVSets ); r++ )
		{
			texco << nif->getArray<Vector2>( iUVSets.child( r, 0 ) );
			if ( texco.last().count() != verts.count() )
				throw QString( "uv array size differs" );
		}
		
		int numVerts = verts.count();
		
		if ( numVerts != nif->get<int>( iData, TA_NUMVERTICES ) ||
			( norms.count() && norms.count() != numVerts ) ||
			( colors.count() && colors.count() != numVerts ) )
		{
			throw QString( "vertex array size differs" );
		}
		
		// detect unused vertices
		
		QMap<quint16, bool> used;
		
		QVector<Triangle> tris = nif->getArray<Triangle>( iData, TA_TRIANGLES );
		foreach ( Triangle tri, tris )
		{
			for ( int t = 0; t < 3; t++ )
				used.insert( tri[t], true );
		}
		
		QList< QVector< quint16 > > strips;
		QModelIndex iPoints = nif->getIndex( iData, TA_POINTS );
		for ( int r = 0; r < nif->rowCount( iPoints ); r++ )
		{
			strips << nif->getArray<quint16>( iPoints.child( r, 0 ) );
			foreach ( quint16 p, strips.last() )
				used.insert( p, true );
		}
		
		// remove them
		
		qWarning() << "removing" << verts.count() - used.count() << "vertices";
		
		if ( verts.count() == used.count() )
			return;
		
		removeFromArray( verts, used );
		removeFromArray( norms, used );
		removeFromArray( colors, used );
		for ( int c = 0; c < texco.count(); c++ )
			removeFromArray( texco[c], used );
		
		// adjust the faces
		
		QMap<quint16,quint16> map;
		quint16 y = 0;
		for ( quint16 x = 0; x < numVerts; x++ )
		{
			if ( used.contains( x ) )
				map.insert( x, y++ );
		}
		
		QMutableVectorIterator<Triangle> itri( tris );
		while ( itri.hasNext() )
		{
			Triangle & tri = itri.next();
			for ( int t = 0; t < 3; t++ )
				if ( map.contains( tri[t] ) )
					tri[t] = map[ tri[t] ];
		}
		
		QMutableListIterator< QVector<quint16> > istrip( strips );
		while ( istrip.hasNext() )
		{
			QVector<quint16> & strip = istrip.next();
			for ( int s = 0; s < strip.size(); s++ )
			{
				if ( map.contains( strip[s] ) )
					strip[s] = map[ strip[s] ];
			}
		}
		
		// write back the data
		
		nif->setArray<Triangle>( iData, TA_TRIANGLES, tris );
		for ( int r = 0; r < nif->rowCount( iPoints ); r++ )
			nif->setArray<quint16>( iPoints.child( r, 0 ), strips[r] );
		nif->set<int>( iData, TA_NUMVERTICES, verts.count() );
		nif->updateArray( iData, TA_VERTICES );
		nif->setArray<Vector3>( iData, TA_VERTICES, verts );
		nif->updateArray( iData, TA_NORMALS );
		nif->setArray<Vector3>( iData, TA_NORMALS, norms );
		nif->updateArray( iData, TA_VERTEXCOLORS );
		nif->setArray<Color4>( iData, TA_VERTEXCOLORS, colors );
		for ( int r = 0; r < nif->rowCount( iUVSets ); r++ )
		{
			nif->updateArray( iUVSets.child( r, 0 ) );
			nif->setArray<Vector2>( iUVSets.child( r, 0 ), texco[r] );
		}
		
		// process NiSkinData
		
		QModelIndex iSkinInst = nif->getBlock( nif->getLink( iShape, "Skin Instance" ), T_NISKININSTANCE );
		
		QModelIndex iSkinData = nif->getBlock( nif->getLink( iSkinInst, TA_DATA ), T_NISKINDATA );
		QModelIndex iBones = nif->getIndex( iSkinData, TA_BONELIST );
		for ( int b = 0; b < nif->rowCount( iBones ); b++ )
		{
			QVector< QPair<int,float> > weights;
			QModelIndex iWeights = nif->getIndex( iBones.child( b, 0 ), "Vertex Weights" );
			for ( int w = 0; w < nif->rowCount( iWeights ); w++ )
			{
				weights.append( QPair<int,float>( nif->get<int>( iWeights.child( w, 0 ), TA_INDEX ), nif->get<float>( iWeights.child( w, 0 ), "Weight" ) ) );
			}
			
			for ( int x = weights.count() - 1; x >= 0; x-- )
			{
				if ( ! used.contains( weights[x].first ) )
					weights.remove( x );
			}
			
			QMutableVectorIterator< QPair< int, float > > it( weights );
			while ( it.hasNext() )
			{
				QPair<int,float> & w = it.next();
				if ( map.contains( w.first ) )
					w.first = map[ w.first ];
			}
			
			nif->set<int>( iBones.child( b, 0 ), TA_NUMVERTICES, weights.count() );
			nif->updateArray( iWeights );
			for ( int w = 0; w < weights.count(); w++ )
			{
				nif->set<int>( iWeights.child( w, 0 ), TA_INDEX, weights[w].first );
				nif->set<float>( iWeights.child( w, 0 ), "Weight", weights[w].second );
			}
		}
		
		// process NiSkinPartition
		
		QModelIndex iSkinPart = nif->getBlock( nif->getLink( iSkinInst, TA_SKINPARTITION ), T_NISKINPARTITION );
		if ( ! iSkinPart.isValid() )
			iSkinPart = nif->getBlock( nif->getLink( iSkinData, TA_SKINPARTITION ), T_NISKINPARTITION );
		if ( iSkinPart.isValid() )
		{
			nif->removeNiBlock( nif->getBlockNumber( iSkinPart ) );
			qWarning() << "the skin partition was removed, please add it again with the skin partition spell";
		}
	}
	catch ( QString e )
	{
		qWarning() << e.toAscii().data();
	}
}

//! Flip texture UV coordinates
class spFlipTexCoords : public Spell
{
public:
	QString name() const { return Spell::tr("Flip UV"); }
	QString page() const { return Spell::tr("Mesh"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return nif->itemType( index ).toLower() == "texcoord" || nif->inherits( index, "NiTriBasedGeomData" );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QModelIndex idx = index;
		if ( nif->itemType( index ).toLower() != "texcoord" )
		{
			idx = nif->getIndex( nif->getBlock( index ), TA_UVSETS );
		}
		QMenu menu;
		static const char * const flipCmds[3] = { "S = 1.0 - S", "T = 1.0 - T", "S <=> T" };
		for ( int c = 0; c < 3; c++ )
			menu.addAction( flipCmds[c] );

		QAction * act = menu.exec( QCursor::pos() );
		if ( act ) {
			for ( int c = 0; c < 3; c++ )
				if ( act->text() == flipCmds[c] )
					flip( nif, idx, c );
		}

		return index;
	}

	//! Flips UV data in a model index
	void flip( NifModel * nif, const QModelIndex & index, int f )
	{
		if ( nif->isArray( index ) )
		{
			QModelIndex idx = index.child( 0, 0 );
			if ( idx.isValid() )
			{
				if ( nif->isArray( idx ) )
					flip( nif, idx, f );
				else
				{
					QVector<Vector2> tc = nif->getArray<Vector2>( index );
					for ( int c = 0; c < tc.count(); c++ )
						flip( tc[c], f );
					nif->setArray<Vector2>( index, tc );
				}
			}
		}
		else
		{
			Vector2 v = nif->get<Vector2>( index );
			flip( v, f );
			nif->set<Vector2>( index, v );
		}
	}

	//! Flips UV data in a vector
	void flip( Vector2 & v, int f )
	{
		switch ( f )
		{
			case 0:
				v[0] = 1.0 - v[0];
				break;
			case 1:
				v[1] = 1.0 - v[1];
				break;
			default:
				{
					float x = v[0];
					v[0] = v[1];
					v[1] = x;
				}	break;
		}
	}
};

REGISTER_SPELL( spFlipTexCoords )

//! Flips triangle faces, individually or in the selected array
class spFlipFace : public Spell
{
public:
	QString name() const { return Spell::tr("Flip Face"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return ( nif->getValue( index ).type() == NifValue::tTriangle )
			|| ( nif->isArray( index ) && nif->getValue( index.child( 0, 0 ) ).type() == NifValue::tTriangle );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		if ( nif->isArray( index ) )
		{
			QVector<Triangle> tris = nif->getArray<Triangle>( index );
			for ( int t = 0; t < tris.count(); t++ )
				tris[t].flip();
			nif->setArray<Triangle>( index, tris );
		}
		else
		{
			Triangle t = nif->get<Triangle>( index );
			t.flip();
			nif->set<Triangle>( index, t );
		}
		return index;
	}
};

REGISTER_SPELL( spFlipFace )

//! Flips all faces of a triangle based mesh
class spFlipAllFaces : public Spell
{
public:
	QString name() const { return Spell::tr("Flip Faces"); }
	QString page() const { return Spell::tr("Mesh"); }

	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return getTriShapeData( nif, index ).isValid();
	}

	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iData = getTriShapeData( nif, index );

		QVector<Triangle> tris = nif->getArray<Triangle>( iData, TA_TRIANGLES );
		for ( int t = 0; t < tris.count(); t++ )
			tris[t].flip();
		nif->setArray<Triangle>( iData, TA_TRIANGLES, tris );

		return index;
	}
};

REGISTER_SPELL( spFlipAllFaces )

//! Removes redundant triangles from a mesh
class spPruneRedundantTriangles : public Spell
{
public:
	QString name() const { return Spell::tr("Prune Triangles"); }
	QString page() const { return Spell::tr("Mesh"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return getTriShapeData( nif, index ).isValid();
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iData = getTriShapeData( nif, index );
		
		QList<Triangle> tris = nif->getArray<Triangle>( iData, TA_TRIANGLES ).toList();
		int cnt = 0;
		
		int i = 0;
		while ( i < tris.count() )
		{
			const Triangle & t = tris[i];
			if ( t[0] == t[1] || t[1] == t[2] || t[2] == t[0] )
			{
				tris.removeAt( i );
				cnt++;
			}
			else
				i++;
		}
		
		i = 0;
		while ( i < tris.count() )
		{
			const Triangle & t = tris[i];
			
			int j = i + 1;
			while ( j < tris.count() )
			{
				const Triangle & r = tris[j];
				
				if ( ( t[0] == r[0] && t[1] == r[1] && t[2] == r[2] )
					|| ( t[0] == r[1] && t[1] == r[2] && t[2] == r[0] )
					|| ( t[0] == r[2] && t[1] == r[0] && t[2] == r[1] ) )
				{
					tris.removeAt( j );
					cnt++;
				}
				else
					j++;
			}
			i++;
		}
		
		if ( cnt > 0 )
		{
			qWarning() << QString( Spell::tr("%1 triangles removed") ).arg( cnt );
			nif->set<int>( iData, TA_NUMTRIANGLES, tris.count() );
			nif->set<int>( iData, TA_NUMTRIANGLEPOINTS, tris.count() * 3 );
			nif->updateArray( iData, TA_TRIANGLES );
			nif->setArray<Triangle>( iData, TA_TRIANGLES, tris.toVector() );
		}
		return index;
	}
};

REGISTER_SPELL( spPruneRedundantTriangles )

//! Removes duplicate vertices from a mesh
class spRemoveDuplicateVertices : public Spell
{
public:
	QString name() const { return Spell::tr("Remove Duplicate Vertices"); }
	QString page() const { return Spell::tr("Mesh"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return getShape( nif, index ).isValid();
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		try
		{
			QModelIndex iShape = getShape( nif, index );
			QModelIndex iData = nif->getBlock( nif->getLink( iShape, TA_DATA ) );
			
			// read the data
			
			QVector<Vector3> verts = nif->getArray<Vector3>( iData, TA_VERTICES );
			if ( ! verts.count() )
				throw QString( "no vertices?" );
			QVector<Vector3> norms = nif->getArray<Vector3>( iData, TA_NORMALS );
			QVector<Color4> colors = nif->getArray<Color4>( iData, TA_VERTEXCOLORS );
			QList< QVector<Vector2> > texco;
			QModelIndex iUVSets = nif->getIndex( iData, TA_UVSETS );
			for ( int r = 0; r < nif->rowCount( iUVSets ); r++ )
			{
				texco << nif->getArray<Vector2>( iUVSets.child( r, 0 ) );
				if ( texco.last().count() != verts.count() )
					throw QString( Spell::tr("uv array size differs") );
			}
			
			int numVerts = verts.count();
			
			if ( numVerts != nif->get<int>( iData, TA_NUMVERTICES ) ||
				( norms.count() && norms.count() != numVerts ) ||
				( colors.count() && colors.count() != numVerts ) )
			{
				throw QString( Spell::tr("vertex array size differs") );
			}
			
			// detect the dublicates
			
			QMap<quint16,quint16> map;
			
			for ( int a = 0; a < numVerts; a++ )
			{
				Vector3 v = verts[a];
				for ( int b = 0; b < a; b++ )
				{
					if ( ! ( v == verts[b] ) )
						continue;
					if ( norms.count() && ! ( norms[a] == norms[b] ) )
						continue;
					if ( colors.count() && ! ( colors[a] == colors[b] ) )
						continue;
					int t = 0;
					for ( t = 0; t < texco.count(); t++ )
					{
						if ( ! ( texco[t][a] == texco[t][b] ) )
							break;
					}
					if ( t < texco.count() )
						continue;
					
					map.insert( b, a );
				}
			}
			
			//qWarning() << QString( Spell::tr("detected % duplicates") ).arg( map.count() );
			
			// adjust the faces
			
			QVector< Triangle > tris = nif->getArray< Triangle >( iData, TA_TRIANGLES );
			QMutableVectorIterator<Triangle> itri( tris );
			while ( itri.hasNext() )
			{
				Triangle & t = itri.next();
				for ( int p = 0; p < 3; p++ )
					if ( map.contains( t[p] ) )
						t[p] = map.value( t[p] );
			}
			nif->setArray<Triangle>( iData, TA_TRIANGLES, tris );
			
			QModelIndex iPoints = nif->getIndex( iData, TA_POINTS );
			for ( int r = 0; r < nif->rowCount( iPoints ); r++ )
			{
				QVector<quint16> strip = nif->getArray<quint16>( iPoints.child( r, 0 ) );
				QMutableVectorIterator<quint16> istrp( strip );
				while ( istrp.hasNext() )
				{
					quint16 & p = istrp.next();
					if ( map.contains( p ) )
						p = map.value( p );
				}
				nif->setArray<quint16>( iPoints.child( r, 0 ), strip );
			}
			
			// finally, remove the now unused vertices
			
			removeWasteVertices( nif, iData, iShape );
		}
		catch ( QString e )
		{
			qWarning() << e.toAscii().data();
		}
		
		return index;
	}
};

REGISTER_SPELL( spRemoveDuplicateVertices )

//! Removes unused vertices
class spRemoveWasteVertices : public Spell
{
public:
	QString name() const { return Spell::tr("Remove Unused Vertices"); }
	QString page() const { return Spell::tr("Mesh"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return getShape( nif, index ).isValid();
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iShape = getShape( nif, index );
		QModelIndex iData = nif->getBlock( nif->getLink( iShape, TA_DATA ) );
		
		removeWasteVertices( nif, iData, iShape );
		
		return index;
	}
};

REGISTER_SPELL( spRemoveWasteVertices )

/*
 * spUpdateCenterRadius
 */
bool spUpdateCenterRadius::isApplicable( const NifModel * nif, const QModelIndex & index )
{
	return nif->getBlock( index, "NiGeometryData" ).isValid();
}

QModelIndex spUpdateCenterRadius::cast( NifModel * nif, const QModelIndex & index )
{
	QModelIndex iData = nif->getBlock( index );
	
	QVector<Vector3> verts = nif->getArray<Vector3>( iData, TA_VERTICES );
	if ( ! verts.count() )
		return index;
	
	Vector3 center;
	float radius = 0.0f;

	/*
		Oblivion and CT_volatile meshes require a
		different center algorithm
	*/
	if( ( ( nif->getVersionNumber() & 0x14000000 ) && ( nif->getUserVersion() == 11 ) )
		|| ( nif->get<ushort>(iData, "Consistency Flags") & 0x8000 ) )
	{
		/* is a Oblivion mesh! */
		float xMin(FLT_MAX), xMax(-FLT_MAX);
		float yMin(FLT_MAX), yMax(-FLT_MAX);
		float zMin(FLT_MAX), zMax(-FLT_MAX);
		foreach( Vector3 v, verts )
		{
			if( v[0] < xMin )
				xMin = v[0];
			else if ( v[0] > xMax )
				xMax = v[0];

			if( v[1] < yMin )
				yMin = v[1];
			else if ( v[1] > yMax )
				yMax = v[1];

			if( v[2] < zMin )
				zMin = v[2];
			else if ( v[2] > zMax )
				zMax = v[2];
		}

		center = Vector3( xMin + xMax, yMin + yMax, zMin + zMax ) / 2;
	}
	else {
		foreach( Vector3 v, verts )
		{
			center += v;
		}
		center /= verts.count();
	}
	
	float d;
	foreach ( Vector3 v, verts )
	{
		if ( ( d = ( center - v ).length() ) > radius )
			radius = d;
	}
	
	nif->set<Vector3>( iData, TA_CENTER, center );
	nif->set<float>( iData, TA_RADIUS, radius );
	
	return index;
}

REGISTER_SPELL( spUpdateCenterRadius )
