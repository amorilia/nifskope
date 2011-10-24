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

#include "Parser.h"
#include "niff.h"
#include "Tag.h"
#include "Attr.h"
#include "List.h"
#include "Field.h"
#include <string>
#include <sstream>

namespace NifLib
{
	class Compiler: public Parser
	{
		NifLib::List<NifLib::Field *> flist;
		NIFuint nVersion;// global nif file variable
		int	Read_bool (NifStream *s, char *b);
		void AddField(NifLib::Tag *field, char *buf, int bl);
		template <typename T> static T str2(const std::string &val)
		{
			std::stringstream aa;
			aa << val;
			T k;
			aa >> k;
			return k;
		}
		int Evaluate(NifLib::Attr* cond);
		int EvalDeduceType(const char *val, int len);
		int EvaluateL2(NifLib::List<NIFuint> &l2);

		/*
		*	Starts from current count-1 and scans untill the first field.
		*	Returns NULL when not found.
		*/
		NifLib::Field *FFBackwards(int attrid, const char *val, int len);
	public:
		Compiler(const char *fname);
		~Compiler();

		static NIFuint HeaderString2Version(const char *buf, int bl);
		void ReadNif(const char *fname);
		bool V12Check(NifLib::Tag *field);
		NifLib::Tag *Find(int tagid, int attrid, const char *attrvalue, int len);

		void Build();
	};
}

#endif /*__COMPILER_H__*/
