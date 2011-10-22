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

#include <cstdlib>
#include <string.h>

/*
*	Core tags - the ones that form structures and types.
*	Make sure (length(TAGS[i % 2 == 0])) == (length(TAGS[i+1]) - 2).
*	TAGSL[i] == length(TAGS[i % 2 == 0])
*
*	Changing them or adding new ones will require rebuild one way or another
*	so these tables should be updated too.
*/
#define TAGS_NUML1 6
#define TAGS_NUML2 2
char const * const TAGS[] = {
	"<basic"   , "</basic>"   ,
	"<version" , "</version>" ,
	"<enum"    , "</enum>"    ,
	"<compound", "</compound>",
	"<niobject", "</niobject>",
	"<bitflags", "</bitflags>",

	"<option"  , "</option>"  ,
	"<add"     , "</add>"
};
int const TAGSL[] = {6, 8, 5, 9, 9, 9, 7, 4};

#define ATTR_NUM 22
char const * const ATTR[] = {
	"abstract",
	"arg",
	"arr1",
	"arr2",
	"arr3",
	"cond",
	"count",
	"default",
	"inherit",
	"istemplate",
	"name",
	"niflibtype",
	"nifskopetype",
	"num",
	"storage",
	"template",
	"type",
	"userver",
	"value",
	"ver1",
	"ver2",
	"vercond",
	"version"
};
int const ATTRL[] =
	{8, 3, 4, 4, 4, 4, 5, 7, 7, 9, 4, 10, 12, 3, 7, 8, 4, 7, 5, 4, 4, 7, 7};

inline int
AttrId(const char *buf, int bl)
{
	int i;
	for (i = 0; i < ATTR_NUM; i++)
		if (ATTRL[i] <= bl)
			if (strncmp (buf, ATTR[i], ATTRL[i]) == 0)
				return i;
	return -1;
}

inline int
ValidAttrId(int id)
{
	return id >= 0 && id < ATTR_NUM;
}

inline int
AttrLen(int id)
{
	return ATTRL[id];
}

inline const char *
AttrText(int id)
{
	return ATTR[id];
}

inline int
TagLen(int id)
{
	return TAGSL[id];
}

inline const char *
TagOpener(int id)
{
	return TAGS[2*id];
}

inline const char *
TagCloser(int id)
{
	return TAGS[(2*id) + 1];
}

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
void PrintBlockA(const char *buf, int len);

/*
*	debug: print "len" from "buf" as Bytes in "col"-umns
*/
void PrintBlockB(const char *buf, int len, int col);

/*
*	debug: write "buf" "len" bytes to file named "fname"
*/
void WriteToFile(const char *buf, int len, const char *fname);

#endif /*__NIFLIB_H__*/
