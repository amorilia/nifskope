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

#include "ns_base.h"

#include "basemodel.h"
#include "niftypes.h"
#include "options.h"

#include <QByteArray>
#include <QColor>
#include <QFile>
#include <QTime>

//! \file basemodel.cpp BaseModel and BaseModelEval

BaseModel::BaseModel( QObject * parent ) : QAbstractItemModel( parent )
{
}

BaseModel::~BaseModel()
{
}

void BaseModel::msg( const Message & m ) const
{
}

bool BaseModel::isArray( const QModelIndex & index ) const
{
	return false;
}
 
int BaseModel::getArraySize( NifItem * array ) const
{
	return 0;
}

int BaseModel::evaluateString( NifItem * array, const QString & text ) const
{
	return 0;
}

bool BaseModel::updateArray( const QModelIndex & array )
{
	return false;
}

bool BaseModel::updateArray( const QModelIndex & parent, const QString & name )
{
	return false;
}

QString BaseModel::itemName( const QModelIndex & index ) const
{
	return QString();
}

QString BaseModel::itemType( const QModelIndex & index ) const
{
	return QString();
}

QString BaseModel::itemTmplt( const QModelIndex & index ) const
{
	return QString();
}

NifValue BaseModel::getValue( const QModelIndex & index ) const
{
	return NifValue();
}

QString BaseModel::itemArg( const QModelIndex & index ) const
{
	return QString();
}

QString BaseModel::itemArr1( const QModelIndex & index ) const
{
	return QString();
}

QString BaseModel::itemArr2( const QModelIndex & index ) const
{
	return QString();
}

QString BaseModel::itemCond( const QModelIndex & index ) const
{
	return QString();
}

quint32 BaseModel::itemVer1( const QModelIndex & index ) const
{
	return 0;
}

quint32 BaseModel::itemVer2( const QModelIndex & index ) const
{
	return 0;
}

QString BaseModel::itemText( const QModelIndex & index ) const
{
	return QString();
}


bool BaseModel::setValue( const QModelIndex & index, const NifValue & val )
{
	return false;
}

bool BaseModel::setValue( const QModelIndex & parent, const QString & name, const NifValue & val )
{
	return false;
}

QModelIndex BaseModel::index( int row, int column, const QModelIndex & parent ) const
{
	return QModelIndex();
}

QModelIndex BaseModel::parent( const QModelIndex & child ) const
{
	return QModelIndex();
}

int BaseModel::rowCount( const QModelIndex & parent ) const
{
	return 0;
}

QVariant BaseModel::data( const QModelIndex & index, int role ) const
{
	return QVariant();
}

bool BaseModel::setData( const QModelIndex & index, const QVariant & value, int role )
{
	return false;
}

QVariant BaseModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
	return QVariant();
}

Qt::ItemFlags BaseModel::flags( const QModelIndex & index ) const
{
	return Qt::ItemFlags ();
}

bool BaseModel::loadFromFile( const QString & filename )
{
	return false;
}

bool BaseModel::saveToFile( const QString & filename ) const
{
	return false;
}
 
NifItem * BaseModel::getItem( NifItem * item, const QString & name ) const
{
	return 0;
}

NifItem * BaseModel::getItemX( NifItem * item, const QString & name ) const
{
	return 0;
}

QModelIndex BaseModel::getIndex( const QModelIndex & parent, const QString & name ) const
{
	return QModelIndex();
}

bool BaseModel::evalCondition( NifItem * item, bool chkParents ) const
{
	return false;
}

bool BaseModel::evalConditionHelper( NifItem * item, const QString & cond ) const
{	
	return false;
}

bool BaseModel::evalVersion( const QModelIndex & index, bool chkParents ) const
{
	return false;
}

bool BaseModel::evalCondition( const QModelIndex & index, bool chkParents ) const
{
	return false;
}
