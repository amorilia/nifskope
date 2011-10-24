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

#include "niflib.h"
#include <string.h>
#include <stdio.h>
#include <cstdlib>
// formatting the ++ way
#include <iomanip>
#include <sstream>

void *
NifAlloc(size_t size)
{
	return malloc (size);
}

void
NifRelease(void *ptr)
{
	free (ptr);
}

/*
*	debug: print "len" from "buf" as ASCII
*/
void
PrintBlockA(const char *buf, int len)
{
	char *b = (char *)NifAlloc (len + 1);
	if (!b) {
		ERR("PrintBlock: Out of memory")
		return;
	}
	b[len] = '\0';
	memcpy (b, buf, len);
	INFO("\"" << b << "\"")
	NifRelease (b);
}

/*
*	debug: print "len" from "buf" as Bytes in "col"-umns
*/
void
PrintBlockB(const char *buf, int len, int col)
{
	if (len <= 0) {
		INFO("PrintBlockB: empty, len = " << len)
		return;
	}
	int i;
	std::stringstream d;
#define FMT std::setw (2) << std::setfill ('0') << std::hex << std::uppercase
	d << FMT << (int)buf[0];
	for (i = 1; i < len; i++)
		if ((i+1) % col)
			d << ' ' << FMT << (int)buf[i];
		else
			d << ' ' << FMT << (int)buf[i] << std::endl;
	INFO("\"" << d.str () << "\"")
#undef FMT
}

/*
*	debug: write "buf" "len" bytes to file named "fname"
*/
void
WriteToFile(const char *buf, int len, const char *fname)
{
	FILE *fh = fopen (fname, "w");
	if (fh) {
		if (fwrite (buf, len, 1, fh) != 1)
			INFO("WriteToFile: fwrite failed")
		fclose (fh);
	} else
		INFO("WriteToFile: fopen failed")
}
