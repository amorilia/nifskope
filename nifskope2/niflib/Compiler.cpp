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

#include "Compiler.h"
#include "List.h"
#include "Tag.h"
#include "Attr.h"
#include "niflib.h"
#include <cstdlib>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include <string.h>

namespace NifLib
{
	void
	Compiler::AddField(NifLib::Tag *field, char *buf, int bl)
	{
		NifLib::Field *f = new NifLib::Field();
		f->Tag = field;
		f->Value.CopyFrom (buf, bl);
		flist.Add (f);
	}

	/*
	*	Returns whatever "cond" returns. Searches for fields up in "flist"
	*	Not all-purpose evaluator
	*	( no fancy algortihms and/or datastructures here ):
	*	- if you have more than one condition always put brackets:
	*		() && () || () ...
	*		brackets can be omited only when you have exactly one condition:
	*		User Version >= 3.14
	*	- you may have spaces in variable names:
	*		( User Version >= 3.14 )
	*		(User Version >= 3.14)
	*		(UserVersion >= 3.14)
	*	- it can use following consts:
	*		11 - uint
	*		1.1 - version
	*		1.1.2.123 - version
	*	- it expects variable name left and const value right:
	*		( User Version >= 3.14 )
	*		it may work in other scenarios, but its experimental
	*	- it expects following operators between lvalue and rvalue:
	*		'==' '>=' '>' '<' '<=' '&' '!='
	*	- it expects following operators between brackets '(' ')':
	*		'||' '&&'
	*	- it expects following operator before an opening bracket '(':
	*		'!'
	*	- it turns '&amp;' to '&', '&lt;' to '<', '&gt;' to '>'
	*	See "nif.xml" for examples.
	*	If you fail the rules above, the evaluator will fail to evaluate
	*	or may evaluate wrongly.
	*	All suggestions and improvements are welcome of course.
	*/
	int
	Compiler::Evaluate(NifLib::Attr* cond)
	{
		//INFO(std::dec)
		/*NifLib::Attr testcond;
		NifLib::Attr *cond = &testcond;
		const char * test = "(( Version >= 10.0.1.0 ) &amp;&amp; (!(( Version >= 20.2.0.7) &amp;&amp; (User Version == 11 ))))";
		cond->Value.CopyFrom(test, strlen (test));*/
		// TODO: the most used ones like
		// "Version", "User Version" and "User Version 2"
		// can become fields
		// TODO: compile those - not need to evaluate them each time
		char *buf = (char *)NifAlloc (cond->Value.len);
		if (!buf) {
			ERR("Compiler::Evaluate: Out of memory")
			return 0;
		}
		int len = cond->Value.len;
		
		// pass1
		// - "&amp; to &", "&lt; to <", "&gt; to >"
		int i, j = 0;
		for (i = 0; i < len; i++) {
			if (Parser::StartsWith ("&amp;", 5, &(cond->Value.buf[i]), len - i)) {
				buf[j++] = '&';
				i += 4;
			} else
			if (Parser::StartsWith ("&lt;", 4, &(cond->Value.buf[i]), len - i)) {
				buf[j++] = '<';
				i += 3;
			} else
			if (Parser::StartsWith ("&gt;", 4, &(cond->Value.buf[i]), len - i)) {
				buf[j++] = '>';
				i += 3;
			} else
				buf[j++] = cond->Value.buf[i];
		}
		len = j;
		//INFO("E p1: \"" << std::string (buf, len) << "\"")

		// pass2
		// "(User Version == 10) || (User Version == 11)"
		// - variable names contain spaces
		// () delmiters: || &&
		// delimiters: == >= > < <= & !=
		// values: 20.2.0.7 11
		// [(]name delimiter value[)]
		//   "Has Texture Transform"
		//   "Use External == 0"
		//   "(Has Faces) &amp;&amp; (Num Strips != 0)"
		//   "Flags &amp; 2"
		//   "Unknown 12 > 1"
		// find delimiters, if none found => single name probably "bool"
		int bc = 0;// '(' count
		int op = EVAL_OP_NONE;
		int opb = EVAL_OPB_NONE;
		int lt = EVAL_TYPE_UNKNOWN, rt = EVAL_TYPE_UNKNOWN;
		//char *lbuf = NULL;
		//int llen = 0;
		char *rbuf = NULL;
		int rlen = 0;
		NifLib::List<NIFuint> l2;
		for (i = 0; i < len; i++) {
			op = EVAL_OP_NONE;
			opb = EVAL_OPB_NONE;
			lt = EVAL_TYPE_UNKNOWN;
			rt = EVAL_TYPE_UNKNOWN;
			//lbuf = NULL;
			//llen = 0;
			rbuf = NULL;
			rlen = 0;
			if (buf[i] == '!') {
				l2.Add (1);
				l2.Add (0);
			}
			if (buf[i] == '(') {
				bc++;
				l2.Add (2);
				l2.Add (0);
			}
			if (buf[i] == ')') {
				l2.Add (3);
				l2.Add (0);
			}
			if (Parser::StartsWith ("==", 2, &buf[i], len - i)) op = EVAL_OP_EQU;
			else if (Parser::StartsWith (">=", 2, &buf[i], len - i)) op = EVAL_OP_GTEQU;
			else if (buf[i] == '>') op = 3;
			else if (Parser::StartsWith ("<=", 2, &buf[i], len - i)) op = 4;
			else if (buf[i] == '<') op = 5;
			else if (buf[i] == '&') op = 6;
			else if (Parser::StartsWith ("!=", 2, &buf[i], len - i)) op = 7;
			if (Parser::StartsWith ("||", 2, &buf[i], len - i)) opb = EVAL_OPB_OR;
			else if (Parser::StartsWith ("&&", 2, &buf[i], len - i)) opb = EVAL_OPB_AND;
			if (opb) {
				l2.Add (4);
				l2.Add (opb);
				op = 0;// don't mix ops
				i += OPBL[opb];
				//INFO("E : opb #" << opb << " at [" << i << "], len: 2")
			}
			if (op > 0) {
				//INFO("E : op  #" << op << " at [" << i << "], len: " << OPL[op])
				// l_operand: scan <- non-empty..(
				int j;
				NifLib::Field *lf;
				NifLib::Field *rf;
				if (i > 0)
					for (j = i - 1; j > -1; j--)
						if (buf[j] > ' ') {
							int k = j;
							if (bc) {
								while (--k > -1 && buf[k] != '(')
									;
								/*// find out if there is '!'
								if (k > 0)
									ne = Parser::FindPrevw (buf, k - 1, '!') > -1;*/
							}
							else k = 0;// TODO: scan to prev. delimiter
							// "trim" start
							while (++k < i)
								if (buf[k] > ' ')
									break;
							lf = FFBackwards (ANAME, &buf[k], j - k + 1);
							lt = EvalDeduceType (&buf[k], j - k + 1);
							//lbuf = &buf[k];
							//llen = j - k + 1;
							//INFO("E lo: \"" << std::string (&buf[k], j - k + 1) << "\"")
							break;
						}
				// r_operand: scan -> non-empty..)
				if (i < len - OPL[op])
					for (j = i + OPL[op]; j < len; j++)
						if (buf[j] > ' ') {
							int k = j;
							if (bc)
								while (++k < len && buf[k] != ')')
									;
							else k = len - 1;// TODO: scan to next delimiter
							// "trim" end
							while (--k > i + OPL[op])
								if (buf[k] > ' ')
									break;
							rf = FFBackwards (ANAME, &buf[j], k - j + 1);
							rt = EvalDeduceType (&buf[j], k - j + 1);
							rbuf = &buf[j];
							rlen = k - j + 1;
							i = j + rlen - 1;// -1 for the ')'
							//INFO("E ro: \"" << std::string (&buf[j], k - j + 1) << "\"")
							break;
						}
				// evaluate
				/*if (lf) INFO("E: L: [" << "f," << lt << "]")
				else INFO("E: L: [" << " ," << lt << "]")
				if (rf) INFO("E: R: [" << "f," << rt << "]")
				else INFO("E: R: [" << " ," << rt << "]")*/
				NIFuint tmp_result = 0;
				if (lf && rt != EVAL_TYPE_UNKNOWN) {
					// left is a field, right is a const
					if (rt == EVAL_TYPE_VERSION ||
						rt == EVAL_TYPE_UINT) {// supported const type #1 and #2
						NIFuint item;
						if (rt == EVAL_TYPE_VERSION) {
							//INFO("E: EVAL_TYPE_VERSION")
							item = HeaderString2Version (rbuf, rlen);
						} else {//rt == EVAL_TYPE_UINT
							//INFO("E: EVAL_TYPE_UINT")
							item = str2<NIFuint> (std::string (rbuf, rlen));
						}
						if (lf->Value.len > 4) {
							INFO("E: Type mishmash: "
								<< lf->Value.len << " > " << 4)
						} else {
							if (op == EVAL_OP_EQU)
								tmp_result =
									lf->Value.Equals ((const char *)&item, lf->Value.len);
							else if (op == EVAL_OP_GTEQU)
								tmp_result =
									(NIFuint)*(NIFuint *)&(lf->Value.buf[0]) >= item;
							else {
								INFO("E : EVAL_OP no implemented yet: " << op)
							}
						}
					}
				} else {
					INFO("E: Operand combination not implemented yet")
				}
				/*if (ne)
					tmp_result = !tmp_result;*/
				l2.Add (5);
				l2.Add (tmp_result);
				INFO("E : tmp_result: " << tmp_result << ", bc: " << bc)
			} // if (op > 0)
		}// main for
		NifRelease (buf);

		// pass 3 - brackets
		return EvaluateL2 (l2);
	}

