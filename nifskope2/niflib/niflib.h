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

#include <iostream>
#include <cstdlib>
#include <string.h>
#include <sstream>
#include <sys/time.h>

#define INFO(MSG)\
{\
	std::cout << MSG << std::endl;\
}

#define ERR(MSG)\
{\
	std::cout << "err: " << MSG << std::endl;\
}

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
#define TBASIC 0
#define TENUM 2
#define TCOMPOUND 3
#define TNIOBJECT 4
#define TBITFLAGS 5

#define ATTR_NUM 22
char const * const ATTR[] = {
	/*  0 */"abstract",
	/*  1 */"arg",
	/*  2 */"arr1",
	/*  3 */"arr2",
	/*  4 */"arr3",
	/*  5 */"cond",
	/*  6 */"count",
	/*  7 */"default",
	/*  8 */"inherit",
	/*  9 */"istemplate",
	/* 10 */"name",
	/* 11 */"niflibtype",
	/* 12 */"nifskopetype",
	/* 13 */"num",
	/* 14 */"storage",
	/* 15 */"template",
	/* 16 */"type",
	/* 17 */"userver",
	/* 18 */"value",
	/* 19 */"ver1",
	/* 20 */"ver2",
	/* 21 */"vercond",
	/* 22 */"version"
};
int const ATTRL[] =
	{8, 3, 4, 4, 4, 4, 5, 7, 7, 10, 4, 10, 12, 3, 7, 8, 4, 7, 5, 4, 4, 7, 7};

#define AARG 1
#define AARR1 2
#define AARR2 3
#define ACOND 5
#define ADEFAULT 7
#define AINHERIT 8
#define ANAME 10
#define ANIFLIBTYPE 11
#define ASTORAGE 14
#define ATEMPLATE 15
#define ATYPE 16
#define AUSERVER 17
#define AVALUE 18
#define AVER1 19
#define AVER2 20
#define AVERCOND 21

/*
*	"Operator" lengths
*/
const int OPL[] = {0, 2, 2, 1, 2, 1, 1, 2, 1};
#define EVAL_OP_NONE 0
#define EVAL_OP_EQU 1
#define EVAL_OP_GTEQU 2
#define EVAL_OP_GT 3
#define EVAL_OP_LTEQU 4
#define EVAL_OP_LT 5
#define EVAL_OP_AND 6
#define EVAL_OP_NOTEQU 7
#define EVAL_OP_SUB 8

const int OPBL[] = {0, 2, 2};
#define EVAL_OPB_NONE 0
#define EVAL_OPB_OR 1
#define EVAL_OPB_AND 2

#define EVAL_TYPE_UNKNOWN -1
#define EVAL_TYPE_VERSION 1
#define EVAL_TYPE_UINT 2

/*
*	"nif.xml" C++ types map
*	// 0100 0000 - unsigned
*	// 1000 0000 - signed
*	// 0010 0000 - int
*	// 0001 0000 - float
*	// 0000 0000 - 0 bytes
*	// 0000 0001 - 1 byte
*	// 0000 0010 - 2 bytes
*	// 0000 0100 - 4 bytes
*	// 0000 1000 - 8 bytes, etc.
*	// 0011 0000 - dynamic type
*	// 1111 0000 - complex or unknown type
*/
#define NIFT_U 0x40
#define NIFT_S 0x80
#define NIFT_F 0x10
#define NIFT_1 0x01
#define NIFT_2 0x02
#define NIFT_4 0x04
#define NIFT_D 0x30
#define NIFT_T 0xF0
#define NIFT_SIZE 0x0F
std::string NIFT2Str(int NLType);

