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
#include <QDialog>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>

// Brief description is deliberately not autolinked to class Spell
/*! \file headerstring.cpp
 * \brief Header string editing spells (spEditStringIndex)
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

static QIcon * txt_xpm_icon = 0;

//! Edit the index of a header string
class spEditStringIndex : public Spell
{
public:
	QString name() const { return Spell::tr("Edit String Index"); }
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
		NifValue::Type type = nif->getValue( index ).type() ;
		if (type == NifValue::tStringIndex)
				return true;
		if ((type == NifValue::tString || type == NifValue::tFilePath) && nif->checkVersion( NF_V20010003, 0 ) )
			return true;
		return false;
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		int offset = nif->get<int>( index );
		QStringList strings;
		QString string;
		if (nif->getValue( index ).type() == NifValue::tStringIndex && nif->checkVersion( NF_V20010003, 0 ) )
		{
			QModelIndex header = nif->getHeader();
			QVector<QString> stringVector = nif->getArray<QString>( header, TA_HSTRINGS );
			strings = stringVector.toList();
			if (offset >= 0 && offset < stringVector.size())
				string = stringVector.at(offset);
		}
		else
		{
			return index;
		}
		
		QDialog dlg;
		
		QLabel * lb = new QLabel( & dlg );
		lb->setText( Spell::tr("Select a string or enter a new one") );
		
		QListWidget * lw = new QListWidget( & dlg );
		lw->addItems( strings );
		
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
		
		nif->set<QString>( index, le->text() );
		
		return index;
	}
};

REGISTER_SPELL( spEditStringIndex )

