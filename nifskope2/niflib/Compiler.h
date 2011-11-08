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

#ifndef __COMPILER_H__
#define __COMPILER_H__

#include "niflib.h"
#include "Parser.h"
#include "niff.h"
#include "Tag.h"
#include "Attr.h"
#include "List.h"
#include "Field.h"
#include <string>
#include <sstream>
#include <map>
#include "TreeNode.h"

namespace NifLib
{
	class Compiler: public Parser
	{
		int POS;
		NifLib::Tag *blockTag;
		NifLib::Field *i2j;// jagged array
		//
		NifLib::Attr *ARG;
		NifLib::Attr *TEMPLATE;
		NifLib::List<NifLib::Field *> flist;// core data
		NifLib::TreeNode<NifLib::Field *> ftree;

		NifLib::Field *fVersion;
		NifLib::Field *fUserVersion;
		NifLib::Field *fUserVersion2;

		std::map<std::string, NIFuint> strversion_cache;
		std::map<std::string, int> tagnames_cache[TAGS_NUML1];
		std::map<std::string, int> btypes_cache;

		/*
		*	A "view" for the ANAME attribute - fast lookup by ANAME.
		*	"FFBackwards" returns the last in the list and
		*	"AddField" adds newest field index there, so
		*	"FFBackwards" always returns the first found by that ANAME
		*	from flist.Count () - 1 backwards (towards 0).
		*	That is required because of AINHERIT. A field in "derived"
		*	may depend on a field in "parent" or "parent" "parent", etc.
		*	so instead of tracing back AINHERIT in a complex tree-like
		*	structures, I use a "view" to a "list" thus effectively
		*	detaching the core data container structure from "algorithms".
		*/
		std::map<std::string, NifLib::List<int> *> fview_aname;

		NIFuint nVersion, nUserVersion, nUserVersion2;// global nif file variable(s)

		NifLib::Field *AddField(NifLib::Tag *field, char *buf, int bl, int type);
		NifLib::TreeNode<NifLib::Field *> *AddNode(
			NifLib::Tag *t,
			NifLib::Field *f,
			NifLib::TreeNode<NifLib::Field *> *pnode);

		/*
		*	Starts from current count-1 and scans untill the first field.
		*	Returns NULL when not found.
		*/
		NifLib::Field *FFBackwards(const char *val, int len);
		NifLib::Field *FFVersion(const char *val, int len);
		int	FFBackwardsIdx(int attrid, const char *val, int len);

		/*
		*	Clear the view. Use before each block read.
		*/
		void Reset_FieldViewAName();

		bool V12Check(NifLib::Tag *field);
		
		template <typename T> static T str2(const std::string &val)
		{
			std::stringstream aa;
			aa << val;
			T k;
			aa >> k;
			return k;
		}
		inline bool IsUInt(const char *buf, int len)
		{
			if (len <= 0)
				return false;
			int x;
			for (x = 0; x < len; x++)
				if (buf[x] < '0' || buf[x] > '9' )
					return false;
			return true;
		}
		int BType(const char *buf, int bl);
		int Evaluate(NifLib::Attr* cond);
		int EvalDeduceType(const char *val, int len);
		int EvaluateL2(NifLib::List<NIFuint> &l2);

		/*
		*	Initialise AARR attribute. Set *i2j to the array field
		*	if any, to indicate jagged array
		*/
		NIFint InitArr(NifLib::Attr *aarr);

		/*
		*	Read one object from the .nif
		*/
		int ReadObject(NifStream &s, NifLib::Tag *t,
			NifLib::TreeNode<NifLib::Field *> *n);
		int ReadNifBlock(int i, NifStream &s, const char *n, int nlen);
	public:
		Compiler(const char *fname);
		~Compiler();
		void Reset();

		NifLib::Field *operator[](int index);
		int FCount();

		NIFuint HeaderString2Version(const char *buf, int bl);

		/*
		*	Returns TBASIC for that ATYPE attribute if there is TBASIC
		*	reachable for it.
		*/
		NifLib::Tag *GetBasicType(NifLib::Attr *type);

		/*
		*	Returns tag "tagid" with ANAME "attrvalue".
		*	Returns NULL if there is no such tag.
		*/
		NifLib::Tag *Find(int tagid, const char *attrvalue, int len);

		int ReadNif(const char *fname);
		void WriteNif(const char *fname);

		void Build();

		void DbgPrintFields();

		void PrintNode(NifLib::TreeNode<NifLib::Field *> *node, std::string ofs);

		/*
		*	Returns tag ANAME attribute as a string
		*/
		static std::string TagName(NifLib::Tag *tag);

		NifLib::TreeNode<NifLib::Field *> *AsTree();
	};
}

#endif /*__COMPILER_H__*/
