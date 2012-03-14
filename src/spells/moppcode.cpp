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
/*! \file moppcode.cpp
 * \brief Havok MOPP spells
 *
 * Note that this code only works on the Windows platform due an external
 * dependency on the Havok SDK, with which NifMopp.dll is compiled.
 *
 * Most classes here inherit from the Spell class.
 */

// Need to include headers before testing this
#ifdef Q_OS_WIN32

// This code is only intended to be run with Win32 platform.

extern "C" void * __stdcall SetDllDirectoryA( const char * lpPathName );
extern "C" void * __stdcall LoadLibraryA( const char * lpModuleName );
extern "C" void * __stdcall GetProcAddress ( void * hModule, const char * lpProcName );
extern "C" void __stdcall FreeLibrary( void * lpModule );

//! Interface to the external MOPP library
class HavokMoppCode
{
private:
	typedef int (__stdcall * fnGenerateMoppCode)(int nVerts, Vector3 const* verts, int nTris, Triangle const *tris);
	typedef int (__stdcall * fnGenerateMoppCodeWithSubshapes)(int nShapes, int const *shapes, int nVerts, Vector3 const* verts, int nTris, Triangle const *tris);
	typedef int (__stdcall * fnRetrieveMoppCode)(int nBuffer, char *buffer);
	typedef int (__stdcall * fnRetrieveMoppScale)(float *value);
	typedef int (__stdcall * fnRetrieveMoppOrigin)(Vector3 *value);
	
	void *hMoppLib;
	fnGenerateMoppCode GenerateMoppCode;
	fnRetrieveMoppCode RetrieveMoppCode;
	fnRetrieveMoppScale RetrieveMoppScale;
	fnRetrieveMoppOrigin RetrieveMoppOrigin;
	fnGenerateMoppCodeWithSubshapes GenerateMoppCodeWithSubshapes;

public:
	HavokMoppCode() : hMoppLib(0), GenerateMoppCode(0), RetrieveMoppCode(0)
		, RetrieveMoppScale(0), RetrieveMoppOrigin(0), GenerateMoppCodeWithSubshapes(0) {}
	
	~HavokMoppCode() {
		if (hMoppLib) FreeLibrary(hMoppLib);
	}
	
	bool Initialize()
	{
		if (hMoppLib == NULL)
		{
			SetDllDirectoryA( QCoreApplication::applicationDirPath().toLocal8Bit() );
			hMoppLib = LoadLibraryA( "NifMopp.dll" );
			GenerateMoppCode = (fnGenerateMoppCode)GetProcAddress( hMoppLib, "GenerateMoppCode" );
			RetrieveMoppCode = (fnRetrieveMoppCode)GetProcAddress( hMoppLib, "RetrieveMoppCode" );
			RetrieveMoppScale = (fnRetrieveMoppScale)GetProcAddress( hMoppLib, "RetrieveMoppScale" );
			RetrieveMoppOrigin = (fnRetrieveMoppOrigin)GetProcAddress( hMoppLib, "RetrieveMoppOrigin" );
			GenerateMoppCodeWithSubshapes = (fnGenerateMoppCodeWithSubshapes)GetProcAddress( hMoppLib, "GenerateMoppCodeWithSubshapes" );
		}
		return ( NULL != GenerateMoppCode  && NULL != RetrieveMoppCode 
			&& NULL != RetrieveMoppScale && NULL != RetrieveMoppOrigin
			);
	}
	
	QByteArray CalculateMoppCode( QVector<Vector3> const & verts, QVector<Triangle> const & tris, Vector3* origin, float* scale)
	{
		QByteArray code;
		if ( Initialize() )
		{
			int len = GenerateMoppCode( verts.size(), &verts[0], tris.size(), &tris[0] );
			if ( len > 0 )
			{
				code.resize( len );
				if ( 0 != RetrieveMoppCode( len , code.data() ) )
				{
					if ( NULL != scale )
						RetrieveMoppScale(scale);
					if ( NULL != origin )
						RetrieveMoppOrigin(origin);
				}
				else
				{
					code.clear();
				}
			}
		}
		return code;
	}
	
