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

// "NifLib"
#include "List.h"
#include "Field.h"
#include "Tag.h"
#include "Attr.h"
#include "TreeNode.h"
#include "niflib.h"

namespace NifSkopeQt4
{
	void Qt4MainWindow::createMainMenu()
	{
		// TODO: QML
		aLoad = new QAction (tr ("&Load..."), this);
		aSaveAs = new QAction (tr ("&Save As..."), this);
		QMenu *mImport = new QMenu (tr ("Import"), this);
		QAction *aImport3ds = new QAction (tr ("Import.3DS..."), this);
		QAction *aImportobj = new QAction (tr ("Import.OBJ..."), this);
		QMenu *mExport = new QMenu (tr("Export"), this);
		QAction *aExportobj = new QAction (tr ("Export.OBJ..."), this);
		QAction *aAutoSanitize = new QAction (tr ("&Auto Sanitize Before Save"), this);
		QAction *aNewWindow = new QAction (tr ("&New Window"), this);
		QAction *aReloadXML = new QAction (tr ("Reload &XML"), this);
		QAction *aReloadXMLNif = new QAction (tr ("&Reload XML + Nif"), this);
		QAction *aXMLChecker = new QAction (tr ("XML Checker"), this);
		QAction *aResFiles = new QAction (tr ("Resource Files"), this);
		QAction *aQuit = new QAction (tr ("&Quit"), this);
		
		aLoad->setShortcut (QKeySequence (tr ("Ctrl+L")));
		aSaveAs->setShortcut (QKeySequence (tr ("Ctrl+S")));
		aAutoSanitize->setCheckable(true);
		aAutoSanitize->setChecked(true);
		aNewWindow->setShortcut (QKeySequence::New );
		aReloadXMLNif->setShortcut (QKeySequence (tr ("Alt+X")) );

		QMenu *mFile = menuBar ()->addMenu (tr ("&File"));
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
		connect(aLoad, SIGNAL(triggered()), this, SLOT(handleFileLoad()));

		// View
		createDockWidgets ();
		QMenu *mView = menuBar ()->addMenu (tr ("&View"));
		mView->addAction (dockRefr->toggleViewAction ());
		mView->addAction (dockTVBL->toggleViewAction ());
		mView->addAction (dockTVBD->toggleViewAction ());
		mView->addAction (dockTVKFM->toggleViewAction ());
		mView->addAction (dockInsp->toggleViewAction ());
		mView->addSeparator ();
		QMenu *mToolbars = mView->addMenu (tr ("&Toolbars"));
		mToolbars->setTitle (tr ("&Toolbars"));
		createToolbars ();
		foreach (QObject *o, children ()) {
			QToolBar *tb = qobject_cast<QToolBar*>(o);
			if (tb)
				mToolbars->addAction (tb->toggleViewAction ());
		}
		mView->addSeparator ();
		QMenu *mBlockList = mView->addMenu (tr ("Block List"));
		mBlockList->setTitle (tr ("Block List"));
		QActionGroup *gListMode = new QActionGroup (this);
		QAction *aHierarchy = new QAction (tr ("Show Blocks in Tree"), this);
		aHierarchy->setCheckable (true);
		aHierarchy->setEnabled (false);//TODO: implement me
		QAction *aList = new QAction (tr ("Show Blocks in List"), this);
		aList->setCheckable (true);
		aList->setEnabled (false);//TODO: implement me
		QAction *aBlockView = new QAction (tr ("Block View"), this);
		aBlockView->setCheckable (true);
		gListMode->addAction (aList);
		gListMode->addAction (aHierarchy);
		gListMode->addAction (aBlockView);
		gListMode->setExclusive (true);
		mBlockList->addAction (aHierarchy);
		mBlockList->addAction (aList);
		mBlockList->addAction (aBlockView);
		aBlockView->setChecked (true);
		QMenu *mBlockDetails = mView->addMenu (tr ("Block Details"));
		mBlockDetails->setTitle (tr ("Block Details"));
		QAction *aCondition = new QAction (tr ("Hide Version Mismatched Rows"), this);
		aCondition->setCheckable (true);
		aCondition->setChecked (false);
		QAction *aRCondition =
			new QAction (tr ("Realtime Row Version Updating (slow)"), this);
		aRCondition->setCheckable (true);
		aRCondition->setChecked (false);
		aRCondition->setEnabled (false);
		mBlockDetails->addAction (aCondition);
		mBlockDetails->addAction (aRCondition);
		mView->addSeparator ();
		QAction *aSelectFont = new QAction (tr ("Select Font ..."), this);
		connect(aSelectFont, SIGNAL(triggered()), this, SLOT(handleSelectFont()));
		mView->addAction (aSelectFont);

		// Render - the actions are created by createToolbars ()
		// TODO: all actions should be created in one place
		QMenu *mRender = menuBar ()->addMenu (tr ("&Render"));
		mRender->addAction (aViewTop);
		mRender->addAction (aViewFront);
		mRender->addAction (aViewSide);
		mRender->addAction (aViewWalk);
		mRender->addAction (aViewUser);
		mRender->addSeparator();
		mRender->addAction (aViewFlip);
		mRender->addAction (aViewPerspective);
		QAction *aViewUserSave = new QAction (tr ("Save User View"), this);
		aViewUserSave->setToolTip (tr ("Save current view rotation, position and distance"));
		aViewUserSave->setShortcut (Qt::CTRL + Qt::Key_F9);
		mRender->addAction (aViewUserSave);
		mRender->addSeparator ();
		//
		QAction *aDrawAxes = new QAction (tr ("Draw &Axes"), this);
		aDrawAxes->setToolTip (tr ("draw xyz-Axes"));
		aDrawAxes->setCheckable (true);
		QAction *aDrawNodes = new QAction (tr ("Draw &Nodes"), this);
		aDrawNodes->setToolTip (tr ("draw bones/nodes"));
		aDrawNodes->setCheckable (true);
		QAction *aDrawHavok = new QAction (tr ("Draw &Havok"), this);
		aDrawHavok->setToolTip (tr ("draw the havok shapes"));
		aDrawHavok->setCheckable (true);
		QAction *aDrawConstraints = new QAction (tr ("Draw &Constraints"), this);
		aDrawConstraints->setToolTip (tr ("draw the havok constraints"));
		aDrawConstraints->setCheckable (true);
		QAction *aDrawFurn = new QAction (tr ("Draw &Furniture"), this);
		aDrawFurn->setToolTip (tr ("draw the furniture markers"));
		aDrawFurn->setCheckable (true);
		QAction *aDrawHidden = new QAction (tr ("Show Hid&den"), this);
		aDrawHidden->setToolTip (tr ("always draw nodes and meshes"));
		aDrawHidden->setCheckable (true);
		QAction *aDrawStats = new QAction (tr ("Show S&tats"), this);
		aDrawStats->setToolTip (tr ("display some statistics about the selected node"));
		aDrawStats->setCheckable (true);
		QAction *aSettings = new QAction (tr ("&Settings..."), this);
		aSettings->setToolTip (tr ("show the settings dialog"));
		//
		mRender->addActions (
			QList<QAction*>()
			<< aDrawAxes
			<< aDrawNodes
			<< aDrawHavok
			<< aDrawConstraints
			<< aDrawFurn
			<< aDrawHidden
			<< aDrawStats
			<< aSettings
		);

		// Tools ("Spells")
		// TODO: automatic creation based on a list of "registered"
		// TODO: script-based plug-ins
		QMenu *mTools = menuBar ()->addMenu (tr ("&Tools"));
		QMenu *mAnimation = new QMenu (tr ("Animation"), this);
		QAction *aAttach_KF = new QAction (tr ("Attach .KF"), this);
		QAction *aEditStringPalettes = new QAction (tr ("Edit String Palettes"), this);
		mAnimation->addActions (QList<QAction*>()
			<< aAttach_KF << aEditStringPalettes);
		QMenu *mBlock = new QMenu (tr ("Block"), this);
		QAction *aInsert = new QAction (tr ("Insert"), this);
		QAction *aRemoveById = new QAction (tr ("Remove By Id"), this);
		QAction *aSortByName = new QAction (tr ("Sort By Name"), this);
		mBlock->addActions (QList<QAction*>()
			<< aInsert << aRemoveById << aSortByName);
		QMenu *mSanitize = new QMenu (tr ("Sanitize"), this);
		QAction *aReorderLinkArrays = new QAction (tr ("Reorder Link Arrays"), this);
		QAction *aCollapseLinkArrays = new QAction (tr ("Collapse Link Arrays"), this);
		QAction *aAdjustTextureSources =
			new QAction (tr ("Adjust Texture Sources"), this);
		QAction *aReorderBlocks = new QAction (tr ("Reorder Blocks"), this);
		QAction *aCheckLinks = new QAction (tr ("Check Links"), this);
		mSanitize->addActions (QList<QAction*>()
			<< aReorderLinkArrays
			<< aCollapseLinkArrays
			<< aAdjustTextureSources
			<< aReorderBlocks
			<< aCheckLinks);
		QMenu *mOptimize = new QMenu (tr ("Export"), this);
		QAction *aCombineProperties = new QAction (tr ("Combine Properties"), this);
		QAction *aSplitProperties = new QAction (tr ("Split Properties"), this);
		QAction *aRemoveBogusNodes = new QAction (tr ("Remove Bogus Nodes"), this);
		QAction *aStripifyAllTriShapes =
			new QAction (tr ("Stripify All TriShapes"), this);
		mOptimize->addActions (QList<QAction*>()
			<< aCombineProperties
			<< aSplitProperties
			<< aRemoveBogusNodes
			<< aStripifyAllTriShapes);
		QMenu *mBatch = new QMenu (tr ("Export"), this);
		QAction *MakeAllSkinPartitions =
			new QAction (tr ("Make All Skin Partitions"), this);
		QAction *aUpdateAllTangentSpaces =
			new QAction (tr ("Update All Tangent Spaces"), this);
		QAction *aMultiApplyMode = new QAction (tr ("Multi Apply Mode"), this);
		mBatch->addActions (QList<QAction*>()
			<< MakeAllSkinPartitions
			<< aUpdateAllTangentSpaces
			<< aMultiApplyMode);
		mTools->addMenu (mAnimation);
		mTools->addMenu (mBlock);
		mTools->addMenu (mSanitize);
		mTools->addMenu (mOptimize);
		mTools->addMenu (mBatch);

		// Help
		QMenu *mHelp = menuBar ()->addMenu (tr ("&Help"));
		mHelp->addAction (dockRefr->toggleViewAction ());
		QAction *aHelpWebsite =
			new QAction (tr ("NifSkope Documentation && &Tutorials"), this);
		aHelpWebsite->setData (
			QUrl ("http://niftools.sourceforge.net/wiki/index.php/NifSkope"));
		connect(aHelpWebsite, SIGNAL(triggered()), this, SLOT(handleOpenURL()));
		QAction *aHelpForum =
			new QAction (tr ("NifSkope Help && Bug Report &Forum"), this);
		aHelpForum->setData (
			QUrl ("http://niftools.sourceforge.net/forum/viewforum.php?f=24"));
		connect(aHelpForum, SIGNAL(triggered()), this, SLOT(handleOpenURL()));
		QAction *aNifToolsWebsite = new QAction (tr ("NifTools &Wiki"), this);
		aNifToolsWebsite->setData (QUrl ("http://niftools.sourceforge.net"));
		connect(aNifToolsWebsite, SIGNAL(triggered()), this, SLOT(handleOpenURL()));
		QAction *aNifToolsDownloads = new QAction (tr ("NifTools &Downloads"), this);
		aNifToolsDownloads->setData (
			QUrl ("http://sourceforge.net/project/showfiles.php?group_id=149157"));
		connect(aNifToolsDownloads, SIGNAL(triggered()), this, SLOT(handleOpenURL()));
		QAction *aNifSkope = new QAction (tr ("About &NifSkope"), this);
		connect(aNifSkope, SIGNAL(triggered()), this, SLOT(About()));
		QAction *aAboutQt = new QAction (tr ("About &Qt"), this);
		connect(aAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
		mHelp->addAction (aHelpWebsite);
		mHelp->addAction (aHelpForum);
		mHelp->addSeparator ();
		mHelp->addAction (aNifToolsWebsite);
		mHelp->addAction (aNifToolsDownloads);
		mHelp->addSeparator ();
		mHelp->addAction (aAboutQt);
		mHelp->addAction (aNifSkope);
	}

	void
	Qt4MainWindow::createDockWidgets()
	{
		// Block List
		tvBlockList = new QTreeView;
		tvBlockList->setUniformRowHeights (true);
		tvBlockList->setAlternatingRowColors (true);
		tvBlockList->setContextMenuPolicy (Qt::CustomContextMenu);
		tvBlockList->setHorizontalScrollBarPolicy (Qt::ScrollBarAsNeeded);
		dockTVBL = new QDockWidget (tr ("Block List"));
		dockTVBL->setObjectName ("dockTVBL");
		dockTVBL->setWidget (tvBlockList);

		// Block Details
		tvBlockDetails = new QTreeView;
		tvBlockDetails->setUniformRowHeights (true);
		tvBlockDetails->setAlternatingRowColors (true);
		tvBlockDetails->setHorizontalScrollBarPolicy (Qt::ScrollBarAsNeeded);
		dockTVBD = new QDockWidget (tr ("Block Details"));
		dockTVBD->setObjectName ("dockTVBD");
		dockTVBD->setWidget (tvBlockDetails);

		// KFM
		tvKFM = new QTreeView;
		tvKFM->header ()->setStretchLastSection (false);
		dockTVKFM = new QDockWidget (tr ("KFM"));
		dockTVKFM->setObjectName ("dockTVKFM");
		dockTVKFM->setWidget (tvKFM);
		dockTVKFM->toggleViewAction ()->setShortcut (Qt::Key_F4);
		dockTVKFM->toggleViewAction ()->setChecked (false);

		// Inspect
		inspect = new QDialog;
		dockInsp = new QDockWidget (tr ("Inspect"));
		dockInsp->setObjectName ("InspectDock");
		dockInsp->setWidget (inspect);
		dockInsp->toggleViewAction ()->setChecked (false);

		// Interactive Help
		refrbrwsr = new QTextBrowser;
		dockRefr = new QDockWidget (tr ("Interactive Help"));
		dockRefr->setObjectName ("RefrDock");
		dockRefr->setWidget (refrbrwsr);
		dockRefr->toggleViewAction ()->setShortcut (Qt::Key_F1);
		dockRefr->toggleViewAction ()->setChecked (false);

		addDockWidget (Qt::BottomDockWidgetArea, dockTVBD);
		addDockWidget (Qt::LeftDockWidgetArea, dockTVBL);
		addDockWidget (Qt::RightDockWidgetArea, dockTVKFM);
		addDockWidget (Qt::RightDockWidgetArea, dockInsp, Qt::Vertical);
		addDockWidget (Qt::BottomDockWidgetArea, dockRefr);
	}

	void
	Qt4MainWindow::createToolbars()
	{
		// core layout - main toolbars
		QToolBar *tCommon = new QToolBar (tr ("Load && Save"));
		tCommon->setObjectName ("tCommon");
		tCommon->setAllowedAreas (Qt::TopToolBarArea | Qt::BottomToolBarArea);
		tCommon->addAction (aLoad);
		QLineEdit *leLoadFile = new QLineEdit;
		tCommon->addWidget (leLoadFile);
		QLineEdit *leSaveFile = new QLineEdit;
		tCommon->addWidget (leSaveFile);
		tCommon->addAction (aSaveAs);

		QToolBar *tAnim = new QToolBar (tr ("Animation"));
		tAnim->setObjectName ("tAnim");
		tAnim->setAllowedAreas (Qt::TopToolBarArea | Qt::BottomToolBarArea);
		tAnim->setIconSize (QSize (16, 16));
		// actions
		QAction *aAnimPlay = new QAction (QIcon (":/btn/play"), tr ("&Play"), this);
		aAnimPlay->setCheckable (true);
		aAnimPlay->setChecked (true);
		aAnimPlay->setToolTip (tr("Start Animation"));
		QAction *aAnimLoop = new QAction (QIcon (":/btn/loop"), tr ("&Loop"), this);
		aAnimLoop->setCheckable (true);
		aAnimLoop->setChecked (true);
		QAction *aAnimSwitch = new QAction (QIcon (":/btn/switch"), tr ("&Switch"), this);
		aAnimSwitch->setCheckable (true);
		aAnimSwitch->setChecked (true);
		// toolbar
		tAnim->addAction (aAnimPlay);
		QSlider *sldTime = new QSlider (Qt::Horizontal);
		tAnim->addWidget (sldTime);// has an editor - a floating one IIRC
		tAnim->addAction (aAnimLoop);
		tAnim->addAction (aAnimSwitch);
		QComboBox *animGroups = new QComboBox;
		animGroups->setMinimumWidth (100);
		tAnim->addWidget (animGroups);

		QToolBar *tView = new QToolBar (tr ("View"));
		tView->setObjectName ("tView" );
		tView->setAllowedAreas (Qt::TopToolBarArea | Qt::BottomToolBarArea);
		tView->setIconSize (QSize (16, 16));
		// actions
		QActionGroup *grpView = new QActionGroup (this);
		grpView->setExclusive (true);
		/*connect(grpView, SIGNAL(triggered(QAction *)),
				this, SLOT(viewAction(QAction *)));*/
		aViewTop = new QAction (QIcon (":/btn/viewTop"), tr ("Top"), grpView);
		aViewTop->setToolTip (tr ("View from above") );
		aViewTop->setCheckable (true);
		aViewTop->setShortcut (Qt::Key_F5);
		grpView->addAction (aViewTop);
		aViewFront =
			new QAction (QIcon (":/btn/viewFront"), tr("Front"), grpView);
		aViewFront->setToolTip (tr ("View from the front"));
		aViewFront->setCheckable (true);
		aViewFront->setChecked (true);
		aViewFront->setShortcut (Qt::Key_F6);
		grpView->addAction (aViewFront);
		aViewSide = new QAction (QIcon (":/btn/viewSide"), tr ("Side"), grpView);
		aViewSide->setToolTip (tr ("View from the side"));
		aViewSide->setCheckable (true);
		aViewSide->setShortcut (Qt::Key_F7);
		grpView->addAction (aViewSide);
		aViewUser = new QAction (QIcon (":/btn/viewUser" ), tr ("User"), grpView);
		aViewUser->setToolTip (tr ("Restore the view as it was when Save User View was activated"));
		aViewUser->setCheckable (true);
		aViewUser->setShortcut (Qt::Key_F8);
		grpView->addAction (aViewUser);
		aViewWalk = new QAction (QIcon (":/btn/viewWalk"), tr ("Walk"), grpView);
		aViewWalk->setToolTip (tr ("Enable walk mode"));
		aViewWalk->setCheckable (true);
		aViewWalk->setShortcut (Qt::Key_F9);
		grpView->addAction (aViewWalk);
		aViewFlip = new QAction (QIcon (":/btn/viewFlip"), tr ("Flip"), this);
		aViewFlip->setToolTip (tr ("Flip View from Front to Back, Top to Bottom, Side to Other Side"));
		aViewFlip->setCheckable (true);
		aViewFlip->setShortcut (Qt::Key_F11);
		grpView->addAction (aViewFlip);
		aViewPerspective =
			new QAction (QIcon (":/btn/viewPers"), tr ("Perspective"), this);
		aViewPerspective->setToolTip (tr ("Perspective View Transformation or Orthogonal View Transformation"));
		aViewPerspective->setCheckable (true);
		aViewPerspective->setShortcut (Qt::Key_F10);
		grpView->addAction (aViewPerspective);
		// toolbar
		tView->addAction (aViewTop);
		tView->addAction (aViewFront);
		tView->addAction (aViewSide);
		tView->addAction (aViewUser);
		tView->addAction (aViewWalk);
		tView->addSeparator ();
		tView->addAction (aViewFlip);
		tView->addAction (aViewPerspective);

		addToolBar (Qt::TopToolBarArea, tCommon);
		addToolBar (Qt::TopToolBarArea, tAnim);
		addToolBar (Qt::TopToolBarArea, tView);
	}

	void
	Qt4MainWindow::Reset()
	{
		if (mdlBlockList) {
			QItemSelectionModel *sm = tvBlockList->selectionModel ();
			disconnect (sm, SIGNAL(selectionChanged(
					const QItemSelection &, const QItemSelection &)),
				this, SLOT(handleBLselChanged(
					const QItemSelection &, const QItemSelection &)));
			tvBlockList->setModel (NULL);
			delete mdlBlockList;
			mdlBlockList = NULL;
		}

		if (mdlBlockDetails) {
			tvBlockDetails->setModel (NULL);
			delete mdlBlockDetails;
			mdlBlockDetails = NULL;
		}
	}

	void
	Qt4MainWindow::handleFileLoad()
	{
		// UI part of the handler
		QString fileName = QFileDialog::getOpenFileName (
			this, tr ("Open File"), "",
			QString::fromStdString (App->File.GetLoadFormats (" ", ";;")));
		if (fileName == NULL)
			return;
		// Model part of the handler
		App->File.FileName = fileName.toStdString ();
		App->File.Load ();
		// UI part of the handler
		//  Display it in "Block List" "As Blocks"
		// one .nif file loaded at a time
		if (optSingleFile)
			Reset ();
		else {
			QMessageBox::information (
				this, "Info", "Multiple file load is N/A yet");
			return;
		}
		mdlBlockList = new QNifBlockModel (this);
		tvBlockList->setModel (mdlBlockList);
		tvBlockList->header ()->setResizeMode (0, QHeaderView::ResizeToContents);
		tvBlockList->header ()->setResizeMode (1, QHeaderView::ResizeToContents);
		tvBlockList->header ()->setResizeMode (2, QHeaderView::Interactive);
		QItemSelectionModel *sm = tvBlockList->selectionModel ();
		connect (sm, SIGNAL(selectionChanged(
					const QItemSelection &, const QItemSelection &)),
				this, SLOT(handleBLselChanged(
					const QItemSelection &, const QItemSelection &)));
		// Block Details
		mdlBlockDetails = new QNifModel (this);
		mdlBlockDetails->SetRoot (App->AsNifTree ());//TODO: remove later
 		tvBlockDetails->setModel (mdlBlockDetails);
		//tvBlockDetails->header ()->setResizeMode (0, QHeaderView::ResizeToContents);
		// the above aint working all the time - TODO: figure out when and why
		tvBlockDetails->header ()->setResizeMode (1, QHeaderView::ResizeToContents);
		tvBlockDetails->header ()->setResizeMode (2, QHeaderView::ResizeToContents);
		//tvBlockDetails->expandAll ();
	}

	void
	Qt4MainWindow::handleBLselChanged(
		const QItemSelection &selected,
		const QItemSelection &deselected)
	{
		if (selected.indexes ().count () > 0) {
			void *p = selected.indexes ().value (0).internalPointer ();
			if (!p)
				return;
			NifLib::Node *n = static_cast<NifLib::Node *>(p);
			if (!n)
				return;
			else {
				QNifModel *mdlNew = new QNifModel (this);
				mdlNew->SetRoot (n);
				tvBlockDetails->setModel (mdlNew);
				delete mdlBlockDetails;
				mdlBlockDetails = mdlNew;
			}
		}
	}

	void
	Qt4MainWindow::handleSelectFont()
	{
		bool ok;
		QFont fnt = QFontDialog::getFont (&ok, this->font (), this);
		if (!ok)
			return;
		QApplication::setFont (fnt);
	}

	void
	Qt4MainWindow::handleOpenURL()
	{
		if (!sender ())
			return;
		QAction *aURL = qobject_cast<QAction*>(sender ());
		if (!aURL)
			return;
		QUrl URL = aURL->data ().toUrl ();
		if (!URL.isValid ())
			return;
		QDesktopServices::openUrl (URL);
	}

	void
	Qt4MainWindow::About()
	{
		QString text = tr (
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

		QMessageBox mb (tr ("About NifSkope %1 (revision %2)")
			.arg(NS_VERSION).arg("F"),
			text,
			QMessageBox::Information,
			QMessageBox::Ok + QMessageBox::Default, 0, 0, this);
		mb.setIconPixmap (QPixmap ( ":/res/nifskope.png" ));
		mb.exec ();
	}

	Qt4MainWindow::Qt4MainWindow()
		: QMainWindow()
		,optSingleFile(1), mdlBlockList(0), mdlBlockDetails(0)
	{
		resize (800, 600);
		// f.InitialPosition = DesktopCenter
		this->move (
			QApplication::desktop ()->screen ()->rect ().center () -
			this->rect ().center ());
		createMainMenu ();
	}

	Qt4MainWindow::~Qt4MainWindow()
	{
		if (mdlBlockDetails)
			delete mdlBlockDetails;
		if (mdlBlockList)
			delete mdlBlockList;
		NSINFO("~Qt4MainWindow ()")
	}
}
