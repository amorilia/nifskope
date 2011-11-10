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

#include <NifSkopeApp.h>

#include <QtGui>
#include <QtCore/qabstractitemmodel.h>

// "NifLib"
#include "List.h"
#include "Field.h"
#include "Tag.h"
#include "Attr.h"
#include "TreeNode.h"
#include "niflib.h"

namespace NifSkopeQt4
{
	class MainWindow;

	class QNifModel: public QAbstractItemModel
	{
	protected:
		MainWindow *win;
		QList<QVariant> headers;
		NifLib::TreeNode<NifLib::Field *> *rn;
	public:
		QNifModel(MainWindow *data, QObject *parent = 0);
		~QNifModel();

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex &index) const;
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount(const QModelIndex &parent = QModelIndex()) const;
    	/*bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    	bool hasChildren(const QModelIndex &index = QModelIndex()) const;
    	void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    	QStringList mimeTypes() const;
    	QMimeData *mimeData(const QModelIndexList &indexes) const;
    	bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent);
    	Qt::DropActions supportedDropActions() const;*/
		// specific
		void SetRoot(NifLib::TreeNode<NifLib::Field *> *node);
	};

	class QNifBlockModel: public QNifModel
	{
	public:
		QNifBlockModel(MainWindow *data, QObject *parent = 0);
		~QNifBlockModel();
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		//QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	};

	class MainWindow: public QMainWindow
	{
		void createMainMenu();
		void createDockWidgets();
		void createToolbars();
		QAction *aLoad;
		QAction *aSaveAs;

		QTreeView *tvBlockList;
		QTreeView *tvBlockDetails;
		QNifModel *mBlockDetails;

		QDockWidget *dockTVBL;
		QDockWidget *dockTVBD;
		QDockWidget *dockTVKFM;
		QDockWidget *dockInsp;
		QDockWidget *dockRefr;

		QAction *aViewTop;
		QAction *aViewFront;
		QAction *aViewSide;
		QAction *aViewUser;
		QAction *aViewWalk;
		QAction *aViewFlip;
		QAction *aViewPerspective;
	Q_OBJECT
	public:
		MainWindow();
		NifSkope::NifSkopeApp *App;// TODO: init by Qt4App because of NewWindow() only
	protected slots:
		void stbBLselectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
		void sFileLoad();
		void sSelectFont();
		void sOpenURL();
	public slots:
		void About();
	};
}

#endif /*__QT4MAINWINDOW_H__*/
