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

#ifndef __NS_UTILS_H__
#define __NS_UTILS_H__

#include <string>

/*
* Converts a 4 bytes int to a version string. Two formats:
*  - ( ;NF_V03030013) - N.N
*  - [NF_V03030013; ) - N.N.N.N
* When the format is N.N, trailing zeroes are omitted for bytes 3 and 4 only.
* It will return "" on 0, and "255.255.255.255" when -1 is passed
*/
std::string nifver2str(unsigned int v);

/*
* Converts a 4 bytes int to a version string. Format: "N.N.N.N",
* lower-case, not padded with zeroes:
*  - for v=-1 it should return "ff.ff.ff.ff".
*  - for v=0 it should return "".
*  - for v=1 it should return "0.0.0.1".
*/
std::string kfmver2str(unsigned int v);

/*
* Converts a string into a 4 bytes int version. Formats:
*  - "a.b" - a:, b: [0-255]
*  - "a.b.c" - a:, b:, c:  [0-255]
*  - "a.b.c.d" - a:, b:, c:, d:  [0-255]
*  - "a.bcd" - a: [0-255]; b:, c:, d: [0-9]
* a, b, c and d can be hexadecimal as well, upper and/or lower case 
*/
unsigned int nifstr2ver(std::string s);

/*
* Converts a string into a 4 bytes int version. Formats:
*  - "a.b" - a:, b: [0-255]
*  - "a.b.c" - a:, b:, c:  [0-255]
*  - "a.b.c.d" - a:, b:, c:, d:  [0-255]
*  - "a.bcd" - a: [0-255]; b:, c:, d: [0-9]
* a, b, c and d can be hexadecimal as well, upper and/or lower case 
*/
unsigned int kfmstr2ver(std::string s);

#endif /* __NS_UTILS_H__ */
