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

/*
*	Fast nif file reader
*/

#ifndef __NIFF_H__
#define __NIFF_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char   NIFbyte;	// 1 B
typedef char            NIFchar;	// 1 B
typedef int             NIFint;		// 4 DW
typedef unsigned int    NIFuint;	// 4 DW
typedef short           NIFshort;	// 2 W
typedef unsigned short  NIFushort;	// 2 W
typedef float           NIFfloat;	// 4 DW

static const size_t SNIFbyte = sizeof(NIFbyte);
static const size_t SNIFchar = sizeof(NIFchar);
static const size_t SNIFint = sizeof(NIFint);
static const size_t SNIFuint = sizeof(NIFuint);
static const size_t SNIFshort = sizeof(NIFshort);
static const size_t SNIFushort = sizeof(NIFushort);
static const size_t SNIFfloat = sizeof(NIFfloat);

#define SIZEOFWORD 2
#define SIZEOFDWORD 4

#define MAKE_SURE(COND,MSG)\
	if(!(COND)) {\
		printf (stderr, MSG);\
		exit (1);\
	}

class NifStream
{
	FILE *f;
	char *buf;
	int buf_len;// length - like 120
	int buf_pos;// position - like 23
	int buf_cap;// cpacity - like 1000
	inline int Read(register void *ptr, register int size);
public:
	NifStream(const char *fname, int bufsize);

	/*
	*	"It's usually imperative that the function's definition (the part
	*	between the {...}) be placed in a header file. If you put the inline
	*	function's definition into a .cpp file, and if it is called from some
	*	other .cpp file, you'll get an "unresolved external" error from the
	*	linker."
	*/
	int ReadByte(register NIFbyte *buf, register int count);

	int ReadChar(register NIFchar *buf, register int count);

	/*
	*	It always returns bytes read.
	*	Ptr needs to have space for "cond" as well.
	*/
	int ReadCharCond(register NIFchar *buf, register int count, register NIFchar cond);

	int ReadUInt(register NIFuint *buf, register int count);

	int ReadInt(register NIFint *buf, register int count);

	int ReadUShort(register NIFushort *buf, register int count);

	int ReadShort(register NIFshort *buf, register int count);

	int ReadFloat(register NIFfloat *buf, register int count);

	int ByteOrder;// TODO: implement me: 0 - 3 2 1 0 , != 0 - 0 1 2 3

	~NifStream();
};

#endif /* __NIFF_H__ */