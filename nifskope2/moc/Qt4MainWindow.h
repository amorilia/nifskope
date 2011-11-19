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

#ifndef __QT4MAINWINDOW_H__
#define __QT4MAINWINDOW_H__

// NifSkope
#include <NifSkopeApp.h>
#include "QNifModel.h"
#include "QNifBlockModel.h"

// Qt
#include <QtGui>

#include "nifskope.h"

namespace NifSkopeQt4
{
	enum BlockListMode {
		BLM_TREE,
		BLM_LIST
	};

	// default "config"
	// TODO: class per file
	class Cfg
	{
	public:
		class BlockListCfg
		{
		public:
			BlockListCfg()
			{
				Mode = BLM_TREE;
			}
			BlockListMode Mode;
		};
		BlockListCfg BlockList;
	};

	class Qt4MainWindow: public QMainWindow
	{
		Cfg cfg;
		bool optSingleFile;

		void createMainMenu();
		void createDockWidgets();
		void createToolbars();
		void ResetBlockList();
		void Reset();
		void ChangeBlockListMode(BlockListMode to);
		void handleOpenURL();

		QTimer *onIdle;

		// MM:File
		QAction *aOpen;
		QAction *aSaveAs;
		QAction *aImport3ds;
		QAction *aImportobj;
		QAction *aExportobj;
		QAction *aAutoSanitize;
		QAction *aNewWindow;
		QAction *aReloadXML;
		QAction *aReloadXMLNif;
		QAction *aXMLChecker;
		QAction *aResFiles;
		QAction *aQuit;

		// MM:View->Block List
		QAction *aBLToTree;
		QAction *aBLToList;
		// MM:View->Block Details
		QAction *aBDHideVMRows;
		QAction *aBDRTVUpdate;
		// MM:View
		QAction *aSelectFont;

		// MM:Render
		QAction *aSaveUserView;
		QAction *aDrawAxes;
		QAction *aDrawNodes;
		QAction *aDrawHavok;
		QAction *aDrawConstraints;
		QAction *aDrawFurn;
		QAction *aDrawHidden;
		QAction *aDrawStats;
		QAction *aSettings;

		// Toolbar Animation
		QAction *aAnimPlay;
		QAction *aAnimLoop;
		QAction *aAnimSwitch;

		QTreeView *tvBlockList;
		QAbstractItemModel *mdlBlockList;
		QTreeView *tvBlockDetails;
		QNifModel *mdlBlockDetails;
		QTreeView *tvKFM;
		QDialog *inspect;
		QTextBrowser *refrbrwsr;

		QDockWidget *dockTVBL;
		QDockWidget *dockTVBD;
		QDockWidget *dockTVKFM;
		QDockWidget *dockInsp;
		QDockWidget *dockRefr;

		// Toolbar View, MM:View
		QAction *aViewTop;
		QAction *aViewFront;
		QAction *aViewSide;
		QAction *aViewUser;
		QAction *aViewWalk;
		QAction *aViewFlip;
		QAction *aViewPerspective;

		// MM:Help
		QAction *aHelpWebsite;
		QAction *aHelpForum;
		QAction *aNifToolsWebsite;
		QAction *aNifToolsDownloads;
		QAction *aAboutNifSkope;
		QAction *aAboutQt;

	Q_OBJECT
	public:
		Qt4MainWindow();
		NifSkope::NifSkopeApp *App;// TODO: init by Qt4App because of NewWindow() only
		~Qt4MainWindow();
	protected slots:
		void handleBLselChanged(
			const QItemSelection &selected,
			const QItemSelection &deselected);

		/*
		*	Action handlers - they're named after their corresponding action:
		*	Action: aOpen
		*	Handler: handleOpen
		*/

		/*
		*	Load a .nif file
		*/
		void handleOpen();

		void handleQuit();

		void handleSelectFont();

		void handleBLToTree();
		void handleBLToList();

		void handleHelpWebsite();
		void handleHelpForum();
		void handleNifToolsWebsite();
		void handleNifToolsDownloads();
		void handleAboutNifSkope();//About()
		void handleAboutQt(); //aboutQt()
	public slots:
		void About();
	};
}

#endif /*__QT4MAINWINDOW_H__*/
