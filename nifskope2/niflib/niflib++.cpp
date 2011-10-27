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

template <typename T> class TreeNode
{
public:
	TreeNode<T> *Parent;
	NifLib::List< TreeNode<T> > Nodes;
};

int
main(int argc, char **argv)
{
	struct timeval tstart, tstop;
	gettimeofday (&tstart, NULL);
	NifLib::Compiler p("nif.xml");
	gettimeofday (&tstop, NULL);
	INFO("XML loaded & parsed in " << time_interval (&tstart, &tstop) / (1000) << " ms")

	//p.SaveFile ("nif3.xml");
	int r = 0;
	p.Build ();
	gettimeofday (&tstart, NULL);
	r = p.ReadNif ("../../../nfiskope_bin/data/meshes/clothes/DLD89/ShaiyaDress.nif");
	gettimeofday (&tstop, NULL);
	if (r) {
		INFO("nif loaded & parsed in " << time_interval (&tstart, &tstop) / (1000) << " ms")
		p.WriteNif ("aaa.nif");
	} else {
		INFO("ReadNif failed")
		p.DbgPrintFields ();
	}

	/*const char *pfix = "/mnt/archive/rain/temp/nif/";
	std::string line;
	std::ifstream myf("flist.txt");
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
				std::stringstream fname;
				fname << std::string (pfix) << line;
				cnt++;
				INFO(cnt << ":\"" << fname.str () << "\"");
				if (!p.ReadNif (fname.str ().c_str ())) {
					INFO ("files done: " << cnt)
					break;
				}
			}
		}
	}*/

	return EXIT_SUCCESS;
}
