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

#include "nifmodel.h"

#include "NvTriStrip/qtwrapper.h"

#include "gl/gltex.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTextStream>
#include <QApplication>

#define tr(x) QApplication::tr(x)

/*
 *  .OBJ EXPORT
 */



static void writeData( const NifModel * nif, const QModelIndex & iData, QTextStream & obj, int ofs[1], Transform t )
{
	// copy vertices
	
	QVector<Vector3> verts = nif->getArray<Vector3>( iData, TA_VERTICES );
	foreach ( Vector3 v, verts )
	{
		v = t * v;
		obj << "v " << qSetRealNumberPrecision(17) << v[0] << " " << v[1] << " " << v[2] << "\r\n";
	}
	
	// copy texcoords
	
	QModelIndex iUV = nif->getIndex( iData, TA_UVSETS );
	if ( ! iUV.isValid() )
		iUV = nif->getIndex( iData, TA_UVSETS2 );
	
	QVector<Vector2> texco = nif->getArray<Vector2>( iUV.child( 0, 0 ) );
	foreach( Vector2 t, texco )
		obj << "vt " << t[0] << " " << 1.0 - t[1] << "\r\n";
	
	// copy normals
	
	QVector<Vector3> norms = nif->getArray<Vector3>( iData, TA_NORMALS );
	foreach ( Vector3 n, norms )
	{
		n = t.rotation * n;
		obj << "vn " << n[0] << " " << n[1] << " " << n[2] << "\r\n";
	}
	
	// get the triangles
	
	QVector<Triangle> tris;
	
	QModelIndex iPoints = nif->getIndex( iData, TA_POINTS );
	if ( iPoints.isValid() )
	{
		QList< QVector<quint16> > strips;
		for ( int r = 0; r < nif->rowCount( iPoints ); r++ )
			strips.append( nif->getArray<quint16>( iPoints.child( r, 0 ) ) );
		tris = triangulate( strips );
	}
	else
	{
		tris = nif->getArray<Triangle>( iData, TA_TRIANGLES );
	}
	
	// write the triangles
	
	foreach ( Triangle t, tris )
	{
		obj << "f";
		for ( int p = 0; p < 3; p++ )
		{
			obj << " " << ofs[0] + t[p];
			if ( norms.count() )
				if ( texco.count() )
					obj << "/" << ofs[1] + t[p] << "/" << ofs[2] + t[p];
				else
					obj << "//" << ofs[2] + t[p];
			else
				if ( texco.count() )
					obj << "/" << ofs[1] + t[p];
		}
		obj << "\r\n";
	}
	
	ofs[0] += verts.count();
	ofs[1] += texco.count();
	ofs[2] += norms.count();
}

