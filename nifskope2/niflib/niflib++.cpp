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
#include "List.h"

template <typename T> class TreeNode
{
public:
	TreeNode<T> *Parent;
	NifLib::List< TreeNode<T> > Nodes;
};

/*	0 - abstract
*	1 - arg
*	2 - arr1
*	3 - arr2
*	4 - arr3
*	5 - cond
*	6 - count
*	7 - default
*	8 - inherit
*	9 - istemplate
*	10 - name
*	11 - niflibtype
*	12 - nifskopetype
*	13 - num
*	14 - storage
*	15 - template
*	16 - type
*	17 - userver
*	18 - value
*	19 - ver1
*	20 - ver2
*	21 - vercond
*	22 - version
*
* niftoolsxml                               22
*  version                                  13
*  basic      10    12  9    11              6
*  enum       10          19             14
*   option    10                            18
*  compound   10    12  9 19 11
*   add       10 16 12    19    20 17  0     1  2  3  4  7 15 21  5
*  niobject   10          19    20 17  0     8
*   add
*  bitflags   10                         14
*   option
*
*	0 - niftoolsxml
*	1 - version
*	2 - basic
*	3 - enum
*	4 - option
*	5 - compound
*	6 - add
*	7 - niobject
*	8 - bitflags
*/

class NiObject
{
public:
	void Read(NifStream &stream);
	void Write(NifStream &stream);
};

class Nif
{
public:
	Nif(const char *fileName);
	NifLib::List<NiObject> Items;
	void Read();
	void Write();
};

// Returns (b - a) in microseconds
long
time_interval(struct timeval *a, struct timeval *b)
{
	return ( (b->tv_sec - a->tv_sec) * 1000000 ) + (b->tv_usec - a->tv_usec);
}

int
main(int argc, char **argv)
{
	struct timeval tstart, tstop;
	gettimeofday (&tstart, NULL);
	NifLib::Parser p("nif.xml");
	gettimeofday (&tstop, NULL);
	INFO("loaded & parsed in " << time_interval (&tstart, &tstop) / (1000) << " ms")
	// reading works
	/*NifStream test ("../../../nfiskope_bin/data/meshes/clothes/DLD89/ShaiyaDress.nif", 1024*1024);

	char buf;
	int cnt = 0;
	struct timeval tstart, tstop;
	gettimeofday (&tstart, NULL);
	while (test.ReadChar(&buf, 1) == 1) cnt++;
	gettimeofday (&tstop, NULL);
	long ttaken = time_interval (&tstart, &tstop) / (1000*1000);
	INFO("time: " << time_interval (&tstart, &tstop) / (1000) << "ms")
	INFO("chars/second: " << (cnt/(ttaken ? ttaken : 1)))
	INFO(cnt << " chars where read")*/

	return EXIT_SUCCESS;
}
