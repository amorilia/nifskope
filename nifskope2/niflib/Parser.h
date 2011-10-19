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

#ifndef __PARSER_H__
#define __PARSER_H__

namespace NifLib
{
	class Parser
	{
		char *gbuf;
		int size;
		/*
		*	"Turn" "nif.xml" into objects
		*/
		void Process(char *buf, int buflen);
	public:
		Parser(const char *fname);
		/*
		*	Loads a file "fname" in a buffer "*buf".
		*	Allocates "*buf" and specifies its size in "*size".
		*	Returns 0 on failure, 1 - otherwise.
		*/
		static int LoadFile(const char *fname, char **buf, int *size);
		/*
		*	Returns true if "buf" starts with "q"
		*/
		static int StartsWith(const char *q, int qlen, const char *buf, int buflen);
		/*
		*	Finds first occurrence of "q" and returns its start index
		*	relative to "buf".
		*	Returns "buflen" on failure.
		*/
		static int FindFirst(const char *q, int qlen, const char *buf, int buflen);
		/*
		*	Find a block what starts with "a" and ends with "b" in "buf".
		*	Handles nested blocks:
		*   "a1.b1b1" - a="a1", b="b1" will return 0, blcklen=5
		*   "a1a1.b1b1" - a="a1", b="b1" will return 0, blcklen=9
		*   "a1a1.b1" - a="a1", b="b1" will return 2, blcklen=5
		*	Returns its starting index relative to "buf".
		*	Returns its length, including "a' and "b", in "blcklen".
		*	Returns "buflen" on failure.
		*/
		static int Find(const char *a, int alen, const char *b, int blen, char *buf, int buflen, int *blcklen);
	};
}

#endif /*__PARSER_H__*/
