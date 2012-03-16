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

#include "skeleton.h"

#include "gl/gltools.h"

#include "NvTriStrip/qtwrapper.h"

#include <QDebug>
#include <QFile>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>

#define SKEL_DAT ":/res/spells/skel.dat"

// Brief description is deliberately not autolinked to class Spell
/*! \file skeleton.cpp
 * \brief Skeleton spells
 *
 * All classes here inherit from the Spell class.
 */

//! A map of bone names to their transforms
typedef QMap<QString,Transform> TransMap;

//! "Fix" a v4.0.0.2 skeleton
class spFixSkeleton : public Spell
{
public:
	QString name() const { return Spell::tr("Fix Bip01"); }
	QString page() const { return Spell::tr("Skeleton"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return ( nif->getVersion() == STR_V04000002 && nif->itemType( index ) == B_NIBLOCK && nif->get<QString>( index, TA_NAME ) == "Bip01" ); //&& QFile::exists( SKEL_DAT ) );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QFile file( SKEL_DAT );
		if ( file.open( QIODevice::ReadOnly ) )
		{
			QDataStream stream( &file );
			
			TransMap local;
			TransMap world;
			QString name;
			do
			{
				stream >> name;
				if ( !name.isEmpty() )
				{
					Transform t;
					stream >> t;
					local.insert( name, t );
					stream >> t;
					world.insert( name, t );
				}
			}
			while ( ! name.isEmpty() );
			
			TransMap bones;
			doBones( nif, index, Transform(), local, bones );
			
			foreach ( int link, nif->getChildLinks( nif->getBlockNumber( index ) ) )
			{
				QModelIndex iChild = nif->getBlock( link );
				if ( iChild.isValid() )
				{
					if ( nif->itemName( iChild ) == T_NINODE )
					{
						doNodes( nif, iChild, Transform(), world, bones );
					}
					else if ( nif->inherits( iChild, T_NITRIBASEDGEOM ) )
					{
						doShape( nif, iChild, Transform(), world, bones );
					}
				}
			}
			
			nif->reset();
		}
		
		return index;
	}
	
	void doBones( NifModel * nif, const QModelIndex & index, const Transform & tparent, const TransMap & local, TransMap & bones )
	{
		QString name = nif->get<QString>( index, TA_NAME );
		if ( name.startsWith( "Bip01" ) )
		{
			Transform tlocal( nif, index );
			bones.insert( name, tparent * tlocal );
			
			local.value( name ).writeBack( nif, index );
			
			foreach ( int link, nif->getChildLinks( nif->getBlockNumber( index ) ) )
			{
				QModelIndex iChild = nif->getBlock( link, T_NINODE );
				if ( iChild.isValid() )
					doBones( nif, iChild, tparent * tlocal, local, bones );
			}
		}
	}
	
	bool doNodes( NifModel * nif, const QModelIndex & index, const Transform & tparent, const TransMap & world, const TransMap & bones )
	{
		bool hasSkinnedChildren = false;
		
		QString name = nif->get<QString>( index, TA_NAME );
		if ( ! name.startsWith( "Bip01" ) )
		{
			Transform tlocal( nif, index );
			
			foreach ( int link, nif->getChildLinks( nif->getBlockNumber( index ) ) )
			{
				QModelIndex iChild = nif->getBlock( link );
				if ( iChild.isValid() )
				{
					if ( nif->itemName( iChild ) == T_NINODE )
					{
						hasSkinnedChildren |= doNodes( nif, iChild, tparent * tlocal, world, bones );
					}
					else if ( nif->inherits( iChild, T_NITRIBASEDGEOM ) )
					{
						hasSkinnedChildren |= doShape( nif, iChild, tparent * tlocal, world, bones );
					}
				}
			}
		}
		
		if ( hasSkinnedChildren )
		{
			Transform().writeBack( nif, index );
		}
		
		return hasSkinnedChildren;
	}
	bool doShape( NifModel * nif, const QModelIndex & index, const Transform & tparent, const TransMap & world, const TransMap & bones )
	{
		QModelIndex iShapeData = nif->getBlock( nif->getLink( index, TA_DATA ) );
		QModelIndex iSkinInstance = nif->getBlock( nif->getLink( index, TA_SKININSTANCE ), T_NISKININSTANCE );
		if ( ! iSkinInstance.isValid() || ! iShapeData.isValid() )
			return false;
		QStringList names;
		QModelIndex iNames = nif->getIndex( iSkinInstance, TA_BONES );
		if ( iNames.isValid() )
			iNames = nif->getIndex( iNames, TA_BONES );
		if ( iNames.isValid() )
			for ( int n = 0; n < nif->rowCount( iNames ); n++ )
			{
				QModelIndex iBone = nif->getBlock( nif->getLink( iNames.child( n, 0 ) ), T_NINODE );
				if ( iBone.isValid() )
					names.append( nif->get<QString>( iBone, TA_NAME ) );
				else
					names.append("");
			}
		QModelIndex iSkinData = nif->getBlock( nif->getLink( iSkinInstance, TA_DATA ), T_NISKINDATA );
		if ( !iSkinData.isValid() )
			return false;
		QModelIndex iBones = nif->getIndex( iSkinData, TA_BONELIST );
		if ( ! iBones.isValid() )
			return false;
		
		Transform t( nif, iSkinData );
		t = tparent * t;
		t.writeBack( nif, iSkinData );
		
		for ( int b = 0; b < nif->rowCount( iBones ) && b < names.count(); b++ )
		{
			QModelIndex iBone = iBones.child( b, 0 );
			
			t = Transform( nif, iBone );
			
			t.rotation = world.value( names[ b ] ).rotation.inverted() * bones.value( names[ b ] ).rotation * t.rotation;
			t.translation = world.value( names[ b ] ).rotation.inverted() * bones.value( names[ b ] ).rotation * t.translation;
			
			t.writeBack( nif, iBone );
		}
		
		Vector3 center = nif->get<Vector3>( iShapeData, TA_CENTER );
		nif->set<Vector3>( iShapeData, TA_CENTER, tparent * center );
		return true;
	}
};

REGISTER_SPELL( spFixSkeleton )

//! Read skeleton data for use in Fix Skeleton
class spScanSkeleton : public Spell
{
public:
	QString name() const { return Spell::tr("Scan Bip01"); }
	QString page() const { return Spell::tr("Skeleton"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return ( nif->getVersion() == STR_V04000002 && nif->itemType( index ) == B_NIBLOCK && nif->get<QString>( index, TA_NAME ) == "Bip01" );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QFile file( SKEL_DAT );
		if ( file.open( QIODevice::WriteOnly ) )
		{
			QDataStream stream( &file );
			scan( nif, index, Transform(), stream );
			stream << QString();
		}
		return index;
	}
	
	void scan( NifModel * nif, const QModelIndex & index, const Transform & tparent, QDataStream & stream )
	{
		QString name = nif->get<QString>( index, TA_NAME );
		if ( name.startsWith( "Bip01" ) )
		{
			Transform local( nif, index );
			stream << name << local << tparent * local;
			qWarning() << name;
			foreach ( int link, nif->getChildLinks( nif->getBlockNumber( index ) ) )
			{
				QModelIndex iChild = nif->getBlock( link, T_NINODE );
				if ( iChild.isValid() )
					scan( nif, iChild, tparent * local, stream );
			}
		}
	}
};

//REGISTER_SPELL( spScanSkeleton )

//! Unknown; unused?
template <> inline bool qMapLessThanKey<Triangle>(const Triangle &s1, const Triangle &s2)
{
	int d = 0;
	if (d == 0) d = (s1[0] - s2[0]);
	if (d == 0) d = (s1[1] - s2[1]);
	if (d == 0) d = (s1[2] - s2[2]);
	return d < 0; 
}

//! Rotate a Triangle
inline void qRotate(Triangle &t)
{
	if (t[1] < t[0] && t[1] < t[2]) {
		t.set( t[1], t[2], t[0] );
	} else if (t[2] < t[0]) {
		t.set( t[2], t[0], t[1] );
	}
}

//! Make skin partition
class spSkinPartition : public Spell
{
public:
	QString name() const { return Spell::tr("Make Skin Partition"); }
	QString page() const { return Spell::tr("Mesh"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & iShape )
	{
		if ( nif->isNiBlock( iShape, T_NITRISHAPE ) || nif->isNiBlock( iShape, T_NITRISTRIPS ) )
		{
			QModelIndex iSkinInst = nif->getBlock( nif->getLink( iShape, TA_SKININSTANCE ), T_NISKININSTANCE );
			if ( iSkinInst.isValid() )
			{
				return nif->getBlock( nif->getLink( iSkinInst, TA_DATA ), T_NISKINDATA ).isValid();
			}
		}
		return false;
	}
	
	typedef QPair<int,float> boneweight;
	
	//! Helper for sorting a boneweight list
	struct boneweight_equivalence {
		bool operator()(const boneweight& lhs, const boneweight& rhs) {
			if (lhs.second == 0.0) {
				if (rhs.second == 0.0) {
					return rhs.first < lhs.first;
				} else {
					return true;
				}
				return false;
			} else if ( rhs.second == lhs.second ) {
				return lhs.first < rhs.first;
			} else {
				return rhs.second < lhs.second;
			}
		}
	};

	//! A bone and Triangle set
	typedef struct {
		QList<int> bones;
		QVector<Triangle> triangles;
	} Partition;
	
	QModelIndex cast( NifModel * nif, const QModelIndex & iBlock )
	{
		int mbpp = 0, mbpv = 0;
		bool make_strips = false;
		return cast( nif, iBlock, mbpp, mbpv, make_strips );
	}
	
	//! Cast with extra parameters
	QModelIndex cast( NifModel * nif, const QModelIndex & iBlock, int & maxBonesPerPartition, int & maxBonesPerVertex, bool make_strips, bool pad = false )
	{
		QPersistentModelIndex iShape = iBlock;
		QString iShapeType = "";
		if ( nif->isNiBlock( iShape, T_NITRISHAPE ) ) {
			iShapeType = T_NITRISHAPE;
		} else if ( nif->isNiBlock( iShape, T_NITRISTRIPS ) ) {
			iShapeType = T_NITRISTRIPS;
		}
		try
		{
			QPersistentModelIndex iData;
			if ( iShapeType == T_NITRISHAPE ) {
				iData = nif->getBlock( nif->getLink( iShape, TA_DATA ), T_NITRISHAPEDATA );
			} else if ( iShapeType == T_NITRISTRIPS ) {
				iData = nif->getBlock( nif->getLink( iShape, TA_DATA ), T_NITRISTRIPSDATA );
			}
			QPersistentModelIndex iSkinInst = nif->getBlock( nif->getLink( iShape, TA_SKININSTANCE ), T_NISKININSTANCE );
			QPersistentModelIndex iSkinData = nif->getBlock( nif->getLink( iSkinInst, TA_DATA ), T_NISKINDATA );
			QModelIndex iSkinPart = nif->getBlock( nif->getLink( iSkinInst, TA_SKINPARTITION ), T_NISKINPARTITION );
			if ( ! iSkinPart.isValid() )
				iSkinPart = nif->getBlock( nif->getLink( iSkinData, TA_SKINPARTITION ), T_NISKINPARTITION );
			
			// read in the weights from NiSkinData
			
			int numVerts = nif->get<int>( iData, TA_NUMVERTICES );
			QVector< QList< boneweight > > weights( numVerts );
			
			QModelIndex iBoneList = nif->getIndex( iSkinData, TA_BONELIST );
			int numBones = nif->rowCount( iBoneList );
			for ( int bone = 0; bone < numBones; bone++ )
			{
				QModelIndex iVertexWeights = nif->getIndex( iBoneList.child( bone, 0 ), TA_VERTEXWEIGHTS );
				for ( int r = 0; r < nif->rowCount( iVertexWeights ); r++ )
				{
					int vertex = nif->get<int>( iVertexWeights.child( r, 0 ), TA_INDEX );
					float weight = nif->get<float>( iVertexWeights.child( r, 0 ), TA_WEIGHT );
					if ( vertex >= weights.count() )
						throw QString( Spell::tr("bad "T_NISKINDATA" - vertex count does not match") );
					weights[vertex].append( boneweight( bone, weight ) );
				}
			}
			
			// count min and max bones per vertex
			
			int minBones, maxBones;
			minBones = maxBones = weights.value( 0 ).count();
			foreach ( QList< boneweight > list, weights )
			{
				if ( list.count() < minBones )
					minBones = list.count();
				if ( list.count() > maxBones )
					maxBones = list.count();
			}
			
			if ( minBones <= 0 )
				throw QString( Spell::tr("bad "T_NISKINDATA" - some vertices have no weights at all") );
			
			// query max bones per vertex/partition
			
			if ( maxBonesPerPartition <= 0 || maxBonesPerVertex <= 0 )
			{
				SkinPartitionDialog dlg( maxBones );
				
				if ( dlg.exec() != QDialog::Accepted )
					return iBlock;
				
				maxBonesPerPartition = dlg.maxBonesPerPartition();
				maxBonesPerVertex = dlg.maxBonesPerVertex();
				make_strips = dlg.makeStrips();
				pad = dlg.padPartitions();
			}
			
			// reduce vertex influences if necessary
			
			if ( maxBones > maxBonesPerVertex )
			{
				QMutableVectorIterator< QList< boneweight > > it( weights );
				int c = 0;
				while ( it.hasNext() )
				{
					QList< boneweight > & lst = it.next();
					qSort( lst.begin(), lst.end(), boneweight_equivalence() );

					if ( lst.count() > maxBonesPerVertex )
						c++;
					
					while ( lst.count() > maxBonesPerVertex )
					{
						lst.removeLast();
					}
					
					float totalWeight = 0;
					foreach ( boneweight bw, lst )
					{
						totalWeight += bw.second;
					}
					
					for ( int b = 0; b < lst.count(); b++ )
					{	// normalize
						lst[b].second /= totalWeight;
					}
				}
				qWarning() << QString( Spell::tr( "reduced %1 vertices to %2 bone influences (maximum number of bones per vertex was %3)" ) ).arg( c ).arg( maxBonesPerVertex ).arg( maxBones );
			}
			
			maxBones = maxBonesPerVertex;
			
			// reduces bone weights so that the triangles fit into the partitions

			QList<Triangle> triangles;
			if ( iShapeType == T_NITRISHAPE ) {
				triangles = nif->getArray<Triangle>( iData, TA_TRIANGLES ).toList();
			} else if ( iShapeType == T_NITRISTRIPS ) {
				// triangulate first (code copied from strippify.cpp)
				QList< QVector<quint16> > strips;
				QModelIndex iPoints = nif->getIndex( iData, TA_POINTS );
				for ( int s = 0; s < nif->rowCount( iPoints ); s++ )
				{
					QVector<quint16> strip;
					QModelIndex iStrip = iPoints.child( s, 0 );
					for ( int p = 0; p < nif->rowCount( iStrip ); p++ ) {
						strip.append( nif->get<int>( iStrip.child( p, 0 ) ) );
					}
					strips.append( strip );
				}
				triangles = triangulate(strips).toList();
			}

			QMap<Triangle, quint32> trimap;
			quint32 defaultPart = 0;
			if (nif->inherits(iSkinInst, T_BSDISMEMBERSKININSTANCE))
			{
				// First find a partition to dump dangling faces.  Torso is prefered if available.
				quint32 nparts = nif->get<uint>(iSkinInst, TA_NUMPARTITIONS);
				QModelIndex iPartData = nif->getIndex( iSkinInst, TA_PARTITIONS );
				for (quint32 i=0; i<nparts; ++i) {
					QModelIndex iPart = iPartData.child(i,0);
					if (!iPart.isValid()) continue;
					if ( nif->get<uint>(iPart, TA_BODYPART) == 0 /* Torso */) {
						defaultPart = i;
						break;
					}
				}
				defaultPart = qMin(nparts-1, defaultPart);

				// enumerate existing partitions and select faces into same partition
				quint32 nskinparts = nif->get<int>( iSkinPart, TA_NUMSKINPARTITIONBLOCKS );
				iPartData = nif->getIndex( iSkinPart, TA_SKINPARTITIONBLOCKS );
				for (quint32 i=0; i<nskinparts; ++i) {
					QModelIndex iPart = iPartData.child(i,0);
					if (!iPart.isValid()) continue;

					quint32 finalPart = qMin(nparts-1, i);

					QVector<int> vertmap = nif->getArray<int>( iPart, TA_VERTEXMAP );

					quint8 hasFaces = nif->get<quint8>(iPart, TA_HASFACES);
					quint8 numStrips = nif->get<quint8>(iPart, TA_NUMSTRIPS);				  
					QVector<Triangle> partTriangles;
					if ( hasFaces && numStrips == 0 ) {
						partTriangles = nif->getArray<Triangle>( iPart, TA_TRIANGLES );
					} else if ( numStrips != 0 ) {
						// triangulate first (code copied from strippify.cpp)
						QList< QVector<quint16> > strips;
						QModelIndex iPoints = nif->getIndex( iPart, TA_STRIPS );
						for ( int s = 0; s < nif->rowCount( iPoints ); s++ )
						{
							QVector<quint16> strip;
							QModelIndex iStrip = iPoints.child( s, 0 );
							for ( int p = 0; p < nif->rowCount( iStrip ); p++ ) {
								strip.append( nif->get<int>( iStrip.child( p, 0 ) ) );
							}
							strips.append( strip );
						}
						partTriangles = triangulate(strips);
					}
					for (int j = 0; j<partTriangles.count(); ++j) 
					{
						Triangle t = partTriangles[j];
						Triangle tri = t;
						if (!vertmap.isEmpty()) {
							tri[0] = vertmap[tri[0]];
							tri[1] = vertmap[tri[1]];
							tri[2] = vertmap[tri[2]];
						}
						qRotate(tri);
						trimap.insert( tri, finalPart );
					}
				}
			}
			
			QMap< int, int > match;
			bool doMatch = true;
			
			QList<int> tribones;
			
			int cnt = 0;
			
			foreach ( Triangle tri, triangles )
			{
				do
				{
					tribones.clear();
					for ( int c = 0; c < 3; c++ )
					{
						foreach ( boneweight bw, weights[tri[c]] )
						{
							if ( ! tribones.contains( bw.first ) )
								tribones.append( bw.first );
						}
					}
					
					if ( tribones.count() > maxBonesPerPartition )
					{
						// sum up the weights for each bone
						// bones with weight == 1 can't be removed
						
						QMap<int,float> sum;
						QList<int> nono;
						
						for ( int t = 0; t < 3; t++ )
						{
							if ( weights[tri[t]].count() == 1 )
								nono.append( weights[tri[t]].first().first );
							
							foreach ( boneweight bw, weights[ tri[t] ] )
								sum[ bw.first ] += bw.second;
						}
						
						// select the bone to remove
						
						float minWeight = 5.0;
						int minBone = -1;
						
						foreach ( int b, sum.keys() )
						{
							if ( ! nono.contains( b ) && sum[b] < minWeight )
							{
								minWeight = sum[b];
								minBone = b;
							}
						}
						
						if ( minBone < 0 )	// this shouldn't never happen
							throw QString( "internal error 0x01" );
						
						// do a vertex match detect
						
						if ( doMatch )
						{
							QVector<Vector3> verts = nif->getArray<Vector3>( iData, TA_VERTICES );
							for ( int a = 0; a < verts.count(); a++ )
							{
								match.insertMulti( a, a );
								for ( int b = a + 1; b < verts.count(); b++ )
								{
									if ( verts[a] == verts[b] && weights[a] == weights[b] )
									{
										match.insertMulti( a, b );
										match.insertMulti( b, a );
									}
								}
							}
						}
						
						// now remove that bone from all vertices of this triangle and from all matching vertices too
						
						for ( int t = 0; t < 3; t++ )
						{
							bool rem = false;
							foreach ( int v, match.values( tri[t] ) )
							{
								QList< boneweight > & bws = weights[ v ];
								QMutableListIterator< boneweight > it( bws );
								while ( it.hasNext() )
								{
									boneweight & bw = it.next();
									if ( bw.first == minBone )
									{
										it.remove();
										rem = true;
									}
								}
								
								float totalWeight = 0;
								foreach ( boneweight bw, bws )
								{
									totalWeight += bw.second;
								}
								
								if ( totalWeight == 0 )
									throw QString( "internal error 0x02" );
								
								for ( int b = 0; b < bws.count(); b++ )
								{	// normalize
									bws[b].second /= totalWeight;
								}
							}
							if ( rem )
								cnt++;
						}
					}
				} while ( tribones.count() > maxBonesPerPartition );
			}
			
			if ( cnt > 0 )
				qWarning() << QString( Spell::tr( "removed %1 bone influences" ) ).arg( cnt );
			
			// split the triangles into partitions

			bool merged = true;

			QList<Partition> parts;

			if (!trimap.isEmpty()) {
				QMutableListIterator<Triangle> it( triangles );
				while ( it.hasNext() )
				{
					Triangle tri = it.next();
					qRotate(tri);
					QMap<Triangle, quint32>::iterator partItr = trimap.find(tri);
					int partIdx = ( partItr != trimap.end()) ? partItr.value() : defaultPart;
					if (partIdx >= 0)
					{
						//Triangle & tri = *it;

						// Ensure enough partitions
						while ( partIdx >= int(parts.size()) )
							parts.push_back( Partition() );

						Partition& part = parts[partIdx];

						QList<int> tribones;
						for ( int c = 0; c < 3; c++ )
						{
							foreach ( boneweight bw, weights[tri[c]] )
							{
								if ( ! tribones.contains( bw.first ) )
									tribones.append( bw.first );
							}
						}

						part.bones = mergeBones( part.bones, tribones );
						part.triangles.append( tri );
						it.remove();
					}
				}
				merged = false; // when explicit mapping enabled, no merging is allowed
			}

			while ( ! triangles.isEmpty() )
			{
				Partition part;
				
				QHash<int,bool> usedVerts;
				
				bool addtriangles;
				do
				{
					QMutableListIterator<Triangle> it( triangles );
					while ( it.hasNext() )
					{
						Triangle & tri = it.next();
						
						QList<int> tribones;
						for ( int c = 0; c < 3; c++ )
						{
							foreach ( boneweight bw, weights[tri[c]] )
							{
								if ( ! tribones.contains( bw.first ) )
									tribones.append( bw.first );
							}
						}
						
						if ( part.bones.isEmpty() || containsBones( part.bones, tribones ) )
						{
							part.bones = mergeBones( part.bones, tribones );
							part.triangles.append( tri );
							usedVerts[ tri[0] ] = true;
							usedVerts[ tri[1] ] = true;
							usedVerts[ tri[2] ] = true;
							it.remove();
						}
					}
					
					addtriangles = false;
					
					if ( part.bones.count() < maxBonesPerPartition )
					{	// if we have room left in the partition then add an adjacent triangle
						it.toFront();
						while ( it.hasNext() )
						{
							Triangle & tri = it.next();
							
							if ( usedVerts.contains( tri[0] ) || usedVerts.contains( tri[1] ) || usedVerts.contains( tri[2] ) )
							{
								QList<int> tribones;
								for ( int c = 0; c < 3; c++ )
								{
									foreach ( boneweight bw, weights[tri[c]] )
									{
										if ( ! tribones.contains( bw.first ) )
											tribones.append( bw.first );
									}
								}
								
								tribones = mergeBones( part.bones, tribones );
								if ( tribones.count() <= maxBonesPerPartition )
								{
									part.bones = tribones;
									part.triangles.append( tri );
									usedVerts[ tri[0] ] = true;
									usedVerts[ tri[1] ] = true;
									usedVerts[ tri[2] ] = true;
									it.remove();
									addtriangles = true;
									//break;
								}
							}
						}
					}
				}
				while ( addtriangles );
				
				parts.append( part );
			}
			
			//qWarning() << parts.count() << "small partitions";
			
			// merge partitions
			
			while (merged)
			{
				merged = false;
				for ( int p1 = 0; p1 < parts.count() && ! merged; p1++ )
				{
					if ( parts[p1].bones.count() < maxBonesPerPartition )
					{
						for ( int p2 = p1+1; p2 < parts.count() && ! merged; p2++ )
						{
							QList<int> mergedBones = mergeBones( parts[p1].bones, parts[p2].bones );
							if ( mergedBones.count() <= maxBonesPerPartition )
							{
								parts[p1].bones = mergedBones;
								parts[p1].triangles << parts[p2].triangles;
								parts.removeAt( p2 );
								merged = true;
							}
						}
					}
				}
			}
			
			//qWarning() << parts.count() << "partitions";
			
			// create the NiSkinPartition if it doesn't exist yet
			
			if ( ! iSkinPart.isValid() )
			{
				iSkinPart = nif->insertNiBlock( T_NISKINPARTITION, nif->getBlockNumber( iSkinData ) + 1 );
				nif->setLink( iSkinInst, TA_SKINPARTITION, nif->getBlockNumber( iSkinPart ) );
				nif->setLink( iSkinData, TA_SKINPARTITION, nif->getBlockNumber( iSkinPart ) );
			}
			
			// start writing NiSkinPartition
			
			nif->set<int>( iSkinPart, TA_NUMSKINPARTITIONBLOCKS, parts.count() );
			nif->updateArray( iSkinPart, TA_SKINPARTITIONBLOCKS );
			
			QModelIndex iBSSkinInstPartData;
			if (nif->inherits(iSkinInst, T_BSDISMEMBERSKININSTANCE))
			{
				quint32 nparts = nif->get<uint>(iSkinInst, TA_NUMPARTITIONS);
				iBSSkinInstPartData = nif->getIndex( iSkinInst, TA_PARTITIONS );
				// why is QList.count() signed? cast to squash warning
				if (nparts != (quint32) parts.count())
				{
					qWarning() << T_BSDISMEMBERSKININSTANCE" partition count does not match Skin Partition count.  Adjusting to fit.";
					nif->set<uint>(iSkinInst, TA_NUMPARTITIONS, parts.count());
					nif->updateArray( iSkinInst, TA_PARTITIONS );
				}
			}

			QList<int> prevPartBones;

			for ( int p = 0; p < parts.count(); p++ )
			{
				QModelIndex iPart = nif->getIndex( iSkinPart, TA_SKINPARTITIONBLOCKS ).child( p, 0 );
				
				QList<int> bones = parts[p].bones;
				qSort( bones );

				// set partition flags for bs skin instance if present
				if (iBSSkinInstPartData.isValid()) {
					if (bones != prevPartBones) {
						prevPartBones = bones;
						nif->set<uint>(iBSSkinInstPartData.child(p,0), TA_PARTFLAG, 257);
					}
				}
				
				QVector<Triangle> triangles = parts[p].triangles;
				
				// Create the vertex map

				int idx = 0;
				QVector<int> vidx(numVerts, -1);
				foreach ( Triangle tri, triangles ) {
					for ( int t = 0; t < 3; t++ ) {
						int v = tri[t];
						if ( vidx[v] < 0)
							vidx[v] = idx++;
					}
				}
				QVector<int> vertices(idx, -1);
				for (int i = 0; i < numVerts; ++i) {
					int v = vidx[i];
					if (v >= 0) {
						vertices[v] = i;
					}
				}

				// map the vertices
				
				for ( int tri = 0; tri < triangles.count(); tri++ )
				{
					for ( int t = 0; t < 3; t++ )
					{
						triangles[tri][t] = vertices.indexOf( triangles[tri][t] );
					}
				}
				
				// stripify the triangles
				QList< QVector<quint16> > strips;
				int numTriangles = 0;
				if ( make_strips == true )
				{
					strips = stripify( triangles );

					foreach ( QVector<quint16> strip, strips )
					{
						numTriangles += strip.count() - 2;
					}
				}
				else
				{
					numTriangles = triangles.count();
				}
				
				// fill in counts
				if ( pad )
				{
					while ( bones.size() < maxBonesPerPartition ) {
						bones.append(0);
					}
				}

				// resort the bone weights in bone order
				QMutableVectorIterator< QList< boneweight > > it( weights );
				while ( it.hasNext() ) {
					QList< boneweight > &bw = it.next();
					qSort( bw.begin(), bw.end(), boneweight_equivalence() );
				}

				nif->set<int>( iPart, TA_NUMVERTICES, vertices.count() );
				nif->set<int>( iPart, TA_NUMTRIANGLES, numTriangles );
				nif->set<int>( iPart, TA_NUMBONES, bones.count() );
				nif->set<int>( iPart, TA_NUMSTRIPS, strips.count() );
				nif->set<int>( iPart, TA_NUMWEIGHTSPERVERTEX, maxBones );
				
				// fill in bone map
				
				QModelIndex iBoneMap = nif->getIndex( iPart, TA_BONES );
				nif->updateArray( iBoneMap );
				nif->setArray<int>( iBoneMap, bones.toVector() );
				
				// fill in vertex map
				
				nif->set<int>( iPart, TA_HASVERTEXMAP, 1 );
				QModelIndex iVertexMap = nif->getIndex( iPart, TA_VERTEXMAP );
				nif->updateArray( iVertexMap );
				nif->setArray<int>( iVertexMap, vertices );
				
				// fill in vertex weights
				
				nif->set<int>( iPart, TA_HASVERTEXWEIGHTS, 1 );
				QModelIndex iVWeights = nif->getIndex( iPart, TA_VERTEXWEIGHTS );
				nif->updateArray( iVWeights );
				for ( int v = 0; v < nif->rowCount( iVWeights ); v++ )
				{
					QModelIndex iVertex = iVWeights.child( v, 0 );
					nif->updateArray( iVertex );
					QList< boneweight > list = weights.value( vertices[v] );
					for ( int b = 0; b < maxBones; b++ )
						nif->set<float>( iVertex.child( b, 0 ), list.count() > b ? list[ b ].second : 0.0 );
				}

				nif->set<int>( iPart, TA_HASFACES, 1 );

				if ( make_strips == true )
				{
					//Clear out any existing triangle data that might be left over from an existing Skin Partition
					QModelIndex iTriangles = nif->getIndex( iPart, TA_TRIANGLES );
					nif->updateArray( iTriangles );

					// write the strips
					QModelIndex iStripLengths = nif->getIndex( iPart, TA_STRIPLENGTHS );
					nif->updateArray( iStripLengths );
					for ( int s = 0; s < nif->rowCount( iStripLengths ); s++ )
						nif->set<int>( iStripLengths.child( s, 0 ), strips.value( s ).count() );
					
					QModelIndex iStrips = nif->getIndex( iPart, TA_STRIPS );
					nif->updateArray( iStrips );
					for ( int s = 0; s < nif->rowCount( iStrips ); s++ )
					{
						nif->updateArray( iStrips.child( s, 0 ) );
						nif->setArray<quint16>( iStrips.child( s, 0 ), strips.value( s ) );
					}
				}
				else
				{
					//Clear out any existing strip data that might be left over from an existing Skin Partition
					QModelIndex iStripLengths = nif->getIndex( iPart, TA_STRIPLENGTHS );
					nif->updateArray( iStripLengths );
					QModelIndex iStrips = nif->getIndex( iPart, TA_STRIPS );
					nif->updateArray( iStrips );

					QModelIndex iTriangles = nif->getIndex( iPart, TA_TRIANGLES );
					nif->updateArray( iTriangles );
					nif->setArray<Triangle>( iTriangles, triangles );
				}
				
				// fill in vertex bones
				
				nif->set<int>( iPart, TA_HASBONEINDICES, 1 );
				QModelIndex iVBones = nif->getIndex( iPart, TA_BONEINDICES );
				nif->updateArray( iVBones );
				for ( int v = 0; v < nif->rowCount( iVBones ); v++ )
				{
					QModelIndex iVertex = iVBones.child( v, 0 );
					nif->updateArray( iVertex );
					QList< boneweight > list = weights.value( vertices[v] );
					for ( int b = 0; b < maxBones; b++ )
						nif->set<int>( iVertex.child( b, 0 ), list.count() > b ? bones.indexOf( list[ b ].first ) : 0 );
				}
			}
			
			// done
			
			return iShape;
		}
		catch ( QString err )
		{
			if ( ! err.isEmpty() )
				QMessageBox::warning( 0, APP, err );
			return iShape;
		}
	}
	
	static QList<int> mergeBones( QList<int> a, QList<int> b )
	{
		foreach ( int c, b )
		{
			if ( ! a.contains( c ) )
			{
				a.append( c );
			}
		}
		return a;
	}
	
	static bool containsBones( QList<int> a, QList<int> b )
	{
		foreach ( int c, b )
		{
			if ( ! a.contains( c ) )
				return false;
		}
		return true;
	}
};

REGISTER_SPELL( spSkinPartition )

//! Make all skin partitions
class spAllSkinPartitions : public Spell
{
public:
	QString name() const { return Spell::tr( "Make All Skin Partitions" ); }
	QString page() const { return Spell::tr( "Batch" ); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return nif && ! index.isValid();
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		Q_UNUSED(index);
		QList< QPersistentModelIndex > indices;
		
		spSkinPartition Partitioner;
		
		for ( int n = 0; n < nif->getBlockCount(); n++ )
		{
			QModelIndex idx = nif->getBlock( n );
			if ( Partitioner.isApplicable( nif, idx ) )
				indices.append( idx );
		}
		
		int mbpp = 0, mbpv = 0;
		bool make_strips = false;
		
		foreach ( QModelIndex idx, indices )
		{
			Partitioner.cast( nif, idx, mbpp, mbpv, make_strips );
		}
		
		qWarning() << QString( Spell::tr( "did %1 partitions" ) ).arg( indices.count() );
		
		return QModelIndex();
	}
};

REGISTER_SPELL( spAllSkinPartitions )


SkinPartitionDialog::SkinPartitionDialog( int ) : QDialog()
{
	spnVert = new QSpinBox( this );
	spnVert->setMinimum( 1 );
	spnVert->setMaximum( 8 );
	spnVert->setValue( 4 );
	connect( spnVert, SIGNAL( valueChanged( int ) ), this, SLOT( changed() ) );
	
	spnPart = new QSpinBox( this );
	spnPart->setMinimum( 4 );
	spnPart->setMaximum( 99 );
	spnPart->setValue( 18 );

	ckTStrip = new QCheckBox( "&Stripify Triangles" );
	ckTStrip->setChecked( true );
	ckTStrip->setToolTip( "Determines whether the triangles in each partition will be arranged into strips or as a list of individual triangles.  Different gaems work best with one or the other." );
	connect( ckTStrip, SIGNAL( clicked() ), this, SLOT( changed() ) );

	ckPad = new QCheckBox( "&Pad Small Partitions" );
	ckPad->setChecked( false );
	ckPad->setToolTip( "Determines whether partitions that will have fewer than the selected maximum number of bones will have extra bones added to bring them up to that number." );
	connect( ckPad, SIGNAL( clicked() ), this, SLOT( changed() ) );

	
	QLabel * labVert = new QLabel( this );
	labVert->setText( Spell::tr(
	"<b>Number of Bones per Vertex</b><br>"
	"Hint: Most games use 4 bones per vertex<br>"
	"Note: If the mesh contains vertices which are<br>"
	"influenced by more than x bones the number of<br>"
	"influences will be reduced for these vertices<br>"
	) );
	
	QLabel * labPart = new QLabel( this );
	labPart->setText( Spell::tr(
	"<b>Number of Bones per Partition</b><br>"
	"Hint: Oblivion uses 18 bones pp<br>"
	"CivIV (non shader meshes) 4 bones pp<br>"
	"CivIV (shader enabled meshes) 18 bones pp<br>"
	"Note: To fit the triangles into the partitions<br>"
	"some bone influences may be removed again."
	) );

	QLabel * labTStrip = new QLabel( this );
	labTStrip->setText( Spell::tr(
	"<b>Whether or not to stripify the triangles in each partition.</b><br>"
	"Hint:  Morrowind and Freedom force do not support strips.<br>"
	"Strips generally perform faster, if the game supports them."
	) );

	QLabel * labPad = new QLabel( this );
	labPad->setText( Spell::tr(
	"<b>Whether or not to pad partitions that will have fewer bones than specified above.</b><br>"
	"Hint:  Freedom Force seems to require this, but it doesn't seem to affect other games."
	) );
	
	QPushButton * btOk = new QPushButton( this );
	btOk->setText( Spell::tr( "Ok" ) );
	connect( btOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	
	QPushButton * btCancel = new QPushButton( this );
	btCancel->setText( Spell::tr( "Cancel" ) );
	connect( btCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	
	QGridLayout * grid = new QGridLayout( this );
	grid->addWidget( labVert, 0, 0 );	 grid->addWidget( spnVert, 0, 1 );
	grid->addWidget( labPart, 1, 0 );	 grid->addWidget( spnPart, 1, 1 );
	grid->addWidget( labTStrip, 2, 0);	grid->addWidget( ckTStrip, 2, 1 );
	grid->addWidget( labPad, 3, 0);		grid->addWidget( ckPad, 3, 1 );
	grid->addWidget( btOk, 4, 0 );		 grid->addWidget( btCancel, 4, 1 );
}

void SkinPartitionDialog::changed()
{
	spnPart->setMinimum( spnVert->value() );
}

int SkinPartitionDialog::maxBonesPerVertex()
{
	return spnVert->value();
}

int SkinPartitionDialog::maxBonesPerPartition()
{
	return spnPart->value();
}

bool SkinPartitionDialog::makeStrips()
{
	return ckTStrip->isChecked();
}

bool SkinPartitionDialog::padPartitions()
{
	return ckPad->isChecked();
}

//! Fix bone bounds
class spFixBoneBounds : public Spell
{
public:
	QString name() const { return Spell::tr( "Fix Bone Bounds" ); }
	QString page() const { return Spell::tr( "Skeleton" ); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return nif->isNiBlock( index, T_NISKINDATA );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & iSkinData )
	{
		QModelIndex iSkinInstance = nif->getBlock( nif->getParent( nif->getBlockNumber( iSkinData ) ), T_NISKININSTANCE );
		QModelIndex iMesh = nif->getBlock( nif->getParent( nif->getBlockNumber( iSkinInstance ) ) );
		QModelIndex iMeshData = nif->getBlock( nif->getLink( iMesh, TA_DATA ) );
		int skelRoot = nif->getLink( iSkinInstance, TA_SKELETONROOT );
		if ( ! nif->inherits( iMeshData, T_NITRIBASEDGEOMDATA ) || skelRoot < 0 || skelRoot != nif->getParent( nif->getBlockNumber( iMesh ) ) )
			return iSkinData;
		
		Transform meshTrans( nif, iMesh );
		
		QVector<Transform> boneTrans;
		QModelIndex iBoneMap = nif->getIndex( iSkinInstance, TA_BONES );
		for ( int n = 0; n < nif->rowCount( iBoneMap ); n++ )
		{
			QModelIndex iBone = nif->getBlock( nif->getLink( iBoneMap.child( n, 0 ) ), T_NINODE );
			if ( skelRoot != nif->getParent( nif->getBlockNumber( iBone ) ) )
				return iSkinData;
			boneTrans.append( Transform( nif, iBone ) );
		}
		
		QVector<Vector3> verts = nif->getArray<Vector3>( iMeshData, TA_VERTICES );
		
		QModelIndex iBoneDataList = nif->getIndex( iSkinData, TA_BONELIST );
		for ( int b = 0; b < nif->rowCount( iBoneDataList ); b++ )
		{
			Vector3 mn;
			Vector3 mx;
			
			Vector3 center;
			float radius = 0;
			
			QModelIndex iWeightList = nif->getIndex( iBoneDataList.child( b, 0 ), TA_VERTEXWEIGHTS );
			for ( int w = 0; w < nif->rowCount( iWeightList ); w++ )
			{
				int v = nif->get<int>( iWeightList.child( w, 0 ), TA_INDEX );
				if ( w == 0 )
				{
					mn = verts.value( v );
					mx = verts.value( v );
				}
				else
				{
					mn.boundMin( verts.value( v ) );
					mx.boundMax( verts.value( v ) );
				}
			}
			
			mn = meshTrans * mn;
			mx = meshTrans * mx;
			
			Transform bt( boneTrans[b] );
			mn = bt.rotation.inverted() * ( mn - bt.translation ) / bt.scale;
			mx = bt.rotation.inverted() * ( mx - bt.translation ) / bt.scale;
			
			center = ( mn + mx ) / 2;
			radius = qMax( ( mn - center ).length(), ( mx - center ).length() );
			
			nif->set<Vector3>( iBoneDataList.child( b, 0 ), TA_BOUNDINGSPHEREOFFSET, center );
			nif->set<float>( iBoneDataList.child( b, 0 ), TA_BOUNDINGSPHERERADIUS, radius );
		}
		
		return iSkinData;
	}
	
};

REGISTER_SPELL( spFixBoneBounds )

//! Mirror an armature branch.
/*!
 * Renames nodes, flips translations and rotations, flips meshes, flips skin data.
 */
class spMirrorSkeleton : public Spell
{
public:
	QString name() const { return Spell::tr("Mirror armature"); }
	QString page() const { return Spell::tr("Skeleton"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return ( nif->getVersion() == STR_V04000002 && nif->itemType( index ) == B_NIBLOCK )
			&& ( ( nif->get<QString>( index, TA_NAME ).startsWith( "Bip01 L" ) ) || ( nif->get<QString>( index, TA_NAME ).startsWith( "Bip01 R" ) ) );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		if ( nif->getLink( index, TA_CONTROLLER ) != -1 )
		{
			int keyframeResponse = QMessageBox::question( 0, Spell::tr("Mirror Armature"), Spell::tr("Do you wish to flip or delete animation?"), Spell::tr("Flip"), Spell::tr("Delete"), Spell::tr("Cancel"));
			if( keyframeResponse == 2 ) return index;
			else if( keyframeResponse == 1 )
			{
				// delete blocks
				int n = 0;
				while ( n < nif->getBlockCount() )
				{
					QModelIndex iBlock = nif->getBlock( n );
					if ( nif->itemName( iBlock ).indexOf( T_NIKEYFRAME ) >= 0 )
						nif->removeNiBlock( n );
					else
						n++;
				}
			}
		}
		// TODO: Duplicate the branch
		// Perhaps have the user do this?
		
		// Traverse nodes starting with Bip01 L/R and "flip"
		doBones( nif, index );
		return index;
	}
	
	void doBones( NifModel * nif, const QModelIndex & index )
	{
		// Need correct naming scheme!
		QString name = nif->get<QString>( index, TA_NAME );
		if ( name.startsWith( "Bip01 L" ) || name.startsWith( "Bip01 R" ) )
		{
			Transform tlocal( nif, index );
			
			// rename as appropriate
			if ( name.startsWith( "Bip01 L" ) )
				name.replace(QString(" L"), QString(" R"));
			else
				name.replace(QString(" R"), QString(" L"));
			
			nif->set<QString>( index, TA_NAME, name );
			
			// translation is a Vector3
			// want [x,y,z] -> [x,y,-z]
			tlocal.translation = Vector3( tlocal.translation[0], tlocal.translation[1], -tlocal.translation[2] );
			
			// rotation is a Matrix, want to negate Y and P components - will get to R component in SkinInstance
			// convert to Euler, then [Y,P,R] -> [-Y,-P,R]
			float x, y, z;
			tlocal.rotation.toEuler( x, y, z );
			tlocal.rotation.fromEuler( -x, -y, z );
			
			// Apply
			tlocal.writeBack( nif, index );
		}
		
		// traverse
		foreach ( int link, nif->getChildLinks( nif->getBlockNumber( index ) ) )
		{
			QModelIndex iChild = nif->getBlock( link );
			QString childName = nif->get<QString>( iChild, TA_NAME );
			// Might as well rename children now if we can - this is less case-critical than Bip01 L/R
			if ( childName.contains( "Left ", Qt::CaseInsensitive ) ) {
				childName.replace(QString("Left "), QString("Right "), Qt::CaseInsensitive );
			} else if ( childName.contains( "Right ", Qt::CaseInsensitive ) ) {
				childName.replace(QString("Right "), QString("Left "), Qt::CaseInsensitive );
			}
			
			nif->set<QString>( iChild, TA_NAME, childName );

			//qWarning() << "Checking child: " << iChild;
			if ( iChild.isValid() )
			{
				if ( nif->itemName( iChild ) == T_NINODE )
				{
					// repeat
					doBones( nif, iChild );
				}
				else if ( nif->inherits( iChild, T_NITRIBASEDGEOM ) )
				{
					// Scale NiTriShape vertices, flip normals
					// Change SkinInstance bones
					doShapes( nif, iChild );
				}
				else if ( nif->inherits( iChild, T_NIKEYFRAMECONTROLLER ) )
				{
					// Flip keyframe data, fun
					doKeyframes( nif, iChild );
				}
			}
		}
	}
	
	void doShapes( NifModel * nif, const QModelIndex & index )
	{
		//qWarning() << "Entering doShapes";
		QModelIndex iData = nif->getBlock( nif->getLink( index, TA_DATA ) );
		QModelIndex iSkinInstance = nif->getBlock( nif->getLink( index, TA_SKININSTANCE ), T_NISKININSTANCE );
		if ( iData.isValid() && iSkinInstance.isValid() )
		{
			// from spScaleVertices
			QVector<Vector3> vertices = nif->getArray<Vector3>( iData, TA_VERTICES );
			QMutableVectorIterator<Vector3> it( vertices );
			while ( it.hasNext() )
			{
				Vector3 & v = it.next();
				for ( int a = 0; a < 3; a++ )
					v[a] = -v[a];
			}
			nif->setArray<Vector3>( iData, TA_VERTICES, vertices );
			
			// fix centre Z - don't recalculate
			Vector3 shapeCentre = nif->get<Vector3>( iData, TA_CENTER );
			shapeCentre = Vector3( shapeCentre[0], shapeCentre[1], -shapeCentre[2] );
			nif->set<Vector3>( iData, TA_CENTER, shapeCentre );
			
			// from spFlipFace
			QVector<Triangle> tris = nif->getArray<Triangle>( iData, TA_TRIANGLES );
			for ( int t = 0; t < tris.count(); t++ )
			{
				tris[t].flip();
			}
			nif->setArray<Triangle>( iData, TA_TRIANGLES, tris );
			
			// from spFlipNormals
			QVector<Vector3> norms = nif->getArray<Vector3>( iData, TA_NORMALS );
			for ( int n = 0; n < norms.count(); n++ )
			{
				norms[n] = -norms[n];
			}
			nif->setArray<Vector3>( iData, TA_NORMALS, norms );
			
			// from spFixSkeleton - get the bones from the skin data
			// weirdness with rounding, sometimes...? probably "good enough" for 99% of cases
			QModelIndex iSkinData = nif->getBlock( nif->getLink( iSkinInstance, TA_DATA ), T_NISKINDATA );
			if ( ! iSkinData.isValid() ) return;
			QModelIndex iBones = nif->getIndex( iSkinData, TA_BONELIST );
			if ( ! iBones.isValid() ) return;
			
			for ( int b = 0; b < nif->rowCount( iBones ); b++ )
			{
				QModelIndex iBone = iBones.child( b, 0 );
				
				Transform tlocal( nif, iBone );
				
				// translation is a Vector3
				// want [x,y,z] -> [x,y,-z]
				tlocal.translation = Vector3( tlocal.translation[0], tlocal.translation[1], -tlocal.translation[2] );
				
				// rotation is a Matrix, want to negate Y and P components, R=180+R
				// convert to Euler, then [Y,P,R] -> [-Y,-P,R]
				float x, y, z;
				tlocal.rotation.toEuler( x, y, z );
				tlocal.rotation.fromEuler( -x, -y, PI+z );
				
				// Fix offset Z
				Vector3 offset = nif->get<Vector3>( iBone, TA_BOUNDINGSPHEREOFFSET );
				offset = Vector3( offset[0], offset[1], -offset[2] );
				nif->set<Vector3>( iBone, TA_BOUNDINGSPHEREOFFSET, offset );
				
				// Apply
				tlocal.writeBack( nif, iBone );
			}
		}
	}

	void doKeyframes( NifModel * nif, QModelIndex & index )
	{
		// do stuff
		QModelIndex keyframeData = nif->getBlock( nif->getLink( index, TA_DATA ), T_NIKEYFRAMEDATA );
		if ( ! keyframeData.isValid() ) return;
		QModelIndex iQuats = nif->getIndex( keyframeData, TA_QUATERNIONKEYS );
		if ( iQuats.isValid() )
		{
			for ( int q = 0; q < nif->rowCount( iQuats ); q++ )
			{
				QModelIndex iQuat = iQuats.child( q, 0 );
				
				Quat value = nif->get<Quat>( iQuat, TA_VALUE );
				Matrix tlocal;
				tlocal.fromQuat( value );
				
				float x, y, z;
				tlocal.toEuler( x, y, z );
				tlocal.fromEuler( -x, -y, z );
				
				value = tlocal.toQuat();
				
				nif->set<Quat>( iQuat, TA_VALUE, value );
			}
		}

		QModelIndex iTransKeys = nif->getIndex( keyframeData, TA_TRANSLATIONS );
		if ( iTransKeys.isValid() )
		{
			iTransKeys = nif->getIndex( iTransKeys, TA_KEYS );
			if ( iTransKeys.isValid() )
			{
				for ( int k = 0; k < nif->rowCount( iTransKeys ); k++ )
				{
					QModelIndex iKey = iTransKeys.child( k, 0 );

					Vector3 value = nif->get<Vector3>( iKey, TA_VALUE );
					value = Vector3( value[0], value[1], -value[2] );
					nif->set<Vector3>( iKey, TA_VALUE, value );
				}
			}
		}
	}
};

REGISTER_SPELL( spMirrorSkeleton )
