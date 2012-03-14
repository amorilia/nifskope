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

#include "3ds.h"

#include "spellbook.h"

#include "NvTriStrip/qtwrapper.h"

#include "gl/gltex.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QString>
#include <QApplication>

#define tr(x) QApplication::tr("3dsImport", x)


struct objPoint
{
	int v, t, n;
	
	bool operator==( const objPoint & other ) const
	{
		return v == other.v && t == other.t && n == other.n;
	}
};

struct objFace
{
	int v1, v2, v3;
	bool dblside;
};

struct objMaterial
{
	QString name;
	Color3 Ka, Kd, Ks;
	float alpha, glossiness;
	QString map_Kd;
	
	objMaterial() : name( "Untextured" ), alpha( 1.0f ), glossiness( 15.0f ) {}
};

struct objMatFace {
	QString matName;
	QVector< short > subFaces;
};

// The 3ds file can be made up of several objects
struct objMesh {
	QString name;					// The object name
	QVector<Vector3> vertices;		// The array of vertices
	QVector<Vector3> normals;		// The array of the normals for the vertices
	QVector<Vector2> texcoords;		// The array of texture coordinates for the vertices
	QVector<objFace> faces;			// The array of face indices
	QVector<objMatFace> matfaces;	// The array of materials for this mesh
	Vector3 pos;					// The position to move the object to
	Vector3 rot;					// The angles to rotate the object

	objMesh() : pos( 0.0f, 0.0f, 0.0f ), rot( 0.0f, 0.0f, 0.0f ) {}
};

struct objKeyframe {
	Vector3 pos;
	float rotAngle;
	Vector3 rotAxis;
	float scale;

	objKeyframe()
		: pos( 0.0f, 0.0f, 0.0f ), rotAngle( 0 ), rotAxis( 0.0f, 0.0f, 0.0f ), scale( 0.0f )
	{}
};

struct objKfSequence {
	short objectId;
	QString objectName;
	long startTime, endTime, curTime;
	Vector3 pivot;
	QMap< short, objKeyframe > frames;

	objKfSequence() : pivot( 0.0f, 0.0f, 0.0f ) {}
};

static void addLink( NifModel * nif, QModelIndex iBlock, QString name, qint32 link )
{
	QModelIndex iArray = nif->getIndex( iBlock, name );
	QModelIndex iSize = nif->getIndex( iBlock, QString( "Num %1" ).arg( name ) );
	int numIndices = nif->get<int>( iSize );
	nif->set<int>( iSize, numIndices + 1 );
	nif->updateArray( iArray );
	nif->setLink( iArray.child( numIndices, 0 ), link );
}

static Color3 GetColorFromChunk( Chunk * cnk )
{
	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;

	Chunk * ColorChunk;

	ColorChunk = cnk->getChild( COLOR_F );
	if( !ColorChunk ) {
		ColorChunk = cnk->getChild( LIN_COLOR_F );
	}
	if( ColorChunk ) {
		r = ColorChunk->read< float >();
		g = ColorChunk->read< float >();
		b = ColorChunk->read< float >();
	}

	ColorChunk = cnk->getChild( COLOR_24 );
	if( !ColorChunk ) {
		ColorChunk = cnk->getChild( LIN_COLOR_24 );
	}
	if( ColorChunk ) {
		r = (float)( ColorChunk->read< unsigned char >() ) / 255.0f;
		g = (float)( ColorChunk->read< unsigned char >() ) / 255.0f;
		b = (float)( ColorChunk->read< unsigned char >() ) / 255.0f;
	}		

	return Color3( r, g, b );
}

static float GetPercentageFromChunk( Chunk * cnk )
{
	float f = 0.0f;

	Chunk * PercChunk = cnk->getChild( FLOAT_PERCENTAGE );
	if( PercChunk ) {
		f = PercChunk->read< float >();
	}

	PercChunk = cnk->getChild( INT_PERCENTAGE );
	if( PercChunk ) {
		f = (float)( PercChunk->read< unsigned short >() / 255.0f );
	}

	return f;
}


