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

#include "nifmodel.h"
#include "niftypes.h"
#include "options.h"
#include "config.h"
#include "spellbook.h"

#include <QByteArray>
#include <QColor>
#include <QDebug>
#include <QFile>
#include <QTime>
#include <QSettings>
#include <QtEndian>

//! \file nifmodel.cpp NifModel implementation, NifModelEval

NifModel::NifModel(QObject *parent)
	: BaseModel (parent)
{
}

/*bool NifModel::loadXML()
{
	return false;	
}*/

QString NifModel::parseXmlDescription( const QString & filename )
{
	return QString();
}

bool
NifModel::evalVersion(NifItem *item, bool chkParents) const
{
	return false;
}

void
NifModel::clear()
{
}

NifItem *
NifModel::getFooterItem() const
{
	return NULL;
}

QModelIndex
NifModel::getFooter() const
{
	return QModelIndex ();
}

void
NifModel::updateFooter()
{
}

QModelIndex
NifModel::getHeader() const
{
	return QModelIndex ();
}

NifItem *
NifModel::getHeaderItem() const
{
	return NULL;
}

void
NifModel::updateHeader()
{
}

NifItem *
NifModel::getItem(NifItem *item, const QString &name) const
{
	return NULL;
}

bool
NifModel::updateByteArrayItem(NifItem *array, bool fast)
{
	return false;
}

bool
NifModel::updateArrayItem(NifItem *array, bool fast)
{
	return false;
}

bool
NifModel::updateArrays(NifItem *parent, bool fast)
{
	return false;
}

QModelIndex
NifModel::insertNiBlock(const QString &identifier, int at, bool fast)
{
	return QModelIndex();
}

void
NifModel::removeNiBlock(int blocknum)
{
}

void
NifModel::moveNiBlock(int src, int dst)
{
}

void NifModel::updateStrings(NifModel *src, NifModel* tgt, NifItem *item)
{
}

QMap<qint32,qint32> NifModel::moveAllNiBlocks( NifModel * targetnif, bool update )
{
	return QMap<qint32,qint32> ();
}

void NifModel::reorderBlocks( const QVector<qint32> & order )
{
}

void NifModel::mapLinks( const QMap<qint32,qint32> & map )
{
}

QString NifModel::getBlockName( const QModelIndex & idx ) const
{
	return QString ();
}

QString NifModel::getBlockType( const QModelIndex & idx ) const
{
	return QString ();
}

int NifModel::getBlockNumber( const QModelIndex & idx ) const
{
	return 0;
}

QModelIndex NifModel::getBlock( const QModelIndex & idx, const QString & id ) const
{
	return QModelIndex ();
}

QModelIndex NifModel::getBlockOrHeader( const QModelIndex & idx ) const
{
	return QModelIndex ();
}

int NifModel::getBlockNumber( NifItem * block ) const
{
	return 0;
}

QModelIndex NifModel::getBlock( int x, const QString & name ) const
{
	return QModelIndex();
}

bool NifModel::isNiBlock( const QModelIndex & index, const QString & name ) const
{
	return false;
}

NifItem * NifModel::getBlockItem( int x ) const
{
	return NULL;
}

int NifModel::getBlockCount() const
{
	return -1;
}

void NifModel::insertAncestor( NifItem * parent, const QString & identifier, int at )
{
}

bool NifModel::inherits( const QString & name, const QString & aunty )
{
	return false;
}

bool NifModel::inherits( const QModelIndex & idx, const QString & aunty ) const
{
	return false;
}

void NifModel::insertType( const QModelIndex & parent, const NifData & data, int at )
{
}

void NifModel::insertType( NifItem * parent, const NifData & data, int at )
{
}

bool NifModel::setItemValue( NifItem * item, const NifValue & val )
{
	return false;
}

QVariant NifModel::data( const QModelIndex & idx, int role ) const
{
	return QVariant();
}

bool NifModel::setData( const QModelIndex & index, const QVariant & value, int role )
{
	return false;
}

