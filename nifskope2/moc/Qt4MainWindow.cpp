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
#define NEW_ACTN(ENABLED,OBJ,TXT,ICN,KEY,C1,C2)\
	{\
		a##OBJ = new QAction (QIcon (ICN), tr (TXT), this);\
		a##OBJ->setEnabled (ENABLED);\
		a##OBJ->setShortcut (KEY);\
		a##OBJ->setCheckable (C1);\
		a##OBJ->setChecked (C2);\
		connect(a##OBJ, SIGNAL(triggered()), this, SLOT(handle##OBJ()));\
	}
#define KEY_NONE QKeySequence ()
	void
	Qt4MainWindow::createMainMenu()
	{
		NEW_ACTN(1, Open, "&Open...", "", QKeySequence::Open, 0, 0)
		NEW_ACTN(0, SaveAs, "&Save...", "", QKeySequence::Save, 0, 0)
		NEW_ACTN(0, Import3ds, "Import.3DS...", "", KEY_NONE, 0, 0)
		NEW_ACTN(0, Importobj, "Import.OBJ...", "", KEY_NONE, 0, 0)
		NEW_ACTN(0, Exportobj, "Export.OBJ...", "", KEY_NONE, 0, 0)
		NEW_ACTN(0, AutoSanitize, "&Auto Sanitize Before Save", "", KEY_NONE, 1, 1)
		NEW_ACTN(0, NewWindow, "&New Window", "", QKeySequence::New, 0, 0)
		NEW_ACTN(0, ReloadXML, "Reload &XML", "", QKeySequence (tr ("Alt+X")), 0, 0)
		NEW_ACTN(0, ReloadXMLNif, "&Reload XML + Nif", "", KEY_NONE, 0, 0)
		NEW_ACTN(0, XMLChecker, "XML Checker", "", KEY_NONE, 0, 0)
		NEW_ACTN(0, ResFiles, "Resource Files", "", KEY_NONE, 0, 0)
		NEW_ACTN(1, Quit, "&Quit", "", QKeySequence::Quit, 0, 0)
		QActionGroup *gListMode = new QActionGroup (this);
			NEW_ACTN(1, BLToTree, "Tree", "", KEY_NONE, 1, 0)
			NEW_ACTN(1, BLToList, "List", "", KEY_NONE, 1, 0)
		gListMode->addAction (aBLToTree);
		gListMode->addAction (aBLToList);
		gListMode->setExclusive (true);
		NEW_ACTN(0, BDHideVMRows, "Hide Version Mismatched Rows", "", KEY_NONE, 1, 0)
		NEW_ACTN(0, BDRTVUpdate, "Realtime Row Version Updating (slow)", "", KEY_NONE, 1, 0)
		NEW_ACTN(1, SelectFont, "Select Font...", "", KEY_NONE, 0, 0)
		NEW_ACTN(0, AnimPlay, "&Play", ":/btn/play", KEY_NONE, 1, 1)
		aAnimPlay->setToolTip (tr("Start Animation"));
		NEW_ACTN(0, AnimLoop, "&Loop", ":/btn/loop", KEY_NONE, 1, 1)
		NEW_ACTN(0, AnimSwitch, "&Switch", ":/btn/switch", KEY_NONE, 1, 1)
		QActionGroup *grpView = new QActionGroup (this);
		grpView->setExclusive (true);
			NEW_ACTN(0, ViewTop, "Top", ":/btn/viewTop", Qt::Key_F5, 1, 0)
			aViewTop->setToolTip (tr ("View from above"));
			NEW_ACTN(0, ViewFront, "Front", ":/btn/viewFront", Qt::Key_F6, 1, 1)
			aViewFront->setToolTip (tr ("View from the front"));
			NEW_ACTN(0, ViewSide, "Side", ":/btn/viewSide", Qt::Key_F7, 1, 0)
			aViewSide->setToolTip (tr ("View from the side"));
			NEW_ACTN(0, ViewUser, "User", ":/btn/viewUser", Qt::Key_F8, 1, 0)
			aViewUser->setToolTip (tr ("Restore the view as it was when Save User View was activated"));
			NEW_ACTN(0, ViewWalk, "Walk", ":/btn/viewWalk", Qt::Key_F9, 1, 0)
			aViewWalk->setToolTip (tr ("Enable walk mode"));
			NEW_ACTN(0, ViewFlip, "Flip", ":/btn/viewFlip", Qt::Key_F11, 1, 0)
			aViewFlip->setToolTip (tr ("Flip View from Front to Back, Top to Bottom, Side to Other Side"));
			NEW_ACTN(0, ViewPerspective, "Perspective", ":/btn/viewPers", Qt::Key_F10, 1, 0)
			aViewPerspective->setToolTip (tr ("Perspective View Transformation or Orthogonal View Transformation"));
		grpView->addAction (aViewTop);
		grpView->addAction (aViewFront);
		grpView->addAction (aViewSide);
		grpView->addAction (aViewUser);
		grpView->addAction (aViewWalk);
		grpView->addAction (aViewFlip);
		grpView->addAction (aViewPerspective);
		NEW_ACTN(0, SaveUserView, "Save User View", "", Qt::CTRL + Qt::Key_F9, 0, 0)
		aSaveUserView->setToolTip (tr ("Save current view rotation, position and distance"));
		NEW_ACTN(0, DrawAxes, "Draw &Axes", "", KEY_NONE, 1, 0)
		aDrawAxes->setToolTip (tr ("draw xyz-Axes"));
		NEW_ACTN(0, DrawNodes, "Draw &Nodes", "", KEY_NONE, 1, 0)
		aDrawNodes->setToolTip (tr ("draw bones/nodes"));
		NEW_ACTN(0, DrawHavok, "Draw &Havok", "", KEY_NONE, 1, 0)
		aDrawHavok->setToolTip (tr ("draw the havok shapes"));
		NEW_ACTN(0, DrawConstraints, "Draw &Constraints", "", KEY_NONE, 1, 0)
		aDrawConstraints->setToolTip (tr ("draw the havok constraints"));
		NEW_ACTN(0, DrawFurn, "Draw &Furniture", "", KEY_NONE, 1, 0)
		aDrawFurn->setToolTip (tr ("draw the furniture markers"));
		NEW_ACTN(0, DrawHidden, "Show Hid&den", "", KEY_NONE, 1, 0)
		aDrawHidden->setToolTip (tr ("always draw nodes and meshes"));
		NEW_ACTN(0, DrawStats, "Show S&tats", "", KEY_NONE, 1, 0)
		aDrawStats->setToolTip (tr ("display some statistics about the selected node"));
		NEW_ACTN(0, Settings, "&Settings...", "", KEY_NONE, 1, 0)
		aSettings->setToolTip (tr ("show the settings dialog"));
		NEW_ACTN(1, HelpWebsite, "NifSkope Documentation && &Tutorials", "", KEY_NONE, 0, 0)
		aHelpWebsite->setData (QUrl (SITE));
		NEW_ACTN(1, HelpForum, "NifSkope Help && Bug Report &Forum", "", KEY_NONE, 0, 0)
		aHelpForum->setData (QUrl (FORUM));
		NEW_ACTN(1, NifToolsWebsite, "NifTools &Wiki", "", KEY_NONE, 0, 0)
		aNifToolsWebsite->setData (QUrl (WIKI));
		NEW_ACTN(1, NifToolsDownloads, "NifTools &Downloads", "", KEY_NONE, 0, 0)
		aNifToolsDownloads->setData (QUrl (DOWNLOADS));
		NEW_ACTN(1, AboutNifSkope, "About &NifSkope", "", KEY_NONE, 0, 0)
		NEW_ACTN(1, AboutQt, "About &Qt", "", KEY_NONE, 0, 0)

		// File
		QMenu *mFile = menuBar ()->addMenu (tr ("&File"));
		mFile->addAction (aOpen);
		mFile->addAction (aSaveAs);
		mFile->addSeparator();
		QMenu *mImport = new QMenu (tr ("Import"), this);
		mFile->addMenu (mImport);
			mImport->addAction (aImport3ds);
			mImport->addAction (aImportobj);
		QMenu *mExport = new QMenu (tr("Export"), this);
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
		createToolbars ();
		foreach (QObject *o, children ()) {
			QToolBar *tb = qobject_cast<QToolBar*>(o);
			if (tb)
				mToolbars->addAction (tb->toggleViewAction ());
		}
		mView->addSeparator ();
		QMenu *mBlockList = mView->addMenu (tr ("Block List"));
			mBlockList->addAction (aBLToTree);
			mBlockList->addAction (aBLToList);
		QMenu *mBlockDetails = mView->addMenu (tr ("Block Details"));
			mBlockDetails->addAction (aBDHideVMRows);
			mBlockDetails->addAction (aBDRTVUpdate);
		mView->addSeparator ();
		mView->addAction (aSelectFont);

		// Render
		QMenu *mRender = menuBar ()->addMenu (tr ("&Render"));
		mRender->addAction (aViewTop);
		mRender->addAction (aViewFront);
		mRender->addAction (aViewSide);
		mRender->addAction (aViewWalk);
		mRender->addAction (aViewUser);
		mRender->addSeparator();
		mRender->addAction (aViewFlip);
		mRender->addAction (aViewPerspective);
		mRender->addAction (aSaveUserView);
		mRender->addSeparator ();
		mRender->addAction (aDrawAxes);
		mRender->addAction (aDrawNodes);
		mRender->addAction (aDrawHavok);
		mRender->addAction (aDrawConstraints);
		mRender->addAction (aDrawFurn);
		mRender->addAction (aDrawHidden);
		mRender->addAction (aDrawStats);
		mRender->addAction (aSettings);

		// Tools ("Spells")
		// TODO: automatic creation based on a list of "registered"
		// TODO: script-based plug-ins
		QMenu *mTools = menuBar ()->addMenu (tr ("&Tools"));
		mTools->setEnabled (false);
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
		mHelp->addAction (aHelpWebsite);
		mHelp->addAction (aHelpForum);
		mHelp->addSeparator ();
		mHelp->addAction (aNifToolsWebsite);
		mHelp->addAction (aNifToolsDownloads);
		mHelp->addSeparator ();
		mHelp->addAction (aAboutQt);
		mHelp->addAction (aAboutNifSkope);
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

		// setup treeviews
		/*tvBlockList->header ()->setResizeMode (0, QHeaderView::ResizeToContents);
		tvBlockList->header ()->setResizeMode (1, QHeaderView::ResizeToContents);
		tvBlockList->header ()->setResizeMode (2, QHeaderView::Interactive);
		tvBlockDetails->header ()->setResizeMode (0, QHeaderView::ResizeToContents);
		tvBlockDetails->header ()->setResizeMode (1, QHeaderView::ResizeToContents);
		tvBlockDetails->header ()->setResizeMode (2, QHeaderView::ResizeToContents);*/
	}

	void
	Qt4MainWindow::createToolbars()
	{
		// core layout - main toolbars
		QToolBar *tCommon = new QToolBar (tr ("Load && Save"));
		tCommon->setObjectName ("tCommon");
		tCommon->setAllowedAreas (Qt::TopToolBarArea | Qt::BottomToolBarArea);
		tCommon->addAction (aOpen);
		QLineEdit *leLoadFile = new QLineEdit;
		tCommon->addWidget (leLoadFile);
		QLineEdit *leSaveFile = new QLineEdit;
		tCommon->addWidget (leSaveFile);
		tCommon->addAction (aSaveAs);

		QToolBar *tAnim = new QToolBar (tr ("Animation"));
		tAnim->setObjectName ("tAnim");
		tAnim->setAllowedAreas (Qt::TopToolBarArea | Qt::BottomToolBarArea);
		tAnim->setIconSize (QSize (16, 16));
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
	Qt4MainWindow::ResetBlockList()
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
	}

	void
	Qt4MainWindow::Reset()
	{
		ResetBlockList ();

		if (mdlBlockDetails) {
			tvBlockDetails->setModel (NULL);
			delete mdlBlockDetails;
			mdlBlockDetails = NULL;
		}
	}

	void
	Qt4MainWindow::ChangeBlockListMode(BlockListMode to)
	{
		ResetBlockList ();
		if (to == BLM_LIST) {
			mdlBlockList = new QNifBlockModel (this);
			tvBlockList->setModel (mdlBlockList);
		} else
		if (to == BLM_TREE) {
			QNifBlockModel *tmp = new QNifBlockModel (this);
			tmp->SetRoot (App->AsNifTree ());
			tmp->Tree = true;
			mdlBlockList = tmp;
			tvBlockList->setModel (mdlBlockList);
		}
		QItemSelectionModel *sm = tvBlockList->selectionModel ();
		connect (sm, SIGNAL(selectionChanged(
			const QItemSelection &, const QItemSelection &)),
			this, SLOT(handleBLselChanged(
			const QItemSelection &, const QItemSelection &)));
		cfg.BlockList.Mode = to;
	}

	void
	Qt4MainWindow::handleOpen()
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
		if (optSingleFile)
			Reset ();
		else {
			QMessageBox::information (
				this, "Info", "Multiple file load is N/A yet");
			return;
		}
		// Block List
		ChangeBlockListMode (cfg.BlockList.Mode);
		// Block Details
		mdlBlockDetails = new QNifModel (this);
 		tvBlockDetails->setModel (mdlBlockDetails);
	}

	void
	Qt4MainWindow::handleQuit()
	{
		close ();
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
			n = App->GetTreeNode (n);// get real node
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
	Qt4MainWindow::handleBLToTree()
	{
		ChangeBlockListMode (BLM_TREE);
	}

	void
	Qt4MainWindow::handleBLToList()
	{
		ChangeBlockListMode (BLM_LIST);
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

	void
	Qt4MainWindow::handleHelpWebsite()
	{
		handleOpenURL ();
	}

	void
	Qt4MainWindow::handleHelpForum()
	{
		handleOpenURL ();
	}

	void
	Qt4MainWindow::handleNifToolsWebsite()
	{
		handleOpenURL ();
	}

	void
	Qt4MainWindow::handleNifToolsDownloads()
	{
		handleOpenURL ();
	}

	void
	Qt4MainWindow::handleAboutNifSkope()
	{
		About ();
	}

	void
	Qt4MainWindow::handleAboutQt()
	{
		QApplication::aboutQt ();
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
		// load defaults
		aBLToTree->setChecked (cfg.BlockList.Mode == BLM_TREE);
		aBLToList->setChecked (cfg.BlockList.Mode == BLM_LIST);
	}

	Qt4MainWindow::~Qt4MainWindow()
	{
		if (mdlBlockDetails)
			delete mdlBlockDetails;
		if (mdlBlockList)
			delete mdlBlockList;
		NSINFO("~Qt4MainWindow ()")
	}
#undef KEY_NONE
#undef NEW_ACTN
}
