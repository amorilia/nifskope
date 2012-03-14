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

#include "NvTriStrip/qtwrapper.h"

// TODO: Move these to blocks.h / misc.h / wherever
template <typename T> void copyArray( NifModel * nif, const QModelIndex & iDst, const QModelIndex & iSrc )
{
	if ( iDst.isValid() && iSrc.isValid() )
	{
		nif->updateArray( iDst );
		nif->setArray<T>( iDst, nif->getArray<T>( iSrc ) );
	}
}

template <typename T> void copyArray( NifModel * nif, const QModelIndex & iDst, const QModelIndex & iSrc, const QString & name )
{
	copyArray<T>( nif, nif->getIndex( iDst, name ), nif->getIndex( iSrc, name ) );
}

template <typename T> void copyValue( NifModel * nif, const QModelIndex & iDst, const QModelIndex & iSrc, const QString & name )
{
	nif->set<T>( iDst, name, nif->get<T>( iSrc, name ) );
}


class spStrippify : public Spell
{
	QString name() const { return Spell::tr("Stripify"); }
	QString page() const { return Spell::tr("Mesh"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return nif->checkVersion( NF_V10000000, 0 ) && nif->isNiBlock( index, T_NITRISHAPE );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QPersistentModelIndex idx = index;
		QPersistentModelIndex iData = nif->getBlock( nif->getLink( idx, TA_DATA ), T_NITRISHAPEDATA );
		
		if ( ! iData.isValid() )	return idx;
		
		QVector<Triangle> triangles;
		QModelIndex iTriangles = nif->getIndex( iData, TA_TRIANGLES );
		if ( iTriangles.isValid() )
		{
			int skip = 0;
			for ( int t = 0; t < nif->rowCount( iTriangles ); t++ )
			{
				Triangle tri = nif->get<Triangle>( iTriangles.child( t, 0 ) );
				if ( tri[0] != tri[1] && tri[1] != tri[2] && tri[2] != tri[0] )
					triangles.append( tri );
				else
					skip++;
			}
			//qWarning() << "num triangles" << triangles.count() << "skipped" << skip;
		}
		else
			return idx;
		
		QList< QVector<quint16> > strips = stripify ( triangles );
		
		if ( strips.count() <= 0 )
			return idx;
		
		nif->insertNiBlock( T_NITRISTRIPSDATA, nif->getBlockNumber( idx )+1 );
		QModelIndex iStripData = nif->getBlock( nif->getBlockNumber( idx ) + 1, T_NITRISTRIPSDATA );
		if ( iStripData.isValid() )
		{
			copyValue<int>( nif, iStripData, iData, TA_NUMVERTICES );
			
			nif->set<int>( iStripData, TA_HASVERTICES, 1 );
			copyArray<Vector3>( nif, iStripData, iData, TA_VERTICES );
			
			copyValue<int>( nif, iStripData, iData, TA_HASNORMALS );
			copyArray<Vector3>( nif, iStripData, iData, TA_NORMALS );
			
			copyValue<int>( nif, iStripData, iData, TA_TSPACEFLAG );
			copyArray<Vector3>( nif, iStripData, iData, TA_BINORMALS );
			copyArray<Vector3>( nif, iStripData, iData, TA_TANGENTS );
			
			copyValue<int>( nif, iStripData, iData, "Has Vertex Colors" );
			copyArray<Color4>( nif, iStripData, iData, TA_VERTEXCOLORS );
			
			copyValue<int>( nif, iStripData, iData, TA_HASUV );
			copyValue<int>( nif, iStripData, iData, TA_NUMUVSETS );
			copyValue<int>( nif, iStripData, iData, "BS Num UV Sets" );
			copyValue<int>( nif, iStripData, iData, TA_NUMUVSETS2 );
			QModelIndex iDstUV = nif->getIndex( iStripData, TA_UVSETS );
			QModelIndex iSrcUV = nif->getIndex( iData, TA_UVSETS );
			if ( iDstUV.isValid() && iSrcUV.isValid() )
			{
				nif->updateArray( iDstUV );
				for ( int r = 0; r < nif->rowCount( iDstUV ); r++ )
				{
					copyArray<Vector2>( nif, iDstUV.child( r, 0 ), iSrcUV.child( r, 0 ) );
				}
			}
			iDstUV = nif->getIndex( iStripData, TA_UVSETS2 );
			iSrcUV = nif->getIndex( iData, TA_UVSETS2 );
			if ( iDstUV.isValid() && iSrcUV.isValid() )
			{
				nif->updateArray( iDstUV );
				for ( int r = 0; r < nif->rowCount( iDstUV ); r++ )
				{
					copyArray<Vector2>( nif, iDstUV.child( r, 0 ), iSrcUV.child( r, 0 ) );
				}
			}
			
			copyValue<Vector3>( nif, iStripData, iData, TA_CENTER );
			copyValue<float>( nif, iStripData, iData, TA_RADIUS );
			
			nif->set<int>( iStripData, TA_NUMSTRIPS, strips.count() );
			nif->set<int>( iStripData, TA_HASPOINTS, 1 );
			
			QModelIndex iLengths = nif->getIndex( iStripData, TA_STRIPLENGTHS );
			QModelIndex iPoints = nif->getIndex( iStripData, TA_POINTS );
			
			if ( iLengths.isValid() && iPoints.isValid() )
			{
				nif->updateArray( iLengths );
				nif->updateArray( iPoints );
				int x = 0;
				int z = 0;
				foreach ( QVector<quint16> strip, strips )
				{
					nif->set<int>( iLengths.child( x, 0 ), strip.count() );
					QModelIndex iStrip = iPoints.child( x, 0 );
					nif->updateArray( iStrip );
					nif->setArray<quint16>( iStrip, strip );
					x++;
					z += strip.count() - 2;
				}
				nif->set<int>( iStripData, TA_NUMTRIANGLES, z );
				
				nif->setData( idx.sibling( idx.row(), NifModel::NameCol ), T_NITRISTRIPS );
				int lnk = nif->getLink( idx, TA_DATA );
				nif->setLink( idx, TA_DATA, nif->getBlockNumber( iStripData ) );
				nif->removeNiBlock( lnk );
			}
		}
		return idx;
	}
};

REGISTER_SPELL( spStrippify )


class spStrippifyAll : public Spell
{
public:
	QString name() const { return Spell::tr("Stripify all TriShapes"); }
	QString page() const { return Spell::tr("Optimize"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return nif->checkVersion( NF_V10000000, 0 ) && ! index.isValid();
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & )
	{
		QList<QPersistentModelIndex> iTriShapes;
		
		for ( int l = 0; l < nif->getBlockCount(); l++ )
		{
			QModelIndex idx = nif->getBlock( l, T_NITRISHAPE );
			if ( idx.isValid() )
				iTriShapes << idx;
		}
		
		spStrippify Stripper;
		
		foreach ( QModelIndex idx, iTriShapes )
			Stripper.castIfApplicable( nif, idx );
		
		return QModelIndex();
	}
};

REGISTER_SPELL( spStrippifyAll )


class spTriangulate : public Spell
{
	QString name() const { return Spell::tr("Triangulate"); }
	QString page() const { return Spell::tr("Mesh"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return nif->isNiBlock( index, T_NITRISTRIPS );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QPersistentModelIndex idx = index;
		QPersistentModelIndex iStripData = nif->getBlock( nif->getLink( idx, TA_DATA ), T_NITRISTRIPSDATA );
		
		if ( ! iStripData.isValid() )	return idx;
		
		QList< QVector<quint16> > strips;

		QModelIndex iPoints = nif->getIndex( iStripData, TA_POINTS );
		
		if ( ! iPoints.isValid() ) return idx;
		
		for ( int s = 0; s < nif->rowCount( iPoints ); s++ )
		{
			QVector<quint16> strip;
			QModelIndex iStrip = iPoints.child( s, 0 );
			for ( int p = 0; p < nif->rowCount( iStrip ); p++ )
				strip.append( nif->get<int>( iStrip.child( p, 0 ) ) );
			strips.append( strip );
		}
		
		QVector<Triangle> triangles = triangulate ( strips );
		
		nif->insertNiBlock( T_NITRISHAPEDATA, nif->getBlockNumber( idx ) + 1 );
		QModelIndex iTriData = nif->getBlock( nif->getBlockNumber( idx ) + 1, T_NITRISHAPEDATA );
		if ( iTriData.isValid() )
		{
			copyValue<int>( nif, iTriData, iStripData, TA_NUMVERTICES );
			
			nif->set<int>( iTriData, TA_HASVERTICES, 1 );
			copyArray<Vector3>( nif, iTriData, iStripData, TA_VERTICES );
			
			copyValue<int>( nif, iTriData, iStripData, TA_HASNORMALS );
			copyArray<Vector3>( nif, iTriData, iStripData, TA_NORMALS );
			
			copyValue<int>( nif, iTriData, iStripData, TA_TSPACEFLAG );
			copyArray<Vector3>( nif, iTriData, iStripData, TA_BINORMALS );
			copyArray<Vector3>( nif, iTriData, iStripData, TA_TANGENTS );
			
			copyValue<int>( nif, iTriData, iStripData, "Has Vertex Colors" );
			copyArray<Color4>( nif, iTriData, iStripData, TA_VERTEXCOLORS );
			
			copyValue<int>( nif, iTriData, iStripData, TA_HASUV );
			copyValue<int>( nif, iTriData, iStripData, TA_NUMUVSETS );
			copyValue<int>( nif, iTriData, iStripData, "BS Num UV Sets" );
			copyValue<int>( nif, iTriData, iStripData, TA_NUMUVSETS2 );
			QModelIndex iDstUV = nif->getIndex( iTriData, TA_UVSETS );
			QModelIndex iSrcUV = nif->getIndex( iStripData, TA_UVSETS );
			if ( iDstUV.isValid() && iSrcUV.isValid() )
			{
				nif->updateArray( iDstUV );
				for ( int r = 0; r < nif->rowCount( iDstUV ); r++ )
				{
					copyArray<Vector2>( nif, iDstUV.child( r, 0 ), iSrcUV.child( r, 0 ) );
				}
			}
			iDstUV = nif->getIndex( iTriData, TA_UVSETS2 );
			iSrcUV = nif->getIndex( iStripData, TA_UVSETS2 );
			if ( iDstUV.isValid() && iSrcUV.isValid() )
			{
				nif->updateArray( iDstUV );
				for ( int r = 0; r < nif->rowCount( iDstUV ); r++ )
				{
					copyArray<Vector2>( nif, iDstUV.child( r, 0 ), iSrcUV.child( r, 0 ) );
				}
			}
			
			copyValue<Vector3>( nif, iTriData, iStripData, TA_CENTER );
			copyValue<float>( nif, iTriData, iStripData, TA_RADIUS );
			
			nif->set<int>( iTriData, TA_NUMTRIANGLES, triangles.count() );
			nif->set<int>( iTriData, TA_NUMTRIANGLEPOINTS, triangles.count() * 3 );
			nif->set<int>( iTriData, TA_HASTRIANGLES, 1 );
			
			QModelIndex iTriangles = nif->getIndex( iTriData, TA_TRIANGLES );
			if ( iTriangles.isValid() )
			{
				nif->updateArray( iTriangles );
				nif->setArray<Triangle>( iTriangles, triangles );
			}
			
			nif->setData( idx.sibling( idx.row(), NifModel::NameCol ), T_NITRISHAPE );
			int lnk = nif->getLink( idx, TA_DATA );
			nif->setLink( idx, TA_DATA, nif->getBlockNumber( iTriData ) );
			nif->removeNiBlock( lnk );
		}
		return idx;
	}
};

REGISTER_SPELL( spTriangulate )


class spStichStrips : public Spell
{
public:
	QString name() const { return Spell::tr("Stich Strips"); }
	QString page() const { return Spell::tr("Mesh"); }
	
