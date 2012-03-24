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

#include "ns_utils.h"

std::string nifver2str(unsigned int v)
{
	if ( v == 0 )
		return "";
	std::stringstream tmp;
	unsigned char *a = (unsigned char *)&v;
	tmp << (int)a[3] << NF_VSEPARATOR << (int)a[2];
	if ( v < NF_V03030013 ) {
		if ((int)a[0] > 0)
			tmp << (int)a[1] << (int)a[0];
		else if (a[1] > 0)
			tmp << (int)a[1];
	}
	else
		tmp << NF_VSEPARATOR << (int)a[1] << NF_VSEPARATOR << (int)a[0];
	return tmp.str ();
}

std::string kfmver2str(unsigned int v)
{
	if ( v == 0 )
		return "";
	std::stringstream tmp;
	unsigned char *a = (unsigned char *)&v;
	tmp << std::hex << std::nouppercase;
	tmp << (int)a[3]
		<< NF_VSEPARATOR << (int)a[2]
		<< NF_VSEPARATOR << (int)a[1]
		<< NF_VSEPARATOR << (int)a[0];
	return tmp.str ();
}
