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
	QNifBlockModel::QNifBlockModel(Qt4MainWindow *data, QObject *parent)
		: QNifModel (data, parent)
	{
		cols.clear ();
		cols << Column ("N", &QNifBlockModel::CId)
		<< Column ("Name", &QNifBlockModel::CName)
		<< Column ("Value", &QNifBlockModel::CValue);
		Tree = false;
	}

	int
	QNifBlockModel::rowCount(const QModelIndex &parent) const
	{
		if (!rn)
			return 0;
		if (!Tree && parent.column () > 0)
			return 0;
		else
			return QNifModel::rowCount (parent);
		/*if (!parent.isValid ())
			return rn->Nodes.Count ();
		else
			return 0;*/
	}

	QNifBlockModel::~QNifBlockModel()
	{
	}
}
