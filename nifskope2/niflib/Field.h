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

#ifndef __FIELD_H__
#define __FIELD_H__

#include "Buffer.h"
#include "Tag.h"
#include "niff.h"
#include <string>

namespace NifLib
{
	class Compiler;
	class Field
	{
		NifLib::Attr *Type();
		std::string AsBasicTypeValue(NifLib::Tag *btype, Compiler *typesprovider);
	public:
		Field();
		Buffer Value;			// value
		NifLib::Tag *Tag;		// type and other attributes
		NifLib::Tag *BlockTag;	// file block tag
		NifLib::Field *JField;	// jagged array field, if this is jagged array
		NIFuint AsNIFuint();
		int NLType;// NifLib type

		/*
		*	Returns a string representation of the field.
		*	Can be enormous in case of arrays.
		*/
		std::string AsString(Compiler *typesprovider);

		/*
		*	Helper function. Returns the file block name this feild belongs to.
		*	TODO: is "std::string" good enough as a result type?
		*/
		std::string BlockName();

		/*
		*	Helper function. Returns the name of the field.
		*/
		std::string Name();

		/*
		*	Helper function. Returns the tag block name this feild belongs to.
		*/
		std::string OwnerName();

		/*
		*	Helper function. Returns "true" if the field type is TENUM.
		*/
		bool IsEnum (Compiler *typesprovider);

		/*
		*	Helper function. Returns "true" if the field type is TCOMPOUND or TNIOBJECT.
		*/
		bool IsStruct(Compiler *typesprovider);

		/*
		*	Helper function. Returns the name of the enum if the field is TENUM.
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
		*	Returns field tag type
		*/
		std::string TagType();

		/*
		*	Returns field tag attribute
		*/
		std::string TagAttr(int attrId);
	};
}

#endif /*__FIELD_H__*/
