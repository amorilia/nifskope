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

#include "transform.h"
#include "config.h"

#include "widgets/nifeditors.h"

#include <QApplication>
#include <QBuffer>
#include <QClipboard>
#include <QCheckBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QSettings>

/* XPM */
static char const * transform_xpm[] = {
"64 64 6 1",
" 	c None",
".	c #1800FF",
"+	c #FF0301",
"@	c #C46EBC",
"#	c #0DFF00",
"$	c #2BFFAC",
"                                                                ",
"                                                                ",
"                                                                ",
"                             .                                  ",
"                            ...                                 ",
"                           .....                                ",
"                          .......                               ",
"                            ...                                 ",
"                            ...                                 ",
"                            ...                                 ",
"                            ...                                 ",
"                            ...                                 ",
"                            ...                                 ",
"                            ...                                 ",
"                            ...                                 ",
"                            ...                                 ",
"                            ...                    ##           ",
"     +                      ...                  ####           ",
"    ++++                    ...                 #####           ",
"     +++++                  ...                #######          ",
"      +++++                 ...              #######            ",
"        +++++               ...             #####               ",
"          +++++             ...           #####                 ",
"           +++++            ...          #####                  ",
"             +++++          ...        #####                    ",
"               +++++        ...       #####                     ",
"                +++++       ...      ####                       ",
"                  +++++     ...    #####                        ",
"                    +++++   ...   ####                          ",
"                     ++++++ ... #####                           ",
"                       +++++...#####                            ",
"                         +++...###                              ",
"                          ++...+#                               ",
"                           #...++                               ",
"                         ###...++++                             ",
"                        ####...++++++                           ",
"                      ##### ...  +++++                          ",
"                     ####   ...    +++++                        ",
"                   #####    ...     ++++++                      ",
"                  #####     ...       +++++                     ",
"                #####       ...         +++++                   ",
"               #####        ...          ++++++                 ",
"              ####          ...            +++++                ",
"            #####           ...              +++++              ",
"           ####             ...               ++++++            ",
"         #####              ...                 +++++  +        ",
"        #####               ...                   +++++++       ",
"      #####                 ...                    +++++++      ",
"     #####                  ...                      +++++      ",
"    ####                    ...                      ++++       ",
"    ###                     ...                        +        ",
"                            ...                                 ",
"                            ...                                 ",
"                            ...                                 ",
"                            ...                                 ",
"                            ...                                 ",
"                            ...                                 ",
"                            ...                                 ",
"                                                                ",
"                                                                ",
"                                                                ",
"                                                                ",
"                                                                ",
"                                                                "};

bool spApplyTransformation::isApplicable( const NifModel * nif, const QModelIndex & index )
{
	return nif->itemType( index ) == B_NIBLOCK && ( nif->inherits( nif->itemName( index ), T_NINODE )
			|| nif->itemName( index ) == T_NITRISHAPE || nif->itemName( index ) == T_NITRISTRIPS );
}