void NifModel::reset()
{
}

bool NifModel::removeRows( int row, int count, const QModelIndex & parent )
{
	return false;
}

bool NifModel::setHeaderString( const QString & s )
{
	return false;
}

bool
NifModel::load(QIODevice &device)
{
	return false;
}

bool
NifModel::save(QIODevice &device) const
{
	return false;
}

bool NifModel::load( QIODevice & device, const QModelIndex & index )
{
	return false;
}

bool NifModel::loadAndMapLinks( QIODevice & device, const QModelIndex & index, const QMap<qint32,qint32> & map )
{
	return false;
}

bool NifModel::loadHeaderOnly( const QString & fname )
{
	return false;
}

bool NifModel::earlyRejection( const QString & filepath, const QString & blockId, quint32 version )
{
	return false;
}

bool NifModel::save( QIODevice & device, const QModelIndex & index ) const
{
	return false;
}

int NifModel::fileOffset( const QModelIndex & index ) const
{
	return 0;
}

int NifModel::blockSize( const QModelIndex & index ) const
{
	return 0;
}

int NifModel::blockSize( NifItem * parent ) const
{
	return 0;
}

int NifModel::blockSize( NifItem * parent, NifSStream& stream ) const
{
	return 0;
}

bool
NifModel::load(NifItem *parent, NifIStream &stream, bool fast)
{
	return false;
}

bool NifModel::save( NifItem * parent, NifOStream & stream ) const
{
	return false;
}

bool NifModel::fileOffset( NifItem * parent, NifItem * target, NifSStream & stream, int & ofs ) const
{
	return false;
}

NifItem * NifModel::insertBranch( NifItem * parentItem, const NifData & data, int at )
{
	return NULL;
}

void NifModel::updateLinks( int block )
{
}

void NifModel::updateLinks( int block, NifItem * parent )
{
}

void NifModel::checkLinks( int block, QStack<int> & parents )
{
}

void NifModel::adjustLinks( NifItem * parent, int block, int delta )
{
}

void NifModel::mapLinks( NifItem * parent, const QMap<qint32,qint32> & map )
{
}

qint32 NifModel::getLink( const QModelIndex & index ) const
{
	return 0;
}

qint32 NifModel::getLink( const QModelIndex & parent, const QString & name ) const
{
	return 0;
}

QVector<qint32> NifModel::getLinkArray( const QModelIndex & iArray ) const
{
	return QVector<qint32> ();
}

QVector<qint32> NifModel::getLinkArray( const QModelIndex & parent, const QString & name ) const
{
	return QVector<qint32> ();
}

bool NifModel::setLink( const QModelIndex & parent, const QString & name, qint32 l )
{
	return false;
}

bool NifModel::setLink( const QModelIndex & index, qint32 l )
{
	return false;
}

bool NifModel::setLinkArray( const QModelIndex & iArray, const QVector<qint32> & links )
{
	return false;
}

bool NifModel::setLinkArray( const QModelIndex & parent, const QString & name, const QVector<qint32> & links )
{
	return false;
}

bool NifModel::isLink( const QModelIndex & index, bool * isChildLink ) const
{
	return false;
}

int NifModel::getParent( int block ) const
{
	return 0;
}

QString NifModel::string( const QModelIndex & index, bool extraInfo ) const
{
	return QString ();
}

QString NifModel::string( const QModelIndex & index, const QString & name, bool extraInfo ) const
{
	return QString ();
}

bool NifModel::assignString( const QModelIndex & index, const QString & string, bool replace)
{
	return false;
}

bool NifModel::assignString( NifItem * item, const QString & string, bool replace )
{
	return false;
}

bool NifModel::assignString( const QModelIndex & index, const QString & name, const QString & string, bool replace )
{
	return false;
}

void NifModel::convertNiBlock( const QString & identifier, const QModelIndex& index , bool fast )
{
}

bool NifModel::holdUpdates(bool value)
{
	return false;
}

void NifModel::updateModel( UpdateType value )
{
}