static void writeShape( const NifModel * nif, const QModelIndex & iShape, QTextStream & obj, QTextStream & mtl, int ofs[], Transform t )
{
	QString name = nif->get<QString>( iShape, TA_NAME );
	QString matn = name, map_Kd, map_Ks, map_Ns, map_d ,disp, decal, bump;
	
	Color3 mata, matd, mats;
	float matt = 1.0, matg = 33.0;
	
	foreach ( qint32 link, nif->getChildLinks( nif->getBlockNumber( iShape ) ) )
	{
		QModelIndex iProp = nif->getBlock( link );
		if ( nif->isNiBlock( iProp, T_NIMATERIALPROPERTY ) )
		{
			mata = nif->get<Color3>( iProp, TA_AMBIENTCOLOR );
			matd = nif->get<Color3>( iProp, TA_DIFFUSECOLOR );
			mats = nif->get<Color3>( iProp, TA_SPECULARCOLOR );
			matt = nif->get<float>( iProp, TA_ALPHA );
			matg = nif->get<float>( iProp, TA_GLOSSINESS );
			//matn = nif->get<QString>( iProp, TA_NAME );
		}
		else if ( nif->isNiBlock( iProp, T_NITEXTURINGPROPERTY ) )
		{
			QModelIndex iBase = nif->getBlock( nif->getLink( nif->getIndex( iProp, TA_BASETEXTURE ), TA_SOURCE ), T_NISOURCETEXTURE );
			map_Kd = TexCache::find( nif->get<QString>( iBase, TA_FILENAME ), nif->getFolder() );

			QModelIndex iDark = nif->getBlock( nif->getLink( nif->getIndex( iProp, TA_DECAL1TEXTURE ), TA_SOURCE ), T_NISOURCETEXTURE );
			decal = TexCache::find( nif->get<QString>( iDark, TA_FILENAME ), nif->getFolder() );

			QModelIndex iBump = nif->getBlock( nif->getLink( nif->getIndex( iProp, TA_BUMPMAPTEXTURE ), TA_SOURCE ), T_NISOURCETEXTURE );
			bump = TexCache::find( nif->get<QString>( iBump, TA_FILENAME ), nif->getFolder() );
		}
		else if ( nif->isNiBlock( iProp, T_NITEXTUREPROPERTY ) )
		{
			QModelIndex iSource = nif->getBlock( nif->getLink( iProp, TA_IMAGE ), T_NIIMAGE );
			map_Kd = TexCache::find( nif->get<QString>( iSource, TA_FILENAME ), nif->getFolder() );
		}
		else if ( nif->isNiBlock( iProp, T_NISKININSTANCE ) )
		{
			QMessageBox::warning(
				0,
				"OBJ Export Warning",
				QString("The shape ") + name + QString(" is skinned, but the "
				"obj format does not support skinning. This mesh will be "
				"exported statically in its bind pose, without skin weights.")
			);
		}
		else if ( nif->isNiBlock( iProp, T_BSSHADERNOLIGHTINGPROPERTY ) 
			    || nif->isNiBlock( iProp, T_SKYSHADERPROPERTY ) 
				 || nif->isNiBlock( iProp, T_TILESHADERPROPERTY ) 
			     )
		{
			map_Kd = TexCache::find( nif->get<QString>( iProp, TA_FILENAME ), nif->getFolder() );
		}
		else if ( nif->isNiBlock( iProp, T_BSSHADERPPLIGHINGPROPERTY ) 
			    || nif->isNiBlock( iProp, T_LIGHTING30SHADERPROPERTY ) 
			     )
		{
			QModelIndex iArray = nif->getIndex( nif->getBlock( nif->getLink( iProp, TA_TEXTURESET ) ) , TA_TEXTURES);
			map_Kd = TexCache::find( nif->get<QString>( iArray.child( 0, 0 ) ), nif->getFolder() );
		}
	}
	
	//if ( ! texfn.isEmpty() )
	//	matn += ":" + texfn;
	
	matn = QString( "Material.%1" ).arg( ofs[0], 6, 16, QChar( '0' ) );
	
	mtl << "\r\n";
	mtl << "newmtl " << matn << "\r\n";
	mtl << "Ka " << mata[0] << " "  << mata[1] << " "  << mata[2] << "\r\n";
	mtl << "Kd " << matd[0] << " "  << matd[1] << " "  << matd[2] << "\r\n";
	mtl << "Ks " << mats[0] << " "  << mats[1] << " " << mats[2] << "\r\n";
	mtl << "d " << matt << "\r\n";
	mtl << "Ns " << matg << "\r\n";
	if ( ! map_Kd.isEmpty() )
		mtl << "map_Kd " << map_Kd << "\r\n\r\n";
	if ( ! decal.isEmpty() )
		mtl << "decal " << decal << "\r\n\r\n";
	if ( ! bump.isEmpty() )
		mtl << "bump " << decal << "\r\n\r\n";
	
	obj << "\r\n# " << name << "\r\n\r\ng " << name << "\r\n" << "usemtl " << matn << "\r\n\r\n";
	
	writeData( nif, nif->getBlock( nif->getLink( iShape, TA_DATA ) ), obj, ofs, t );
}