QModelIndex spApplyTransformation::cast( NifModel * nif, const QModelIndex & index )
{
	if ( ( nif->getLink( index, TA_CONTROLLER ) != -1 || nif->getLink( index, "Skin Instance" ) != -1 ) )
		if ( QMessageBox::question( 0, Spell::tr("Apply Transformation"), Spell::tr("On animated and or skinned nodes Apply Transformation most likely won't work the way you expected it."), Spell::tr("Try anyway"), Spell::tr("Cancel") ) != 0 )
			return index;
	
	if ( nif->inherits( nif->itemName( index ), T_NINODE ) )
	{
		Transform tp( nif, index );
		bool ok = false;
		foreach ( int l, nif->getChildLinks( nif->getBlockNumber( index ) ) )
		{
			QModelIndex iChild = nif->getBlock( l );
			if ( iChild.isValid() && nif->inherits( nif->itemName( iChild ), T_NIAVOBJECT ) )
			{
				Transform tc( nif, iChild );
				tc = tp * tc;
				tc.writeBack( nif, iChild );
				ok = true;
			}
		}
		if ( ok )
		{
			tp = Transform();
			tp.writeBack( nif, index );
		}
	}
	else
	{
		QModelIndex iData;
		if ( nif->itemName( index ) == T_NITRISHAPE) 
			iData = nif->getBlock( nif->getLink( index, TA_DATA ), T_NITRISHAPEDATA );
		else if ( nif->itemName( index ) == T_NITRISTRIPS ) 
			iData = nif->getBlock( nif->getLink( index, TA_DATA ), T_NITRISTRIPSDATA );
		
		if ( iData.isValid() )
		{
			Transform t( nif, index );
			QModelIndex iVertices = nif->getIndex( iData, TA_VERTICES );
			if ( iVertices.isValid() )
			{
				QVector<Vector3> a = nif->getArray<Vector3>( iVertices );
				for ( int v = 0; v < nif->rowCount( iVertices ); v++ )
					a[v] = t * a[v];
				nif->setArray<Vector3>( iVertices, a );
				
				QModelIndex iNormals = nif->getIndex( iData, TA_NORMALS );
				if ( iNormals.isValid() )
				{
					a = nif->getArray<Vector3>( iNormals );
					for ( int n = 0; n < nif->rowCount( iNormals ); n++ )
						a[n] = t.rotation * a[n];
					nif->setArray<Vector3>( iNormals, a );
				}
			}
			QModelIndex iCenter = nif->getIndex( iData, TA_CENTER );
			if ( iCenter.isValid() )
				nif->set<Vector3>( iCenter, t * nif->get<Vector3>( iCenter ) );
			QModelIndex iRadius = nif->getIndex( iData, TA_RADIUS );
			if ( iRadius.isValid() )
				nif->set<float>( iRadius, t.scale * nif->get<float>( iRadius ) );
			t = Transform();
			t.writeBack( nif, index );
		}
	}
	return index;
}

REGISTER_SPELL( spApplyTransformation )

class spClearTransformation : public Spell
{
public:
	QString name() const { return Spell::tr("Clear"); }
	QString page() const { return Spell::tr("Transform"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return Transform::canConstruct( nif, index );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		Transform tp;
		tp.writeBack( nif, index );
		return index;
	}
};

REGISTER_SPELL( spClearTransformation )

class spCopyTransformation : public Spell
{
public:
	QString name() const { return Spell::tr("Copy"); }
	QString page() const { return Spell::tr("Transform"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return Transform::canConstruct( nif, index );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QByteArray data;
		QBuffer buffer( & data );
		if ( buffer.open( QIODevice::WriteOnly ) )
		{
			QDataStream ds( &buffer );
			ds << Transform( nif, index );
			
			QMimeData * mime = new QMimeData;
			mime->setData( QString( "nifskope/transform" ), data );
			QApplication::clipboard()->setMimeData( mime );
		}
		return index;
	}
};

REGISTER_SPELL( spCopyTransformation )

class spPasteTransformation : public Spell
{
public:
	QString name() const { return Spell::tr("Paste"); }
	QString page() const { return Spell::tr("Transform"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		const QMimeData * mime = QApplication::clipboard()->mimeData();
		if ( Transform::canConstruct( nif, index ) && mime )
			foreach ( QString form, mime->formats() )
				if ( form == "nifskope/transform" )
					return true;
		
		return false;
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		const QMimeData * mime = QApplication::clipboard()->mimeData();
		if ( mime )
		{
			foreach ( QString form, mime->formats() )
			{
				if ( form == "nifskope/transform" )
				{
					QByteArray data = mime->data( form );
					QBuffer buffer( & data );
					if ( buffer.open( QIODevice::ReadOnly ) )
					{
						QDataStream ds( &buffer );
						Transform t;
						ds >> t;
						t.writeBack( nif, index );
						return index;
					}
				}
			}
		}
		return index;
	}
};

REGISTER_SPELL( spPasteTransformation )

QIcon * transform_xpm_icon = 0;

class spEditTransformation : public Spell
{
public:
	QString name() const { return Spell::tr("Edit"); }
	QString page() const { return Spell::tr("Transform"); }
	bool instant() const { return true; }
	QIcon icon() const
	{
		if ( ! transform_xpm_icon )
			transform_xpm_icon = new QIcon( transform_xpm );
		return *transform_xpm_icon;
	}
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		if ( Transform::canConstruct( nif, index ) )
			return true;
		
