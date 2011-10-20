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

#ifndef __NIFLIB_H__
#define __NIFLIB_H__

/*
*	Core tags - the ones that form structures and types.
*	Make sure (length(TAGS[i % 2 == 0])) == (length(TAGS[i+1]) - 2).
*	TAGSL[i] == length(TAGS[i % 2 == 0])
*/
#define TAGS_NUM 6
char const * const TAGS[] = {
	"<basic"   , "</basic>",
	"<version" , "</version>",
	"<enum"    , "</enum>",
	"<compound", "</compound>",
	"<niobject", "</niobject>",
	"<bitflags", "</bitflags>"
};
int const TAGSL[] = {6, 8, 4, 9, 9, 9};

#include <iostream>

#define INFO(MSG)\
{\
	std::cout << MSG << std::endl;\
}

#define ERR(MSG)\
{\
	std::cout << "err: " << MSG << std::endl;\
}

void *NifAlloc(size_t size);

void NifRelease(void *ptr);

/*
*	debug: print "len" from "buf" as ASCII
*/
void PrintBlockA(char *buf, int len);
/*
*	debug: print "len" from "buf" as Bytes in "col"-umns
*/
void PrintBlockB(char *buf, int len, int col);
/*
*	debug: write "buf" "len" bytes to file named "fname"
*/
void WriteToFile(char *buf, int len, const char *fname);

#endif /*__NIFLIB_H__*/
