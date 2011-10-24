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
	Parser::Process(const char *fname)
	{
		char *buf = NULL;
		int buflen = 0;
		if (LoadFile (fname, &buf, &buflen)) {
			INFO("\"" << fname << "\" loaded, size: " << buflen << " bytes")
			if (buflen <= 0)
				return;
		} else {
			ERR("\"" << fname << "\" load failed")
			return;
		}
		// 1 - remove the impossible :)
		// 1.1 ignore all before "<niftoolsxml"
		// 1.2 ignore <!--.*--> - including new line for multiline comments
		char *tmp = (char *)NifAlloc (buflen);
		if (!tmp) {
			ERR("Out of memory. NifLib.Parser.Process: failed to allocate " <<
				buflen << "bytes")
			NifRelease (buf);
			return;
		}
		char *buforigin = buf;
		int sidx, eidx;
		int slen = strlen ("<niftoolsxml");
		int elen = strlen ("</niftoolsxml>");
		sidx = FindFirst ("<niftoolsxml", slen, buf, buflen);
		eidx = FindFirst ("</niftoolsxml>", elen, buf, buflen);
		if (sidx != buflen && eidx != buflen) {
			// init header, find first '<'
			int i, q = 0;
			for (i = sidx + slen; i < buflen; i++) {
				if (buf[i] == '"')
					q = !q;
				if (!q && buf[i] == '<') {
					header.CopyFrom (buf, i);
					break;
				}
			}
			// init footer
			if (buflen - (eidx + elen) > 0)
				footer.CopyFrom (&buf[eidx], buflen - eidx);
			// 1.1 ( point out to <niftoolsxml.*</niftoolsxml> )
			buflen = (eidx - sidx) + elen + 1;// keep the XML valid for now
			buf += sidx;
			// 1.2 Remove XML comments because there are blocks enclosed in them
			//     and those blocks should be ignored, and in general - the parser
			//     doesn't need them.
			int dst = 0;// points in "tmp"
			for (;;) {
				int clen;
				int c = Find ("<!--", 4, "-->", 3, buf, buflen, &clen);
				if (c != buflen) {
					memcpy (&tmp[dst], buf, c);
					dst += c;
					buflen -= (c + clen);
					buf += (c + clen);
				} else {// add the final block
					memcpy (&tmp[dst], buf, buflen);
					dst += buflen;
					break;
				}
			}
			// "dst" is the length of "tmp"
			//WriteToFile (tmp, dst, "nif2.xml");
			NifRelease (buforigin);
			Tokenize (tmp, dst, 0, TAGS_NUML1);
			NifRelease (tmp);
		}
	}

	/*
	*	to tags and attributes
	*/
	void
	Parser::Tokenize(char *buf, int buflen, int tmin, int tcnt)
	{
		int i, bl = buflen, q = 0, ws = 1, j, ll;
		for (i = 0; i < buflen; i++) {
			// no need to scan for tags in certain "spaces"
			if (buf[i] == '"')
				q = !q;
			ws = (buf[i] == ' ' || buf[i] == '\t' || buf[i] == '\n');
			if (ws || q)
				continue;
			// tokenize - tag ordering is irrelevant
			for (j = tmin; j < tmin + tcnt; j++)
				if ((ll = TryParseTag (j, &buf[i], bl - i)))
					break;
			i += ll;
		}
	}

	/*
	*	Tries to get a "tag" from the XML.
	*	Returns length on success, 0 otherwise.
	*	"/>" is supported as a "tag closer".
	*	"buf" is the buffer to scan in, "bl" - its length
	*/
	int
	Parser::TryParseTag(int tagid, char *buf, int bl)
	{
		int l = TagLen (tagid);
		const char *tago = TagOpener (tagid);
		const char *tagc = TagCloser (tagid);
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
					Add (tagid, buf, i);
					return i + l + 2;
				}
				if (buf[i] == '<')
					t++;
				else if (buf[i] == '>') {
					if (t > 0 && i > 0 && buf[i-1] == '/') {
						t--;
						if (t == 0) {
							Add (tagid, buf, i - 1);
							return i;
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
	Parser::Add(int tagid, char *buf, int bl)
	{
		NifLib::Tag *t = new NifLib::Tag();
		t->Name = tagid;
		if (tagid < TAGS_NUML1) {
			if (!objs[tagid])
				objs[tagid] = new NifLib::List<NifLib::Tag *>();
			t->Id = gid++;
			objs[tagid]->Add (t);
			currentL1 = t;
			Tokenize (buf, bl, TAGS_NUML1, TAGS_NUML2);
			currentL1 = NULL;
		} else {
			if (!currentL1) {
				INFO("Parent tag not found")
				delete t;
				return 0;
			}
			currentL1->Tags.Add (t);// subtags should always be sequential
			t->Id = currentL1->Tags.Count () - 1;
			t->Parent = currentL1;
		}
		// parse attributes and add them
		// < attr="val" >cmnt
		// < attr="val"
		int attrid = 0;
		// idx, quotes, comment, tag opener, tag closer
		int i, q = 0, c = 0, to = 0, tc = 0;
		for (i = 0; i < bl; i++) {
			if (!to) to = buf[i] == '<' && !q;
			if (!tc) tc = buf[i] == '>' && !q;
			if (buf[i] == '"')
				q = !q;
			// handle comments
			if (!c && buf[i] == '>' && i + 1 < bl && !q) {
				c = 1;// search only once for comment because of subtags
				int j, q2 = 0;
				for (j = ++i; ; j++) {// checked in the prev. if operator
					if (buf[j] == '"')
						q2 = !q2;
					if (j == bl - 1) {// comment only
						int a, b, len = (j - i) + 1;
						Trim (&a, &b, &buf[i], len);
						if (b >= a)
							t->Value.CopyFrom (&buf[i + a], (b - a) + 1);
						break;
					}
					else if (buf[j] == '<' && !q2) {// comment and subtags
						int a, b, len = (j - i);
						Trim (&a, &b, &buf[i], len);
						if (b >= a)
							t->Value.CopyFrom (&buf[i + a], (b - a) + 1);
						break;
					}
				}
			}
			// handle attributes
			if (buf[i] == '=' && !q && to && !tc) {
				// name: find non-empty char <-, scan to an empty one
				Attr *attr = new Attr();
				int j;
				if (i > 0)
					for (j = i - 1; j > -1; j--)
						if (buf[j] > ' ') {
							int k = j;
							while (--k > -1 && buf[k] > ' ')
								;
							attr->Name = AttrId (&buf[k + 1], j - k);
							break;
						}
				// value: find '"' ->, scan to the next one
				if (i < bl-1)
					for (j = i + 1; j < bl; j++)
						if (buf[j] == '"') {
							int k = j;
							while (++k < bl && buf[k] != '"')
								;
							attr->Value.CopyFrom (&buf[j + 1], k - j - 1);
							/*if (attr->Name == AVERCOND)
								PrintBlockA (&buf[j + 1], k - j - 1);*/
							break;
						}
				// add attribute
				if (ValidAttrId (attr->Name)) {
					if (t->AttrExists (attr->Name)) {
						ERR("Duplicate attribute name: \"" <<
							AttrText (attr->Name) << "\"" << " for tag #" << t->Id)
						delete attr;
					} else {
						attr->Id = attrid++;
						t->Attr.Add(attr);
					}
				} else {
					ERR("Invalid attribute name in tag #" << t->Id
						<< " \"" << TagOpener (t->Name) << "\"")
					delete attr;
				}
			}
		}
		return 1;
	}

	Parser::Parser(const char *fname)
	{
		gid = 0;
		currentL1 = NULL;
		objs.SetLength (TAGS_NUML1);
		Process (fname);
	}

	Parser::~Parser()
	{
		if (!objs.Empty()) {
			for (int i = 0; i < objs.Count (); i++) {
				NifLib::List<NifLib::Tag *> *l = objs[i];
				int j;
				for (j = 0; j < l->Count (); j++)
					delete (*l)[j];
				l->Clear ();
				delete l;
			}
			objs.Clear ();
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
	*	Writes "objs" into a XML file.
	*	Warning - there are no XML Comment "objs".
	*/
	int
	Parser::SaveFile(const char *fname)
	{
		FILE *fh = fopen (fname, "w");
		if (!fh) {
			INFO("SaveFile: fopen failed")
			return 0;
		}
#define FWRITE(BUF,SIZE)\
	if (fwrite (BUF, SIZE, 1, fh) != 1) {\
		INFO("SaveFile: fwrite failed")\
		return 0;\
	}
		if (header.len > 0) {
			FWRITE(header.buf, header.len);
			FWRITE("\n", 1)
		}
		// same sequence as input:
		int count = 0;
		for (int i = 0; i < objs.Count (); i++)
			count += objs[i]->Count();
		NifLib::List<NifLib::Tag *> slist(count);
		for (int i = 0; i < objs.Count (); i++)
			for (int j = 0; j < objs[i]->Count(); j++)
				slist[(*objs[i])[j]->Id] = (*objs[i])[j];
		// write
		for (int i = 0; i < slist.Count (); i++){
			NifLib::Tag *tag = slist[i];
			// l1 tag opener
			FWRITE(TagOpener (tag->Name), TagLen (tag->Name))
			// attributes
			for (int k = 0; k < tag->Attr.Count (); k++) {
				FWRITE(" ", 1)
				FWRITE(AttrText (tag->Attr[k]->Name), AttrLen (tag->Attr[k]->Name))
				FWRITE("=\"", 2)
				FWRITE(tag->Attr[k]->Value.buf,	tag->Attr[k]->Value.len)
				FWRITE("\"", 1)
			}
			//
			FWRITE(">", 1)
			// l1 comment
			if (tag->Value.len > 0) {
				FWRITE("\n\t", 2)
				FWRITE(tag->Value.buf, tag->Value.len)
			}
			FWRITE("\n", 1)
			// l2 tags
			int l;
			for (l = 0; l < tag->Tags.Count(); l++) {
				NifLib::Tag *tl2 = tag->Tags[l];
				FWRITE("\t", 1)
				FWRITE(TagOpener (tl2->Name), TagLen (tl2->Name))
				// l2 attributes
				for (int k = 0; k < tl2->Attr.Count (); k++) {
					FWRITE(" ", 1)
					FWRITE(
						AttrText (tl2->Attr[k]->Name),
						AttrLen (tl2->Attr[k]->Name))
					FWRITE("=\"", 2)
					FWRITE(tl2->Attr[k]->Value.buf, tl2->Attr[k]->Value.len)
					FWRITE("\"", 1)
				}
				//
				FWRITE(">", 1)
				// l2 comment
				if (tl2->Value.len > 0) {
					FWRITE("\n\t\t", 3)
					FWRITE(tl2->Value.buf, tl2->Value.len)
				}
				FWRITE("\n\t", 2)
				FWRITE(TagCloser (tl2->Name), TagLen (tl2->Name) + 2)
				FWRITE("\n", 1)
			}
			// l1 tag closer
			FWRITE(TagCloser (tag->Name), TagLen (tag->Name) + 2)
			FWRITE("\n\n", 2)
		}
		if (footer.len > 0)
			FWRITE(footer.buf, footer.len);
#undef FWRITE
		fclose (fh);
		return -1;
	}

	/*
	*	Moves "a" and "b" to positions not containing white space.
	*	*a <= *b when there is at least one non-white space character
	*/
	void
	Parser::Trim(int *a, int *b, const char *buf, int bl)
	{
		for (*a = 0; *a < bl; (*a)++)
			if (buf[*a] > ' ')
				break;
		for (*b = bl - 1; *b >= 0; (*b)--)
				if (buf[*b] > ' ')
					break;
	}

	/*
	*	Finds "c" starting from "pos" in "buf" while avoiding
	*	whitespaces only.
	*	Returns -1 when not found.
	*/
	int
	Parser::FindPrevw(const char *buf, int pos, char c)
	{
		if (c <= ' ')
			return -1;
		if (pos > 0) {
			if (buf[pos - 1] <= ' ') {// scan at all ?
				while (--pos > -1 && buf[pos] <= ' ')
					if (buf[pos] == c)
						return pos;
			} else {
				if (buf[pos] == c)
					return pos;
			}
		}
		return -1;
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
}
