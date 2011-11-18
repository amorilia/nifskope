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

#include "nifskope.h"

namespace NifSkopeQt4
{

	// TODO: handle NULL

	QVariant QNifModel::CId(NifLib::Node *node)
	{
		if (dummy->find (node) != dummy->end ())
			return QVariant (QString ("%0").arg ((*dummy)[node]));
		else
			return QVariant (QString ("-1"));
	}

	QVariant
	QNifModel::CName(NifLib::Node *node)
	{
		return QVariant (QString (node->Value->Name ().c_str ()));
	}

	QVariant
	QNifModel::CType(NifLib::Node *node)
	{
		if (node->Parent == win->App->AsTree ())
			return QVariant (QString ("NiBlock"));
		else
			return QVariant (QString (node->Value->TagType ().c_str ()));
	}

	QVariant
	QNifModel::CValue(NifLib::Node *node)
	{
		if (node->Parent == win->App->AsTree ())
			return QVariant (QString (
				win->App->GetNodeValue (node).c_str ()));
		NifLib::Field *f = node->Value;
		if (f->IsArray1D ()) {
			if (f->IsArrayJ ())
				return QVariant (QString ("[1D J arr]"));
			if (f->IsCharArray ())
				return QVariant (QString (win->App->ToStr (f).c_str ()));
			return QVariant (QString ("[1D arr]"));
		} else
		if (f->IsArray2D ()) {
			if (f->IsArrayJ ())
				return QVariant (QString ("[2D J arr]"));
			return QVariant (QString ("[2D arr]"));
		} else
			return QVariant (QString (win->App->ToStr (f, node).c_str ()));
	}

	QVariant
	QNifModel::CArgument(NifLib::Node *node)
	{
		return QVariant (QString (node->Value->TagAttr (AARG).c_str ()));
	}

	QVariant
	QNifModel::CArray1(NifLib::Node *node)
	{
		return QVariant (QString (node->Value->TagAttr (AARR1).c_str ()));
	}

	QVariant
	QNifModel::CArray2(NifLib::Node *node)
	{
		return QVariant (QString (node->Value->TagAttr (AARR2).c_str ()));
	}

	QVariant
	QNifModel::CCondition(NifLib::Node *node)
	{
		return QVariant (QString (node->Value->TagAttr (ACOND).c_str ()));
	}

	QVariant
	QNifModel::CSince(NifLib::Node *node)
	{
		return QVariant (QString (node->Value->TagAttr (AVER1).c_str ()));
	}

	QVariant
	QNifModel::CUntil(NifLib::Node *node)
	{
		return QVariant (QString (node->Value->TagAttr (AVER2).c_str ()));
	}

	QVariant
	QNifModel::CVersionCondition(NifLib::Node *node)
	{
		return QVariant (QString (node->Value->TagAttr (AVERCOND).c_str ()));
	}

	QNifModel::QNifModel(Qt4MainWindow *data, QObject *parent)
		: QAbstractItemModel (parent)
	{
		dummy = new std::map<NifLib::Node *,int> ();
		// TODO: can this become "what is available" instead of
		// "what should be available"? A.k.a. dynamic columns.
		cols << Column ("N", &QNifModel::CId)
		<< Column ("Name", &QNifModel::CName)
		<< Column ("Type", &QNifModel::CType)
		<< Column ("Value", &QNifModel::CValue)
		<< Column ("Argument", &QNifModel::CArgument)
		<< Column ("Array1", &QNifModel::CArray1)
		<< Column ("Array2", &QNifModel::CArray2)
		<< Column ("Condition", &QNifModel::CCondition)
		<< Column ("Since", &QNifModel::CSince)
		<< Column ("Until", &QNifModel::CUntil)
		<< Column ("Version Condition", &QNifModel::CVersionCondition);
		win = data;
		rn = win->App->AsTree ();
	}

   	QNifModel::~QNifModel()
	{
		dummy->clear();
		delete dummy;
	}

	QVariant
	QNifModel::data(const QModelIndex &index, int role) const
	{
		if (!index.isValid ())
			return QVariant ();
		if (role != Qt::DisplayRole)
			return QVariant ();
		NifLib::Node *item = static_cast<NifLib::Node *>(index.internalPointer ());
		int ci = index.column ();
		if (ci >= 0 && ci < cols.count ())
			return (((QNifModel *const)this)->*cols[ci].Format) (item);
		else
			return QVariant ("[ERROR: unknown column requested]");
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
			return cols[section].HeaderName;
		return QVariant ();
	}

	QModelIndex
	QNifModel::index(int row, int column, const QModelIndex &parent) const
	{
 		if (!hasIndex (row, column, parent))
        	return QModelIndex ();
		NifLib::Node *parentItem;
		if (!parent.isValid ())
			parentItem = rn;
		else
			parentItem = static_cast<NifLib::Node *>(parent.internalPointer ());
		// It requests row 0 when Nodes.Count () is 0
		if (row >= parentItem->Nodes.Count () || row < 0)
			return QModelIndex ();
		NifLib::Node *childItem = parentItem->Nodes[row];
		if (childItem) {
			(*dummy)[childItem] = row;
			return createIndex (row, column, childItem);
		}
		else
			return QModelIndex ();
	}

	QModelIndex
	QNifModel::parent(const QModelIndex &index) const
	{
		if (!index.isValid ())
			return QModelIndex ();
		NifLib::Node *childItem = static_cast<NifLib::Node *>(index.internalPointer ());
		NifLib::Node *parentItem = childItem->Parent;
		if (parentItem == rn)
			return QModelIndex ();

		if (dummy->find (parentItem) != dummy->end ())
			return createIndex ((*dummy)[parentItem], 0, parentItem);
		else
			return QModelIndex ();
	}

	int
	QNifModel::rowCount(const QModelIndex &parent) const
	{
		NifLib::Node *parentItem;
		if (parent.column () > 0)
			return 0;
		if (!parent.isValid ())
			parentItem = rn;
		else
			parentItem = static_cast<NifLib::Node *>(parent.internalPointer ());
		win->App->ExpandNode (parentItem);// expand if needed
		return parentItem->Nodes.Count ();
	}

	int
	QNifModel::columnCount(const QModelIndex &parent) const
	{
		return cols.count ();
	}

	void
	QNifModel::SetRoot(NifLib::Node *node)
	{
		if (node) {
			rn = node;
			dummy->clear();
		}
	}
}