/*
*	<basic by ANAME ANIFLIBTYPE
*/
#define BTYPESNUM 17
const struct
{	char const * const nval; const int nlen;// ANAME
	char const * const lval; const int llen;// ANIFLIBTYPE
	const int type;// NifLib type
} BTYPES[] = {
	/*  0 */{"bool", 4, "bool", 4, NIFT_D},
	/*  1 */{"byte", 4, "byte", 4, NIFT_U | NIFT_1},
	/*  2 */{"uint", 4, "unsigned int", 12, NIFT_U | NIFT_4},
	/*  3 */{"ushort", 6, "unsigned short", 14, NIFT_U | NIFT_2},
	/*  4 */{"int", 3, "int", 3, NIFT_S | NIFT_4},
	/*  5 */{"short", 5, "short", 5, NIFT_S | NIFT_2},
	/*  6 */{"BlockTypeIndex", 14, "unsigned short", 14, NIFT_U | NIFT_2},
	/*  7 */{"char", 4, "byte", 4, NIFT_U | NIFT_1},
	/*  8 */{"FileVersion", 11, "unsigned int", 12, NIFT_U | NIFT_4},
	/*  9 */{"Flags", 5, "unsigned short", 14, NIFT_U | NIFT_2},
	/* 10 */{"float", 5, "float", 5, NIFT_F | NIFT_4},
	/* 11 */{"HeaderString", 12, "HeaderString", 12, NIFT_D},
	/* 12 */{"LineString", 10, "LineString", 10, NIFT_D},
	/* 13 */{"Ptr", 3, "*", 1, NIFT_S | NIFT_4},
	/* 14 */{"Ref", 3, "Ref", 3, NIFT_S | NIFT_4},
	/* 15 */{"StringOffset", 12, "unsigned int", 12, NIFT_U | NIFT_4},
	/* 16 */{"StringIndex", 11, "IndexString", 11, NIFT_U | NIFT_4}
	/*
	ANAME            ANIFLIBTYPE
	"bool"           "bool"           32-bit from 4.0.0.2, and 8-bit from 4.1.0.1 on
    "byte"           "byte"           unsigned 8-bit integer
    "uint"           "unsigned int"   unsigned 32-bit integer
    "ushort"         "unsigned short" unsigned 16-bit integer
    "int"            "int"            signed 32-bit integer
    "short"          "short"          signed 16-bit integer
    "BlockTypeIndex" "unsigned short" 16-bit (signed?) integer
    "char"           "byte"           8-bit character
    "FileVersion"    "unsigned int"   32-bit integer
    "Flags"          "unsigned short" 16-bit integer
    "float"          "float"          standard 32-bit floating point number
    "HeaderString"   "HeaderString"   var len str that ends with (0x0A)
    "LineString"     "LineString"     var len str that ends with (0x0A)
    "Ptr"            "*"              signed 32-bit integer
    "Ref"            "Ref"            signed 32-bit integer
    "StringOffset"   "unsigned int"   32-bit unsigned integer
    "StringIndex"    "IndexString"    32-bit unsigned integer
	*/
};
// Basic Type ANAME
#define BTN_BOOL 0
#define BTN_BYTE 1
#define BTN_UINT 2
#define BTN_USHORT 3
#define BTN_INT 4
#define BTN_SHORT 5
#define BTN_BLOCKTYPEINDEX 6
#define BTN_CHAR 7
#define BTN_FILEVERSION 8
#define BTN_FLAGS 9
#define BTN_FLOAT 10
#define BTN_HEADERSTRING 11
#define BTN_LINESTRING 12
#define BTN_PTR 13
#define BTN_REF 14
#define BTN_STRINGOFFSET 15
#define BTN_STRINGINDEX 16
// Basic Type ANIFLIBTYPE
#define BTL_BOOL 0
#define BTL_BYTE 1
#define BTL_UINT 2
#define BTL_USHORT 3
#define BTL_INT 4
#define BTL_SHORT 5
#define BTL_FLOAT 10
#define BTL_HEADERSTRING 11
#define BTL_LINESTRING 12
#define BTL_PTR 13
#define BTL_REF 14
#define BTL_INDEXSTRING 16

inline const char *
BtnText(int id)
{
	return BTYPES[id].nval;
}

inline int
BtnLen(int id)
{
	return BTYPES[id].nlen;
}

inline int
BtnType(int id)
{
	return BTYPES[id].type;
}

#define BTN(ID) BtnText (ID), BtnLen (ID)

inline const char *
BtlText(int id)
{
	return BTYPES[id].lval;
}

inline int
BtlLen(int id)
{
	return BTYPES[id].llen;
}

inline int
BtlType(int id)
{
	return BTYPES[id].type;
}

std::string StreamBlockB(const char *buf, int len, int col);
/*
*	Returns type by ANAME or ANIFLIBTYPE.
*	Handles the type="char" case and the like.
*	TODO: hash
*/
inline int
BType(const char *buf, int bl)
{
	if (!buf)
		ERR ("BType: buf is NULL")
	if (bl <= 0)
		ERR ("BType: bl is invalid: " << bl)
	if (bl > 100)
		ERR ("BType: bl is invalid: " << bl << ", \"" << StreamBlockB (buf, 100, 16) << "\"")
	//std::stringstream a, b;
	//if (!buf) a << "[NULL], " << bl;
	//	else  a << "[" << StreamBlockB (buf, bl, bl + 1) << "], " << bl;
	int i;
	for (i = 0; i < BTYPESNUM; i++) {
		if (BtlLen (i) <= bl)
			if (strncmp (buf, BtlText (i), BtlLen (i)) == 0) {
				//b << "\"" << BtlText (i) << "\", " << BtlLen (i);
				//INFO("BType: req: " << a.str () << ", result: " << b.str ())
				return BtlType (i);
			}
		if (BtnLen (i) <= bl)
			if (strncmp (buf, BtnText (i), BtnLen (i)) == 0) {
				//b << "\"" << BtnText (i) << "\", " << BtnLen (i);
				//INFO("BType: req: " << a.str () << ", result: " << b.str ())
				return BtnType (i);
			}
	}
	//b << "[NIFT_T]";
	//INFO("BType: req: " << a.str () << ", result: " << b.str ())
	return NIFT_T;
}

#define BTL(ID) BtlText (ID), BtlLen (ID)

// Returns (b - a) in microseconds
long time_interval(struct timeval *a, struct timeval *b);

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
*	debug: print "len" from "buf" as Bytes in "col"-umns
*/
std::string StreamBlockB(const char *buf, int len, int col);

/*
*	debug: write "buf" "len" bytes to file named "fname"
*/
void WriteToFile(const char *buf, int len, const char *fname);

#endif /*__NIFLIB_H__*/