	int
	Compiler::EvalDeduceType(const char *val, int len)
	{
		//PrintBlockA (val, len);
		int i, pc = 0, dc = 0;
		for (i = 0; i < len; i++)
			if (val[i] == '.')
				pc++;
			else if (val[i] >= '0' && val[i] <= '9')
				dc++;
			else return EVAL_TYPE_UNKNOWN;
		if (pc && dc)
			return EVAL_TYPE_VERSION;
		if (dc)
			return EVAL_TYPE_UINT;
		return EVAL_TYPE_UNKNOWN;
	}

	/*
	*	Handles brackets.
	*	Works with: uint, EVAL_OPB_OR, EVAL_OPB_AND and "!"
	*	Example: "((1)&&(!((0)&&(1))))"
	*	Note: "(Has Normals) && (TSpace Flag & 240)", so
	*		  here we'll recieve something like:
	*		  "(1)&&(240)"
	*/
	int
	Compiler::EvaluateL2(NifLib::List<NIFuint> &l2)
	{
		//INFO("--")
		int i, k;
		int c1 = 0, c2 = 0, c3 = 0;
		//INFO("EL2 :" << std::string (buf, len))
		/*INFO ("l2: " << l2.Count ())
		for (i = 0; i < l2.Count () - 1; i += 2)
			INFO(l2[i] << " " << l2[i+1])*/
		// 1 - '!', 2 - '(', 3 - ')', 4 - OPB, 5 - res
		// scan for 2 5 3
		NifLib::List<int> t1;
		for (i = 0; i <= l2.Count () - (3 * 2); i += 2)
			if (l2[i] == 2 && l2[i + 2] == 5 && l2[i + 4] == 3) {
				t1.Add (i + 2);
				i += (2 * 2);
				c1++;
			}
			else
				t1.Add (i);
		for (k = i; k < l2.Count (); k += 2)
			t1.Add (k);
		/*INFO ("t1: " << t1.Count ())
		for (i = 0; i < t1.Count (); i++)
			INFO(l2[t1[i]] << " " << l2[t1[i]+1])*/
		// scan for 1 5
		NifLib::List<int> t1a;
		for (i = 0; i <= t1.Count () - 1; i++) {
			if (l2[t1[i]] == 1 &&
				l2[t1[i+1]] == 5) {
				l2[t1[i+1]+1] = !l2[t1[i+1]+1];
				t1a.Add (t1[i + 1]);
				i++;
				c2++;
			}
			else
				t1a.Add (t1[i]);
		}
		for (k = i; k < t1.Count (); k++)
			t1a.Add (t1[k]);
		/*INFO ("t1a: " << t1a.Count ())
		for (i = 0; i < t1a.Count (); i++)
			INFO(l2[t1a[i]] << " " << l2[t1a[i]+1])*/
		// scan for 5 4 5
		NifLib::List<NIFuint> t2;
		for (i = 0; i <= t1a.Count () - 3; i++) {
			if (l2[t1a[i]] == 5 &&
				l2[t1a[i+1]] == 4 &&
				l2[t1a[i+2]] == 5) {
				t2.Add (5);
				if ( l2[t1a[i+1] + 1] == EVAL_OPB_OR )
					t2.Add (l2[t1a[i]+1] || l2[t1a[i+2]+1]);
				else// ( l2[t1[i+2] + 1] == EVAL_OPB_AND )
					t2.Add (l2[t1a[i]+1] && l2[t1a[i+2]+1]);
				i += 2;
				c3++;
			}
			else {
				t2.Add (l2[t1a[i]]);
				t2.Add (l2[t1a[i]+1]);
			}
		}
		for (k = i; k < t1a.Count (); k++) {
			t2.Add (l2[t1a[k]]);
			t2.Add (l2[t1a[k]+1]);	
		}
		/*INFO("t2: " << t2.Count ())
		for (i = 0; i < t2.Count () - 1; i += 2)
			INFO(t2[i] << " " << t2[i+1])*/
		if (t2.Count () > 2) {
			if (c1 == 0 && c2 == 0 && c3 == 0) {
				// nothing is changed and no result
				ERR("EvaluateL2 wrong expression")
				return 0;
			}
			else
				return EvaluateL2 (t2);
		} else
			return t2[1];
	}