static void writeParent( const NifModel * nif, const QModelIndex & iNode, QTextStream & obj, QTextStream & mtl, int ofs[], Transform t )
{
	t = t * Transform( nif, iNode );
	foreach ( int l, nif->getChildLinks( nif->getBlockNumber( iNode ) ) )
	{
		QModelIndex iChild = nif->getBlock( l );
		if ( nif->inherits( iChild, T_NINODE ) )
			writeParent( nif, iChild, obj, mtl, ofs, t );
		else if ( nif->isNiBlock( iChild, T_NITRISHAPE ) || nif->isNiBlock( iChild, T_NITRISTRIPS ) )
			writeShape( nif, iChild, obj, mtl, ofs, t * Transform( nif, iChild ) );
		else if ( nif->inherits( iChild, T_NICOLLISIONOBJECT ) )
		{
			QModelIndex iBody = nif->getBlock( nif->getLink( iChild, TA_BODY ) );
			if ( iBody.isValid() )
			{
				Transform bt;
				bt.scale = 7;
				if ( nif->isNiBlock( iBody, T_BHKRIGIDBODYT ) )
				{
					bt.rotation.fromQuat( nif->get<Quat>( iBody, TA_ROTATION ) );
					bt.translation = Vector3( nif->get<Vector4>( iBody, TA_TRANSLATION ) * 7 );
				}
				QModelIndex iShape = nif->getBlock( nif->getLink( iBody, TA_SHAPE ) );
				if ( nif->isNiBlock( iShape, T_BHKMOPPBVTREESHAPE ) )
				{
					iShape = nif->getBlock( nif->getLink( iShape, TA_SHAPE ) );
					if ( nif->isNiBlock( iShape, T_BHKPACKEDNITRISTRIPSSHAPE ) )
					{
						QModelIndex iData = nif->getBlock( nif->getLink( iShape, TA_DATA ) );
						if ( nif->isNiBlock( iData, T_HKPACKEDNITRISTRIPSDATA ) )
						{
							bt = t * bt;
							obj << "\r\n# "T_BHKPACKEDNITRISTRIPSSHAPE"\r\n\r\ng collision\r\n" << "usemtl collision\r\n\r\n";
							QVector<Vector3> verts = nif->getArray<Vector3>( iData, TA_VERTICES );
							foreach ( Vector3 v, verts )
							{
								v = bt * v;
								obj << "v " << v[0] << " " << v[1] << " " << v[2] << "\r\n";
							}
							
							QModelIndex iTris = nif->getIndex( iData, TA_TRIANGLES );
							for ( int t = 0; t < nif->rowCount( iTris ); t++ )
							{
								Triangle tri = nif->get<Triangle>( iTris.child( t, 0 ), TA_TRIANGLE );
								Vector3 n = nif->get<Vector3>( iTris.child( t, 0 ), TA_NORMAL );
								
								Vector3 a = verts.value( tri[0] );
								Vector3 b = verts.value( tri[1] );
								Vector3 c = verts.value( tri[2] );
								
								Vector3 fn = Vector3::crossproduct( b - a, c - a );
								fn.normalize();
								
								bool flip = Vector3::dotproduct( n, fn ) < 0;
								
								obj << "f"
									<< " " << tri[0] + ofs[0]
									<< " " << tri[ flip ? 2 : 1 ] + ofs[0]
									<< " " << tri[ flip ? 1 : 2 ] + ofs[0]
									<< "\r\n";
							}
							ofs[0] += verts.count();
						}
					}
				}
				else if ( nif->isNiBlock( iShape, T_BHKNITRISTRIPSSHAPE ) )
				{
					bt.scale = 1;
					obj << "\r\n# "T_BHKNITRISTRIPSSHAPE"\r\n\r\ng collision\r\n" << "usemtl collision\r\n\r\n";
					QModelIndex iStrips = nif->getIndex( iShape, TA_STRIPSDATA );
					for ( int r = 0; r < nif->rowCount( iStrips ); r++ )
						writeData( nif, nif->getBlock( nif->getLink( iStrips.child( r, 0 ) ), T_NITRISTRIPSDATA ), obj, ofs, t * bt );
				}
			}
		}
	}
}

void exportObj( const NifModel * nif, const QModelIndex & index )
{
	//--Determine how the file will export, and be sure the user wants to continue--//
	QList<int> roots;
	QModelIndex iBlock = nif->getBlock( index );

	QString question;
	if ( iBlock.isValid() )
	{
		roots.append( nif->getBlockNumber(index) );
		if ( nif->itemName(index) == T_NINODE )
		{
			question = tr(T_NINODE" selected.  All children of selected node will be exported.");
		} else if ( nif->itemName(index) == T_NITRISHAPE || nif->itemName(index) == T_NITRISTRIPS )
		{
			question = nif->itemName(index) + tr(" selected.  Selected mesh will be exported.");
		}
	}
	
	if ( question.size() == 0 )
	{
		question = tr("No "T_NINODE", "T_NITRISHAPE",or "T_NITRISTRIPS" is selected.  Entire scene will be exported.");
		roots = nif->getRootLinks();
	}

	int result = QMessageBox::question( 0, tr("Export OBJ"), question, QMessageBox::Ok, QMessageBox::Cancel );
	if ( result == QMessageBox::Cancel ) {
		return;
	}

	//--Allow the user to select the file--//

	QSettings settings;
	settings.beginGroup( "import-export" );
	settings.beginGroup( "obj" );

	QString fname = QFileDialog::getSaveFileName( 0, tr("Choose a .OBJ file for export"), settings.value( TA_FILENAME ).toString(), "*.obj" );
	if ( fname.isEmpty() )
		return;
	
	while ( fname.endsWith( ".obj", Qt::CaseInsensitive ) )
		fname = fname.left( fname.length() - 4 );
	
	QFile fobj( fname + ".obj" );
	if ( ! fobj.open( QIODevice::WriteOnly ) )
	{
		qWarning() << "could not open " << fobj.fileName() << " for write access";
		return;
	}
	
	QFile fmtl( fname + ".mtl" );
	if ( ! fmtl.open( QIODevice::WriteOnly ) )
	{
		qWarning() << "could not open " << fmtl.fileName() << " for write access";
		return;
	}
	
	fname = fmtl.fileName();
	int i = fname.lastIndexOf( "/" );
	if ( i >= 0 )
		fname = fname.remove( 0, i+1 );
	
	QTextStream sobj( &fobj );
	QTextStream smtl( &fmtl );
	
	sobj << "# exported with NifSkope\r\n\r\n" << "mtllib " << fname << "\r\n";

	//--Translate NIF structure into file structure --//

	int ofs[3] = { 1, 1, 1 };
	foreach ( int l, roots )
	{
		QModelIndex iBlock = nif->getBlock( l );
		if ( nif->inherits( iBlock, T_NINODE ) )
			writeParent( nif, iBlock, sobj, smtl, ofs, Transform() );
		else if ( nif->isNiBlock( iBlock, T_NITRISHAPE ) || nif->isNiBlock( iBlock, T_NITRISTRIPS ) )
			writeShape( nif, iBlock, sobj, smtl, ofs, Transform() );
	}
	
	settings.setValue( TA_FILENAME, fobj.fileName() );
}



