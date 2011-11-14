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

#include "QNifBlockModel.h"
#include "Qt4MainWindow.h"

namespace NifSkopeQt4
{
#define TREEITEM NifLib::TreeNode<NifLib::Field *>
	QNifBlockModel::QNifBlockModel(Qt4MainWindow *data, QObject *parent)
		: QNifModel (data, parent)
	{
		headers.clear ();
		headers << "N" << "Name" << "Value";
	}
	QNifBlockModel::~QNifBlockModel()
	{
	}
	QVariant
	QNifBlockModel::data(const QModelIndex &index, int role) const
	{
		if (!index.isValid ())
			return QVariant ();
		if (role != Qt::DisplayRole)
			return QVariant ();
		TREEITEM *item = static_cast<TREEITEM *>(index.internalPointer ());
		NifLib::Field *f = item->Value;
		int col = index.column ();
		if (col == 0)// No
			return QVariant (QString ("%0").arg (item->Index));
		else if (col == 1)// Name
			return QVariant (QString (f->Name ().c_str ()));
		else if (col == 2)// Value
			return QVariant (QString (
				win->App->GetRootNodeValue (item->Index).c_str ()));
		else
			return QVariant ();
	}
	int
	QNifBlockModel::rowCount(const QModelIndex &parent) const
	{
		if (parent.column () > 0)
			return 0;
		TREEITEM *rootItem = win->App->AsTree ();
		if (!parent.isValid ())
			return rootItem->Nodes.Count ();
		else
			return 0;
	}
#undef TREEITEM
}
