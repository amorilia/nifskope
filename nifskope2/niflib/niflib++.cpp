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
*	It is not an XML parser.
*	It is "nif.xml" interpreter.
*/

#include <vector>

#include "niflib.h"

#include "niff.h"
#include <sys/time.h>

#include "Parser.h"
#include "Compiler.h"
#include "List.h"

#include <fstream>
#include <sstream>

#include <openssl/md5.h>

int
md5offile(const char *fname, unsigned char *md5_result)
{
	MD5_CTX md5_ctx;
	MD5_Init (&md5_ctx);
	const int FBUF1_MAX = 1*1024*1024;
	static char FBUF1[FBUF1_MAX];
	FILE *fh = fopen (fname, "r");
	if (fh) {
		int rr;
		while ((rr = fread (&(FBUF1[0]), 1, FBUF1_MAX, fh)) > 0) {
			MD5_Update (&md5_ctx, &(FBUF1[0]), rr);
		}
		MD5_Final (&md5_result[0], &md5_ctx);
		fclose (fh);
		return 1;
	}
	return 0;
}

int
md5filesareequal(const char *fname1, const char *fname2)
{
	unsigned char md5_result_a[MD5_DIGEST_LENGTH];
	unsigned char md5_result_b[MD5_DIGEST_LENGTH];
	int a = md5offile (fname1, &md5_result_a[0]);
	if (!a) {
		ERR("md5offile failed for \"" << fname1 << "\"")
		return 0;
	}
	int b = md5offile (fname2, &md5_result_b[0]);
	if (!b) {
		ERR("md5offile failed for \"" << fname2 << "\"")
		return 0;
	}
	return !strncmp (
		(char *)&md5_result_a[0],
		(char *)&md5_result_b[0],
		MD5_DIGEST_LENGTH);
}

int
main(int argc, char **argv)
{
	// TODO: nifVerify:
	//	- check index arrays for invalid indexes
	struct timeval tstart, tstop;
	gettimeofday (&tstart, NULL);
	NifLib::Compiler p("nif.xml");
	gettimeofday (&tstop, NULL);
	INFO("XML loaded & parsed in " << time_interval (&tstart, &tstop) / (1000) << " ms")

	//p.SaveFile ("nif3.xml");
	/*int r = 0;
	p.Build ();
	gettimeofday (&tstart, NULL);
	//r = p.ReadNif ("../../../nfiskope_bin/data/meshes/clothes/DLD89/ShaiyaDress.nif");
	const char *f = "../../../nfiskope_bin/data/meshes/clothes/DLD89/ShaiyaDress.nif";
	try {
		r = p.ReadNif (f);
	} catch (...) {
		ERR("ReadNif: An exception was thrown")
	}
	gettimeofday (&tstop, NULL);
	if (r) {
		INFO("nif loaded & parsed in " << time_interval (&tstart, &tstop) / (1000) << " ms")
		p.WriteNif ("aaa.nif");
		if (!md5filesareequal (
			f,
			"aaa.nif")) {
			INFO ("differ")
		}
		p.DbgPrintFields ();
	} else {
		INFO("ReadNif failed")
		p.DbgPrintFields ();
	}
	return 0;*/

	const char *pfix = "/mnt/archive/rain/temp/nif/";
	std::string line;
	std::ifstream myf("flist_nif2.txt");
	int cnt = 0;
	if ( myf.is_open() ) {
		p.Build ();
		while ( myf.good() ) {
			getline (myf, line);
			if (!myf.eof() && line.length() > 0) {
				int i, l = line.length();
				const char *buf = line.c_str();
				const char *ext = ".NIF";
				for (i = l - 4; i < l; i++) {
					char t = buf[i] < 'A' + 32 ? buf[i] : buf[i] - 32;
					if (ext[i - (l - 4)] != t)
						break;
				}
				if (i != l)
					continue;
				//if (l > 3 && !NifLib::Parser::StartsWith ("loki", 4, buf, 4))
				//	continue;
				std::stringstream fname;
				fname << std::string (pfix) << line;
				std::string fname2 = fname.str ();
				cnt++;
				INFO(cnt << ":\"" << fname2 << "\"");
				if (!p.ReadNif (fname2.c_str ())) {
					//p.DbgPrintFields ();
					INFO ("files done: " << cnt)
					break;	
				} else {// it was read, write it and md5sum compare it to the original
					p.WriteNif ("aaa.nif");
					if (!md5filesareequal (fname2.c_str () , "aaa.nif")) {
						ERR ("read differs written")
						INFO ("files done: " << cnt)
						break;
					}
				}
			}
		}
	}

	return EXIT_SUCCESS;
}
