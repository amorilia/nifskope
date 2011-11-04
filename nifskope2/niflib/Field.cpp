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
	NifLib::Attr *
	Field::Type()
	{
		NifLib::Attr *atype = Tag->AttrById (ATYPE);
		if (!atype)
			atype = Tag->AttrById (ANIFLIBTYPE);
		if (!atype)
			atype = Tag->AttrById (ANAME);
		return atype;
	}

	std::string
	Field::AsBasicTypeValue(NifLib::Tag *btype, Compiler *typesprovider)
	{
		// 0000 0000 - unsigned
		// 1000 0000 - signed
		// 0000 0000 - int
		// 0100 0000 - float
		// 0000 0000 - 0 bytes
		// 0000 0001 - 1 byte
		// 0000 0010 - 2 bytes
		// 0000 0100 - 4 bytes
		// 0000 1000 - 8 bytes, etc.
		std::stringstream result;
		NifLib::Attr *_ta = btype->AttrById (ANIFLIBTYPE);
		if (!_ta)
			result << "[UNKNOWN TBASIC ANIFLIBTYPE]";
		else if (_ta->Value.Equals ("HeaderString", 12) ||
			_ta->Value.Equals ("LineString", 10))
			result << std::string (Value.buf, Value.len - 1);
		else if (_ta->Value.Equals ("unsigned int", 12)||
			_ta->Value.Equals ("IndexString", 11))
			result << (unsigned int)*(unsigned int *)&Value.buf[0];
		else if (_ta->Value.Equals ("Ref", 3))
			result << "Ref:" << (int)*(int *)&Value.buf[0];
		else if (_ta->Value.Equals ("*", 1))
			result << "*:" << (int)*(int *)&Value.buf[0];
		else if (_ta->Value.Equals ("int", 3))
			result << (int)*(int *)&Value.buf[0];
		else if (_ta->Value.Equals ("byte", 4)) {
			if (Type ()->Value.Equals ("char", 4))
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
		else// TODO: this could be avoided if "nif.xml" specifies sizes
			result << "[NEW ANIFLIBTYPE: "
				<< std::string (_ta->Value.buf, _ta->Value.len) << "]";
		return result.str ();
	}

	Field::Field()
	{
		Tag = NULL;
		BlockTag = NULL;
		JField = NULL;
	}

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
		NifLib::Attr *ftype = Type ();
		NifLib::Tag *btype = typesprovider->GetBasicType (ftype);
		if (IsEnum (typesprovider))
			return AsEnumName (typesprovider);
		if (btype)
			return AsBasicTypeValue (btype, typesprovider);
		else {
			std::stringstream result;
			result << "[" << std::string (ftype->Value.buf, ftype->Value.len) << "]";
			return result.str ();
		}
		/*if (btype) {
			NifLib::Attr *_ta = btype->AttrById (ANIFLIBTYPE);
			if (!_ta)
				result << "[UNKNOWN]";
			else if (_ta->Value.Equals ("HeaderString", 12) ||
				_ta->Value.Equals ("LineString", 10))
				result << std::string (Value.buf, Value.len - 1);
			else if (_ta->Value.Equals ("unsigned int", 12)||
				_ta->Value.Equals ("IndexString", 11))
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
			else if (_ta->Value.Equals ("unsigned short", 14)) {
				unsigned short *buf = (unsigned short *)&Value.buf[0];
				if (JField) {// TODO: this repeats a lot
					// a trinagle strips index array looks like this
					NifLib::Tag *jt = JField->Tag;
					NifLib::Attr *jatype = Tag->AttrById (ATYPE);
					if (!jatype)
						jatype = jt->AttrById (ANIFLIBTYPE);
					NifLib::Tag *jbtype = typesprovider->GetBasicType (jatype);
					_ta = jbtype->AttrById (ANIFLIBTYPE);
					if (_ta && _ta->Value.Equals ("unsigned short", 14)) {
						unsigned short *len = (unsigned short *)&(JField->Value.buf[0]);
						int lsize = JField->Value.len / 2;
						int base = 0;
						for (int l = 0; l < lsize; l++) {
							result << std::endl;
							for (int i = base; i < base + len[l]; i++)
								result << "\t[" << i-base << "]=" << buf[i] << std::endl;
							base += len[l];
						}
					}
				} else {
					int size = Value.len / 2;					
					result << std::endl;
					for (int i = 0; i < size; i++)
						result << "\t[" << i << "]=" << buf[i] << std::endl;
				}
				//result << (unsigned short)*(unsigned short *)&Value.buf[0];
			}
			else if (_ta->Value.Equals ("short", 5))
				result << (short)*(short *)&Value.buf[0];
			else if (_ta->Value.Equals ("bool", 4))
				result << StreamBlockB (Value.buf, Value.len, Value.len + 1);
			else if (_ta->Value.Equals ("float", 5))
				result << (float)*(float *)&Value.buf[0];
			else result << "[" << std::string (_ta->Value.buf, _ta->Value.len) << "]";
		} else { // not a basic type
			if (atype->Value.Equals ("Vector3", 7)) {
				NIFfloat *buf = (NIFfloat *)&Value.buf[0];
				int size = Value.len / 3 / 4;// "Num Vertices"
				result << std::endl;
				for (int i = 0; i < size; i++)
					result << "\tv #" << i << ": ("
						<< buf[3*i] << ", " << buf[3*i+1] << ", " << buf[3*i+2]
						<< ")" << std::endl;
			}
			else if (atype->Value.Equals ("TexCoord", 8)) {
				NIFfloat *buf = (NIFfloat *)&Value.buf[0];
				int size = Value.len / 2 / 4;// "Num Vertices"
				result << std::endl;
				for (int i = 0; i < size; i++)
					result << "\ttc #" << i << ": ("
						<< buf[2*i] << ", " << buf[2*i+1] << ")" << std::endl;
			}
			else
				result << "[" << std::string (atype->Value.buf, atype->Value.len) << "]";
		}*/
	}

	/*
	*	Helper function, returns file block name
	*	TODO: is "std::string" good enough as a result type?
	*/
	std::string
	Field::BlockName()
	{
		return Compiler::TagName (BlockTag);
	}

	/*
	*	Helper function. Returns the name of the field.
	*/
	std::string
	Field::Name()
	{
		return Compiler::TagName (Tag);
	}

	/*
	*	Helper function. Returns the TCOMPUND or TNIOBJECT this field belongs to.
	*/
	std::string
	Field::OwnerName()
	{
		return Compiler::TagName (Tag->Owner);
	}

	/*
	*	Helper function. Returns "true" if the field type is TENUM.
	*/
	bool
	Field::IsEnum(Compiler *typesprovider)
	{
		NifLib::Attr *type = Type ();
		if (!type)
			return false;
		return (typesprovider->Find (TENUM, type->Value.buf, type->Value.len));
	}

	/*
	*	Helper function. Returns "true" if the field type is TCOMPOUND or TNIOBJECT.
	*/
	bool
	Field::IsStruct(Compiler *typesprovider)
	{
		NifLib::Attr *type = Type ();
		if (!type)
			return false;
		return (typesprovider->Find (TCOMPOUND, type->Value.buf, type->Value.len) ||
				typesprovider->Find (TNIOBJECT, type->Value.buf, type->Value.len));
	}

	/*
	*	Helper function. Returns the name of the enum if the field is TENUM.
	*/
	std::string
	Field::AsEnumName(Compiler *typesprovider)
	{
		NifLib::Attr *type = Tag->AttrById (ATYPE);
		if (!type)
			return std::string ("ERROR: field without ATYPE");
		NifLib::Tag *e = typesprovider->Find (TENUM, type->Value.buf, type->Value.len);
		if (e) { // its enum
			std::string value =
				AsBasicTypeValue (typesprovider->GetBasicType (Type ()), typesprovider);
			for (int i = 0; i < e->Tags.Count (); i++) {
				NifLib::Attr *v = e->Tags[i]->AttrById (AVALUE);
				if (!v)
					return std::string ("ERROR: TENUM TOPTION without AVALUE");
				if (std::string (v->Value.buf, v->Value.len) == value) {
					NifLib::Attr *n = e->Tags[i]->AttrById (ANAME);
					if (!n)
						return std::string ("ERROR: TENUM TOPTION without ANAME");
					return std::string (n->Value.buf, n->Value.len);
				}
			}
			return value;// name not found
		}
		return std::string ("ERROR: field is not TENUM");
	}
}
