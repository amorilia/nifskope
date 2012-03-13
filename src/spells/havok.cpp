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
#include "qhull.h"

#include "NvTriStrip/qtwrapper.h"

#include "blocks.h"

#include <QDebug>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLayout>
#include <QMap>
#include <QMessageBox>
#include <QPushButton>

// Brief description is deliberately not autolinked to class Spell
/*! \file havok.cpp
 * \brief Havok spells
 *
 * All classes here inherit from the Spell class.
 */

//! For Havok coordinate transforms
static const float havokConst = 7.0;

//! Creates a convex hull using Qhull
class spCreateCVS : public Spell
{
public:
	QString name() const { return Spell::tr("Create Convex Shape"); }
	QString page() const { return Spell::tr("Havok"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		if( ! nif->inherits( index, T_NITRIBASEDGEOM ) || ! nif->checkVersion( NF_V10000100, 0 ) )
			return false;
		
		QModelIndex iData = nif->getBlock( nif->getLink( index, TA_DATA ) );
		return iData.isValid();
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iData = nif->getBlock( nif->getLink( index, TA_DATA ) );
		if( !iData.isValid() )
			return index;
		
		/* those will be filled with the CVS data */
		QVector<Vector4> convex_verts, convex_norms;
		
		/* get the verts of our mesh */
		QVector<Vector3> verts = nif->getArray<Vector3>( iData, TA_VERTICES );
		
		// to store results
		QVector<Vector4> hullVerts, hullNorms;
		
		// ask for precision
		QDialog dlg;
		QVBoxLayout * vbox = new QVBoxLayout;
		dlg.setLayout( vbox );
		
		vbox->addWidget( new QLabel( Spell::tr( "Enter the maximum roundoff error to use" ) ) );
		vbox->addWidget( new QLabel( Spell::tr( "Larger values will give a less precise but better performing hull" ) ) );
		
		QDoubleSpinBox * precSpin = new QDoubleSpinBox;
		precSpin->setRange( 0, 5 );
		precSpin->setDecimals( 3 );
		precSpin->setSingleStep( 0.01 );
		precSpin->setValue( 0.25 );
		vbox->addWidget( precSpin );
		
		QHBoxLayout * hbox = new QHBoxLayout;
		vbox->addLayout( hbox );
		
		QPushButton * ok = new QPushButton;
		ok->setText( Spell::tr( "Ok" ) );
		hbox->addWidget( ok );
		
		QPushButton * cancel = new QPushButton;
		cancel->setText( Spell::tr("Cancel") );
		hbox->addWidget( cancel );
		
		QObject::connect( ok, SIGNAL( clicked() ), &dlg, SLOT( accept() ) );
		QObject::connect( cancel, SIGNAL( clicked() ), &dlg, SLOT( reject() ) );
		
		if( dlg.exec() != QDialog::Accepted )
		{
			return index;
		}
		
		/* make a convex hull from it */
		compute_convex_hull( verts, hullVerts, hullNorms, (float) precSpin->value() );
		
		// sort and remove duplicate vertices
		QList<Vector4> sortedVerts;
		foreach( Vector4 vert, hullVerts )
		{
			vert /= havokConst;
			if( ! sortedVerts.contains( vert ) )
			{
				sortedVerts.append( vert );
			}
		}
		qSort( sortedVerts.begin(), sortedVerts.end(), Vector4::lexLessThan );
		QListIterator<Vector4> vertIter( sortedVerts );
		while( vertIter.hasNext() )
		{
			Vector4 sorted = vertIter.next();
			convex_verts.append( sorted );
		}
		
		// sort and remove duplicate normals
		QList<Vector4> sortedNorms;
		foreach( Vector4 norm, hullNorms )
		{
			norm = Vector4( Vector3( norm ), norm[3] / havokConst );
			if( ! sortedNorms.contains( norm ) )
			{
				sortedNorms.append( norm );
			}
		}
		qSort( sortedNorms.begin(), sortedNorms.end(), Vector4::lexLessThan );
		QListIterator<Vector4> normIter( sortedNorms );
		while( normIter.hasNext() )
		{
			Vector4 sorted = normIter.next();
			convex_norms.append( sorted );
		}
		
		/* create the CVS block */
		QModelIndex iCVS = nif->insertNiBlock( "bhkConvexVerticesShape" );
		
		/* set CVS verts */
		nif->set<uint>( iCVS, TA_NUMVERTICES, convex_verts.count() );
		nif->updateArray( iCVS, TA_VERTICES );
		nif->setArray<Vector4>( iCVS, TA_VERTICES, convex_verts );
		
		/* set CVS norms */
		nif->set<uint>( iCVS, "Num Normals", convex_norms.count() );
		nif->updateArray( iCVS, TA_NORMALS );
		nif->setArray<Vector4>( iCVS, TA_NORMALS, convex_norms );
		
		// radius is always 0.1?
		nif->set<float>( iCVS, TA_RADIUS, 0.1 );
		
		// for arrow detection: [0, 0, -0, 0, 0, -0]
		nif->set<float>( nif->getIndex( iCVS, "Unknown 6 Floats" ).child( 2, 0 ), -0.0 );
		nif->set<float>( nif->getIndex( iCVS, "Unknown 6 Floats" ).child( 5, 0 ), -0.0 );
		
		QModelIndex iParent = nif->getBlock( nif->getParent( nif->getBlockNumber( index ) ) );
		QModelIndex collisionLink = nif->getIndex( iParent, TA_COLLISIONOBJECT );
		QModelIndex collisionObject = nif->getBlock( nif->getLink( collisionLink ) );
		
		// create bhkCollisionObject
		if( ! collisionObject.isValid() )
		{
			collisionObject = nif->insertNiBlock( T_BHKCOLLISIONOBJECT );
			
			nif->setLink( collisionLink, nif->getBlockNumber( collisionObject ) );
			nif->setLink( collisionObject, "Target", nif->getBlockNumber( iParent ) );
		}
		
		QModelIndex rigidBodyLink = nif->getIndex( collisionObject, TA_BODY );
		QModelIndex rigidBody = nif->getBlock( nif->getLink( rigidBodyLink ) );
		
		// create bhkRigidBody
		if( ! rigidBody.isValid() )
		{
			rigidBody = nif->insertNiBlock( T_BHKRIGIDBODY );
			
			nif->setLink( rigidBodyLink, nif->getBlockNumber( rigidBody ) );
		}
		
		QModelIndex shapeLink = nif->getIndex( rigidBody, TA_SHAPE );
		QModelIndex shape = nif->getBlock( nif->getLink( shapeLink ) );
		
		// set link and delete old one
		nif->setLink( shapeLink, nif->getBlockNumber( iCVS ) );
		
		if( shape.isValid() )
		{
			// cheaper than calling spRemoveBranch
			nif->removeNiBlock( nif->getBlockNumber( shape ) );
		}
		
		QMessageBox::information( 0, "NifSkope", Spell::tr( "Created hull with %1 vertices, %2 normals" ).arg( convex_verts.count() ).arg( convex_norms.count() ) );
		
		// returning iCVS here can crash NifSkope if a child array is selected
		return index;
	}
};

REGISTER_SPELL( spCreateCVS )

//! Transforms Havok constraints
class spConstraintHelper : public Spell
{
public:
	QString name() const { return Spell::tr("A -> B"); }
	QString page() const { return Spell::tr("Havok"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return nif && (
			nif->isNiBlock( nif->getBlock( index ), "bhkMalleableConstraint" )
			|| nif->isNiBlock( nif->getBlock( index ), "bhkRagdollConstraint" )
			|| nif->isNiBlock( nif->getBlock( index ), "bhkLimitedHingeConstraint" )
			|| nif->isNiBlock( nif->getBlock( index ), "bhkHingeConstraint" )
			|| nif->isNiBlock( nif->getBlock( index ), "bhkPrismaticConstraint" ) );
	}

	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iConstraint = nif->getBlock( index );
		QString name = nif->itemName( iConstraint );
		if ( name == "bhkMalleableConstraint" )
		{
			if ( nif->getIndex( iConstraint, "Ragdoll" ).isValid() )
			{
				name = "bhkRagdollConstraint";
			}
			else if ( nif->getIndex( iConstraint, "Limited Hinge" ).isValid() )
			{
				name = "bhkLimitedHingeConstraint";
			}
			else if ( nif->getIndex( iConstraint, "Hinge" ).isValid() )
			{
				name = "bhkHingeConstraint";
			}
		}
		
		QModelIndex iBodyA = nif->getBlock( nif->getLink( nif->getIndex( iConstraint, "Entities" ).child( 0, 0 ) ), T_BHKRIGIDBODY );
		QModelIndex iBodyB = nif->getBlock( nif->getLink( nif->getIndex( iConstraint, "Entities" ).child( 1, 0 ) ), T_BHKRIGIDBODY );
		
		if ( ! iBodyA.isValid() || ! iBodyB.isValid() )
		{
			qWarning() << "coudn't find the bodies for this constraint";
			return index;
		}
		
		Transform transA = bodyTrans( nif, iBodyA );
		Transform transB = bodyTrans( nif, iBodyB );

		if ( name == "bhkLimitedHingeConstraint" )
		{
			iConstraint = nif->getIndex( iConstraint, "Limited Hinge" );
			if ( ! iConstraint.isValid() )
				return index;
		}

		if ( name == "bhkRagdollConstraint" )
		{
			iConstraint = nif->getIndex( iConstraint, "Ragdoll" );
			if ( ! iConstraint.isValid() )
				return index;
		}
		
		if ( name == "bhkHingeConstraint" )
		{
			iConstraint = nif->getIndex( iConstraint, "Hinge" );
			if ( ! iConstraint.isValid() )
				return index;
		}

		Vector3 pivot = Vector3( nif->get<Vector4>( iConstraint, "Pivot A" ) ) * havokConst;
		pivot = transA * pivot;
		pivot = transB.rotation.inverted() * ( pivot - transB.translation ) / transB.scale / havokConst;
		nif->set<Vector4>( iConstraint, "Pivot B", Vector4( pivot[0], pivot[1], pivot[2], 0 ) );
		
		if ( name == "bhkLimitedHingeConstraint" )
		{
			Vector3 axle = Vector3( nif->get<Vector4>( iConstraint, "Axle A" ) );
			axle = transA.rotation * axle;
			axle = transB.rotation.inverted() * axle;
			nif->set<Vector4>( iConstraint, "Axle B", Vector4( axle[0], axle[1], axle[2], 0 ) );
		
			axle = Vector3( nif->get<Vector4>( iConstraint, "Perp2 Axle In A2" ) );
			axle = transA.rotation * axle;
			axle = transB.rotation.inverted() * axle;
			nif->set<Vector4>( iConstraint, "Perp2 Axle In B2", Vector4( axle[0], axle[1], axle[2], 0 ) );
		}

		if ( name == "bhkRagdollConstraint" )
		{
			Vector3 axle = Vector3( nif->get<Vector4>( iConstraint, "Plane A" ) );
			axle = transA.rotation * axle;
			axle = transB.rotation.inverted() * axle;
			nif->set<Vector4>( iConstraint, "Plane B", Vector4( axle[0], axle[1], axle[2], 0 ) );

			axle = Vector3( nif->get<Vector4>( iConstraint, "Twist A" ) );
			axle = transA.rotation * axle;
			axle = transB.rotation.inverted() * axle;
			nif->set<Vector4>( iConstraint, "Twist B", Vector4( axle[0], axle[1], axle[2], 0 ) );
		}
		
		return index;
	}
	