	static QModelIndex getStripsData( const NifModel * nif, const QModelIndex & index )
	{
		if ( nif->isNiBlock( index, T_NITRISTRIPS ) )
			return nif->getBlock( nif->getLink( index, TA_DATA ), T_NITRISTRIPSDATA );
		else
			return nif->getBlock( index, T_NITRISTRIPSDATA );
	}
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iData = getStripsData( nif, index );
		return iData.isValid() && nif->get<int>( iData, TA_NUMSTRIPS ) > 1;
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iData = getStripsData( nif, index );
		QModelIndex iLength = nif->getIndex( iData, TA_STRIPLENGTHS );
		QModelIndex iPoints = nif->getIndex( iData, TA_POINTS );
		if ( ! ( iLength.isValid() && iPoints.isValid() ) )
			return index;
		
		QList< QVector<quint16> > strips;
		for ( int r = 0; r < nif->rowCount( iPoints ); r++ )
			strips += nif->getArray<quint16>( iPoints.child( r, 0 ) );
		
		if ( strips.isEmpty() )
			return index;
		
		QVector<quint16> strip = strips.first();
		strips.pop_front();
		
		foreach ( QVector<quint16> s, strips )
		{	// TODO: optimize this
			if ( strip.count() & 1 )
				strip << strip.last() << s.first() << s.first() << s;
			else
				strip << strip.last() << s.first() << s;
		}
		
