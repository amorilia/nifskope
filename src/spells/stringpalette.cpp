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

#include "stringpalette.h"

#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListView>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QStringListModel>

// Brief description is deliberately not autolinked to class Spell
/*! \file stringpalette.cpp
 * \brief String palette editing spells
 *
 * All classes here inherit from the Spell class.
 */

/* XPM */
static char const * txt_xpm[] = {
"32 32 36 1",
" 	c None",
".	c #FFFFFF","+	c #000000","@	c #BDBDBD","#	c #717171","$	c #252525",
"%	c #4F4F4F","&	c #A9A9A9","*	c #A8A8A8","=	c #555555","-	c #EAEAEA",
";	c #151515",">	c #131313",",	c #D0D0D0","'	c #AAAAAA",")	c #080808",
"!	c #ABABAB","~	c #565656","{	c #D1D1D1","]	c #4D4D4D","^	c #4E4E4E",
"/	c #FDFDFD","(	c #A4A4A4","_	c #0A0A0A",":	c #A5A5A5","<	c #050505",
"[	c #C4C4C4","}	c #E9E9E9","|	c #D5D5D5","1	c #141414","2	c #3E3E3E",
"3	c #DDDDDD","4	c #424242","5	c #070707","6	c #040404","7	c #202020",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
" ...........          ....      ",
" .+++++++++.         .@#$.      ",
" .+++++++++.         .+++.      ",
" ....+++..............+++...    ",
"    .+++.   %++&.*++=++++++.    ",
"    .+++.  .-;+>,>+;-++++++.    ",
"    .+++.   .'++)++!..+++...    ",
"    .+++.    .=+++~. .+++.      ",
"    .+++.    .{+++{. .+++.      ",
"    .+++.    .]+++^. .+++/      ",
"    .+++.   .(++_++:..<++[..    ",
"    .+++.  .}>+;|;+1}.2++++.    ",
"    .+++.   ^++'.'++%.34567.    ",
"    .....  .................    ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                "};

QIcon * txt_xpm_icon = 0;

//! Edit a single offset into a string palette.
class spEditStringOffset : public Spell
{
public:
	QString name() const { return Spell::tr("Edit String Offset"); }
	QString page() const { return Spell::tr(""); }
	QIcon icon() const
	{
		if ( ! txt_xpm_icon )
			txt_xpm_icon = new QIcon( txt_xpm );
		return *txt_xpm_icon;
	}
	bool instant() const { return true; }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return nif->getValue( index ).type() == NifValue::tStringOffset && getStringPalette( nif, index ).isValid();
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iPalette = getStringPalette( nif, index );
		
		QMap<QString, int> strings = readStringPalette( nif, iPalette );
		QString string = getString( strings, nif->get<int>( index ) );
		
		QDialog dlg;
		
		QLabel * lb = new QLabel( & dlg );
		lb->setText( Spell::tr("Select a string or enter a new one") );
		
		QListWidget * lw = new QListWidget( & dlg );
		lw->addItems( strings.keys() );
		
		QLineEdit * le = new QLineEdit( & dlg );
		le->setText( string );
		le->setFocus();
		
		QObject::connect( lw, SIGNAL( currentTextChanged( const QString & ) ), le, SLOT( setText( const QString & ) ) );
		QObject::connect( lw, SIGNAL( itemActivated( QListWidgetItem * ) ), & dlg, SLOT( accept() ) );
		QObject::connect( le, SIGNAL( returnPressed() ), & dlg, SLOT( accept() ) );
		
		QPushButton * bo = new QPushButton( Spell::tr("Ok"), & dlg );
		QObject::connect( bo, SIGNAL( clicked() ), & dlg, SLOT( accept() ) );
		
		QPushButton * bc = new QPushButton( Spell::tr("Cancel"), & dlg );
		QObject::connect( bc, SIGNAL( clicked() ), & dlg, SLOT( reject() ) );
		
		QGridLayout * grid = new QGridLayout;
		dlg.setLayout( grid );
		grid->addWidget( lb, 0, 0, 1, 2 );
		grid->addWidget( lw, 1, 0, 1, 2 );
		grid->addWidget( le, 2, 0, 1, 2 );
		grid->addWidget( bo, 3, 0, 1, 1 );
		grid->addWidget( bc, 3, 1, 1, 1 );
		
		if ( dlg.exec() != QDialog::Accepted )
			return index;
		
		nif->set<int>( index, addString( nif, iPalette, le->text() ) );
		
