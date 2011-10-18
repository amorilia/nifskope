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

#include <iostream>
#include <vector>
#include <cstdlib>

#include "niff.h"
#include <sys/time.h>

#define INFO(MSG)\
{\
	std::clog << MSG << std::endl;\
}

#define ERR(MSG)\
{\
	std::cerr << "err: " << MSG << std::endl;\
}

template <typename T> class List
{
	std::vector<T> list;
public:
	T Add(T item)
	{
		list.push_back (item);
		return item;
	}
	int Count()
	{
		return (int)list.size ();
	}
	T &operator[](int idx)
	{
		return list[idx];
	}
};

template <typename T> class TreeNode
{
public:
	TreeNode<T> *Parent;
	List< TreeNode<T> > Nodes;
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
*	basic     10    12  9    11              6
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
*	9 - option
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
	List<NiObject> Items;
	void Read();
	void Write();
};

// Returns (b - a) in microseconds
long
time_interval(struct timeval *a, struct timeval *b)
{
	return ( (b->tv_sec - a->tv_sec) * 1000000 ) + (b->tv_usec - a->tv_usec);
}

class NifXml
{
	List<void *> fsequence;
public:
	NifXml(const char *fileName)
	{
	}
};

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
*	Loads a file "fname" in a buffer "*buf".
*	Allocates "*buf" and specifies its size in "*size".
*	Returns 0 on failure, 1 - otherwise.
*/
int
NifLoadFile(const char *fname, char **buf, int *size)
{
	const int MAX_BUF = 1000000; // bytes
	FILE *fh = fopen (fname, "r");
	int result = 0;
	if (fh) {
		if (fseek (fh, 0, SEEK_END))
			ERR("Seek failed")
		else {
			long fs = ftell (fh);
			if (fseek (fh, 0, SEEK_SET))
				ERR("Seek failed")
			else {
				if (fs > (long)MAX_BUF)
					ERR("Too large, MAX = " << MAX_BUF << " bytes")
				else {
					*buf = (char *) NifAlloc (fs);
					if (!(*buf))
						ERR("Out of memory")
					else {
						*size = (int)fs;
						if (fread (&((*buf)[0]), (int)fs, 1, fh) != 1)
							ERR("Read failed")
						else result = 1;
					}// buf ptr check
				}// file length check
			}// if (fseek (fh, 0, SEEK_SET))
		}// if (fseek (fh, 0, SEEK_END))
		fclose (fh);
	}// if (fh)
	return result;
}

/*
*	Returns true if "buf" starts with "q"
*/
int
StartsWith(const char *q, int qlen, const char *buf, int buflen)
{
	if (!q || !buf || buflen <= 0 || qlen <= 0)
		return 0;
	if (qlen > buflen)
		return 0;
	return !strncmp (q, buf, qlen);
}

/*
*	Finds first occurrence of "q" and returns its start index
*	relative to "buf".
*	Returns "buflen" on failure.
*/
int
FindFirst(const char *q, int qlen, const char *buf, int buflen)
{
	int i;
	if (!q || !buf || buflen <= 0 || qlen <= 0)
		return buflen;
	for (i = 0; i < (buflen - qlen) + 1; i++)
		if (StartsWith (q, qlen, &buf[i], buflen - (i + 1)))
			return i;
	return buflen;
}

/*
*	Find a block what starts with "a" and ends with "b" in "buf".
*	Handles nested blocks:
*   "a1.b1b1" - a="a1", b="b1" will return 0, blcklen=5
*   "a1a1.b1b1" - a="a1", b="b1" will return 0, blcklen=9
*   "a1a1.b1" - a="a1", b="b1" will return 2, blcklen=5
*	Returns its starting index relative to "buf".
*	Returns its length, including "a' and "b", in "blcklen".
*	Returns "buflen" on failure.
*/
int
Find(const char *a, int alen, const char *b, int blen, char *buf, int buflen, int *blcklen)
{
	int i, cnt = 0;
	if (!a || !b || !buf || !blcklen)
		return buflen;
	if (buflen <= 0 || alen <= 0 || blen <= 0)
		return buflen;
	for (i = 0; i < (buflen - alen) + 1; i++)
		if (StartsWith (a, alen, &buf[i], buflen - (i + 1))) {
			// "a" found for 1st time, search for "b" with another "a" in mind
			int k;
			for (k = i; k < (buflen - blen) + 1; k++) {
				if (StartsWith (a, alen, &buf[k], buflen - (k + 1)))
					cnt++;
				if (StartsWith (b, blen, &buf[k], buflen - (k + 1))) {
					if (cnt > 1)// "a" has been found before 1st "b"
						cnt--;// continue searching
					else {
						*blcklen = (k - i) + blen;
						return i;
					}
				}
			}
		}
	return buflen;
}

void
NifXmlProcess (char *buf, int buflen)
{
	// 1 - remove the impossible :)
	// - ignore all before "<niftoolsxml"
	// - ignore <!--.*--> - including new line for multiline comments
	//   If you meet "<!--" count++, if you meet "-->" count--.
	//   Loop ends when count is 0. Malformed XML if EOF.
	/*char *tmp = (char *)NifAlloc (buflen);
	memset (tmp, 0, buflen);*/

	int sidx, eidx;
	int slen = strlen ("<niftoolsxml");
	sidx = FindFirst ("<niftoolsxml", slen, buf, buflen);
	int elen = strlen ("</niftoolsxml>");
	eidx = FindFirst ("</niftoolsxml>", elen, buf, buflen);
	if (sidx != buflen && eidx != buflen) {
		INFO("sidx: " << sidx)
		INFO("eidx: " << eidx)
		int testsize;
		int test = Find ("<!--", 4, "-->", 3, buf, buflen, &testsize);
		if (test != buflen) {
			INFO("found comment at " << test << ", length: " << testsize)
			char *comment = (char *)NifAlloc (testsize + 1);
			comment[testsize] = '\0';
			memcpy (&comment[0], &buf[test], testsize);
			INFO("\"" << comment << "\"")
			NifRelease (comment);
		}
		else
			INFO("find failed")
	}
}

int
main(int argc, char **argv)
{
	const char *nifxml = "nif.xml";
	char *buf = NULL;
	int size = 0;
	if (NifLoadFile (nifxml, &buf, &size)) {
		INFO("\"" << nifxml << "\" loaded, size: " << size << " bytes")
		NifXmlProcess (buf, size);
		NifRelease (buf);
	}
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