	NifLib::Field *
	Compiler::FFBackwards(int attrid, const char *val, int len)
	{
		int i;
		for (i = flist.Count () - 1; i > -1; i--) {
			NifLib::Tag *t = flist[i]->Tag;
			NifLib::Attr *a = t->AttrById (attrid);
			if (a && a->Value.Equals(val, len))
				return flist[i];
		}
		return NULL;
	}

	Compiler::Compiler(const char *fname)
		: Parser (fname)
	{
	}

	Compiler::~Compiler()
	{
		int i;
		for (i = 0; i < flist.Count (); i++)
			delete flist[i];
		flist.Clear ();
	}

	NIFuint
	Compiler::HeaderString2Version(const char *buf, int bl)
	{
		NIFuint r = 0;
		int i = bl - 1;
		for (; i > -1; i--)
			if (buf[i] != '.' && (buf[i] < '0' || buf[i] > '9'))
				break;
		i++;
		//INFO("Version: ")
		//PrintBlockA (&buf[i], bl - i);
		for (int d = 0; d < 4; d++) {
			int j = i;
			while (j < bl && buf[j] != '.')
				j++;
			int len = j - i;
			NIFuint k = str2<NIFuint> (std::string (&buf[i], len));
			r = r | (k << (4 - (d + 1))*8);
			i = j + 1;
			if (i >= bl)
				break;// no more digits
		}
		//PrintBlockB ((char *)&r, 4, 16);
		//INFO("\"" << std::setfill ('0') << std::hex << std::uppercase << r << "\"")
		return r;
	}