/*
 *  .OBJ IMPORT
 */


struct ObjPoint
{
	int v, t, n;
	
	bool operator==( const ObjPoint & other ) const
	{
		return v == other.v && t == other.t && n == other.n;
	}
};

struct ObjFace
{
	ObjPoint p[3];
};

struct ObjMaterial
{
	Color3 Ka, Kd, Ks;
	float d, Ns;
	QString map_Kd;
	
	ObjMaterial() : d( 1.0 ), Ns( 31.0 ) {}
};

static void readMtlLib( const QString & fname, QMap< QString, ObjMaterial > & omaterials )
{
	QFile file( fname );
	if ( ! file.open( QIODevice::ReadOnly ) )
	{
		qWarning() << "failed to open" << fname;
		return;
	}
	
	QTextStream smtl( &file );
	
	QString mtlid;
	ObjMaterial mtl;
	
	while ( ! smtl.atEnd() )
	{
		QString line = smtl.readLine();
		
		QStringList t = line.split( " ", QString::SkipEmptyParts );
		
		if ( t.value( 0 ) == "newmtl" )
		{
			if ( ! mtlid.isEmpty() )
				omaterials.insert( mtlid, mtl );
			mtlid = t.value( 1 );
			mtl = ObjMaterial();
		}
		else if ( t.value( 0 ) == "Ka" )
		{
			mtl.Ka = Color3( t.value( 1 ).toDouble(), t.value( 2 ).toDouble(), t.value( 3 ).toDouble() );
		}
		else if ( t.value( 0 ) == "Kd" )
		{
			mtl.Kd = Color3( t.value( 1 ).toDouble(), t.value( 2 ).toDouble(), t.value( 3 ).toDouble() );
		}
		else if ( t.value( 0 ) == "Ks" )
		{
			mtl.Ks = Color3( t.value( 1 ).toDouble(), t.value( 2 ).toDouble(), t.value( 3 ).toDouble() );
		}
		else if ( t.value( 0 ) == "d" )
		{
			mtl.d = t.value( 1 ).toDouble();
		}
		else if ( t.value( 0 ) == "Ns" )
		{
			mtl.Ns = t.value( 1 ).toDouble();
		}
		else if ( t.value( 0 ) == "map_Kd" )
		{
			// handle spaces in filenames
			mtl.map_Kd = t.value( 1 );
			for (int i = 2; i < t.size (); i++)
				mtl.map_Kd += " " + t.value( i );
		}
	}
	if ( ! mtlid.isEmpty() )
		omaterials.insert( mtlid, mtl );
}

static void addLink( NifModel * nif, QModelIndex iBlock, QString name, qint32 link )
{
	QModelIndex iArray = nif->getIndex( iBlock, name );
	QModelIndex iSize = nif->getIndex( iBlock, QString( "Num %1" ).arg( name ) );
	int numIndices = nif->get<int>( iSize );
	nif->set<int>( iSize, numIndices + 1 );
	nif->updateArray( iArray );
	nif->setLink( iArray.child( numIndices, 0 ), link );
}