void import3ds( NifModel * nif, const QModelIndex & index )
{

	//--Determine how the file will import, and be sure the user wants to continue--//

	// If no existing node is selected, create a group node.  Otherwise use selected node
	QPersistentModelIndex iRoot, iNode, iShape, iMaterial, iData, iTexProp, iTexSource;
	QModelIndex iBlock = nif->getBlock( index );

	//Be sure the user hasn't clicked on a NiTriStrips object
	if ( iBlock.isValid() && nif->itemName(iBlock) == T_NITRISTRIPS )
	{
		int result = QMessageBox::information( 0, tr("Import OBJ"), tr("You cannot import an OBJ file over a NiTriStrips object.  Please convert it to a NiTriShape object first by right-clicking and choosing Mesh > Triangulate") );
		return;
	}

	if ( iBlock.isValid() && nif->itemName( index ) == T_NINODE )
	{
		iNode = index;
	}
	else if ( iBlock.isValid() && nif->itemName( index ) == T_NITRISHAPE )
	{
		iShape = index;
		//Find parent of NiTriShape
		int par_num = nif->getParent( nif->getBlockNumber( index ) );
		if ( par_num != -1 )
		{
			iNode = nif->getBlock( par_num );
		}

		//Find material, texture, and data objects
		QList<int> children = nif->getChildLinks( nif->getBlockNumber(iShape) );
		for( QList<int>::iterator it = children.begin(); it != children.end(); ++it )
		{
			if ( *it != -1 )
			{
				QModelIndex temp = nif->getBlock( *it );
				QString type = nif->itemName( temp );
				if ( type == T_NIMATERIALPROPERTY )
				{
					iMaterial = temp;
				}
				else if ( type == T_NITRISHAPEDATA )
				{
					iData = temp;
				}
				else if ( (type == T_NITEXTURINGPROPERTY) || (type == T_NITEXTUREPROPERTY) )
				{
					iTexProp = temp;

					//Search children of texture property for texture sources/images
					QList<int> children = nif->getChildLinks( nif->getBlockNumber(iTexProp) );
					for( QList<int>::iterator it = children.begin(); it != children.end(); ++it )
					{
						QModelIndex temp = nif->getBlock( *it );
						QString type = nif->itemName( temp );
						if ( (type == T_NISOURCETEXTURE) || (type == T_NIIMAGE) )
						{
							iTexSource = temp;
						}
					}
				}
			}
		}
	}

	QString question;

	if ( iNode.isValid() == true )
	{
		if ( iShape.isValid() == true )
		{
			question = tr("NiTriShape selected.  The first imported mesh will replace the selected one.");
		}
		else
		{
			question = tr("NiNode selected.  Meshes will be attached to the selected node.");
		}
	}
	else
	{
		question = tr("No NiNode or NiTriShape selected.  Meshes will be imported to the root of the file.");
	}

	int result = QMessageBox::question( 0, tr("Import 3DS"), question, QMessageBox::Ok, QMessageBox::Cancel );
	if ( result == QMessageBox::Cancel ) {
		return;
	}


	//--Read the file--//

	float ObjScale;
	QVector< objMesh > ObjMeshes;
	QMap< QString, objMaterial > ObjMaterials;
	QMap< QString, objKfSequence > ObjKeyframes;

	QSettings settings;
	settings.beginGroup( "import-export" );
	settings.beginGroup( "3ds" );
	
	QString fname = QFileDialog::getOpenFileName( 0, tr("Choose a .3ds file to import"), settings.value( tr(TA_FILENAME) ).toString(), "*.3ds" );
	if ( fname.isEmpty() ) {
		return;
	}
	
	QFile fobj( fname );
	if ( !fobj.open( QIODevice::ReadOnly ) )
	{
		qWarning() << tr("Could not open %1 for read access").arg( fobj.fileName() );
		return;
	}
	
	Chunk * FileChunk = Chunk::LoadFile( &fobj );
	if( !FileChunk ) {
		qWarning() << tr("Could not get 3ds data");
		return;
	}

	Chunk * Model = FileChunk->getChild( M3DMAGIC );
	if( !Model ) {
		qWarning() << tr("Could not get 3ds model");
		return;
	}

	Chunk * ModelData = Model->getChild( MDATA );
	if( !ModelData ) {
		qWarning() << tr("Could not get 3ds model data");
		return;
	}

	Chunk * MasterScale = ModelData->getChild( MASTER_SCALE );
	if( MasterScale ) {
		ObjScale = MasterScale->read< float >();
	}
	else {
		ObjScale = 1.0f;
	}

	QList< Chunk * > Materials = ModelData->getChildren( MATERIAL );
	QList< Chunk * > Meshes = ModelData->getChildren( NAMED_OBJECT );

	foreach( Chunk * mat, Materials )
	{
		objMaterial newMat;

		// material name
		Chunk * matName = mat->getChild( MAT_NAME );
		if( matName ) {
			newMat.name = matName->readString();
		}

		// material colors
		Chunk * matColor = mat->getChild( MAT_AMBIENT );
		if( matColor ) {
			newMat.Ka = GetColorFromChunk( matColor );
		}

		matColor = mat->getChild( MAT_DIFFUSE );
		if( matColor ) {
			newMat.Kd = GetColorFromChunk( matColor );
		}

		matColor = mat->getChild( MAT_SPECULAR );
		if( matColor ) {
			newMat.Ks = GetColorFromChunk( matColor );
		}

		// material textures
		Chunk * matTexture = mat->getChild( MAT_TEXMAP );
		if( matTexture ) {
			Chunk * matTexProperty = matTexture->getChild( MAT_MAPNAME );
			if( matTexProperty ) {
				newMat.map_Kd = matTexProperty->readString();
			}
		}

		// material alpha
		Chunk * matAlpha = mat->getChild( MAT_TRANSPARENCY );
		if( matAlpha ) {
			newMat.alpha = 1.0f - GetPercentageFromChunk( matAlpha );
		}

		ObjMaterials.insert( newMat.name, newMat );
	}

	foreach( Chunk * mesh, Meshes )
	{
		objMesh newMesh;

		newMesh.name = mesh->readString();

		foreach( Chunk * TriObj, mesh->getChildren( N_TRI_OBJECT ) )
		{
			Chunk * PointArray = TriObj->getChild( POINT_ARRAY );
			if( PointArray ) {
				unsigned short nPoints = PointArray->read< unsigned short >();

				for( unsigned short i = 0; i < nPoints; i++ )
				{
					float x, y, z;
					x = PointArray->read< float >();
					y = PointArray->read< float >();
					z = PointArray->read< float >();

					newMesh.vertices.append( Vector3( x, y, z ) );
					newMesh.normals.append( Vector3( 0.0f, 0.0f, 1.0f ) );
				}
			}

			Chunk * FaceArray = TriObj->getChild( FACE_ARRAY );
			if( FaceArray ) {

				unsigned short nFaces = FaceArray->read< unsigned short >();

				for( unsigned short i = 0; i < nFaces; i++ )
				{
					Chunk::ChunkTypeFaceArray f;

					f.vertex1	= FaceArray->read< unsigned short >();
					f.vertex2	= FaceArray->read< unsigned short >();
					f.vertex3	= FaceArray->read< unsigned short >();
					f.flags		= FaceArray->read< unsigned short >();

					objFace newFace;

					newFace.v1 = f.vertex1;
					newFace.v2 = f.vertex2;
					newFace.v3 = f.vertex3;

					newFace.dblside = !(f.flags & FACE_FLAG_ONESIDE);

					Vector3 n1 = newMesh.vertices[newFace.v2] - newMesh.vertices[newFace.v1];
					Vector3 n2 = newMesh.vertices[newFace.v3] - newMesh.vertices[newFace.v1];
					Vector3 FaceNormal = Vector3::crossproduct(n1, n2);
					FaceNormal.normalize();
					newMesh.normals[newFace.v1] += FaceNormal;
					newMesh.normals[newFace.v2] += FaceNormal;
					newMesh.normals[newFace.v3] += FaceNormal;

					newMesh.faces.append( newFace );

				}

				objMatFace newMatFace;

				foreach( Chunk * MatFaces, FaceArray->getChildren( MSH_MAT_GROUP ) )
				{
					//Chunk * MatFaces = FaceArray->getChild( MSH_MAT_GROUP );
					if( MatFaces ) {
						newMatFace.matName = MatFaces->readString();

						unsigned short nFaces = MatFaces->read< unsigned short >();

						for( unsigned short i = 0; i < nFaces; i++ ) {
							unsigned short FaceNum = MatFaces->read< unsigned short >();
							newMatFace.subFaces.append( FaceNum );
						}

						newMesh.matfaces.append( newMatFace );
					}
				}
			}

			Chunk * TexVerts = TriObj->getChild( TEX_VERTS );
			if( TexVerts ) {
				unsigned short nVerts = TexVerts->read< unsigned short >();

				for( unsigned short i = 0; i < nVerts; i++ )
				{
					float x, y;
					x = TexVerts->read< float >();
					y = TexVerts->read< float >();

					newMesh.texcoords.append( Vector2( x, -y ) );
				}
			}

		}

		for( int i = 0; i < newMesh.normals.size(); i++ )
		{
			newMesh.normals[i].normalize();
		}

		ObjMeshes.append( newMesh );
	}

	Chunk * Keyframes = Model->getChild( KFDATA );
	if( Keyframes ) {
		if( Chunk * KfHdr = Keyframes->getChild( KFHDR ) )
		{

		}

		QList< Chunk * > KfSegs = Keyframes->getChildren( KFSEG );
		QList< Chunk * > KfCurTimes = Keyframes->getChildren( KFCURTIME );

		for( int i = 0; i < KfSegs.size(); i++ )
		{
			/*
			Chunk::ChunkData * rawData = KfSegs[i]->getData();
			newKfSeg.startTime = *( (long *) rawData );
			rawData += sizeof( long );
			newKfSeg.endTime = *( (long *) rawData );
			KfSegs[i]->clearData();

			Chunk * KfCurTime = KfCurTimes[i];

			rawData = KfCurTimes[i]->getData();
			newKfSeg.curTime = *( (long *) rawData );
			KfCurTimes[i]->clearData();
			*/
		}

		foreach( Chunk * KfObj, Keyframes->getChildren( OBJECT_NODE_TAG ) )
		{
			objKfSequence newKfSeq;
			
			if( Chunk * NodeId = KfObj->getChild( NODE_ID ) ) {
				newKfSeq.objectId = NodeId->read< unsigned short >();
			}

			if( Chunk * NodeHdr = KfObj->getChild( NODE_HDR ) ) {
				newKfSeq.objectName = NodeHdr->readString();

				unsigned short Flags1 = NodeHdr->read< unsigned short >();
				unsigned short Flags2 = NodeHdr->read< unsigned short >();
				unsigned short Hierarchy = NodeHdr->read< unsigned short >();
			}

			if( Chunk * Pivot = KfObj->getChild( PIVOT ) ) {
				float x = Pivot->read< float >();
				float y = Pivot->read< float >();
				float z = Pivot->read< float >();

				newKfSeq.pivot = Vector3( x, y, z );
			}

			if( Chunk * PosTrack = KfObj->getChild( POS_TRACK_TAG ) ) {
				unsigned short flags = PosTrack->read< unsigned short >();

				unsigned short unknown1 = PosTrack->read< unsigned short >();
				unsigned short unknown2 = PosTrack->read< unsigned short >();
				unsigned short unknown3 = PosTrack->read< unsigned short >();
				unsigned short unknown4 = PosTrack->read< unsigned short >();

				unsigned short keys = PosTrack->read< unsigned short >();
				
				unsigned short unknown = PosTrack->read< unsigned short >();

				for( int key = 0; key < keys; key++ )
				{
					unsigned short kfNum = PosTrack->read< unsigned short >();
					unsigned long kfUnknown = PosTrack->read< unsigned long >();
					float kfPosX = PosTrack->read< float >();
					float kfPosY = PosTrack->read< float >();
					float kfPosZ = PosTrack->read< float >();

					newKfSeq.frames[kfNum].pos = Vector3( kfPosX, kfPosY, kfPosZ );
				}
			}

			if( Chunk * RotTrack = KfObj->getChild( ROT_TRACK_TAG ) ) {
				unsigned short flags = RotTrack->read< unsigned short >();

				unsigned short unknown1 = RotTrack->read< unsigned short >();
				unsigned short unknown2 = RotTrack->read< unsigned short >();
				unsigned short unknown3 = RotTrack->read< unsigned short >();
				unsigned short unknown4 = RotTrack->read< unsigned short >();

				unsigned short keys = RotTrack->read< unsigned short >();
				
				unsigned short unknown = RotTrack->read< unsigned short >();

				for( unsigned short key = 0; key < keys; key++ )
				{
					unsigned short kfNum = RotTrack->read< unsigned short >();
					unsigned long kfUnknown = RotTrack->read< unsigned long >();
					float kfRotAngle = RotTrack->read< float >();
					float kfAxisX = RotTrack->read< float >();
					float kfAxisY = RotTrack->read< float >();
					float kfAxisZ = RotTrack->read< float >();
					
					newKfSeq.frames[kfNum].rotAngle = kfRotAngle;
					newKfSeq.frames[kfNum].rotAxis = Vector3( kfAxisX, kfAxisY, kfAxisZ );
				}
			}

			if( Chunk * SclTrack = KfObj->getChild( SCL_TRACK_TAG ) ) {
				unsigned short flags = SclTrack->read< unsigned short >();

				unsigned short unknown1 = SclTrack->read< unsigned short >();
				unsigned short unknown2 = SclTrack->read< unsigned short >();
				unsigned short unknown3 = SclTrack->read< unsigned short >();
				unsigned short unknown4 = SclTrack->read< unsigned short >();

				unsigned short keys = SclTrack->read< unsigned short >();
				
				unsigned short unknown = SclTrack->read< unsigned short >();

				for( unsigned short key = 0; key < keys; key++ )
				{
					unsigned short kfNum = SclTrack->read< unsigned short >();
					unsigned long kfUnknown = SclTrack->read< unsigned long >();
					float kfSclX = SclTrack->read< float >();
					float kfSclY = SclTrack->read< float >();
					float kfSclZ = SclTrack->read< float >();

					newKfSeq.frames[kfNum].scale =	( kfSclX + kfSclY + kfSclZ ) / 3.0f;
				}
			}

			ObjKeyframes.insertMulti( newKfSeq.objectName, newKfSeq );
		}
	}

	fobj.close();

	//--Translate file structures into NIF ones--//

	if ( iNode.isValid() == false )
	{
		iNode = nif->insertNiBlock( T_NINODE );
		nif->set<QString>( iNode, TA_NAME, "Scene Root" );
	}

	//Record root object
	iRoot = iNode;

	// create a NiTriShape foreach material in the object
	for(int objIndex = 0; objIndex < ObjMeshes.size(); objIndex++) {
		objMesh * mesh = &ObjMeshes[objIndex];

		

		// create group node if there is more than 1 material
		bool groupNode = false;
		QPersistentModelIndex iNode = iRoot;
		if ( mesh->matfaces.size() > 1 )
		{
			groupNode = true;

			iNode = nif->insertNiBlock( T_NINODE );
			nif->set<QString>( iNode, TA_NAME, mesh->name );
			addLink( nif, iRoot, TA_CHILDREN, nif->getBlockNumber( iNode ) );
		}

		int shapecount = 0;
		for( int i = 0; i < mesh->matfaces.size(); i++ )
		{
			if ( !ObjMaterials.contains( mesh->matfaces[i].matName ) ) {
				qWarning() << tr("Material '%1' not found in list!").arg( mesh->matfaces[i].matName );
			}

			objMaterial * mat = &ObjMaterials[mesh->matfaces[i].matName];

			if ( iShape.isValid() == false || objIndex != 0 )
			{
				iShape = nif->insertNiBlock( T_NITRISHAPE );
			}
			if ( groupNode )
			{
				nif->set<QString>( iShape, TA_NAME, QString( "%1:%2" ).arg( nif->get<QString>( iNode, TA_NAME ) ).arg( shapecount++ ) );
				addLink( nif, iNode, TA_CHILDREN, nif->getBlockNumber( iShape ) );
			}
			else
			{
				nif->set<QString>( iShape, TA_NAME, mesh->name );
				addLink( nif, iRoot, TA_CHILDREN, nif->getBlockNumber( iShape ) );
			}
			
			if ( iMaterial.isValid() == false || objIndex != 0 )
			{
				iMaterial = nif->insertNiBlock( T_NIMATERIALPROPERTY );
			}
			nif->set<QString>( iMaterial, TA_NAME, mat->name );
			nif->set<Color3>( iMaterial, TA_AMBIENTCOLOR, mat->Ka );
			nif->set<Color3>( iMaterial, TA_DIFFUSECOLOR, mat->Kd );
			nif->set<Color3>( iMaterial, TA_SPECULARCOLOR, mat->Ks );
			nif->set<Color3>( iMaterial, TA_EMISSIVECOLOR, Color3( 0, 0, 0 ) );
			nif->set<float>( iMaterial, TA_ALPHA, mat->alpha );
			nif->set<float>( iMaterial, TA_GLOSSINESS, mat->glossiness );
			
			addLink( nif, iShape, TA_PROPERTIES, nif->getBlockNumber( iMaterial ) );
			
			if ( !mat->map_Kd.isEmpty() )
			{
				if ( nif->getVersionNumber() >= NF_V03030013 )
				{
					//Newer versions use NiTexturingProperty and NiSourceTexture
					if ( iTexProp.isValid() == false || objIndex != 0 || nif->itemType(iTexProp) != T_NITEXTURINGPROPERTY )
					{
						iTexProp = nif->insertNiBlock( T_NITEXTURINGPROPERTY );
					}
					addLink( nif, iShape, TA_PROPERTIES, nif->getBlockNumber( iTexProp ) );
					
					nif->set<int>( iTexProp, TA_HASBASETEXTURE, 1 );
					QModelIndex iBaseMap = nif->getIndex( iTexProp, TA_BASETEXTURE );
					nif->set<int>( iBaseMap, TA_CLAMPMODE, 3 );
					nif->set<int>( iBaseMap, TA_FILTERMODE, 2 );
					
					if ( iTexSource.isValid() == false || objIndex != 0 || nif->itemType(iTexSource) != T_NISOURCETEXTURE )
					{
						iTexSource = nif->insertNiBlock( T_NISOURCETEXTURE );
					}
					nif->setLink( iBaseMap, TA_SOURCE, nif->getBlockNumber( iTexSource ) );
					
					nif->set<int>( iTexSource, TA_PIXELLAYOUT, nif->getVersion() == STR_V20000005 ? 6 : 5 );
					nif->set<int>( iTexSource, TA_USEMIPMAPS, 2 );
					nif->set<int>( iTexSource, TA_ALPHAFORMAT, 3 );
					nif->set<int>( iTexSource, TA_UNKNOWNBYTE, 1 );
					nif->set<int>( iTexSource, TA_UNKNOWNBYTE2, 1 );
					
					nif->set<int>( iTexSource, TA_USEEXTERNAL, 1 );
					nif->set<QString>( iTexSource, TA_FILENAME, mat->map_Kd );
				}
				else
				{
					//Older versions use NiTextureProperty and NiImage
					if ( iTexProp.isValid() == false || objIndex != 0 || nif->itemType(iTexProp) != T_NITEXTUREPROPERTY )
					{
						iTexProp = nif->insertNiBlock( T_NITEXTUREPROPERTY );
					}
					addLink( nif, iShape, TA_PROPERTIES, nif->getBlockNumber( iTexProp ) );
					
					if ( iTexSource.isValid() == false || objIndex != 0 || nif->itemType(iTexSource) != T_NIIMAGE )
					{
						iTexSource = nif->insertNiBlock( T_NIIMAGE );
					}

					nif->setLink( iTexProp, TA_IMAGE, nif->getBlockNumber( iTexSource ) );
					
					nif->set<int>( iTexSource, TA_EXTERNAL, 1 );
					nif->set<QString>( iTexSource, TA_FILENAME, mat->map_Kd );
				}
			}
			
			if ( iData.isValid() == false || objIndex != 0 )
			{
				iData = nif->insertNiBlock( T_NITRISHAPEDATA );
			}
			nif->setLink( iShape, TA_DATA, nif->getBlockNumber( iData ) );
			
			QVector< Triangle > triangles;
			QVector< objPoint > points;
			
			foreach( short faceIndex, mesh->matfaces[i].subFaces )
			{
				objFace face = mesh->faces[faceIndex];

				Triangle tri;
				
				tri.set( face.v1, face.v2, face.v3 );

				triangles.append( tri );
			}
			
			nif->set<int>( iData, TA_NUMVERTICES, mesh->vertices.count() );
			nif->set<int>( iData, TA_HASVERTICES, 1 );
			nif->updateArray( iData, TA_VERTICES );
			nif->setArray<Vector3>( iData, TA_VERTICES,  mesh->vertices );
			nif->set<int>( iData, TA_HASNORMALS, 1 );
			nif->updateArray( iData, TA_NORMALS );
			nif->setArray<Vector3>( iData, TA_NORMALS,  mesh->normals );
			nif->set<int>( iData, TA_HASUV, 1 );
			nif->set<int>( iData, TA_NUMUVSETS, 1 );
			nif->set<int>( iData, TA_NUMUVSETS2, 1 );
			QModelIndex iTexCo = nif->getIndex( iData, TA_UVSETS );
			if ( !iTexCo.isValid() ) {
				iTexCo = nif->getIndex( iData, TA_UVSETS2 );
			}
			nif->updateArray( iTexCo );
			nif->updateArray( iTexCo.child( 0, 0 ) );
			nif->setArray<Vector2>( iTexCo.child( 0, 0 ),  mesh->texcoords );
			
			nif->set<int>( iData, TA_HASTRIANGLES, 1 );
			nif->set<int>( iData, TA_NUMTRIANGLES, triangles.count() );
			nif->set<int>( iData, TA_NUMTRIANGLEPOINTS, triangles.count() * 3 );
			nif->updateArray( iData, TA_TRIANGLES );
			nif->setArray<Triangle>( iData, TA_TRIANGLES, triangles );
			
			Vector3 center;
			foreach ( Vector3 v,  mesh->vertices )
				center += v;
			if (  mesh->vertices.count() > 0 ) center /=  mesh->vertices.count();
			nif->set<Vector3>( iData, TA_CENTER, center );
			float radius = 0;
			foreach ( Vector3 v,  mesh->vertices )
			{
				float d = ( center - v ).length();
				if ( d > radius ) radius = d;
			}
			nif->set<float>( iData, TA_RADIUS, radius );
			
			nif->set<int>( iData, TA_UNKNOWNSHORT2, 0x4000 );
		}

		// set up a controller for animated objects
	}
	
	settings.setValue( TA_FILENAME, fname );
	
	nif->reset();
	return;
}