		nif->set<int>( iData, TA_NUMSTRIPS, 1 );
		nif->updateArray( iLength );
		nif->set<int>( iLength.child( 0, 0 ), strip.size() );
		nif->updateArray( iPoints );
		nif->updateArray( iPoints.child( 0, 0 ) );
		nif->setArray<quint16>( iPoints.child( 0, 0 ), strip );
		
		return index;
	}
};

REGISTER_SPELL( spStichStrips )


class spUnstichStrips : public Spell
{
public:
	QString name() const { return Spell::tr("Unstich Strips"); }
	QString page() const { return Spell::tr("Mesh"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iData = spStichStrips::getStripsData( nif, index );
		return iData.isValid() && nif->get<int>( iData, TA_NUMSTRIPS ) == 1;
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iData = spStichStrips::getStripsData( nif, index );
		QModelIndex iLength = nif->getIndex( iData, TA_STRIPLENGTHS );
		QModelIndex iPoints = nif->getIndex( iData, TA_POINTS );
		if ( ! ( iLength.isValid() && iPoints.isValid() ) )
			return index;
		
		QVector< quint16 > strip = nif->getArray<quint16>( iPoints.child( 0, 0 ) );
		if ( strip.size() <= 3 ) return index;
		
		QList< QVector< quint16 > > strips;
		QVector< quint16 > scratch;
		
		quint16 a = strip[0];
		quint16 b = strip[1];
		bool flip = false;
		for ( int s = 2; s < strip.size(); s++ )
		{
			quint16 c = strip[s];
			
			if ( a != b && b != c && c != a )
			{
				if ( scratch.isEmpty() )
				{
					if ( flip )
						scratch << a << a << b;
					else
						scratch << a << b;
				}
				scratch << c;
			}
			else if ( ! scratch.isEmpty() )
			{
				strips << scratch;
				scratch.clear();
			}
			
			a = b;
			b = c;
			flip = ! flip;
		}
		if ( ! scratch.isEmpty() )
			strips << scratch;
		
		nif->set<int>( iData, TA_NUMSTRIPS, strips.size() );
		nif->updateArray( iLength );
		nif->updateArray( iPoints );
		for ( int r = 0; r < strips.count(); r++ )
		{
			nif->set<int>( iLength.child( r, 0 ), strips[r].size() );
			nif->updateArray( iPoints.child( r, 0 ) );
			nif->setArray<quint16>( iPoints.child( r, 0 ), strips[r] );
		}
		
		return index;
	}
};

REGISTER_SPELL( spUnstichStrips )