void importObj( NifModel * nif, const QModelIndex & index )
{
	//--Determine how the file will import, and be sure the user wants to continue--//

	// If no existing node is selected, create a group node.  Otherwise use selected node
	QPersistentModelIndex iNode, iShape, iMaterial, iData, iTexProp, iTexSource;
	QModelIndex iBlock = nif->getBlock( index );
	bool cBSShaderPPLightingProperty = false;
	//Be sure the user hasn't clicked on a NiTriStrips object
	if ( iBlock.isValid() && nif->itemName(iBlock) == T_NITRISTRIPS )
	{
                QMessageBox::information( 0, tr("Import OBJ"), tr("You cannot import an OBJ file over a "T_NITRISTRIPS" object.  Please convert it to a "T_NITRISHAPE" object first by right-clicking and choosing Mesh > Triangulate") );
		return;
	}

	if ( iBlock.isValid() && nif->itemName(iBlock) == T_NINODE )
	{
		iNode = iBlock;
	}
	else if ( iBlock.isValid() && nif->itemName( iBlock ) == T_NITRISHAPE )
	{
		iShape = iBlock;
		//Find parent of NiTriShape
		int par_num = nif->getParent( nif->getBlockNumber( iBlock ) );
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
				if ( type == T_BSSHADERPPLIGHINGPROPERTY )
				{
					cBSShaderPPLightingProperty = true;
				}
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
			question = tr(T_NITRISHAPE" selected.  The first imported mesh will replace the selected one.");
		}
		else
		{
			question = tr(T_NINODE" selected.  Meshes will be attached to the selected node.");
		}
	}
	else
	{
		question = tr("No "T_NINODE" or "T_NITRISHAPE" selected.  Meshes will be imported to the root of the file.");
	}

	int result = QMessageBox::question( 0, tr("Import OBJ"), question, QMessageBox::Ok, QMessageBox::Cancel );
	if ( result == QMessageBox::Cancel ) {
		return;
	}

	//--Read the file--//

	QSettings settings;
	settings.beginGroup( "import-export" );
	settings.beginGroup( "obj" );
	
	QString fname = QFileDialog::getOpenFileName( 0, tr("Choose a .OBJ file to import"), settings.value( TA_FILENAME ).toString(), "*.obj" );
	if ( fname.isEmpty() )
		return;
	
	QFile fobj( fname );
	if ( ! fobj.open( QIODevice::ReadOnly ) )
	{
		qWarning() << tr("could not open ") << fobj.fileName() << tr(" for read access");
		return;
	}
	
	QTextStream sobj( & fobj );
	
	QVector<Vector3> overts;
	QVector<Vector3> onorms;
	QVector<Vector2> otexco;
	QMap< QString, QVector<ObjFace> * > ofaces;
	QMap< QString, ObjMaterial > omaterials;
	
	QVector<ObjFace> * mfaces = new QVector<ObjFace>();
	
	QString usemtl = STR_NOTHING;
	ofaces.insert( usemtl, mfaces );
	
	while ( ! sobj.atEnd() )
	{	// parse each line of the file
		QString line = sobj.readLine();
		
		QStringList t = line.split( " ", QString::SkipEmptyParts );
		
		if ( t.value( 0 ) == "mtllib" )
		{
			readMtlLib( fname.left( qMax( fname.lastIndexOf( "/" ), fname.lastIndexOf( "\\" ) ) + 1 ) + t.value( 1 ), omaterials );
		}
		else if ( t.value( 0 ) == "usemtl" )
		{
			usemtl = t.value( 1 );
			//if ( usemtl.contains( "_" ) )
			//	usemtl = usemtl.left( usemtl.indexOf( "_" ) );
			
			mfaces = ofaces.value( usemtl );
			if ( ! mfaces )
			{
				mfaces = new QVector<ObjFace>();
				ofaces.insert( usemtl, mfaces );
			}
		}
		else if ( t.value( 0 ) == "v" )
		{
			overts.append( Vector3( t.value( 1 ).toDouble(), t.value( 2 ).toDouble(), t.value( 3 ).toDouble() ) );
		}
		else if ( t.value( 0 ) == "vt" )
		{
			otexco.append( Vector2( t.value( 1 ).toDouble(), 1.0 - t.value( 2 ).toDouble() ) );
		}
		else if ( t.value( 0 ) == "vn" )
		{
			onorms.append( Vector3( t.value( 1 ).toDouble(), t.value( 2 ).toDouble(), t.value( 3 ).toDouble() ) );
		}
		else if ( t.value( 0 ) == "f" )
		{
			if ( t.count() > 5 )
			{
				qWarning() << "please triangulate your mesh before import";
				return;
			}
			
			for ( int j = 1; j < t.count() - 2; j++ )
			{
				ObjFace face;
				for ( int i = 0; i < 3; i++ )
				{
					QStringList lst = t.value( i == 0 ? 1 : j+i ).split( "/" );
					
					int v = lst.value( 0 ).toInt();
					if ( v < 0 ) v += overts.count();	else v--;
					
					int t = lst.value( 1 ).toInt();
					if ( t < 0 ) v += otexco.count(); else t--;
					
					int n = lst.value( 2 ).toInt();
					if ( n < 0 ) n += onorms.count(); else n--;
					
					face.p[i].v = v;
					face.p[i].t = t;
					face.p[i].n = n;
				}
				mfaces->append( face );
			}
		}
	}

	//--Translate file structures into NIF ones--//

	if ( iNode.isValid() == false )
	{
		iNode = nif->insertNiBlock( T_NINODE );
		nif->set<QString>( iNode, TA_NAME, "Scene Root" );
	}

	// create a NiTriShape foreach material in the object
	int shapecount = 0;
	bool first_tri_shape = true;
	QMapIterator< QString, QVector<ObjFace> * > it( ofaces );
	while ( it.hasNext() )
	{
		it.next();
		
		if ( ! it.value()->count() )
			continue;
		
		if ( it.key() != "collision" )
		{
			//If we are on the first shape, and one was selected in the 3D view, use the existing one
			bool newiShape = false;
			if ( iShape.isValid() == false || first_tri_shape == false )
			{
				iShape = nif->insertNiBlock( T_NITRISHAPE );
				newiShape = true;
			}

			if (newiShape)// don't change a name what already exists; // don't add duplicates
			{
				nif->set<QString>( iShape, TA_NAME, QString( "%1:%2" ).arg( nif->get<QString>( iNode, TA_NAME ) ).arg( shapecount++ ) );
				addLink( nif, iNode, TA_CHILDREN, nif->getBlockNumber( iShape ) );
			}
			
			if ( !omaterials.contains( it.key() ) )
				qWarning() << "material" << it.key() << "not found in mtllib";
			
			ObjMaterial mtl = omaterials.value( it.key() );
			
			bool newiMaterial = false;
			if ( iMaterial.isValid() == false || first_tri_shape == false )
			{
				iMaterial = nif->insertNiBlock( T_NIMATERIALPROPERTY );
				newiMaterial = true;
			}
			if (newiMaterial)// don't affect a property  that is already there - that name is generated above on export and it has nothign to do with the stored name
				nif->set<QString>( iMaterial, TA_NAME, it.key() );
			nif->set<Color3>( iMaterial, TA_AMBIENTCOLOR, mtl.Ka );
			nif->set<Color3>( iMaterial, TA_DIFFUSECOLOR, mtl.Kd );
			nif->set<Color3>( iMaterial, TA_SPECULARCOLOR, mtl.Ks );
			if (newiMaterial)// don't affect a property  that is already there
				nif->set<Color3>( iMaterial, TA_EMISSIVECOLOR, Color3( 0, 0, 0 ) );
			nif->set<float>( iMaterial, TA_ALPHA, mtl.d );
			nif->set<float>( iMaterial, TA_GLOSSINESS, mtl.Ns );

			if (newiMaterial)// don't add property that is already there
				addLink( nif, iShape, TA_PROPERTIES, nif->getBlockNumber( iMaterial ) );
			
			if ( ! mtl.map_Kd.isEmpty() )
			{
				if ( nif->getVersionNumber() >= NF_V03030013 )
				{
					//Newer versions use NiTexturingProperty and NiSourceTexture
					if ( iTexProp.isValid() == false || first_tri_shape == false || nif->itemType(iTexProp) != T_NITEXTURINGPROPERTY )
					{
						if (!cBSShaderPPLightingProperty) // no need of NiTexturingProperty when BSShaderPPLightingProperty is present
							iTexProp = nif->insertNiBlock( T_NITEXTURINGPROPERTY );
					}
					QModelIndex iBaseMap;
					if (!cBSShaderPPLightingProperty)
					{// no need of NiTexturingProperty when BSShaderPPLightingProperty is present
						addLink( nif, iShape, TA_PROPERTIES, nif->getBlockNumber( iTexProp ) );

						nif->set<int>( iTexProp, TA_HASBASETEXTURE, 1 );
						iBaseMap = nif->getIndex( iTexProp, TA_BASETEXTURE );
						nif->set<int>( iBaseMap, TA_CLAMPMODE, 3 );
						nif->set<int>( iBaseMap, TA_FILTERMODE, 2 );
					}
					
					if ( iTexSource.isValid() == false || first_tri_shape == false || nif->itemType(iTexSource) != T_NISOURCETEXTURE )
					{
						if (!cBSShaderPPLightingProperty)
							iTexSource = nif->insertNiBlock( T_NISOURCETEXTURE );
					}
					if (!cBSShaderPPLightingProperty)// no need of NiTexturingProperty when BSShaderPPLightingProperty is present
						nif->setLink( iBaseMap, TA_SOURCE, nif->getBlockNumber( iTexSource ) );
					
					if (!cBSShaderPPLightingProperty)
					{// no need of NiTexturingProperty when BSShaderPPLightingProperty is present
						nif->set<int>( iTexSource, TA_PIXELLAYOUT, nif->getVersion() == STR_V20000005 ? 6 : 5 );
						nif->set<int>( iTexSource, TA_USEMIPMAPS, 2 );
						nif->set<int>( iTexSource, TA_ALPHAFORMAT, 3 );
						nif->set<int>( iTexSource, TA_UNKNOWNBYTE, 1 );
						nif->set<int>( iTexSource, TA_UNKNOWNBYTE2, 1 );
					
						nif->set<int>( iTexSource, TA_USEEXTERNAL, 1 );
						nif->set<QString>( iTexSource, TA_FILENAME, TexCache::stripPath( mtl.map_Kd, nif->getFolder() ) );
					}
				} else {
					//Older versions use NiTextureProperty and NiImage
					if ( iTexProp.isValid() == false || first_tri_shape == false || nif->itemType(iTexProp) != T_NITEXTUREPROPERTY )
					{
						iTexProp = nif->insertNiBlock( T_NITEXTUREPROPERTY );
					}
					addLink( nif, iShape, TA_PROPERTIES, nif->getBlockNumber( iTexProp ) );
					
					if ( iTexSource.isValid() == false || first_tri_shape == false || nif->itemType(iTexSource) != T_NIIMAGE )
					{
						iTexSource = nif->insertNiBlock( T_NIIMAGE );
					}

					nif->setLink( iTexProp, TA_IMAGE, nif->getBlockNumber( iTexSource ) );
					
					nif->set<int>( iTexSource, TA_EXTERNAL, 1 );
					nif->set<QString>( iTexSource, TA_FILENAME, TexCache::stripPath( mtl.map_Kd, nif->getFolder() ) );
				}
			}
			
			if ( iData.isValid() == false || first_tri_shape == false )
			{
				iData = nif->insertNiBlock( T_NITRISHAPEDATA );
			}
			nif->setLink( iShape, TA_DATA, nif->getBlockNumber( iData ) );
			
			QVector<Vector3> verts;
			QVector<Vector3> norms;
			QVector<Vector2> texco;
			QVector<Triangle> triangles;
			
			QVector<ObjPoint> points;
			
			foreach ( ObjFace oface, *(it.value()) )
			{
				Triangle tri;
				
				for ( int t = 0; t < 3; t++ )
				{
					ObjPoint p = oface.p[t];
					int ix;
					for ( ix = 0; ix < points.count(); ix++ )
					{
						if ( points[ix] == p )
							break;
					}
					if ( ix == points.count() )
					{
						points.append( p );
						verts.append( overts.value( p.v ) );
						norms.append( onorms.value( p.n ) );
						texco.append( otexco.value( p.t ) );
					}
					tri[t] = ix;
				}
				
				triangles.append( tri );
			}
			
			nif->set<int>( iData, TA_NUMVERTICES, verts.count() );
			nif->set<int>( iData, TA_HASVERTICES, 1 );
			nif->updateArray( iData, TA_VERTICES );
			nif->setArray<Vector3>( iData, TA_VERTICES, verts );
			nif->set<int>( iData, TA_HASNORMALS, 1 );
			nif->updateArray( iData, TA_NORMALS );
			nif->setArray<Vector3>( iData, TA_NORMALS, norms );
			nif->set<int>( iData, TA_HASUV, 1 );
			int cNumUVSets = nif->get<int>( iData, TA_NUMUVSETS);// keep things the way they are
			nif->set<int>( iData, TA_NUMUVSETS, 1 | cNumUVSets );// keep things the way they are
			nif->set<int>( iData, TA_NUMUVSETS2, 1 | cNumUVSets );// keep things the way they are
			QModelIndex iTexCo = nif->getIndex( iData, TA_UVSETS );
			if ( ! iTexCo.isValid() )
				iTexCo = nif->getIndex( iData, TA_UVSETS2 );
			nif->updateArray( iTexCo );
			nif->updateArray( iTexCo.child( 0, 0 ) );
			nif->setArray<Vector2>( iTexCo.child( 0, 0 ), texco );
			
			nif->set<int>( iData, TA_HASTRIANGLES, 1 );
			nif->set<int>( iData, TA_NUMTRIANGLES, triangles.count() );
			nif->set<int>( iData, TA_NUMTRIANGLEPOINTS, triangles.count() * 3 );
			nif->updateArray( iData, TA_TRIANGLES );
			nif->setArray<Triangle>( iData, TA_TRIANGLES, triangles );
			
			// "find me a center": see nif.xml for details
			// TODO: extract to a method somewhere...
			Vector3 center;
			if ( verts.count() > 0 )
			{
				Vector3 min, max;
				min[0] = verts[0][0];
				min[1] = verts[0][1];
				min[2] = verts[0][2];
				max[0] = min[0];
				max[1] = min[1];
				max[2] = min[2];
				foreach ( Vector3 v, verts )
				{
					if (v[0] < min[0]) min[0] = v[0];
					if (v[1] < min[1]) min[1] = v[1];
					if (v[2] < min[2]) min[2] = v[2];
					if (v[0] > max[0]) max[0] = v[0];
					if (v[1] > max[1]) max[1] = v[1];
					if (v[2] > max[2]) max[2] = v[2];
				}
				center[0] = min[0] + ((max[0] - min[0])/2);
				center[1] = min[1] + ((max[1] - min[1])/2);
				center[2] = min[2] + ((max[2] - min[2])/2);
			}
			nif->set<Vector3>( iData, TA_CENTER, center );
			float radius = 0;
			foreach ( Vector3 v, verts )
			{
				float d = ( center - v ).length();
				if ( d > radius ) radius = d;
			}
			nif->set<float>( iData, TA_RADIUS, radius );
			
			nif->set<int>( iData, TA_UNKNOWNSHORT2, 0x4000 );
		}
		else if ( nif->getVersionNumber() == NF_V20000005 )
		{
			// create experimental havok collision mesh
			QVector<Vector3> verts;
			QVector<Vector3> norms;
			QVector<Triangle> triangles;
			
			QVector<ObjPoint> points;
			
			foreach ( ObjFace oface, *(it.value()) )
			{
				Triangle tri;
				
				for ( int t = 0; t < 3; t++ )
				{
					ObjPoint p = oface.p[t];
					int ix;
					for ( ix = 0; ix < points.count(); ix++ )
					{
						if ( points[ix] == p )
							break;
					}
					if ( ix == points.count() )
					{
						points.append( p );
						verts.append( overts.value( p.v ) );
						norms.append( onorms.value( p.n ) );
					}
					tri[t] = ix;
				}
				
				triangles.append( tri );
			}
			
			QPersistentModelIndex iData = nif->insertNiBlock( T_NITRISTRIPSDATA );
			
			nif->set<int>( iData, TA_NUMVERTICES, verts.count() );
			nif->set<int>( iData, TA_HASVERTICES, 1 );
			nif->updateArray( iData, TA_VERTICES );
			nif->setArray<Vector3>( iData, TA_VERTICES, verts );
			nif->set<int>( iData, TA_HASNORMALS, 1 );
			nif->updateArray( iData, TA_NORMALS );
			nif->setArray<Vector3>( iData, TA_NORMALS, norms );
			
			Vector3 center;
			foreach ( Vector3 v, verts )
				center += v;
			if ( verts.count() > 0 ) center /= verts.count();
			nif->set<Vector3>( iData, TA_CENTER, center );
			float radius = 0;
			foreach ( Vector3 v, verts )
			{
				float d = ( center - v ).length();
				if ( d > radius ) radius = d;
			}
			nif->set<float>( iData, TA_RADIUS, radius );
			
			// do not stitch, because it looks better in the cs
			QList< QVector< quint16 > > strips = stripify( triangles, false );
			
			nif->set<int>( iData, TA_NUMSTRIPS, strips.count() );
			nif->set<int>( iData, TA_HASPOINTS, 1 );
			
			QModelIndex iLengths = nif->getIndex( iData, TA_STRIPLENGTHS );
			QModelIndex iPoints = nif->getIndex( iData, TA_POINTS );
			
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
				nif->set<int>( iData, TA_NUMTRIANGLES, z );
			}
			
			QPersistentModelIndex iShape = nif->insertNiBlock( T_BHKNITRISTRIPSSHAPE );
			
			nif->setArray<float>( iShape, TA_UNKNOWNFLOATS1, QVector<float>() << 0.1f << 0.0f );
			nif->setArray<int>( iShape, TA_UNKNOWNINTS1,  QVector<int>() << 0 << 0 << 0 << 0 << 1 );
			nif->set<Vector3>( iShape, TA_SCALE, Vector3( 1.0, 1.0, 1.0 ) );
			addLink( nif, iShape, TA_STRIPSDATA, nif->getBlockNumber( iData ) );
			nif->set<int>( iShape, TA_NUMDATALAYERS, 1 );
			nif->updateArray( iShape, TA_DATALAYERS );
			nif->setArray<int>( iShape, TA_DATALAYERS, QVector<int>() << 1 );
			
			QPersistentModelIndex iBody = nif->insertNiBlock( T_BHKRIGIDBODY );
			nif->setLink( iBody, TA_SHAPE, nif->getBlockNumber( iShape ) );
			
			QPersistentModelIndex iObject = nif->insertNiBlock( T_BHKCOLLISIONOBJECT );
			nif->setLink( iObject, TA_PARENT, nif->getBlockNumber( iNode ) );
			nif->set<int>( iObject, TA_UNKNOWNSHORT, 1 );
			nif->setLink( iObject, TA_BODY, nif->getBlockNumber( iBody ) );
			
			nif->setLink( iNode, TA_COLLISIONOBJECT, nif->getBlockNumber( iObject ) );
		}

		//Finished with the first shape which is the only one that can import over the top of existing data
		first_tri_shape = false;
	}
	
	qDeleteAll( ofaces );
	
	settings.setValue( TA_FILENAME, fname );
	
	nif->reset();
}

