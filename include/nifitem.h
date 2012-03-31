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

#ifndef NIFITEM_H
#define NIFITEM_H

#include "nifvalue.h"
#include "nifexpr.h"

#include <QSharedData>
#include <QVector>

//! \file nifitem.h NifItem, NifBlock, NifData, NifSharedData

//! Shared data for NifData.
/**
 * See QSharedDataPointer for details on data sharing in Qt;
 * <a href="http://doc.trolltech.org/latest/shared.html">shared classes</a>
 * give pointer efficiency to classes.
 */
class NifSharedData : public QSharedData
{
	friend class NifData;

	NifSharedData(const QString &n, const QString &t, const QString & tt,
		const QString &a, const QString &a1, const QString &a2,
		const QString &c, quint32 v1, quint32 v2, bool abs)
		: QSharedData ()
	{
		name = n;
		type = t;
		temp = tt;
		arg = a;
		arr1 = a1;
		arr2 = a2;
		cond = c;
		ver1 = v1;
		ver2 = v2;
		condexpr = Expression (c);
		isAbstract = abs;
	}
	
	NifSharedData(const QString &n, const QString &t, const QString & tt,
		const QString &a, const QString &a1, const QString &a2,
		const QString &c, quint32 v1, quint32 v2, bool abs,
		const QString &vc, const QString &txt)
		: QSharedData ()
	{
		name = n;
		type = t;
		temp = tt;
		arg = a;
		arr1 = a1;
		arr2 = a2;
		cond = c;
		ver1 = v1;
		ver2 = v2;
		condexpr = Expression (c);
		isAbstract = abs;
		vercond = vc;
		verexpr = Expression (vc);
		text = txt;
	}
	
	NifSharedData(const QString &n, const QString &t)
		: QSharedData ()
	{
		name = n;
		type = t;
		ver1 = 0;
		ver2 = 0;
		isAbstract = false;
	}
	
	NifSharedData(const QString &n, const QString &t, const QString &txt)
		: QSharedData ()
	{
		name = n;
		type = t;
		ver1 = 0;
		ver2 = 0;
		text = txt;
		isAbstract = false;
	}
	
	NifSharedData()
		: QSharedData ()
	{
		ver1 = 0;
		ver2 = 0;
		isAbstract = false;
	}
	
	QString  name;
	QString  type;
	QString  temp;			// Template type
	QString  arg;
	QString  arr1;
	QString  arr2;
	QString  cond;
	quint32  ver1;
	quint32  ver2;
	QString  text;			// Description text
	Expression condexpr;	// Condition as an expression
	QString  vercond;
	Expression verexpr;		// Version condition as an expression
	bool isAbstract;
};

//! The data and NifValue stored by a NifItem
class NifData
{
public:
	NifData(const QString &name, const QString &type, const QString &temp,
		const NifValue &val, const QString &arg, const QString &arr1,
		const QString &arr2, const QString &cond, quint32 ver1, quint32 ver2,
		bool isAbstract = false)
	{
		d = new NifSharedData (name, type, temp, arg, arr1, arr2, cond, ver1,
			ver2, isAbstract);
		value = val;
	}

	NifData(const NifData &src)
	{
		d = new NifSharedData (src.name (), src.type (), src.temp (),
			src.arg (), src.arr1 (), src.arr2 (), src.cond (), src.ver1 (),
			src.ver2 (), src.isAbstract (), src.vercond (), src.text ());
		value = src.value;
	} 
	
	NifData(const QString &name, const QString &type = QString (),
		const QString &text = QString ())
	{
		d = new NifSharedData (name, type, text);
	}
	
	NifData()
	{
		d = new NifSharedData ();
	}

// Forwarding Property
#define FPROPERTY(GPFX, GNAME, SPFX, SNAME, TYPE, FIELD)\
	GPFX TYPE GNAME() const { return FIELD; }\
	SPFX SNAME(TYPE value) { FIELD=value; }

	FPROPERTY(inline, name, void, setName, const QString &, d->name)

	FPROPERTY(inline, type, void, setType, const QString &, d->type)

	FPROPERTY(inline, temp, void, setTemp, const QString &, d->temp)

	FPROPERTY(inline, arg, void, setArg, const QString &, d->arg)

	FPROPERTY(inline, arr1, void, setArr1, const QString &, d->arr1)

	FPROPERTY(inline, arr2, void, setArr2, const QString &, d->arr2)

	inline const QString &
	cond() const
	{
		return d->cond;
	}

	void
	setCond(const QString &cond)
	{
		d->cond = cond;
		d->condexpr = Expression (cond);
	}

	FPROPERTY(inline, ver1, void, setVer1, quint32, d->ver1)

	FPROPERTY(inline, ver2, void, setVer2, quint32, d->ver2)

	// The text description of the data.
	FPROPERTY(inline, text, void, setText, const QString &, d->text)