		return index;
	}
	
	//! Gets the string palette referred to by this string offset
	static QModelIndex getStringPalette( const NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iPalette = nif->getBlock( nif->getLink( index.parent(), TA_STRINGPALETTE ) );
		if ( iPalette.isValid() )
			return iPalette;
		
		return QModelIndex();
	}
	
	//! Reads a string palette and returns a map of strings to offsets
	static QMap<QString,int> readStringPalette( const NifModel * nif, const QModelIndex & iPalette )
	{
		QByteArray bytes = nif->get<QByteArray>( iPalette, TA_PALETTE );
		QMap<QString,int> strings;
		int x = 0;
		while ( x < bytes.count() )
		{
			QString s( & bytes.data()[x] );
			strings.insert( s, x );
			x += s.length() + 1;
		}
		return strings;
	}
	
	//! Gets a particular string from a string palette
	static QString getString( const QMap<QString,int> strings, int ofs )
	{
		if ( ofs >= 0 )
		{
			QMapIterator<QString,int> it( strings );
			while ( it.hasNext() )
			{
				it.next();
				if ( ofs == it.value() )
					return it.key();
			}
		}
		return QString();
	}
	
	//! Add a string to a string palette or find the index of it if it exists
	/**
	 * \param nif The model the string palette is in
	 * \param iPalette The index of the string palette
	 * \param string The string to add or find
	 * \return The index of the string in the palette
	 */
	static int addString( NifModel * nif, const QModelIndex & iPalette, const QString & string )
	{
		if ( string.isEmpty() )
			return 0xffffffff;
		
		QMap<QString, int> strings = readStringPalette( nif, iPalette );
		if ( strings.contains( string ) )
			return strings[ string ];
		
		QByteArray bytes = nif->get<QByteArray>( iPalette, TA_PALETTE );
		int ofs = bytes.count();
		bytes += string.toAscii();
		bytes.append( '\0' );
		nif->set<QByteArray>( iPalette, TA_PALETTE, bytes );
		return ofs;
	}
};

REGISTER_SPELL( spEditStringOffset )

