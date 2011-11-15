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
			atype = Tag->AttrById (ANAME);
		if (!atype)
			atype = Tag->AttrById (ANIFLIBTYPE);
		return atype;
	}

	std::string
	Field::AsBasicTypeValue(NifLib::Tag *btype, Compiler *typesprovider)
	{
		// todo: turn into an array of methods
		std::stringstream result;
		NifLib::Attr *_ta = btype->AttrById (ANIFLIBTYPE);
		if (!_ta)
			result << "[UNKNOWN TBASIC ANIFLIBTYPE]";
		else if (NLType () & NIFT_T) {// can handle two NIFT_T
			if (NIFT(NLType (), BTN_HEADERSTRING) ||
				NIFT(NLType (), BTN_LINESTRING))
				result << std::string (Value.buf, Value.len - 1);// avoid '\n'
			else // unknown dynamic
				result << StreamBlockB (Value.buf, Value.len, Value.len + 1);
		} else if ((NLType () & BtnType (BTN_FLOAT)) == BtnType (BTN_FLOAT))
 			result << (NIFfloat)*(NIFfloat *)&Value.buf[0];
		else if (NIFT(NLType (), BTN_FILEVERSION)) {
			if ((Value.len == (NLType () & NIFT_SIZE)) && (Value.len > 0)) {
				result << (NIFint)Value.buf[Value.len-1];
				for (int i = Value.len - 2; i > -1; i--)
					result << "." << (NIFint)Value.buf[i];
			} else
				result << "[Unknown FILEVERSION type]";
		}
		else if (NIFT(NLType (), BTN_CHAR))
			result << std::string (Value.buf, Value.len).c_str ();
		else {
			if ((NLType () & NIFT_BT) == BtnType (BTN_UINT))
				result << (NIFuint)*(NIFuint *)&Value.buf[0];
			else if ((NLType () & NIFT_BT) == BtnType (BTN_USHORT))
				result << (NIFushort)*(NIFushort *)&Value.buf[0];
			else if ((NLType () & NIFT_BT) == BtnType (BTN_BYTE))
				result << (NIFint)*(NIFbyte *)&Value.buf[0];
			else if ((NLType () & NIFT_BT) == BtnType (BTN_INT))
				result << (NIFint)*(NIFint *)&Value.buf[0];
			else if ((NLType () & NIFT_BT) == BtnType (BTN_SHORT))
				result << (NIFshort)*(NIFshort *)&Value.buf[0];
			else
				result << "[" << std::string (_ta->Value.buf, _ta->Value.len) << "]";
		}
		return result.str ();
	}

	Field::Field()
	{
		Tag = NULL;
		BlockTag = NULL;
		JField = NULL;
	}

	int
	Field::NLType()
	{
		return Tag->NLType;
	}

	NIFuint
	Field::AsNIFuint()
	{
		// TODO: optimize
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
	}

	std::string
	Field::BlockName()
	{
		return Compiler::TagName (BlockTag);
	}

	std::string
	Field::Name()
	{
		return Compiler::TagName (Tag);
	}

	std::string
	Field::OwnerName()
	{
		return Compiler::TagName (Tag->Owner);
	}

	bool
	Field::IsEnum(Compiler *typesprovider)
	{
		NifLib::Attr *type = Type ();
		if (!type)
			return false;
		return (typesprovider->Find (TENUM, type->Value.buf, type->Value.len));
	}

	bool
	Field::IsStruct(Compiler *typesprovider)
	{
		NifLib::Attr *type = Type ();
		if (!type)
			return false;
		return (typesprovider->Find (TCOMPOUND, type->Value.buf, type->Value.len) ||
				typesprovider->Find (TNIOBJECT, type->Value.buf, type->Value.len));
	}

	std::string
	Field::AsEnumName(Compiler *typesprovider)
	{
		NifLib::Attr *type = Tag->AttrById (ATYPE);
		if (!type)
			return std::string ("ERROR: field without ATYPE");
		NifLib::Tag *e = typesprovider->Find (TENUM, type->Value.buf, type->Value.len);
		if (e) { // its enum
			// handles type
			std::string value =
				AsBasicTypeValue (typesprovider->GetBasicType (Type ()), typesprovider);
			for (int i = 0; i < e->Tags.Count (); i++) {
				NifLib::Attr *v = e->Tags[i]->AttrById (AVALUE);
				if (!v)
					return std::string ("ERROR: TENUM TOPTION without AVALUE");
				// TODO: TENUM AVALUE compare, TBITFLAGS in mind
				// WHEN: when started using field values extensively
				// those are sometimes "0x"
				std::stringstream c;
				c << std::hex << std::string (v->Value.buf, v->Value.len);
				unsigned int largest = 0;
				c >> largest;
				std::stringstream c2;
				c2 << largest;
				if (c2.str () == value) {
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

	bool
	Field::IsArray1D()
	{
		return (Tag->AttrById (AARR1) != NULL) &&
				(Tag->AttrById (AARR2) == NULL);
	}

	bool
	Field::IsArray2D()
	{
		return (Tag->AttrById (AARR1) != NULL) &&
				(Tag->AttrById (AARR2) != NULL);
	}

	bool
	Field::IsArrayJ()
	{
		return JField != NULL;
	}

	bool
	Field::IsCharArray()
	{
		return (Tag->AttrById (AARR1) != NULL) &&
				NIFT(NLType (), BTN_CHAR);
	}

	int
	Field::TypeId()
	{
		if (NLType () & NIFT_T)
			return NIFT_T;
		else
			return (NLType () & NIFT_ID) >> 8;
	}

	std::string
	Field::TagType()
	{
		return Type ()->ToString ();
	}

	NifLib::Tag *
	Field::TypeTag()
	{
		return Tag->TypeTag;
	}

	std::string
	Field::TagAttr(int attrId)
	{
		NifLib::Attr *a = Tag->AttrById (attrId);
		if (a)
			return a->ToString ();
		else
			return "";
	}

	int
	Field::FixedSize()
	{
		return Tag->TypeTag->FixedSize;
	}
}
