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

/*
*	Fast nif file reader
*/

#include "niff.h"

inline int
NifStream::Read(register void *ptr, register int size)
{
	int result = 0;
	unsigned char *dptr = (unsigned char*)ptr;// make "c++" happy*
	if (size <= 0)
		return 0;
	for (;;) {
		if (buf_pos + size <= buf_len) {
			memcpy (dptr, &(buf[buf_pos]), size);
			buf_pos += size;
			result += size;
			return result;
		}
		int part = buf_len - buf_pos;
		if (part > 0) {
			memcpy (dptr, &(buf[buf_pos]), part);
			buf_pos += part;
			result += part;
		}
		buf_len = fread (&(buf[0]), 1, buf_cap, f);
		if (buf_len <= 0)
			return result;// EOF, sorry
		buf_pos = 0;
		dptr += part;
		size -= part;
	}
	//return read(&(buf[part]), size); - dangerous
}

int
NifStream::ReadByte(register NIFbyte *buf, register int count)
{
	return Read (buf, count);
}

int
NifStream::ReadChar(register NIFchar *buf, register int count)
{
	return Read (buf, count);
}

// It always returns bytes read.
// Ptr needs to have space for "cond" as well.
int
NifStream::ReadCharCond(register NIFchar *ptr, register int size, register NIFchar cond)
{
	if (size <= 0)
		return 0;
	int i, idx = buf_pos, rpos = 0, cnt = 0;
	for (i = 0; i < size; i++) {
		if (idx >= buf_len) {
			cnt = idx - buf_pos;
			memcpy (&ptr[rpos], &(buf[buf_pos]), cnt);
			buf_len = fread (&(buf[0]), 1, buf_cap, f);
			if (buf_len <= 0)
				return i;// EOF - no "cond" found
			buf_pos = 0;
			idx = 0;
			rpos += cnt;
		}
		else if (buf[idx] == cond) {
			idx++;// incl. "cond"
			memcpy (&ptr[rpos], &(buf[buf_pos]), idx - buf_pos);
			buf_pos = idx;
			return i + 1;
		}
		idx++;
	}
	// no "cond" found within "size"
	return rpos;
}

int
NifStream::ReadUInt(register NIFuint *buf, register int count)
{
	return Read (buf, count*SIZEOFDWORD);
}

int
NifStream::ReadInt(register NIFint *buf, register int count)
{
	return Read (buf, count*SIZEOFDWORD);
}

int
NifStream::ReadUShort(register NIFushort *buf, register int count)
{
	return Read (buf, count*SIZEOFWORD);
}

int
NifStream::ReadShort(register NIFshort *buf, register int count)
{
	return Read (buf, count*SIZEOFWORD);
}

int
NifStream::ReadFloat(register NIFfloat *buf, register int count)
{
	return Read (buf, count*SIZEOFDWORD);
}

NifStream::NifStream(const char *fname, int bufsize)
{
	if (bufsize <= 0)
		throw "Invalid bufsize";
	if (!fname)
		throw "Invalid filename";
	f = fopen (fname, "r");
	if (!f)
		throw "File access denied";
	buf = (char *) malloc (bufsize);
	if (!buf) {
		fclose (f);
		throw "Not enough memory";
	}
	buf_cap = bufsize;
	buf_len = 0;
	buf_pos = 0;
}

NifStream::~NifStream()
{
	free (buf);
	fclose (f);
}
