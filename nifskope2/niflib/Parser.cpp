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

#include "Parser.h"
#include <cstdlib>
#include <string.h>
#include <stdio.h>
#include "niflib.h"

namespace NifLib
{
	/*
	*	Turn "nif.xml" into objects
	*	A tag has attributes.
	*	map<tagname,list<tag>>
	*	tag
	*		map<attrname, attr>
	*/
	void
	Parser::Process(char *buf, int buflen)
	{
		// 1 - remove the impossible :)
		// 1.1 ignore all before "<niftoolsxml"
		// 1.2 ignore <!--.*--> - including new line for multiline comments
		char *tmp = (char *)NifAlloc (buflen);
		if (!tmp) {
			ERR("Out of memory. NifLib.Parser.Process: failed to allocate " <<
				buflen << "bytes")
			return;
		}
		int sidx, eidx;
		int slen = strlen ("<niftoolsxml");
		int elen = strlen ("</niftoolsxml>");
		sidx = FindFirst ("<niftoolsxml", slen, buf, buflen);
		eidx = FindFirst ("</niftoolsxml>", elen, buf, buflen);
		if (sidx != buflen && eidx != buflen) {
			INFO("sidx: " << sidx)
			INFO("eidx: " << eidx)
			// 1.1 ( point out to <niftoolsxml.*</niftoolsxml> )
			buflen = (eidx - sidx) + elen + 1;// keep the XML valid for now
			buf += sidx;
			// 1.2 Remove XML comments because there are blocks enclosed in them
			//     and those blocks should be ignored, and in general - the parser
			//     doesn't need them.
			int dst = 0;// points in "tmp"
			int ccnt = 0;
			int csize = 0;
			for (;;) {
				int clen;
				int c = Find ("<!--", 4, "-->", 3, buf, buflen, &clen);
				if (c != buflen) {
					//INFO("XML comment at " << c << ", length: " << clen)
					//PrintBlockA (&buf[c], clen);
					//PrintBlockB (&buf[c], clen, 16);
					memcpy (&tmp[dst], buf, c);
					dst += c;
					buflen -= (c + clen);
					buf += (c + clen);// move forward
					// stats:
					ccnt++;
					csize += clen;
				} else {
					// add the final block
					memcpy (&tmp[dst], buf, buflen);
					dst += buflen;
					// and stop
					break;
				}
			}
			// "dst" points to "first free" i.e. is the length of "tmp"
			INFO("Removed " << ccnt << " XML comment blocks, total of "
				<< csize << " bytes")
			//WriteToFile (tmp, dst, "nif2.xml");
			Tokenize (tmp, dst);
			NifRelease (tmp);
			// TODO: at this point "gbuf" is no longer needed
		}
	}

	/*
	*	to tags and attributes
	*/
	void
	Parser::Tokenize(char *buf, int buflen)
	{
		// < tag (attribute(s)) > tag value </ tag > or />
		int i, bl = buflen, q = 0, ws = 1, j;
		for (i = 0; i < buflen; i++) {
			// no need to scan for tags in certain "spaces"
			if (buf[i] == '"')
				q = !q;
			ws = (buf[i] == ' ' || buf[i] == '\t' || buf[i] == '\n');
			if (ws || q)
				continue;
			// tokenize - tag ordering is irrelevant
			for (j = 0; j < 2 * TAGS_NUM; j += 2)
				TryParseTag (TAGS[j], TAGS[j+1], TAGSL[j/2], &buf[i], bl - i);
		}
		// stats:
		// The new "c++" way, ( known way, new for "c++" )
		for (auto kv = objs.begin (); kv != objs.end (); kv++)
		// The current "c++" way
		//std::map< std::string, NifLib::List<NifLib::Tag *> *>::iterator kv;
		//for (kv = objs.begin (); kv != objs.end (); kv++)
			INFO(kv->first << ": " << kv->second->Count ());
	}

	/*
	*	Tries to get a "tag" from the XML.
	*	Returns true on success, false otherwise.
	*	"tago" and "tagc" are "tag opener" and "tag closer" -
	*	"/>" is supported as a "tag closer".
	*	"l" is the length in bytes of "tago". The length of the "tag closer"
	*	is calculated as l + 2 for the '/' and '>' symbols.
	*	"buf" is the buffer to scan in, "bl" - its length
	*/
	int
	Parser::TryParseTag(const char *tago, const char *tagc, int l, char *buf, int bl)
	{
		if (l + 2 > bl)
			return 0;
		if (!StartsWith (tago, l, buf, l))
			return 0; // no tag opener here
		// Not all have a tag closer:
		//  1 - < tag ... > ... </tag>
		//  2 - < tag ... />
		int i = 0;
		int q = 0; // in quotes
		int t = 0; // in tag
		for (i = 0; i < bl; i++) {
			if (buf[i] == '"')
				q = !q;
			if (!q) {
				if (StartsWith (tagc, l + 2, &buf[i], l + 2)) {
					//PrintBlockA (buf, i);
					Add (tago, buf, i);
					return 1;
				}
				if (buf[i] == '<')
					t++;
				else if (buf[i] == '>') {
					if (t > 0 && i > 0 && buf[i-1] == '/') {
						t--;
						if (t == 0) {
							//PrintBlockA (buf, i - 1);
							Add (tago, buf, i - 1);
							return 1;
						}
					}
				}
			}// not in double quotes
		}// for i
		return 0;
	}

	/*
	*	Adds an item to "objs".
	*	Returns true on success, false otherwise.
	*/
	int
	Parser::Add(char const * const tago, char *buf, int bl)
	{
		// std::map< std::string, NifLib::List<NifLib::Tag *> *> objs;
		if (objs.find (tago) == objs.end ())
			objs[tago] = new NifLib::List<NifLib::Tag *>();
		NifLib::Tag *t = new NifLib::Tag();
		t->Id = gid++;
		t->Name = tago;
		if (t->Parse (buf, bl)) {
			objs[tago]->Add (t);
			//INFO("Add: " << tago)
			return 1;
		}
		return 0;
	}

	Parser::Parser(const char *fname)
	{
		gbuf = NULL;
		gid = 1; // no one has the right ot be 0
		size = 0;
		if (LoadFile (fname, &gbuf, &size)) {
			INFO("\"" << fname << "\" loaded, size: " << size << " bytes")
			Process (gbuf, size);
			NifRelease (gbuf);
		}
	}
	Parser::~Parser()
	{
		if (!objs.empty()) {
			std::map< std::string, NifLib::List<NifLib::Tag *> *>::iterator i;
			for (i = objs.begin(); i != objs.end(); i++) {
				NifLib::List<NifLib::Tag *> *l = i->second;
				int j;
				for (j = 0; j < l->Count(); j++)
					delete (*l)[j];
				l->Clear ();
				delete l;
			}
			objs.clear ();
		}
	}

	/*
	*	Loads a file "fname" in a buffer "*buf".
	*	Allocates "*buf" and specifies its size in "*size".
	*	Returns 0 on failure, 1 - otherwise.
	*/
	int
	Parser::LoadFile(const char *fname, char **buf, int *size)
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
	Parser::StartsWith(const char *q, int qlen, const char *buf, int buflen)
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
	Parser::FindFirst(const char *q, int qlen, const char *buf, int buflen)
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
	Parser::Find(const char *a, int alen, const char *b, int blen, char *buf, int buflen, int *blcklen)
	{
		//INFO("Find: buflen = " << buflen)
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
		//INFO("Find: found nothing")
		return buflen;
	}
}