// documented in stringpalette.h
StringPaletteRegexDialog::StringPaletteRegexDialog( NifModel * nif, QPersistentModelIndex & index, QWidget * parent) : QDialog( parent )
{
	this->nif = nif;
	iPalette = index;
	
	listview = new QListView;
	listmodel = new QStringListModel;
	listview->setModel( listmodel );
	
	grid = new QGridLayout;
	setLayout( grid );
	
	search = new QLineEdit( this );
	replace = new QLineEdit( this );
	
	QLabel * title = new QLabel( this );
	title->setText( Spell::tr( "Entries in the string palette" ) );
	QLabel * subTitle = new QLabel( this );
	subTitle->setText( Spell::tr( "Enter a pair of regular expressions to search and replace." ) );
	QLabel * refText = new QLabel( this );
	refText->setText(
		Spell::tr( "See <a href='%1'>%2</a> for syntax." )
		.arg( "http://doc.trolltech.com/latest/qregexp.html" )
		.arg( "http://doc.trolltech.com/latest/qregexp.html" )
	);
	QLabel * searchText = new QLabel( this );
	searchText->setText( Spell::tr( "Search:" ) );
	QLabel * replaceText = new QLabel( this );
	replaceText->setText( Spell::tr( "Replace:" ) );
	
	QPushButton * ok = new QPushButton( Spell::tr( "Ok" ), this );
	QPushButton * cancel = new QPushButton( Spell::tr( "Cancel" ), this );
	QPushButton * preview = new QPushButton( Spell::tr( "Preview" ), this );
	
	QObject::connect( ok, SIGNAL( clicked() ), this, SLOT( accept() ) );
	QObject::connect( cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	QObject::connect( preview, SIGNAL( clicked() ), this, SLOT( stringlistRegex() ) );
	
	int currentRow = 0;
	grid->addWidget( title, currentRow, 0, 1, 3 );
	currentRow++;
	grid->addWidget( listview, currentRow, 0, 1, 3 );
	currentRow++;
	grid->addWidget( subTitle, currentRow, 0, 1, 3 );
	currentRow++;
	grid->addWidget( refText, currentRow, 0, 1, 3 );
	currentRow++;
	grid->addWidget( searchText, currentRow, 0, 1, 1 );
	grid->addWidget( search, currentRow, 1, 1, 2 );
	currentRow++;
	grid->addWidget( replaceText, currentRow, 0, 1, 1 );
	grid->addWidget( replace, currentRow, 1, 1, 2 );
	currentRow++;
	grid->addWidget( ok, currentRow, 0, 1, 1 );
	grid->addWidget( cancel, currentRow, 1, 1, 1 );
	grid->addWidget( preview, currentRow, 2, 1, 1 );
}

// documented in stringpalette.h
void StringPaletteRegexDialog::setStringList( QStringList & list )
{
	originalList = new QStringList( list );
	listmodel->setStringList( list );
}

// documented in stringpalette.h
QStringList StringPaletteRegexDialog::getStringList()
{
	stringlistRegex();
	return listmodel->stringList();
}

// documented in stringpalette.h
void StringPaletteRegexDialog::stringlistRegex()
{
	QRegExp replacer( search->text() );
	listmodel->setStringList( * originalList );
	listmodel->setStringList( listmodel->stringList().replaceInStrings( replacer, replace->text() ) );
}

//! Edit a string palette entry and update all references
class spEditStringEntries : public Spell
{
public:
	QString name() const { return Spell::tr("Replace Entries"); }
	QString page() const { return Spell::tr(TA_STRINGPALETTE); }
	
	bool instant() const { return false; }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return nif->inherits( index, "NiSequence" )
			&& nif->getBlock( nif->getLink( index, TA_STRINGPALETTE ) ).isValid()
			&& nif->checkVersion( NF_V10020000, NF_V20000005 );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		// string offset is used in ControllerLink which exists in NiSequence
		// a single palette could be share by multiple NiSequences
		
		QPersistentModelIndex iPalette = nif->getBlock( nif->getLink( index, TA_STRINGPALETTE ) );
#ifndef QT_NO_DEBUG
		qWarning() << "This block uses " << iPalette;
#endif
		if ( ! iPalette.isValid() )
		{
			iPalette = nif->getBlock( nif->getLink( index.parent(), TA_STRINGPALETTE ) );
			
			if ( ! iPalette.isValid() )
			{
				qWarning() << Spell::tr( "Cannot find string palette" );
				return QModelIndex();
			}
		}
		
		// display entries in current string palette, in order they appear
		StringPaletteRegexDialog * sprd = new StringPaletteRegexDialog( nif, iPalette );
		
		QByteArray bytes = nif->get<QByteArray>( iPalette, TA_PALETTE );
		
		// map of old offsets to strings
		// QMap is always sorted by key, in this case the offsets
		QMap<int, QString> oldPalette;
		int x = 0;
		while ( x < bytes.count() )
		{
			QString s( & bytes.data()[x] );
			oldPalette.insert( x, s );
			x += s.length() + 1;
		}
		
		QList<int> oldOffsets = oldPalette.keys();
		
		QStringList oldEntries = oldPalette.values();
		
		sprd->setStringList( oldEntries );
		
		// display dialog
		if ( sprd->exec() != QDialog::Accepted )
		{
			return index;
		}
		
		// get replaced entries
		QStringList newEntries = sprd->getStringList();
		
		//qWarning() << newEntries;
		
		// rebuild palette
		bytes.clear();
		x = 0;
		
		QMap<int, int> offsetMap;
		
		for ( int i = 0; i < newEntries.size(); i++ )
		{
			QString s = newEntries.at( i );
			if ( s.length() == 0 )
			{
				// set references to empty
				offsetMap.insert( oldOffsets[i], -1 );
			}
			else
			{
				offsetMap.insert( oldOffsets[i], x );
				bytes += s;
				bytes.append( '\0' );
				x += ( s.length() + 1 );
			}
		}
		
		// find all NiSequence blocks in the current model
		QList<QPersistentModelIndex> sequenceList;
		for ( int i = 0; i < nif->getBlockCount(); i++ )
		{
			QPersistentModelIndex current = nif->getBlock( i, "NiSequence" );
			if ( current.isValid() )
			{
				sequenceList.append( current );
			}
		}
		
#ifndef QT_NO_DEBUG
		qWarning() << "Found sequences " << sequenceList;
#endif
		
		// find their string palettes
		QList<QPersistentModelIndex> sequenceUpdateList;
		QListIterator<QPersistentModelIndex> sequenceListIterator( sequenceList );
		while ( sequenceListIterator.hasNext() )
		{
			QPersistentModelIndex temp = sequenceListIterator.next();
			QPersistentModelIndex tempPalette = nif->getBlock( nif->getLink( temp, TA_STRINGPALETTE ) );
			//qWarning() << "Sequence " << temp << " uses " << tempPalette;
			if ( iPalette == tempPalette )
			{
				//qWarning() << "Identical to this sequence palette!";
				sequenceUpdateList.append( temp );
			}
		}
		
		// update all references to that palette
		QListIterator<QPersistentModelIndex> sequenceUpdateIterator( sequenceUpdateList );
		int numRefsUpdated = 0;
		while ( sequenceUpdateIterator.hasNext() )
		{
			QPersistentModelIndex nextBlock = sequenceUpdateIterator.next();
			//qWarning() << "Need to update " << nextBlock;
			
			QPersistentModelIndex blocks = nif->getIndex( nextBlock, TA_CONTROLLEDBLOCKS );
			for ( int i = 0; i < nif->rowCount( blocks ); i++ )
			{
				QPersistentModelIndex thisBlock = blocks.child( i, 0 );
				for ( int j = 0; j < nif->rowCount( thisBlock ); j++ )
				{
					if( nif->getValue( thisBlock.child( j, 0 ) ).type() == NifValue::tStringOffset )
					{
						// we shouldn't ever exceed the limit of an int, even though the type
						// is properly a uint
						int oldValue = nif->get<int>( thisBlock.child( j, 0 ) );
#ifndef QT_NO_DEBUG
						qWarning() << "Index " << thisBlock.child( j, 0 )
							<< " is a string offset with name "
							<< nif->itemName( thisBlock.child( j, 0 ) )
							<< " and value "
							<< nif->get<int>( thisBlock.child( j, 0 ) );
#endif
						if ( oldValue != -1 )
						{
							int newValue = offsetMap.value( oldValue );
							nif->set<int>( thisBlock.child( j, 0 ), newValue );
							numRefsUpdated++;
						}
					}
				}
			}
		}
		
		// update the palette itself
		nif->set<QByteArray>( iPalette, TA_PALETTE, bytes );
		
		QMessageBox::information( 0, "NifSkope",
				Spell::tr( "Updated %1 offsets in %2 sequences" ).arg( numRefsUpdated ).arg( sequenceUpdateList.size() ) );
		
		return index;
	}
};

