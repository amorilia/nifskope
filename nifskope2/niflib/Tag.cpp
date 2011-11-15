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

#include "Tag.h"
#include "niflib.h"
#include <cstdlib>

namespace NifLib
{
	Tag::Tag()
	{
		Id = -1;
		Name = -1;
		Owner = NULL;
		FixedSize = 0;
		NLType = NIFT_T;
		TypeTag = this;
	}

	bool
	Tag::AttrExists(int attrid)
	{
		for (int i = 0; i < Attr.Count (); i++)
			if (Attr[i]->Name == attrid)
				return true;
		return false;
	}

	NifLib::Attr *Tag::AttrById(int attrid)
	{
		for (int i = 0; i < Attr.Count (); i++)
			if (Attr[i]->Name == attrid)
				return Attr[i];
		return NULL;
	}

	Tag::~Tag()
	{
		for (int i = 0; i < Tags.Count (); i++)
			delete Tags[i];
		for (int i = 0; i < Attr.Count (); i++)
			delete Attr[i];
	}
}