	// Get the condition attribute of the data, as an expression.
	inline const Expression &
	condexpr() const
	{
		return d->condexpr;
	}

	inline const QString &
	vercond() const
	{
		return d->vercond;
	}

	void
	setVerCond(const QString &cond)
	{
		d->vercond = cond;
		d->verexpr = Expression (cond);
	}

	// Get the version condition attribute of the data, as an expression.
	inline const Expression &
	verexpr() const
	{
		return d->verexpr;
	}

	FPROPERTY(inline, isAbstract, void, setAbstract, const bool &,
		d->isAbstract)

#undef FPROPERTY

protected:
	// The internal shared data
	QSharedDataPointer<NifSharedData> d;

public:
	// The value stored with the data
	NifValue value;
};

//! A block representing a niobject in XML.
struct NifBlock {
	QString id;				// Identifier
	QString ancestor;		// Ancestor
	QString text;			// Description text
	bool abstract;			// Abstract flag
	QList<NifData> types;	// Data present
};

//! An item which contains NifData
// It also contains NifData interface, so its not just a container.
class NifItem: public NifData
{
public:
	NifItem(NifItem *parent)
	{
		pNode = parent;
	}
	
	NifItem(const NifData &data, NifItem *parent)
		: NifData (data)
	{
		pNode = parent;
	}
	
	~NifItem()
	{
		qDeleteAll (nodes);
	}
	
	NifItem *
	parent() const
	{
		return pNode;
	}
	
	//! Return the row that this item is at
	int
	row() const
	{
		if (pNode)
			return pNode->nodes.indexOf (const_cast<NifItem*>(this));
		return 0;
	}
	
	//! Allocate memory to insert items
	/*!
	 * \param num The number of nodes to be inserted
	 */
	void
	prepareInsert(int num)
	{
		nodes.reserve (nodes.count () + num);
	}
	
	//! Insert data
	/*!
	 * \param data The data to insert
	 * \param at The position to insert at; append if not specified
	 * \return An item containing the inserted data
	 */
	NifItem *
	insert(const NifData &data, int at = -1)
	{
		NifItem *item = new NifItem (data, this);
		insert (item, at);
		return item;
	}
	
	//! Insert item
	/*!
	 * \param item The data to insert
	 * \param at The position to insert at; append if not specified
	 * \return The row the item was inserted at
	 */
	int
	insert(NifItem *item, int at = -1)
	{
		item->pNode = this;
		if (at < 0 || at > nodes.count ()) {
			nodes.append (item);
			return nodes.count () - 1;
		}
		else {
			nodes.insert (at, item);
			return at;
		}
	}

 	//! Extract item at row
 	/*!
 	* \param row The row to take the item from
 	* \return The item that was removed
 	*/
 	NifItem *
	extract(int row)
 	{
 		NifItem *item = itemAt (row);
 		if (item) {
 			nodes.remove (row);
 			item->pNode = 0;
 		}
 		return item;
 	}
	
	//! Remove item at row
	/*!
	 * \param row The row to remove the item from
	 */
	void
	remove(int row)
	{
		NifItem *item = itemAt (row);
		if (item) {
			nodes.remove (row);
			delete item;
		}
	}
	
	//! Remove several items
	/*!
	 * \param row The row to start from
	 * \param count The number of rows to delete
	 */
	void
	remove(int row, int count)
	{
		for (int c = row; c < (row + count); c++) {
			NifItem *item = itemAt (c);
			if (item)
				delete item;
		}
		nodes.remove (row, count);
	}
	
	//! Return the item at the specified row
	NifItem *
	itemAt(int row)
	{
		return nodes.value (row);
	}
	
	//! Return the item with the specified name
	NifItem *
	byName(const QString &name)
	{
		foreach (NifItem *item, nodes)
			if (item->name () == name)
				return item;
		return 0;
	}
	
	int
	count()
	{
		return nodes.count ();
	}
	
	void
	clear()
	{
		qDeleteAll (nodes);
		nodes.clear ();
	}
	
	//! Determine if this item is present in the specified version
	inline bool
	evalVersion(quint32 v)
	{
		return ((ver1 () == 0 || ver1 () <= v) &&
				(ver2 () == 0 || v <= ver2 ()));
	}
	
	//! Get the sub-items as an array
	template <typename T> QVector<T>
	getArray() const
	{
		QVector<T> array;
		foreach (NifItem *item, nodes)
			array.append (item->value.get<T> ());
		return array;
	}
	
	//! Set the sub-items from an array
	template <typename T> void
	setArray(const QVector<T>  &array)
	{
		int x = 0;
		foreach (NifItem *item, nodes)
			item->value.set<T> (array.value (x++));
	}

private:
	NifItem *pNode;		// The parent of this item
	QVector<NifItem *> nodes;
};

#endif
