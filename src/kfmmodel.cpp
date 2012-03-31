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

#include "kfmmodel.h"
#include "ns_base.h"

KfmModel::KfmModel( QObject * parent ) : BaseModel( parent )
{
	clear();
}

/*bool KfmModel::loadXML()
{
	return false;
}*/

QString KfmModel::parseXmlDescription( const QString & filename )
{
	return QString();
}

QModelIndex KfmModel::getKFMroot() const
{
	return QModelIndex();
}

bool KfmModel::evalVersion( NifItem * item, bool chkParents ) const
{
	return false;
}

void KfmModel::clear()
{
}

bool KfmModel::updateArrayItem( NifItem * array, bool fast )
{
	return false;
}

void KfmModel::insertType( NifItem * parent, const NifData & data, int at )
{
}

bool KfmModel::setItemValue( NifItem * item, const NifValue & val )
{
	return false;
}

bool KfmModel::setHeaderString( const QString & s )
{
	return false;
}

bool KfmModel::load( QIODevice & device )
{
	return false;
}

bool KfmModel::save( QIODevice & device ) const
{
	return false;
}

bool KfmModel::load( NifItem * parent, NifIStream & stream, bool fast )
{
	return false;
}

bool KfmModel::save( NifItem * parent, NifOStream & stream ) const
{
	return false;
}

NifItem * KfmModel::insertBranch( NifItem * parentItem, const NifData & data, int at )
{
	return NULL;
}
