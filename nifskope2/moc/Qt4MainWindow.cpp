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

#include "Qt4MainWindow.h"

#include <nifskope.h>

namespace NifSkopeQt4
{
	void MainWindow::createMainMenu()
	{
		// &File
		//  &Load... Ctrl+O
		//  &Save As... Ctrl+S
		//  -
		//  Import ->
		//	 Import.3DS
		//   Import.OBJ
		//  Export ->
		//	 Export.OBJ
		//  -
		//  [] &Auto Sanitize Before Save addAction(QAction *
		//  -
		//  &New Window Ctrl-N
		//  -
		//  Reload &XML
		//  &Reload XML + Nif Alt+X
		//  XML Checker
		//  -
		//  Resource Files
		//  -
		//  &Quit

		// TODO: should be/could be generated from "nifskopeqt.xml" or something ... :
		QAction *aLoad = new QAction (tr("&Load..."), this);
		QAction *aSaveAs = new QAction (tr("&Save As..."), this);
		QMenu *mImport = new QMenu (tr("Import"), this);
		QAction *aImport3ds = new QAction (tr("Import.3DS..."), this);
		QAction *aImportobj = new QAction (tr("Import.OBJ..."), this);
		QMenu *mExport = new QMenu (tr("Export"), this);
		QAction *aExportobj = new QAction (tr("Export.OBJ..."), this);
		QAction *aAutoSanitize = new QAction (tr("&Auto Sanitize Before Save"), this);
		QAction *aNewWindow = new QAction (tr("&New Window"), this);
		QAction *aReloadXML = new QAction (tr("Reload &XML"), this);
		QAction *aReloadXMLNif = new QAction (tr("&Reload XML + Nif"), this);
		QAction *aXMLChecker = new QAction (tr("XML Checker"), this);
		QAction *aResFiles = new QAction (tr("Resource Files"), this);
		QAction *aQuit = new QAction (tr("&Quit"), this);
		
		aLoad->setShortcut (QKeySequence (tr("Ctrl+L")));
		aSaveAs->setShortcut (QKeySequence (tr("Ctrl+S")));
		aAutoSanitize->setCheckable( true );
		aAutoSanitize->setChecked( true );
		aNewWindow->setShortcut (QKeySequence::New );
		aReloadXMLNif->setShortcut (QKeySequence (tr("Alt+X")) );

		QMenu *mFile = menuBar()->addMenu(tr("&File"));
		mFile->addAction (aLoad);
		mFile->addAction (aSaveAs);
		mFile->addSeparator();
		mFile->addMenu (mImport);
			mImport->addAction (aImport3ds);
			mImport->addAction (aImportobj);
		mFile->addMenu (mExport);
			mExport->addAction (aExportobj);
		mFile->addSeparator();
		mFile->addAction (aAutoSanitize);
		mFile->addSeparator();
		mFile->addAction (aNewWindow);
		mFile->addSeparator();
		mFile->addAction (aReloadXML);
		mFile->addAction (aReloadXMLNif);
		mFile->addAction (aXMLChecker);
		mFile->addSeparator();
		mFile->addAction (aResFiles);
		mFile->addSeparator();
		mFile->addAction (aQuit);
		//connect( aWindow, SIGNAL( triggered() ), this, SLOT( sltWindow() ) );
		connect( aLoad, SIGNAL( triggered() ), this, SLOT( mFileLoad() ) );
	}

	void MainWindow::mFileLoad()
	{
		// UI part of the handler
		QString fileName = QFileDialog::getOpenFileName (
			this, tr("Open File"), "",
			QString::fromStdString (App->File.GetLoadFormats (" ", ";;")));
		if (fileName == NULL)
			return;
		// Model part of the handler
		App->File.FileName = fileName.toStdString ();
		App->File.Load ();
	}

	MainWindow::MainWindow()
		: QMainWindow()
	{
		resize (800, 600);
		// f.InitialPosition = DesktopCenter
		this->move (
			QApplication::desktop ()->screen ()->rect ().center () -
			this->rect ().center ());
		createMainMenu ();
	}
}
