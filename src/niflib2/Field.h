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

#ifndef __FIELD_H__
#define __FIELD_H__

#include "Buffer.h"
#include "Tag.h"
#include "niff.h"
#include "niflib.h"
#include <string>

namespace NifLib
{
	class Compiler;
	class Field
	{
		/*
		*	Return the type attribute.
		*	ATYPE, ANAME, ANIFLIBTYPE - whichever is available - in that order
		*/
		NifLib::Attr *Type();

		/*
		*	Converts TBASIC data value to string.
		*	Returns [ANIFLIBTYPE] or [Unknown.*] for undefined types.
		*/		
		std::string AsBasicTypeValue(NifLib::Tag *btype, Compiler *typesprovider);
	public:
		Field();
		Buffer Value;			// value
		NifLib::Tag *Tag;		// type and other attributes
		NifLib::Tag *BlockTag;	// file block tag
		NifLib::Field *JField;	// jagged array field, if this is jagged array
		int Arr1;// Arr1 size - needed for 2d arrays handling

		/*
		*	Return NifLib Type
		*/
		int NLType();

		/*
		*	Safe cast to NIFint at offset 0
		*/
		NIFint AsNIFint();

		/*
		*	Safe cast to NIFushort at offset 0
		*/
		NIFushort AsNIFushort();

		/*
		*	Safe cast to NIFbyte at offset 0
		*/
		NIFbyte AsNIFbyte();


		/*template <typename T> T AsInt(int idx)
		{
			// TODO: optimize, endianess
			if (Value.len <= 0)
				throw "Field::AsInt: can't convert";
			int qsize = sizeof(T);
			int ofs = idx * qsize;
			if (ofs + 1 > Value.len)
				throw "Field::AsInt: index out of ranage";
			int available = Value.len - ofs;
			if (available < qsize)
				qsize = available;
			if (qsize == 1)
				return (T)Value.buf[idx];
			else {
				if (MACHINE_ENDIANESS == FILE_ENDIANESS)
					memcpy (&T, Value.buf[ofs], qsize);
				else {
					// use union, etc. d[i] = s[qsize-i]
				}
			}
		}*/

		/*
		*	Returns a string representation of the field.
		*	Returns TENUM as ANAME.
		*	Returns unknown or unmanageable types in rectangular brackets: []
		*/
		std::string AsString(Compiler *typesprovider);

		/*
		*	Helper function. Returns the file block name this field belongs to.
		*	TODO: replace "std::string" with a "#define" somewhere
		*/
		std::string BlockName();

		/*
		*	Helper function. Returns the name of the field.
		*/
		std::string Name();

		/*
		*	Helper function. Returns the tag block name this field belongs to.
		*/
		std::string OwnerName();

		/*
		*	Helper function. Returns "true" if the field type is TENUM.
		*/
		bool IsEnum(Compiler *typesprovider);

		/*
		*	Helper function. Returns "true" if the field type is TCOMPOUND or
		*	TNIOBJECT.
		*/
		bool IsStruct(Compiler *typesprovider);

		/*
		*	Helper function. Returns the name of the enum if the field is
		*	TENUM.
		*/
		std::string AsEnumName(Compiler *typesprovider);

		bool IsArray1D();

		bool IsArray2D();

		bool IsArrayJ();

		bool IsCharArray();

		/*
		*	Returns one of the BTN_* types.
		*	NIFT_T if not known.
		*/
		int TypeId();

		/*
		*	Returns the field ATYPE, ANAME or ANIFLIBTYPE value.
		*	Whichever is present in the above-mentioned order:
		*	 '<add name="Value" type="char"'
		*	  - returns "char"
		*/
		std::string TagType();

		/*
		*	Returns field ATYPE value:
		*	type="Vector3", Vector3 Tag
		*/
		NifLib::Tag *TypeTag();

		/*
		*	Returns field tag attribute value as a string:
		*	TagAttr (ATYPE), "Vector3"
		*/
		std::string TagAttr(int attrId);

		/*
		*	Returns the size of an item in case an array of const size items
		*	is stored in the field.
		*	Will not work for (TypeTag->FixedSize > 0) fields.
		*/
		/*inline int ItemSize()
		{
			return NLType () & NIFT_SIZE;
		}*/

		/*
		*	Returns the size of the array in case an array of const size items
		*	is stored in the field.
		*/
		/*inline int ArraySize()
		{
			int s = ItemSize ();
			if (s)
				return Value.len / s;
			else
				return 0;
		}*/

		/*
		*	Returns this field fixed size, > 0 when its a fixed size field
		*/
		int FixedSize();

		virtual ~Field();
	};
}

#endif /*__FIELD_H__*/