	void
	Compiler::ReadNif(const char *fname)
	{
		NifLib::Tag *t = Find (TCOMPOUND, ANAME, "Header", 6);
		NifLib::Attr *tname = t->AttrById (ANAME);
		if (!t) {
			ERR("Missing header structure")
			return;
		}
		INFO("Found header structure")
		NifStream s(fname, 1*1024*1024);
		INFO("Loaded \"" << fname << "\"")

#define SFIELD(L1, L2)\
	"\"" << std::string (L1->Value.buf, L1->Value.len)\
	<< "." << std::string (L2->Value.buf, L2->Value.len) << "\""
#define HEX(N) std::setw (N) << std::setfill ('0') << std::hex << std::uppercase
#define DEC std::dec
		INFO("R: Header")
		for (int i = 0; i < t->Tags.Count (); i++) {// its kinda CS:IP :)
			NifLib::Tag *field = t->Tags[i];// a field
			NifLib::Attr *ftype = field->AttrById (ATYPE);// field type
			NifLib::Attr *fname = field->AttrById (ANAME);// field name
			if (!ftype) {// field must have a type
				INFO("Unknown type for l2 tag in l1 tag #" << t->Id)
				continue;
			}
			if (!V12Check (field)) {
				INFO("R: " << SFIELD(tname, fname)	<< ": *** V12Check")
				continue;
			}

			NifLib::Tag *tt = Find (TBASIC, ANAME, ftype->Value.buf, ftype->Value.len);
			if (!tt) {// try find out if its 'enum'
				tt = Find (TENUM, ANAME, ftype->Value.buf, ftype->Value.len);
				if (tt) {
					NifLib::Attr *estorage = tt->AttrById (ASTORAGE);
					tt = Find (TBASIC, ANAME, estorage->Value.buf, estorage->Value.len);
				}
			}
			if (!tt) {// try find out if its 'bitflags'
				tt = Find (TBITFLAGS, ANAME, ftype->Value.buf, ftype->Value.len);
				if (tt) {
					NifLib::Attr *bfstorage = tt->AttrById (ASTORAGE);
					tt = Find (TBASIC, ANAME, bfstorage->Value.buf, bfstorage->Value.len);
				}
			}
			if (tt) {// *** its a basic type
				NifLib::Attr *ta = tt->AttrById (ANIFLIBTYPE);
				//NifLib::Attr *tarr1 = field->AttrById (AARR1);
				NifLib::Attr *tcond = field->AttrById (ACOND);
				if (tcond) {// we have a condition :)
					INFO("*R :" << "\""
						<< std::string (tcond->Value.buf, tcond->Value.len) << "\"")
					int eval_result = Evaluate (tcond);
					INFO("*R : " << eval_result)
					if (!eval_result)
						continue;
				}
				// <add name="Header String" type="HeaderString">
				// *** read basic type "HeaderString" mapped to our code here:
				//     it has no attribute description in the XML so its
				//     bhaviour is hard-coded
				if (ta->Value.Equals ("HeaderString", 12) ||
					ta->Value.Equals ("LineString", 10)) {
					const NIFint BS = 1024;
					NIFchar buf[BS];
					NIFint rr = s.ReadCharCond (&buf[0], BS, '\n');
					if (rr <= BS) {
						//PrintBlockB (&buf[0], rr, 16);
						INFO("R hstr: " << SFIELD(tname, fname)
							<< ": \"" << std::string (&buf[0], rr - 1) << "\"")
						AddField (field, &buf[0], rr);
					} else {// file format not supported
						ERR("ReadCharCond failed")
					}
					// TODO: see to it that Build() turns those to NIFuint
					nVersion = HeaderString2Version (&buf[0], rr - 1);
				} else
				if (ta->Value.Equals ("unsigned int", 12)) {
					const NIFint BS = 1;
					NIFuint buf[BS];
					NIFint rr = s.ReadUInt (&buf[0], BS);
					if (rr != 4*BS) {// read failed
						ERR("ReadUInt failed")
					}
					INFO("R uint: " << SFIELD(tname, fname) << ": \""
						<< DEC << buf[0] << " " << HEX(8) << buf[0] << "\"")
					AddField (field, (char *)&buf[0], 4*BS);
				} else
				if (ta->Value.Equals ("byte", 4)) {
					const NIFint BS = 1;
					NIFbyte buf[BS];
					NIFint rr = s.ReadByte (&buf[0], BS);
					if (rr != 1*BS) {// read failed
						ERR("ReadByte failed")
					}
					INFO("R byte: " << SFIELD(tname, fname) << ": \""
						<< DEC << (int)buf[0] << " " << HEX(2) << (int)buf[0] << "\"")
					AddField (field, (char *)&buf[0], 1*BS);
				} else
					INFO("R: " << SFIELD(tname, fname) << ": *** not implemented"
					<< " (" << std::string (ta->Value.buf, ta->Value.len) << ")")
			} else {
				INFO("R: " << SFIELD(tname, fname) << ": *** not basic"
				<< " (" << std::string (ftype->Value.buf, ftype->Value.len) << ")")
			}
		}
#undef DEC
#undef HEX
#undef SFIELD
	}