	QByteArray CalculateMoppCode( QVector<int> const & subShapesVerts
								, QVector<Vector3> const & verts
								, QVector<Triangle> const & tris
								, Vector3* origin, float* scale)
	{
		QByteArray code;
		if ( Initialize() )
		{
			int len;
			if (GenerateMoppCodeWithSubshapes != NULL)
				len = GenerateMoppCodeWithSubshapes( subShapesVerts.size(), &subShapesVerts[0], verts.size(), &verts[0], tris.size(), &tris[0] );
			else
				len = GenerateMoppCode( verts.size(), &verts[0], tris.size(), &tris[0] );
			if ( len > 0 )
			{
				code.resize( len );
				if ( 0 != RetrieveMoppCode( len , code.data() ) )
				{
					if ( NULL != scale )
						RetrieveMoppScale(scale);
					if ( NULL != origin )
						RetrieveMoppOrigin(origin);
				}
				else
				{
					code.clear();
				}
			}
		}
		return code;
	}
} TheHavokCode;

//! Update Havok MOPP for a given shape
class spMoppCode : public Spell
{
public:
	QString name() const { return Spell::tr("Update MOPP Code"); }
	QString page() const { return Spell::tr("Havok"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		if (nif->getUserVersion() != 10 && nif->getUserVersion() != 11)
			return false;
		if ( TheHavokCode.Initialize() )
		{
			QModelIndex iData = nif->getBlock( nif->getLink( index, TA_DATA ) );
			if ( nif->isNiBlock( index, T_BHKMOPPBVTREESHAPE ) )
			{
				return ( nif->checkVersion( NF_V20000004, NF_V20000005 ) 
						|| nif->checkVersion( NF_V20020007, NF_V20020007 ) );
			}			
		}
		return false;
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & iBlock )
	{
		if ( !TheHavokCode.Initialize() ) {
			qWarning() << Spell::tr( "unable to locate the NifMopp.dll library" );
			return iBlock;
		}
		
		QPersistentModelIndex ibhkMoppBvTreeShape = iBlock;
		
		QModelIndex ibhkPackedNiTriStripsShape = nif->getBlock( nif->getLink( ibhkMoppBvTreeShape, TA_SHAPE ) );
		if ( !nif->isNiBlock( ibhkPackedNiTriStripsShape, T_BHKPACKEDNITRISTRIPSSHAPE ))
		{
			qWarning() << Spell::tr( "only bhkPackedNiTriStripsShape can be used with bhkMoppBvTreeShape Mopp code at this time" );
			return iBlock;
		}
		
		QModelIndex ihkPackedNiTriStripsData = nif->getBlock( nif->getLink( ibhkPackedNiTriStripsShape, TA_DATA ) );
		if ( !nif->isNiBlock( ihkPackedNiTriStripsData, T_HKPACKEDNITRISTRIPSDATA ))
			return iBlock;
		
		QVector<int> subshapeVerts;
		if ( nif->checkVersion( NF_V20000004, NF_V20000005 ) ) {
			int nSubShapes = nif->get<int>( ibhkPackedNiTriStripsShape, TA_NUMSUBSHAPES );
			QModelIndex ihkSubShapes = nif->getIndex( ibhkPackedNiTriStripsShape, TA_SUBSHAPES );
			subshapeVerts.resize(nSubShapes);
			for ( int t = 0; t < nSubShapes; t++ ) {
				subshapeVerts[t] = nif->get<int>( ihkSubShapes.child( t, 0 ), TA_NUMVERTICES );
			}
		} else if ( nif->checkVersion( NF_V20020007, NF_V20020007 ) ) {
			int nSubShapes = nif->get<int>( ihkPackedNiTriStripsData, TA_NUMSUBSHAPES );
			QModelIndex ihkSubShapes = nif->getIndex( ihkPackedNiTriStripsData, TA_SUBSHAPES );
			subshapeVerts.resize(nSubShapes);
			for ( int t = 0; t < nSubShapes; t++ ) {
				subshapeVerts[t] = nif->get<int>( ihkSubShapes.child( t, 0 ), TA_NUMVERTICES );
			}
		}
		
		QVector<Vector3> verts = nif->getArray<Vector3>( ihkPackedNiTriStripsData, TA_VERTICES );
		QVector<Triangle> triangles;
		
		int nTriangles = nif->get<int>( ihkPackedNiTriStripsData, TA_NUMTRIANGLES );
		QModelIndex iTriangles = nif->getIndex( ihkPackedNiTriStripsData, TA_TRIANGLES );
		triangles.resize(nTriangles);
		for ( int t = 0; t < nTriangles; t++ ) {
			triangles[t] = nif->get<Triangle>( iTriangles.child( t, 0 ), TA_TRIANGLE );
		}
		
		if ( verts.isEmpty() || triangles.isEmpty() )
		{
			qWarning() << Spell::tr( "need vertices and faces to calculate mopp code" );
			return iBlock;
		}
		
		Vector3 origin;
		float scale;
		QByteArray moppcode = TheHavokCode.CalculateMoppCode(subshapeVerts, verts, triangles, &origin, &scale);
		
		if (moppcode.size() == 0)
		{
			qWarning() << Spell::tr( "failed to generate mopp code" );
		}
		else
		{
			QModelIndex iCodeOrigin = nif->getIndex( ibhkMoppBvTreeShape, TA_ORIGIN );
			nif->set<Vector3>( iCodeOrigin, origin );
			
			QModelIndex iCodeScale = nif->getIndex( ibhkMoppBvTreeShape, TA_SCALE );
			nif->set<float>( iCodeScale, scale );
			
			QModelIndex iCodeSize = nif->getIndex( ibhkMoppBvTreeShape, TA_MOPPDATASIZE );
			QModelIndex iCode = nif->getIndex( ibhkMoppBvTreeShape, TA_MOPPDATA );
			if ( iCodeSize.isValid() && iCode.isValid() )
			{
				nif->set<int>( iCodeSize, moppcode.size() );
				nif->updateArray( iCode );
				//nif->set<QByteArray>( iCode, moppcode );
				for ( int i=0; i<moppcode.size(); ++i ){
					nif->set<quint8>( iCode.child( i, 0 ), moppcode[i] );
				}
			}
		}
		
		return iBlock;
	}

};

REGISTER_SPELL( spMoppCode )

//! Update MOPP code on all shapes in this model
class spAllMoppCodes : public Spell
{
public:
	QString name() const { return Spell::tr( "Update All MOPP Code" ); }
	QString page() const { return Spell::tr( "Batch" ); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & idx )
	{
		if (nif->getUserVersion() != 10 && nif->getUserVersion() != 11)
			return false;
		
		if ( TheHavokCode.Initialize() )
		{
			if ( nif && ! idx.isValid() )
				{
					return ( nif->checkVersion( NF_V20000004, NF_V20000005 ) 
							|| nif->checkVersion( NF_V20020007, NF_V20020007 ) );
				}
		}
		return false;
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & )
	{
		QList< QPersistentModelIndex > indices;
		
		spMoppCode TSpacer;
		
		for ( int n = 0; n < nif->getBlockCount(); n++ )
		{
			QModelIndex idx = nif->getBlock( n );
			if ( TSpacer.isApplicable( nif, idx ) )
				indices << idx;
		}
		
		foreach ( QModelIndex idx, indices )
		{
			TSpacer.castIfApplicable( nif, idx );
		}
		
		return QModelIndex();
	}
};

REGISTER_SPELL( spAllMoppCodes )

#endif // Q_OS_WIN32