	static Transform bodyTrans( const NifModel * nif, const QModelIndex & index )
	{
		Transform t;
		if ( nif->isNiBlock( index, T_BHKRIGIDBODYT ) )
		{
			t.translation = Vector3( nif->get<Vector4>( index, TA_TRANSLATION ) * 7 );
			t.rotation.fromQuat( nif->get<Quat>( index, TA_ROTATION ) );
		}
		
		qint32 l = nif->getBlockNumber( index );
		
		while ( ( l = nif->getParent( l ) ) >= 0 )
		{
			QModelIndex iAV = nif->getBlock( l, "NiAVObject" );
			if ( iAV.isValid() )
				t = Transform( nif, iAV ) * t;
		}
		
		return t;
	}
};

REGISTER_SPELL( spConstraintHelper )

//! Calculates Havok spring lengths
class spStiffSpringHelper : public Spell
{
public:
	QString name() const { return Spell::tr( "Calculate Spring Length" ); }
	QString page() const { return Spell::tr( "Havok" ); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & idx )
	{
		return nif && nif->isNiBlock( nif->getBlock( idx ), "bhkStiffSpringConstraint" );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & idx )
	{
		QModelIndex iConstraint = nif->getBlock( idx );
		
		QModelIndex iBodyA = nif->getBlock( nif->getLink( nif->getIndex( iConstraint, "Entities" ).child( 0, 0 ) ), T_BHKRIGIDBODY );
		QModelIndex iBodyB = nif->getBlock( nif->getLink( nif->getIndex( iConstraint, "Entities" ).child( 1, 0 ) ), T_BHKRIGIDBODY );
		
		if ( ! iBodyA.isValid() || ! iBodyB.isValid() )
		{
			qWarning() << "coudn't find the bodies for this constraint";
			return idx;
		}
		
		Transform transA = spConstraintHelper::bodyTrans( nif, iBodyA );
		Transform transB = spConstraintHelper::bodyTrans( nif, iBodyB );
		
		Vector3 pivotA( nif->get<Vector4>( iConstraint, "Pivot A" ) * 7 );
		Vector3 pivotB( nif->get<Vector4>( iConstraint, "Pivot B" ) * 7 );
		
		float length = ( transA * pivotA - transB * pivotB ).length() / 7;
		
		nif->set<float>( iConstraint, "Length", length );
		
		return nif->getIndex( iConstraint, "Length" );
	}
};

REGISTER_SPELL( spStiffSpringHelper )

//! Packs Havok strips
class spPackHavokStrips : public Spell
{
public:
	QString name() const { return Spell::tr( "Pack Strips" ); }
	QString page() const { return Spell::tr( "Havok" ); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & idx )
	{
		return nif->isNiBlock( idx, T_BHKNITRISTRIPSSHAPE );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & iBlock )
	{
		QPersistentModelIndex iShape( iBlock );
		
		QVector<Vector3> vertices;
		QVector<Triangle> triangles;
		QVector<Vector3> normals;
		
		foreach ( qint32 lData, nif->getLinkArray( iShape, TA_STRIPSDATA ) )
		{
			QModelIndex iData = nif->getBlock( lData, T_NITRISTRIPSDATA );
			
			if ( iData.isValid() )
			{
				QVector<Vector3> vrts = nif->getArray<Vector3>( iData, TA_VERTICES );
				QVector<Triangle> tris;
				QVector<Vector3> nrms;
				
				QModelIndex iPoints = nif->getIndex( iData, TA_POINTS );
				for ( int x = 0; x < nif->rowCount( iPoints ); x++ )
				{
					tris += triangulate( nif->getArray<quint16>( iPoints.child( x, 0 ) ) );
				}
				
				QMutableVectorIterator<Triangle> it( tris );
				while ( it.hasNext() )
				{
					Triangle & tri = it.next();
					
					Vector3 a = vrts.value( tri[0] );
					Vector3 b = vrts.value( tri[1] );
					Vector3 c = vrts.value( tri[2] );
					
					nrms << Vector3::crossproduct( b - a, c - a ).normalize();
					
					tri[0] += vertices.count();
					tri[1] += vertices.count();
					tri[2] += vertices.count();
				}
				
				foreach ( Vector3 v, vrts )
					vertices += v / 7;
				triangles += tris;
				normals += nrms;
			}
		}
		
		if ( vertices.isEmpty() || triangles.isEmpty() )
		{
			qWarning() << Spell::tr( "no mesh data was found" );
			return iShape;
		}
		
		QPersistentModelIndex iPackedShape = nif->insertNiBlock( T_BHKPACKEDNITRISTRIPSSHAPE, nif->getBlockNumber( iShape ) );
		
		nif->set<int>( iPackedShape, "Num Sub Shapes", 1 );
		QModelIndex iSubShapes = nif->getIndex( iPackedShape, "Sub Shapes" );
		nif->updateArray( iSubShapes );
		nif->set<int>( iSubShapes.child( 0, 0 ), "Layer", 1 );
		nif->set<int>( iSubShapes.child( 0, 0 ), TA_NUMVERTICES, vertices.count() );
		nif->set<int>( iSubShapes.child( 0, 0 ), "Material", nif->get<int>( iShape, "Material" ) );
		nif->setArray<float>( iPackedShape, "Unknown Floats", QVector<float>() << 0.0f << 0.0f << 0.1f << 0.0f << 1.0f << 1.0f << 1.0f << 1.0f << 0.1f );
		nif->set<float>( iPackedShape, TA_SCALE, 1.0f );
		nif->setArray<float>( iPackedShape, "Unknown Floats 2", QVector<float>() << 1.0f << 1.0f << 1.0f );
		
		QModelIndex iPackedData = nif->insertNiBlock( T_HKPACKEDNITRISTRIPSDATA, nif->getBlockNumber( iPackedShape ) );
		nif->setLink( iPackedShape, TA_DATA, nif->getBlockNumber( iPackedData ) );
		
		nif->set<int>( iPackedData, TA_NUMTRIANGLES, triangles.count() );
		QModelIndex iTriangles = nif->getIndex( iPackedData, TA_TRIANGLES );
		nif->updateArray( iTriangles );
		for ( int t = 0; t < triangles.size(); t++ )
		{
			nif->set<Triangle>( iTriangles.child( t, 0 ), TA_TRIANGLE, triangles[ t ] );
			nif->set<Vector3>( iTriangles.child( t, 0 ), TA_NORMAL, normals.value( t ) );
		}
		
		nif->set<int>( iPackedData, TA_NUMVERTICES, vertices.count() );
		QModelIndex iVertices = nif->getIndex( iPackedData, TA_VERTICES );
		nif->updateArray( iVertices );
		nif->setArray<Vector3>( iVertices, vertices );
		
		QMap<qint32,qint32> lnkmap;
		lnkmap.insert( nif->getBlockNumber( iShape ), nif->getBlockNumber( iPackedShape ) );
		nif->mapLinks( lnkmap );
		
		// *** THIS SOMETIMES CRASHES NIFSKOPE        ***
		// *** UNCOMMENT WHEN BRANCH REMOVER IS FIXED ***
		// See issue #2508255
		spRemoveBranch BranchRemover;
		BranchRemover.castIfApplicable( nif, iShape );
		
		return iPackedShape;
	}
};

REGISTER_SPELL( spPackHavokStrips )