	bool
	Compiler::V12Check(NifLib::Tag *field)
	{
		NifLib::Attr *av1 = field->AttrById (AVER1);
		NifLib::Attr *av2 = field->AttrById (AVER2);
		if (!av1 && !av2)
			return true;
		else if (!av1 && av2) {
			NIFuint v2 = HeaderString2Version (av2->Value.buf, av2->Value.len);
			return nVersion <= v2;
		}
		else if (av1 && !av2) {
			NIFuint v1 = HeaderString2Version (av1->Value.buf, av1->Value.len);
			return nVersion >= v1;
		} else {
			NIFuint v1 = HeaderString2Version (av1->Value.buf, av1->Value.len);
			NIFuint v2 = HeaderString2Version (av2->Value.buf, av2->Value.len);
			return nVersion >= v1 && nVersion <= v2;
		}
	}

	/*
	*	Find l1 tag with (attribute=="attrvalue") with attrvalue "len"
	*/
	NifLib::Tag *
	Compiler::Find(int tagid, int attrid, const char *attrvalue, int len)
	{
		// TODO: void Compiler::Build() should make this fast
		//       and it should be very fast
		for (int i = 0; i < objs[tagid]->Count (); i++) {
			NifLib::Tag *t = (*objs[tagid])[i];
			NifLib::Attr *a = t->AttrById(attrid);
			if (a && a->Value.len == len && !strncmp (a->Value.buf, attrvalue, len))
				return t;
		}
		return NULL;
	}

	void
	Compiler::Build()
	{
		std::map<std::string, int (Compiler::*) (NifStream *, char *)> r;
		r["foo"] = &Compiler::Read_bool;
		int (Compiler::*f) (NifStream *, char *)  = r["foo"];
		(this->*f)(NULL, NULL);

		for (int i = 0; i < objs[TBASIC]->Count (); i++) {
			NifLib::Tag *t = (*objs[TBASIC])[i];
			INFO(TagOpener (TBASIC) << "("
				<< "Name: " << t->Name << ", "
				<< "Id: " << t->Id << ", "
				<< "Attributes: " << t->Attr.Count ())
			for (int j = 0; j < t->Attr.Count (); j++)
				INFO("\t"
					<< "Name: " << AttrText(t->Attr[j]->Name) << ", "
					<< "Id: " << t->Attr[j]->Id << ", "
					<< "Value: " << std::string (t->Attr[j]->Value.buf, t->Attr[j]->Value.len))
		}
	}

	int
	Compiler::Read_bool(NifStream *s, char *b)
	{
		INFO("hi")
		return 1;
	}
}
