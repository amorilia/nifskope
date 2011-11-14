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

#include "QNifModel.h"
#include "Qt4MainWindow.h"

namespace NifSkopeQt4
{
#define TREEITEM NifLib::TreeNode<NifLib::Field *>
	QNifModel::QNifModel(Qt4MainWindow *data, QObject *parent)
		: QAbstractItemModel (parent)
	{
		win = data;
		headers	<< "N"
		<< "Name"
		<< "Type"
		<< "Value"
		<< "Argument"
		<< "Array1"
		<< "Array2"
		<< "Condition"
		<< "Since"
		<< "Until"
		<< "Version Condition";
		rn = win->App->AsTree ();
	}

   	QNifModel::~QNifModel()
	{
	}

	QVariant
	QNifModel::data(const QModelIndex &index, int role) const
	{
		// TODO: method pointers to avoid the "else if"s
		if (!index.isValid ())
			return QVariant ();
		if (role != Qt::DisplayRole)
			return QVariant ();
		TREEITEM *item = static_cast<TREEITEM *>(index.internalPointer ());
		//TREEITEM *root = win->App->AsTree ();
		NifLib::Field *f = item->Value;
		int col = index.column ();
		if (col == 0)// No
			return QVariant (QString ("%0").arg (item->Index));
		else if (col == 1)// Name
			return QVariant (QString (f->Name ().c_str ()));
		else if (col == 2) { // Type
			if (item->Parent == win->App->AsTree ())
				return QVariant (QString ("NiBlock"));
			else
				return QVariant (QString (f->TagType ().c_str ()));
		}
		else if (col == 3) {// Value
			if (item->Parent == win->App->AsTree ())
				return QVariant (QString (
					win->App->GetRootNodeValue (item->Index).c_str ()));
			else {
			if (f->IsArray1D ()) {
				if (f->IsArrayJ ())
					return QVariant (QString ("[1D JAGGED ARRAY]"));
				else {
					if (f->IsCharArray ())
						return QVariant (QString (win->App->ToStr (f).c_str ()));
					else {
						//Add1D (fi, f);
						return QVariant (QString ("[1D ARRAY]"));
					}
				}
			} else
			if (f->IsArray2D ()) {
				if (f->IsArrayJ ())
					return QVariant (QString ("[2D JAGGED ARRAY]"));
				else
					return QVariant (QString ("[2D ARRAY]"));
			} else {
				if (f->Value.len > 64)
					return QVariant (QString ("[LARGE STRUCTURE]"));
				else
					return QVariant (QString (win->App->ToStr (f).c_str ()));
			}
			}
		}
		else if (col == 4)
			return QVariant (QString (f->TagAttr (AARG).c_str ()));
		else if (col == 5)
			return QVariant (QString (f->TagAttr (AARR1).c_str ()));
		else if (col == 6)
			return QVariant (QString (f->TagAttr (AARR2).c_str ()));
		else if (col == 7)
			return QVariant (QString (f->TagAttr (ACOND).c_str ()));
		else if (col == 8)
			return QVariant (QString (f->TagAttr (AVER1).c_str ()));
		else if (col == 9)
			return QVariant (QString (f->TagAttr (AVER2).c_str ()));
		else if (col == 10)
			return QVariant (QString (f->TagAttr (AVERCOND).c_str ()));
		else
			return QVariant ();
	}

	Qt::ItemFlags
	QNifModel::flags(const QModelIndex &index) const
	{
		if (!index.isValid ())
			return 0;
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;// read only
	}

    QVariant
	QNifModel::headerData(int section, Qt::Orientation orientation, int role) const
	{
		if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
			return headers.value (section);
		return QVariant ();
	}

	QModelIndex
	QNifModel::index(int row, int column, const QModelIndex &parent) const
	{
 		if (!hasIndex (row, column, parent))
        	return QModelIndex ();
		//TREEITEM *rootItem = win->App->AsTree ();
		TREEITEM *parentItem;
		if (!parent.isValid ())
			parentItem = rn;
		else
			parentItem = static_cast<TREEITEM *>(parent.internalPointer ());
		TREEITEM *childItem = parentItem->Nodes[row];// TODO: is this safe?
		if (childItem)
			return createIndex (row, column, childItem);
		else
			return QModelIndex ();
	}

	QModelIndex
	QNifModel::parent(const QModelIndex &index) const
	{
		if (!index.isValid ())
			return QModelIndex ();
		TREEITEM *childItem = static_cast<TREEITEM *>(index.internalPointer ());
		TREEITEM *parentItem = childItem->Parent;
		//TREEITEM *rootItem = win->App->AsTree ();
		if (parentItem == rn)
			return QModelIndex ();
		return createIndex (parentItem->Index, 0, parentItem);
	}

	int
	QNifModel::rowCount(const QModelIndex &parent) const
	{
		TREEITEM *parentItem;
		if (parent.column () > 0)
			return 0;
		//TREEITEM *rootItem = win->App->AsTree ();
		if (!parent.isValid ())
			parentItem = rn;
		else
			parentItem = static_cast<TREEITEM *>(parent.internalPointer ());
		
		win->App->ExpandNode (parentItem);// expand if needed

		return parentItem->Nodes.Count ();
	}

	int
	QNifModel::columnCount(const QModelIndex &parent) const
	{
		return headers.count ();
	}

	void
	QNifModel::SetRoot(NifLib::TreeNode<NifLib::Field *> *node)
	{
		if (node)
			rn = node;
	}
#undef TREEITEM
}
