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

#ifndef __QNIFMODEL_H__
#define __QNIFMODEL_H__

// NifSkope
#include <NifSkopeApp.h>

// Qt
#include <QtGui>
#include <QtCore/qabstractitemmodel.h>

// "NifLib"
#include "Field.h"
#include "TreeNode.h"

#include <map>

namespace NifSkopeQt4
{
	class Qt4MainWindow;

	class QNifModel: public QAbstractItemModel
	{
		/*
		*	Dynamic "Index" "property" for "NifLib::Node"
		*/
		std::map<NifLib::Node *, int> *dummy;
	protected:
		struct Column {
			Column(
				QVariant hn,
				QVariant (QNifModel::*fmt)(NifLib::Node *))
			{
				HeaderName = hn;
				Format = fmt;
			}
			QVariant HeaderName;
			QVariant (QNifModel::*Format)(NifLib::Node *);
		};
		Qt4MainWindow *win;
		QList<Column> cols;
		NifLib::Node *rn;

		/*
		*	Each function returns data for the column its named after
		*/
		QVariant CId(NifLib::Node *node);
		QVariant CName(NifLib::Node *node);
		QVariant CType(NifLib::Node *node);
		QVariant CValue(NifLib::Node *node);
		QVariant CArgument(NifLib::Node *node);
		QVariant CArray1(NifLib::Node *node);
		QVariant CArray2(NifLib::Node *node);
		QVariant CCondition(NifLib::Node *node);
		QVariant CSince(NifLib::Node *node);
		QVariant CUntil(NifLib::Node *node);
		QVariant CVersionCondition(NifLib::Node *node);

	public:
		QNifModel(Qt4MainWindow *data, QObject *parent = 0);

		QVariant data(
			const QModelIndex &index,
			int role = Qt::DisplayRole) const;

		Qt::ItemFlags flags(const QModelIndex &index) const;

		QVariant headerData(
			int section,
			Qt::Orientation orientation,
			int role = Qt::DisplayRole) const;

		QModelIndex index(
			int row,
			int column,
			const QModelIndex &parent = QModelIndex()) const;

		QModelIndex parent(const QModelIndex &index) const;

		int rowCount(const QModelIndex &parent = QModelIndex()) const;

		int columnCount(const QModelIndex &parent = QModelIndex()) const;

		/*
		*	Set root node. TODO: doesn't work as "change root node"
		*/
		void SetRoot(NifLib::Node *node);

		~QNifModel();
	};
}

#endif /*__QNIFMODEL_H__*/
