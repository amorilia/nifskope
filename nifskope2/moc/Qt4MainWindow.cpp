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
		App->File.FileName = fileName.toStdString();
		App->File.Load();
	}

	MainWindow::MainWindow()
		: QMainWindow()
	{
		// f.InitialPosition = DesktopCentr
		resize (800, 600);
		this->move (
			QApplication::desktop ()->screen ()->rect ().center () -
			this->rect ().center ());
		createMainMenu ();
	}
}
