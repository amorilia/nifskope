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

#include "Buffer.h"

#include <string.h>
#include <cstdlib>
#include "niflib.h"

namespace NifLib
{
	Buffer::Buffer()
	{
		len = 0;
		buf = NULL;
	}

	int
	Buffer::CopyFrom(const char *srcbuf, int srclen)
	{
		buf = (char *)NifAlloc (srclen);
		if (!buf) {
			ERR("Buffer.CopyFrom: Out of memory")
			return 0;
		}
		len = srclen;
		return memcpy (buf, srcbuf, srclen) != NULL;
	}

	int
	Buffer::Equals(const char *srcbuf, int srclen)
	{
		if (srclen != len)
			return 0;
		if (buf && len > 0)
			return !strncmp (buf, srcbuf, len);
		else
			return 0;
	}

	Buffer::~Buffer()
	{
		if (buf && len > 0)
			NifRelease (buf);
	}
}
