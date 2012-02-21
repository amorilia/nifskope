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

#include "misc.h"

#include <QDebug>

// Brief description is deliberately not autolinked to class Spell
/*! \file misc.cpp
 * \brief Miscellaneous helper spells
 *
 * All classes here inherit from the Spell class.
 */

//! Update an array if eg. the size has changed
class spUpdateArray : public Spell
{
public:
	QString name() const { return Spell::tr( "Update" ); }
	QString page() const { return Spell::tr( "Array" ); }
	QIcon icon() const { return  QIcon( ":/img/update" ); }
	bool instant() const { return true; }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		if ( nif->isArray(index) && nif->evalCondition( index ) )
		{
			//Check if array is of fixed size
			NifItem * item = static_cast<NifItem*>( index.internalPointer() );
			bool static1 = true;
			bool static2 = true;

			if ( item->arr1().isEmpty() == false )
			{
				item->arr1().toInt( &static1 );
			}

			if ( item->arr2().isEmpty() == false )
			{
				item->arr2().toInt( &static2 );
			}

			//Leave this commented out until a way for static arrays to be initialized to the right size is created.
			//if ( static1 && static2 )
			//{
			//	//Neither arr1 or arr2 is a variable name
			//	return false;
			//}

			//One of arr1 or arr2 is a variable name so the array is dynamic
			return true;
		}

		return false;
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		nif->updateArray( index );
		return index;
	}
};

REGISTER_SPELL( spUpdateArray )

//! Updates the header of the NifModel
class spUpdateHeader : public Spell
{
public:
	QString name() const { return Spell::tr( "Update" ); }
	QString page() const { return Spell::tr( "Header" ); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return ( nif->getHeader() == nif->getBlockOrHeader( index ) );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		nif->updateHeader();
		return index;
	}
};

REGISTER_SPELL( spUpdateHeader )

//! Updates the footer of the NifModel
class spUpdateFooter : public Spell
{
public:
	QString name() const { return Spell::tr( "Update" ); }
	QString page() const { return Spell::tr( "Footer" ); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return ( nif->getFooter() == nif->getBlockOrHeader( index ) );
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		nif->updateFooter();
		return index;
	}
};

REGISTER_SPELL( spUpdateFooter )

//! Follows a link
class spFollowLink : public Spell
{
public:
	QString name() const { return Spell::tr( "Follow Link" ); }
	bool instant() const { return true; }
	QIcon icon() const { return  QIcon( ":/img/link" ); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return nif->isLink( index ) && nif->getLink( index ) >= 0;
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QModelIndex idx = nif->getBlock( nif->getLink( index ) );
		if ( idx.isValid() )
			return idx;
		else
			return index;
	}
};

REGISTER_SPELL( spFollowLink )

//! Estimates the file offset of an item in a model
class spFileOffset : public Spell
{
public:
	QString name() const { return Spell::tr( "File Offset" ); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return nif && index.isValid();
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		int ofs = nif->fileOffset( index );
		qWarning( QString( "estimated file offset is %1 (0x%2)" ).arg( ofs ).arg( ofs, 0, 16 ).toAscii() );
		return index;
	}
};

REGISTER_SPELL( spFileOffset )

// definitions for spCollapseArray moved to misc.h
bool spCollapseArray::isApplicable( const NifModel * nif, const QModelIndex & index )
{
	if ( nif->isArray(index) && nif->evalCondition( index ) && index.isValid() &&
			( nif->getBlockType( index ) == "Ref" || nif->getBlockType( index ) == "Ptr" ) )
	{
		// copy from spUpdateArray when that changes
		return true;
	}
	return false;
}

QModelIndex spCollapseArray::cast( NifModel * nif, const QModelIndex & index )
{
	nif->updateArray( index );
	// There's probably an easier way of doing this hiding in NifModel somewhere
	NifItem * item = static_cast<NifItem*>( index.internalPointer() );
	QModelIndex size = nif->getIndex( nif->getBlock( index.parent() ), item->arr1() );
	QModelIndex array = static_cast<QModelIndex>( index );
	return numCollapser( nif, size, array );
}

QModelIndex spCollapseArray::numCollapser( NifModel * nif, QModelIndex &iNumElem, QModelIndex &iArray )
{
	if ( iNumElem.isValid() && iArray.isValid() )
	{
		QVector<qint32> links;
		for ( int r = 0; r < nif->rowCount( iArray ); r++ )
		{
			qint32 l = nif->getLink( iArray.child( r, 0 ) );
			if ( l >= 0 ) links.append( l );
		}
		if ( links.count() < nif->rowCount( iArray ) )
		{
			nif->set<int>( iNumElem, links.count() );
			nif->updateArray( iArray );
			nif->setLinkArray( iArray, links );
		}
	}
	return iArray;
}

REGISTER_SPELL( spCollapseArray )

