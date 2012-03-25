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
#include "ns_base.h"
#include <sstream>
#include <iomanip>

#include <iostream>

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

template <typename T> static T str2(const std::string &val)
{
	bool h = false;
	for (int i = 0; i < (int)val.length (); i++)
		if ((val[i] >= 'a' && val[i] <= 'f') ||
			(val[i] >= 'A' && val[i] <= 'F')) {
			h = true;
			break;
		}
	std::stringstream aa;
	T k;
	if (h)
		aa << std::hex << val;
	else
		aa << val;
	aa >> k;
	return k;
}

unsigned int
nifstr2ver(std::string s)
{
	// TODO: optimize, map as well. See that the test-suite is running
	//		afterwards.
	int bl = s.length ();
	if (bl < 3 || bl > 15)
		return 0;// invalid length
	char const *buf = s.c_str ();
	unsigned int r = 0;// result
	int i, j, d, pcnt = 0;// current, next, digit, '.' count
	for (i = 0; i < bl; i++)
		if (buf[i] == '.') {
			if (++pcnt > 3)
				return 0;// invalid tokens num
		} else
			if ((buf[i] < '0') || (buf[i] > '9'))
				if ((buf[i] < 'a') || (buf[i] > 'f'))//allow hexadecimal
					if ((buf[i] < 'A') || (buf[i] > 'F'))//allow hexadecimal
						return 0;// invalid content
	if (pcnt == 0) {
		if (bl > 13)
			return 0;// invalid length
		unsigned int k = str2<unsigned int> (std::string (&buf[0], bl));
		return (k == 0xffffffff ? 0 : k);
	}
	for (d = 0, i = 0, j = 0; (i < bl) && (d < 4); d++) {
		if ((pcnt > 1) || ((pcnt == 1) && (d == 0))) {
			while (j < bl && buf[j] != '.')
				j++;
			if (pcnt == 1 && bl - j - 1 > 3)
				return 0;// second part can not be longer than 3
		} else
			j++;
		int len = j - i;
		if (!(len & 3))
			return 0;// invalid token length
		unsigned int k = str2<unsigned int> (std::string (&buf[i], len));
		if (k > 255)
			return 0;// invalid token value
		r = r | (k << (4 - (d + 1))*8);
		i = j + (buf[j] == '.' ? 1 : 0);
		j = i;
	}
	return r;
}

unsigned int
kfmstr2ver(std::string s)
{
	return nifstr2ver (s);
}