REGISTER_SPELL( spEditStringEntries )

//! Batch helper for spEditStringEntries
class spStringPaletteLister : public Spell
{
public:
	QString name() const { return Spell::tr( "Edit String Palettes" ); }
	QString page() const { return Spell::tr( "Animation" ); }
	
	bool instant() const { return false; }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return ( ! index.isValid() && nif->checkVersion( NF_V10020000, NF_V20000005 ) );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QMap<QString, QModelIndex> sequenceMap;
		
		QList<QModelIndex> sequenceList;
		for ( int i = 0; i < nif->getBlockCount(); i++ )
		{
			QModelIndex current = nif->getBlock( i, "NiSequence" );
			if ( current.isValid() )
			{
				sequenceList.append( current );
				QString key = QString( "%1 %2" ).arg( current.row(), 4, 10, QChar('0') ).arg( nif->get<QString>( current, TA_NAME ) );
				sequenceMap.insert( key , current );
			}
		}
		
		// consider using QInputDialog::getItem() here, but this works
		QDialog dlg;
		
		QGridLayout * grid = new QGridLayout;
		dlg.setLayout( grid );
		int currentRow = 0;
		
		QLabel * title = new QLabel( & dlg );
		title->setText( Spell::tr( "Select an animation sequence to edit the string palette for" ) );
		grid->addWidget( title, currentRow, 0, 1, 2 );
		currentRow++;
		
		QListWidget * listWidget = new QListWidget( & dlg );
		listWidget->addItems( sequenceMap.keys() );
		QObject::connect( listWidget, SIGNAL( itemActivated( QListWidgetItem * ) ), & dlg, SLOT( accept() ) );
		grid->addWidget( listWidget, currentRow, 0, 1, 2 );
		currentRow++;
		
		QPushButton * ok = new QPushButton( Spell::tr( "Ok" ), & dlg );
		QObject::connect( ok, SIGNAL( clicked() ), & dlg, SLOT( accept() ) );
		grid->addWidget( ok, currentRow, 0, 1, 1 );
		
		QPushButton * cancel = new QPushButton( Spell::tr( "Cancel" ), & dlg );
		QObject::connect( cancel, SIGNAL( clicked() ), & dlg, SLOT( reject() ) );
		grid->addWidget( cancel, currentRow, 1, 1, 1 );
		
		if ( dlg.exec() != QDialog::Accepted )
			return QModelIndex();
		
		spEditStringEntries * caster = new spEditStringEntries();
		
		caster->cast( nif, sequenceMap.value( listWidget->currentItem()->text() ) );
		
		return QModelIndex();
	}
};

REGISTER_SPELL( spStringPaletteLister )
