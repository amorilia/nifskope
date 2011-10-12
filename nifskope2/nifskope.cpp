/***** BEGIN LICENSE BLOCK *****

BSD License

Copyright (c) 2005-2011, NIF File Format Library and Tools
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

/** @file nifskope.cpp
 *  The main file for NifSkope
 */

// MinGW hack to ensure that GetLongPathNameW is defined
/*#ifdef WIN32 
#  ifdef __GNUC__
#    define WINVER 0x0500
#  endif
#endif*/

#include "nifskope.h"
#include <QtGui>
//#include <QDir>
/*#include "config.h"
#include <QAction>
#include <QApplication>
#include <QByteArray>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QDockWidget>
#include <QFontDialog>
#include <QFile>
#include <QFileInfo>
#include <QHeaderView>
#include <QLocale>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QTextEdit>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QTranslator>
#include <QUdpSocket>
#include <QUrl>
#include <QListView>
#include <QTreeView>*/

/*#include "kfmmodel.h"
#include "nifmodel.h"
#include "nifproxy.h"
#include "widgets/nifview.h"
#include "widgets/refrbrowser.h"
#include "widgets/inspect.h"*/

//#include "3dview.h"
#ifdef NIFSKOPE_GL
#include "glview.h"
#endif
#ifdef NIFSKOPE_OGRE
#include "3dview_OGRE.h"
#endif

/*#include "spellbook.h"
#include "widgets/fileselect.h"
#include "widgets/copyfnam.h"
#include "widgets/xmlcheck.h"
#include "options.h"*/

/*#ifdef WIN32
#  define WINDOWS_LEAN_AND_MEAN
#  include "windows.h"
#endif*/

/*#ifdef FSENGINE
#include "fsengine/fsmanager.h"
FSManager * fsmanager = 0;
#endif*/

/**
 *  @brief Qt UI action handler: Help->About
 */
/*void NifSkope::about()
{
	QString text = tr(
	"<p style='white-space:pre'>NifSkope is a tool for analyzing and editing NetImmerse/Gamebryo '.nif' files.</p>"
	"<p>NifSkope is based on NifTool's XML file format specification. "
	"For more information visit our site at <a href='http://niftools.sourceforge.net'>http://niftools.sourceforge.net</a></p>"
	"<p>NifSkope is free software available under a BSD license. "
	"The source is available via <a href='http://niftools.git.sourceforge.net/git/gitweb.cgi?p=niftools/nifskope'>git</a> "
	"(<a href='git://niftools.git.sourceforge.net/gitroot/niftools/nifskope'>clone</a>) on <a href='http://sourceforge.net'>SourceForge</a>. "
	"Instructions on compiling NifSkope are available on the <a href='http://niftools.sourceforge.net/wiki/NifSkope/Compile'>NifTools wiki</a>.</p>"
	"<p>The most recent version of NifSkope can always be downloaded from the <a href='https://sourceforge.net/projects/niftools/files/nifskope/'>"
	"NifTools SourceForge Project page</a>.</p>"
// only the windows build uses havok
// (Q_OS_WIN32 is also defined on win64)
#ifdef Q_OS_WIN32
	"<center><img src=':/img/havok_logo' /></center>"
	"<p>NifSkope uses Havok(R) for the generation of mopp code. "
	"(C)Copyright 1999-2008 Havok.com Inc. (and its Licensors). "
	"All Rights Reserved. "
	"See <a href='http://www.havok.com'>www.havok.com</a> for details.</p>"
#endif
	"<center><img src=':/img/qhull_logo' /></center>"
	"<p>NifSkope uses Qhull for the generation of convex hulls. "
	"Copyright(c) 1993-2010  C.B. Barber and The Geometry Center. "
	"Qhull is free software and may be obtained from <a href='http://www.qhull.org'>www.qhull.org</a>. "
	"See Qhull_COPYING.txt for details."
	);

	QMessageBox mb( tr("About NifSkope %1 (revision %2)").arg(NIFSKOPE_VERSION).arg(NIFSKOPE_REVISION), text, QMessageBox::Information,
		QMessageBox::Ok + QMessageBox::Default, 0, 0, this);
	mb.setIconPixmap( QPixmap( ":/res/nifskope.png" ) );
	mb.exec();
}*/

void
qt4_loadstyle(QApplication &app)
{
	QDir qssDir (QApplication::applicationDirPath ());
	// Check app dir, relative from nifskope/release, linux data dir
	QStringList qssList (QStringList ()
		<< "style.qss"
		<< "../style.qss"
		<< "/usr/share/nifskope/style.qss" );
	QString qssName;
	foreach (QString str, qssList) {
		if (qssDir.exists (str)) {
			qssName = qssDir.filePath (str);
			break;
		}
	}
	// load the style sheet if present
	if (!qssName.isEmpty ()) {
		QFile style (qssName);
		if (style.open (QFile::ReadOnly)) {
			app.setStyleSheet (style.readAll());
			style.close ();
		}
	}
}

int
qt4_init(int argc, char **argv)
{
	QApplication app (argc, argv);
	app.setOrganizationName (NS_ORG);
	app.setApplicationName (NS_NAME);
	app.setOrganizationDomain (NS_URL);
	qt4_loadstyle (app);

	QWidget mainWindow;
	mainWindow.setWindowTitle (
		QApplication::translate("windowlayout", NS_VERSION));
	mainWindow.show();

	return app.exec();
}

/**
 *  main
 */
int
main(int argc, char **argv)
{
	/*NifModel::loadXML();
	KfmModel::loadXML();*/
	return qt4_init (argc, argv);
}
