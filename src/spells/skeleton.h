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

#ifndef SPELL_SKELETON_H
#define SPELL_SKELETON_H

#include <QDialog>
#include <QCheckBox>

//! \file skeleton.h SkinPartitionDialog

class QSpinBox;

//! Dialog box for skin partitions
class SkinPartitionDialog : public QDialog
{
	Q_OBJECT
public:
	//! Constructor
	SkinPartitionDialog( int maxInfluences );
	
	//! Returns the value of spnPart
	int maxBonesPerPartition();
	//! Returns the value of spnVert
	int maxBonesPerVertex();
	//! Returns the value of ckTStrip
	bool makeStrips();
	//! Returns the value of ckPad
	bool padPartitions();
	
protected slots:
	//! Sets the minimum value of spnPart to the value of spnVert
	void changed();
	
protected:
	//! The number of bones per partition
	QSpinBox * spnPart;
	//! The number of bones per vertex
	QSpinBox * spnVert;
	//! Whether strips should be made
	QCheckBox * ckTStrip;
	//! Whether padding should be used
	QCheckBox * ckPad;
	
	//! The maximum number of influences; unused?
	int maxInfluences;
};

#endif
