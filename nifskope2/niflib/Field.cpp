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

#include "Field.h"
#include <string>
#include <sstream>
#include "Compiler.h"
#include "Attr.h"
#include "niflib.h"

namespace NifLib
{
	NIFuint
	Field::AsNIFuint()
	{
		if (Value.len <= 0)
			throw "Field::AsNIFuint: can't convert";
		if (Value.len == 1)
			return *(NIFbyte *)(&Value.buf[0]);
		else if (Value.len == 2 || Value.len == 3)
			return *(NIFushort *)(&Value.buf[0]);
		else
			return *(NIFuint *)(&Value.buf[0]);
	}

	std::string
	Field::AsString(Compiler *typesprovider)
	{
		if (Value.len <= 0)
			return "[EMPTY]";
		NifLib::Attr *atype = Tag->AttrById (ATYPE);
		NifLib::Tag *btype = typesprovider->GetBasicType (atype);
		std::stringstream result;
		if (btype) {
			NifLib::Attr *_ta = btype->AttrById (ANIFLIBTYPE);
			if (!_ta)
				result << "[UNKNOWN]";
			else if (_ta->Value.Equals ("HeaderString", 12) ||
				_ta->Value.Equals ("LineString", 10))
				result << std::string (Value.buf, Value.len - 1);
			else if (_ta->Value.Equals ("unsigned int", 12))
				result << (unsigned int)*(unsigned int *)&Value.buf[0];
			else if (_ta->Value.Equals ("Ref", 3))
				result << "Ref:" << (int)*(int *)&Value.buf[0];
			else if (_ta->Value.Equals ("*", 1))
				result << "*:" << (int)*(int *)&Value.buf[0];
			else if (_ta->Value.Equals ("int", 3))
				result << (int)*(int *)&Value.buf[0];
			else if (_ta->Value.Equals ("byte", 4)) {
				if (atype->Value.Equals ("char", 4))
				result << std::string (Value.buf, Value.len);
				else
				result << (int)*(unsigned char *)&Value.buf[0];
			}
			else if (_ta->Value.Equals ("unsigned short", 14))
				result << (unsigned short)*(unsigned short *)&Value.buf[0];
			else if (_ta->Value.Equals ("short", 5))
				result << (short)*(short *)&Value.buf[0];
			else if (_ta->Value.Equals ("bool", 4))
				result << StreamBlockB (Value.buf, Value.len, Value.len + 1);
			else if (_ta->Value.Equals ("float", 5))
				result << (float)*(float *)&Value.buf[0];
			else result << std::string (_ta->Value.buf, _ta->Value.len);
		} else
			result << "[" << std::string (atype->Value.buf, atype->Value.len) << "]";
		return result.str ();
	}
}