		QModelIndex iTransform = nif->getIndex( index, "Transform" );
		if ( ! iTransform.isValid() )
			iTransform = index;
		
		return ( nif->getValue( iTransform ).type() == NifValue::tMatrix4 );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		NifBlockEditor * edit = new NifBlockEditor( nif, nif->getBlock( index ) );
		if ( Transform::canConstruct( nif, index ) )
		{
			edit->add( new NifVectorEdit( nif, nif->getIndex( index, TA_TRANSLATION ) ) );
			edit->add( new NifRotationEdit( nif, nif->getIndex( index, TA_ROTATION ) ) );
			edit->add( new NifFloatEdit( nif, nif->getIndex( index, TA_SCALE ) ) );
		}
		else
		{
			QModelIndex iTransform = nif->getIndex( index, "Transform" );
			if ( ! iTransform.isValid() )
				iTransform = index;
			edit->add( new NifMatrix4Edit( nif, iTransform ) );
		}
		edit->show();
		return index;
	}
};

REGISTER_SPELL( spEditTransformation )


class spScaleVertices : public Spell
{
public:
	QString name() const { return Spell::tr( "Scale Vertices" ); }
	QString page() const { return Spell::tr( "Transform" ); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return nif->inherits( index, "NiGeometry" );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QDialog dlg;
		
		QGridLayout * grid = new QGridLayout( &dlg );
		
		QList<QDoubleSpinBox *> scale;
		
		for ( int a = 0; a < 3; a++ )
		{
			QDoubleSpinBox * spn = new QDoubleSpinBox;
			scale << spn;
			spn->setValue( 1.0 );
			spn->setDecimals( 4 );
			spn->setRange( -10e+4, 10e+4 );
			grid->addWidget( new QLabel( ( QStringList() << "X" << "Y" << "Z" ).value( a ) ), a, 0 );
			grid->addWidget( spn, a, 1 );
		}
		
		NIFSKOPE_QSETTINGS(settings);
		settings.beginGroup( "spells" );
		settings.beginGroup( page() );
		settings.beginGroup( name() );
		
		QCheckBox * chkNormals = new QCheckBox( Spell::tr("Scale Normals") );

		chkNormals->setChecked( settings.value( "scale normals", true ).toBool() );
		grid->addWidget( chkNormals, 3, 1 );
		
		QPushButton * btScale = new QPushButton( Spell::tr( TA_SCALE ) );
		grid->addWidget( btScale, 4, 0, 1, 2 );
		QObject::connect( btScale, SIGNAL( clicked() ), &dlg, SLOT( accept() ) );
		
		if ( dlg.exec() != QDialog::Accepted )
			return QModelIndex();

		settings.setValue( "scale normals", chkNormals->isChecked() );
		
		QModelIndex iData = nif->getBlock( nif->getLink( nif->getBlock( index ), TA_DATA ), "NiGeometryData" );
		
		QVector<Vector3> vertices = nif->getArray<Vector3>( iData, TA_VERTICES );
		QMutableVectorIterator<Vector3> it( vertices );
		while ( it.hasNext() )
		{
			Vector3 & v = it.next();
			
			for ( int a = 0; a < 3; a++ )
				v[a] *= scale[a]->value();
		}
		nif->setArray<Vector3>( iData, TA_VERTICES, vertices );
		
		if( chkNormals->isChecked() )
		{
			QVector<Vector3> norms = nif->getArray<Vector3>( iData, TA_NORMALS );
			QMutableVectorIterator<Vector3> it( norms );
			while ( it.hasNext() )
			{
				Vector3 & v = it.next();

				for ( int a = 0; a < 3; a++ )
					v[a] *= scale[a]->value();
			}
			nif->setArray<Vector3>( iData, TA_NORMALS, norms );
		}
		
		return QModelIndex();
	}
};

REGISTER_SPELL( spScaleVertices )


