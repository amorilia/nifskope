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
#include "TreeNode.h"

namespace NifLib
{
#define MLEN 14
struct T { struct timeval ta, tb; int c; long s; const char *n; } M[MLEN] =
{{{0, 0}, {0, 0}, 0, 0, "AddField"},
{{0, 0}, {0, 0}, 0, 0, "AddNode"},
{{0 ,0}, {0, 0}, 0, 0, "Evaluate"},
{{0, 0}, {0, 0}, 0, 0, "EvalDeduceType"},
{{0, 0}, {0, 0}, 0, 0, "EvaluateL2"},
{{0 ,0}, {0, 0}, 0, 0, "FFBackwards"},
{{0, 0}, {0, 0}, 0, 0, "HeaderString2Version"},
{{0, 0}, {0, 0}, 0, 0, "GetBasicType"},
{{0 ,0}, {0, 0}, 0, 0, "InitArr"},
{{0, 0}, {0, 0}, 0, 0, "ReadObject"},
{{0, 0}, {0, 0}, 0, 0, "V12Check"},
{{0 ,0}, {0, 0}, 0, 0, "Find"},
{{0, 0}, {0, 0}, 0, 0, "FFVersion"},
{{0, 0}, {0, 0}, 0, 0, "BType"}};
#define AC(N)
#define AT(N)
#define A(N)
#define B(N)
/*#define AC(N) { M[N].c++; }
#define AT(N) { gettimeofday (&(M[N].ta), NULL); }
#define A(N)\
{\
	M[N].c++;\
	gettimeofday (&(M[N].ta), NULL);\
}
#define B(N)\
{\
	gettimeofday (&(M[N].tb), NULL);\
	M[N].s += time_interval (&(M[N].ta), &(M[N].tb)) / (1);\
}*/
#define STDSTR(BUF) std::string (BUF.buf, BUF.len)

	NifLib::Field *
	Compiler::AddField(NifLib::Tag *field, char *buf, int bl, int type)
	{
		A(0)
		NifLib::Field *f = new NifLib::Field();
		f->BlockTag = blockTag;
		f->JField = i2j;
		f->Tag = field;
		f->NLType = type;
		if (buf && bl > 0)
			f->Value.CopyFrom (buf, bl);
		flist.Add (f);
		// defines the view
		NifLib::Attr *fname = field->AttrById (ANAME);
		std::string key (fname->Value.buf, fname->Value.len);
		if (fview_aname.find (key) == fview_aname.end ())
			fview_aname[key] = new NifLib::List<int> ();
		NifLib::List<int> *lst = fview_aname[key];
		lst->Add(flist.Count () - 1);
		B(0)
		return f;
	}

	NifLib::TreeNode<NifLib::Field *> *
	Compiler::AddNode(
			NifLib::Tag *t,
			NifLib::Field *f,
			NifLib::TreeNode<NifLib::Field *> *pnode)
	{
		A(1)
		NifLib::TreeNode<NifLib::Field *> *node = new NifLib::TreeNode<NifLib::Field *>;
		node->Parent = pnode;
		if (!f) {
			f = new NifLib::Field ();
			f->BlockTag = blockTag;
			f->JField = i2j;
			f->Tag = t;
			f->Value.CopyFrom ("", 1);
			node->OwnsValue = 1;// indicate that the node destructor should release
		}
		node->Value = f;
		pnode->Nodes.Add (node);
		node->Index = pnode->Nodes.Count () - 1;
		B(1)
		return node;
	}

	NifLib::Field *
	Compiler::FFBackwards(const char *val, int len)
	{
		A(5)
		if (!val || len <= 0) {
			B(5)
			return NULL;
		}
		std::string key (val, len);
		std::map<std::string, NifLib::List<int> *>::iterator v =
			fview_aname.find (key);
		if (v == fview_aname.end ()) {
			B(5)
			return NULL;
		}
		else {
			NifLib::List<int> *lst = v->second;
			B(5)
			return flist[(*lst)[lst->Count () - 1]];
		}
	}

	/*
	*	FindField Version. Special proc to look for "Version",
	*	"User Version" and "User Version 2" since they're the only
	*	two fields that are not in the block currently being read
	*	except for TCOMPOUND "Header".
	*/
	NifLib::Field *
	Compiler::FFVersion(const char *val, int len)
	{
		A(12)
		if (len == 14 && !strncmp (val, "User Version 2", 14)) {
			B(12)
			return fUserVersion2;
		}
		else if (len == 12 && !strncmp (val, "User Version", 12)) {
			B(12)
			return fUserVersion;
		}
		else if (len == 7 && !strncmp (val, "Version", 7)) {
			B(12)
			return fVersion;
		}
		else {
			B(12)
			return NULL;
		}
	}

	int
	Compiler::FFBackwardsIdx(int attrid, const char *val, int len)
	{
		int i;
		for (i = flist.Count () - 1; i > -1; i--) {
			NifLib::Tag *t = flist[i]->Tag;
			NifLib::Attr *a = t->AttrById (attrid);
			if (a && a->Value.Equals(val, len))
				return i;
		}
		return -1;
	}

	bool
	Compiler::V12Check(NifLib::Tag *field)
	{
		A(10)
		NifLib::Attr *av1 = field->AttrById (AVER1);
		NifLib::Attr *av2 = field->AttrById (AVER2);
		if (!av1 && !av2) {
			B(10)
			return true;
		}
		else if (!av1 && av2) {
			NIFuint v2 = HeaderString2Version (av2->Value.buf, av2->Value.len);
			B(10)
			return nVersion <= v2;
		}
		else if (av1 && !av2) {
			NIFuint v1 = HeaderString2Version (av1->Value.buf, av1->Value.len);
			B(10)
			return nVersion >= v1;
		} else {
			NIFuint v1 = HeaderString2Version (av1->Value.buf, av1->Value.len);
			NIFuint v2 = HeaderString2Version (av2->Value.buf, av2->Value.len);
			B(10)
			return nVersion >= v1 && nVersion <= v2;
		}
	}

	/*
	*	Find l1 tag with (ANAME=="attrvalue") with attrvalue "len"
	*/
	NifLib::Tag *
	Compiler::Find(int tagid, const char *attrvalue, int len)
	{
		A(11)
		std::map<std::string, int>::iterator it;
		if ((it = tagnames_cache[tagid].find (std::string (attrvalue, len))) !=
			tagnames_cache[tagid].end ()) {
			if (it->second == -1) {
				B(11)
				return NULL;
			}
			else {
				B(11)
				return (*objs[tagid])[it->second];
			}
		} else {
			for (int i = 0; i < objs[tagid]->Count (); i++) {
				NifLib::Tag *t = (*objs[tagid])[i];
				NifLib::Attr *a = t->AttrById (ANAME);
				if (a && a->Value.len == len &&	!strncmp (a->Value.buf, attrvalue, len)) {
					tagnames_cache[tagid][std::string (attrvalue, len)] = i;
					B(11)
					return t;
				}
			}
			tagnames_cache[tagid][std::string (attrvalue, len)] = -1;
			B(11)
			return NULL;
		}
	}

	int
	Compiler::BType(const char *buf, int bl)
	{
		A(13)
		std::string key (buf, bl);
		std::map<std::string, int>::iterator v = btypes_cache.find (key);
		if (v == btypes_cache.end ()) {
			B(13)
			return NIFT_T;
		}
		else {
			B(13)
			return v->second;
		}
	}

	/*
	*	Evaluates expressions. Searches for fields up in "flist".
	*	Not all-purposes evaluator.
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
	*		TODO:it may work in other scenarios later
	*	- it expects following operators between lvalue and rvalue:
	*		'==' '>=' '>' '<' '<=' '&' '!=' '-'
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
		A(2)
		int len = cond->Value.len;
		std::stringstream buf1;
		
		// pass1
		// - "&amp; to &", "&lt; to <", "&gt; to >"
		int i;//, j = 0;
		for (i = 0; i < len; i++) {
			if (Parser::StartsWith ("&amp;", 5, &(cond->Value.buf[i]), len - i)) {
				buf1 << '&';
				i += 4;
			} else
			if (Parser::StartsWith ("&lt;", 4, &(cond->Value.buf[i]), len - i)) {
				buf1 << '<';
				i += 3;
			} else
			if (Parser::StartsWith ("&gt;", 4, &(cond->Value.buf[i]), len - i)) {
				buf1 << '>';
				i += 3;
			} else
			if (Parser::StartsWith ("ARG", 3, &(cond->Value.buf[i]), len - i)) {
				if (ARG) {
					buf1 << std::string (ARG->Value.buf, ARG->Value.len);
				} else {
					ERR("E: ARG in text, but not as a field")
					B(2)
					return 0;
				}
				i += 2;
			} else
				buf1 << cond->Value.buf[i];
		}
		std::string buf1s = buf1.str ();
		len = buf1s.length ();
		const char *buf = buf1s.c_str ();

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
		const char *lbuf = NULL;
		int llen = 0;
		const char *rbuf = NULL;
		int rlen = 0;
		NifLib::List<NIFuint> l2;
		for (i = 0; i < len; i++) {
			op = EVAL_OP_NONE;
			opb = EVAL_OPB_NONE;
			lt = EVAL_TYPE_UNKNOWN;
			rt = EVAL_TYPE_UNKNOWN;
			lbuf = NULL;
			llen = 0;
			rbuf = NULL;
			rlen = 0;
			if (buf[i] == '!' && i < (len -1) && buf[i + 1] != '=') {
				l2.Add (1);
				l2.Add (0);
			}
			if (buf[i] == '(') {
				bc++;
				l2.Add (2);
				l2.Add (i);
			}
			if (buf[i] == ')') {
				l2.Add (3);
				l2.Add (i);
			}
			if (Parser::StartsWith ("==", 2, &buf[i], len - i)) op = EVAL_OP_EQU;
			else if (Parser::StartsWith (">=", 2, &buf[i], len - i)) op = EVAL_OP_GTEQU;
			else if (buf[i] == '>') op = EVAL_OP_GT;
			else if (Parser::StartsWith ("<=", 2, &buf[i], len - i)) op = EVAL_OP_LTEQU;
			else if (buf[i] == '<') op = EVAL_OP_LT;
			else if (buf[i] == '&') op = EVAL_OP_AND;
			else if (Parser::StartsWith ("!=", 2, &buf[i], len - i)) op = EVAL_OP_NOTEQU;
			else if (buf[i] == '-') op = EVAL_OP_SUB;
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
				NifLib::Field *lf = NULL;
				NifLib::Field *rf = NULL;if(rf);
				if (i > 0)
					for (j = i - 1; j > -1; j--)
						if (buf[j] > ' ') {
							int k = j;
							if (bc)
								while (--k > -1 && buf[k] != '(')
									;
							else k = -1;// TODO: scan to prev. delimiter
							// "trim" start
							while (++k < i)
								if (buf[k] > ' ')
									break;
							llen = j - k + 1;
							lbuf = &buf[k];
							lt = EvalDeduceType (lbuf, llen);
							if (lt == EVAL_TYPE_UNKNOWN) {
								lf = FFBackwards (lbuf, llen);
								if (!lf)
									lf = FFVersion (lbuf, llen);
							}
							//INFO("E lo: \"" << std::string (lbuf, llen) << "\"")
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
							else k = len;// TODO: scan to next delimiter
							// "trim" end
							while (--k > i + OPL[op])
								if (buf[k] > ' ')
									break;
							rlen = k - j + 1;
							rbuf = &buf[j];
							rt = EvalDeduceType (rbuf, rlen);
							if (rt == EVAL_TYPE_UNKNOWN)
								rf = FFBackwards (rbuf, rlen);
							i = j + rlen - 1;// -1 for the ')'
							//INFO("E ro: \"" << std::string (rbuf, rlen) << "\"")
							break;
						}
				// evaluate
				NIFuint ritem;
				if (rt == EVAL_TYPE_VERSION) {
					//INFO("ER: EVAL_TYPE_VERSION")
					ritem = HeaderString2Version (rbuf, rlen);
				} else if (rt == EVAL_TYPE_UINT) {
					//INFO("ER: EVAL_TYPE_UINT")
					ritem = str2<NIFuint> (std::string (rbuf, rlen));
				}
				NIFuint litem;
				if (lt == EVAL_TYPE_VERSION) {
					//INFO("E: EVAL_TYPE_VERSION")
					litem = HeaderString2Version (lbuf, llen);
				} else if (lt == EVAL_TYPE_UINT) {
					//INFO("E: EVAL_TYPE_UINT")
					litem = str2<NIFuint> (std::string (lbuf, llen));
				}
				/*if (lf) INFO("E: L: [" << "f," << lt << "]")
				else INFO("E: L: [" << " ," << lt << "]")
				if (rf) INFO("E: R: [" << "f," << rt << "]")
				else INFO("E: R: [" << " ," << rt << "]")*/
				NIFuint tmp = 0;
				if (lt != EVAL_TYPE_UNKNOWN && rt != EVAL_TYPE_UNKNOWN) {
					if (op == EVAL_OP_EQU)
						tmp = litem == ritem;
					else if (op == EVAL_OP_GTEQU)
						tmp = litem >= ritem;
					else if (op == EVAL_OP_GT)
						tmp = litem > ritem;
					else if (op == EVAL_OP_LTEQU)
						tmp = litem <= ritem;
					else if (op == EVAL_OP_LT)
						tmp = litem < ritem;
					else if (op == EVAL_OP_AND)
						tmp = litem & ritem;
					else if (op == EVAL_OP_NOTEQU)
						tmp = litem != ritem;
					else if (op == EVAL_OP_SUB)
						tmp = litem - ritem;
					else {
						INFO("E [const, const]: EVAL_OP not supported yet: " << op)
					}
				}
				if (lf && rt != EVAL_TYPE_UNKNOWN) {
					// left is a field, right is a const
					if (rt == EVAL_TYPE_VERSION ||
						rt == EVAL_TYPE_UINT) {// supported const type #1 and #2
						if (lf->Value.len > 4) {
							INFO("E: Type mishmash: "
								<< lf->Value.len << " > " << 4)
						} else {
							//INFO ("lf: " << StreamBlockB (lf->Value.buf, lf->Value.len,
							//	lf->Value.len + 1) << ", ritem: " << ritem)
							if (op == EVAL_OP_EQU)
								tmp = lf->AsNIFuint () == ritem;
							else if (op == EVAL_OP_GTEQU)
								tmp = lf->AsNIFuint () >= ritem;
							else if (op == EVAL_OP_GT)
								tmp = lf->AsNIFuint () > ritem;
							else if (op == EVAL_OP_LTEQU)
								tmp = lf->AsNIFuint () <= ritem;
							else if (op == EVAL_OP_LT)
								tmp = lf->AsNIFuint () < ritem;
							else if (op == EVAL_OP_AND)
								tmp = lf->AsNIFuint () & ritem;
							else if (op == EVAL_OP_NOTEQU)
								tmp = lf->AsNIFuint () != ritem;
							else if (op == EVAL_OP_SUB)
								tmp = lf->AsNIFuint () - ritem;
							else {
								INFO("E : EVAL_OP not supported yet: " << op)
							}
						}
					}
				} else {
					//INFO("E: Operand combination not supported yet")
				}
				l2.Add (5);
				l2.Add (tmp);
				//INFO("E : tmp: " << tmp << ", bc: " << bc)
			} // if (op > 0)
		}// main for

		//E p1: "(Has Faces) && (Num Strips == 0)"
		// 2 3
		int sl = l2.Count () / 2;
		for (i = 0; i < sl - 1; i++)
			if (l2[2 * i] == 2 && l2[(2 * (i + 1))] == 3) {
				int pos = l2[(2 * i) + 1];
				while (++pos < len && buf[pos] <= ' ')// move to first non-empty ->
					;
				int pos2 = l2[(2 * (i + 1)) + 1];
				while (--pos2 >= pos && buf[pos2] <= ' ')// move to first non-empty <-
					;
				int length = (pos2 - pos) + 1;
				//INFO("E: field in brackets, no op: " << std::string (&buf[pos], length))
				NifLib::Field *f = FFBackwards (&buf[pos], length);
				if (f) {
					//INFO("E: field found: " << f->AsNIFuint ())
					NIFuint val = f->AsNIFuint ();
					l2.Insert ((2 * (i + 1)), 5);
					l2.Insert ((2 * (i + 1))+1, val);
				}// else INFO("E: field not found")
			}

		// a field probably - "Has Faces" for example
		if (l2.Count () <= 0) {
			NifLib::Field *v =
				FFBackwards(buf, len);
			if (v) {
				B(2)
				return (int)v->AsNIFuint ();
			} else {
				//INFO("E: *** can't evaluate that")
				B(2)
				return 0;// the above is not necessary an error - return false
			}
		}

		// no brackets
		if (!bc && l2.Count () > 1) {
			B(2)
			return l2[1];
		}

		// pass 3 - brackets
		// TODO: cache queries
		B(2)
		return EvaluateL2 (l2);
	}

	int
	Compiler::EvalDeduceType(const char *val, int len)
	{
		A(3)
		//PrintBlockA (val, len);
		int i, pc = 0, dc = 0;
		for (i = 0; i < len; i++)
			if (val[i] == '.')
				pc++;
			else if (val[i] >= '0' && val[i] <= '9')
				dc++;
			else {
				B(3)
				return EVAL_TYPE_UNKNOWN;
			}
		if (pc && dc) {
			B(3)
			return EVAL_TYPE_VERSION;
		}
		if (dc) {
			B(3)
			return EVAL_TYPE_UINT;
		}
		B(3)
		return EVAL_TYPE_UNKNOWN;
	}

	/*
	*	Handles brackets.
	*	Works with: uint, EVAL_OPB_OR, EVAL_OPB_AND and "!"
	*	Example: "((1)&&(!((0)&&(1))))"
	*	Note: "(Has Normals) && (TSpace Flag & 240)", so
	*		  here we'll receive something like:
	*		  "(1)&&(240)"
	*/
	int
	Compiler::EvaluateL2(NifLib::List<NIFuint> &l2)
	{
		A(4)
		//INFO("--")
		if (l2.Count () < 2) {
			B(4)
			throw "EvaluateL2: invalid argument";
		}
		int i, k;
		int c1 = 0, c2 = 0, c3 = 0;
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
				B(4)
				return 0;
			}
			else {
				B(4)
				return EvaluateL2 (t2);
			}
		} else {
			B(4)
			return t2[1];
		}
	}

	Compiler::Compiler(const char *fname)
		: Parser (fname)
	{
		nVersion = 0;
		nUserVersion = 0;
		nUserVersion2 = 0;
		blockTag = NULL;
		TEMPLATE = NULL;
		fVersion = NULL;
		fUserVersion = NULL;
		fUserVersion2 = NULL;
		i2j = NULL;
		for (int i = 0; i < BTYPESNUM; i++) {
			btypes_cache[std::string (BTYPES[i].nval)] = (i << 8) | BTYPES[i].type;
			if (btypes_cache.find (std::string (BTYPES[i].lval)) ==
				btypes_cache.end ())
				btypes_cache[std::string (BTYPES[i].lval)] = (i << 8) | BTYPES[i].type;
		}
	}

	Compiler::~Compiler()
	{
		Reset ();
		strversion_cache. clear ();
		btypes_cache. clear ();
		for (int i = 0; i < TAGS_NUML1; i++)
			tagnames_cache[i].clear ();
	}

	void
	Compiler::Reset()
	{
		nVersion = 0;
		nUserVersion = 0;
		nUserVersion2 = 0;
		blockTag = NULL;
		TEMPLATE = NULL;
		ARG = NULL;
		fVersion = NULL;
		fUserVersion = NULL;
		fUserVersion2 = NULL;
		i2j = NULL;
		int i;
		for (i = 0; i < flist.Count (); i++)
			delete flist[i];
		flist.Clear ();
		ftree.Clear ();

		Reset_FieldViewAName ();
	}

	NifLib::Field *
	Compiler::operator[](int index)
	{
		return flist[index];
	}

	int
	Compiler::FCount()
	{
		return flist.Count ();
	}

	void
	Compiler::Reset_FieldViewAName()
	{
		std::map<std::string, NifLib::List<int> *>::iterator it;
		for (it = fview_aname.begin (); it != fview_aname.end (); it++) {
			NifLib::List<int> *lst = it->second;
			lst->Clear ();
			delete lst;
		}
		fview_aname.clear ();
	}

	NIFuint
	Compiler::HeaderString2Version(const char *buf, int bl)
	{
		A(6)
		std::map<std::string, NIFuint>::iterator it;
		if ((it = strversion_cache.find (std::string (buf, bl))) !=
			strversion_cache.end ()) {
			B(6)
			return it->second;
		}
		else {
			NIFuint r = 0;
			int i = bl - 1;
			for (; i > -1; i--)
				if (buf[i] != '.' && (buf[i] < '0' || buf[i] > '9'))
					break;
			i++;
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
			strversion_cache[std::string (buf, bl)] = r;
			B(6)
			return r;
		}
	}

	/*
	*	Return TBASIC for that ATYPE attribute if there is TBASIC
	*	reachable for it.
	*/
	NifLib::Tag *
	Compiler::GetBasicType(NifLib::Attr *type)
	{
		A(7)
		NifLib::Tag *tag = Find (TBASIC, type->Value.buf, type->Value.len);
		if (!tag) {// try find out if its TENUM
			tag = Find (TENUM, type->Value.buf, type->Value.len);
			if (tag) {
				NifLib::Attr *t2 = tag->AttrById (ASTORAGE);
				tag = Find (TBASIC, t2->Value.buf, t2->Value.len);
			}
		}
		if (!tag) {// try find out if its TBITFLAGS
			tag = Find (TBITFLAGS, type->Value.buf, type->Value.len);
			if (tag) {
				NifLib::Attr *t2 = tag->AttrById (ASTORAGE);
				tag = Find (TBASIC, t2->Value.buf, t2->Value.len);
			}
		}
		B(7)
		return tag;
	}

	/*
	*	Initialise AARR attribute. Set *i2j to an array field,
	*	if any, to indicate jagged array
	*/
	NIFint
	Compiler::InitArr(NifLib::Attr *arr)
	{
		A(8)
		NIFint result = 1;
		if (arr) {
			//INFO ("Query InitArr for \"" << std::string (arr->Value.buf, arr->Value.len) << "\"")
			//PrintBlockA (arr->Value.buf, arr->Value.len);
			if (IsUInt (arr->Value.buf, arr->Value.len))
				result = str2<NIFint> (std::string (arr->Value.buf, arr->Value.len));
			else {// not a const int
				NifLib::Field *v =
					FFBackwards (arr->Value.buf, arr->Value.len);
				if (v) {// a field
					if (v->Tag->AttrById (AARR1))
						i2j = v;// jagged - v is an array field
					else
						result = v->AsNIFuint ();// can be 0
				}
				else {// an expression
					result = Evaluate (arr);// TODO: error handling
					//INFO("Block #" << STDSTR(blockTag->Value)
					//	<< ", InitArr expression: \"" << STDSTR(arr->Value) << "\""
					//	<< ", result: " << result)
				}
			}
		}
		B(8)
		return result;
	}

	int
	Compiler::ReadObject(
		NifStream &s, NifLib::Tag *t, NifLib::TreeNode<NifLib::Field *> *n)
	{
		AC(9)
		if (!t) {
			ERR("R: Missing tag")
			return 0;
		}
		NifLib::Attr *tname = t->AttrById (ANAME);
		if (!tname) {
			ERR("R: A tag should have a name")
			return 0;
		}
		/*INFO("Reading \""
			<< std::string (tname->Value.buf, tname->Value.len) << "\"")*/
		// handle "inherit"
		NifLib::Attr *p = t->AttrById (AINHERIT);
		if (p) {
			NifLib::Tag *tp = Find (TCOMPOUND, p->Value.buf, p->Value.len);
			if (!tp)
				tp = Find (TNIOBJECT, p->Value.buf, p->Value.len);
			if (!tp) {
				ERR("R: Unknown parent block for \""
					<< std::string (tname->Value.buf, tname->Value.len) << "\"")
				return 0;
			}
			if (!ReadObject (s, tp, AddNode(tp, NULL, n))) {
				return 0;
			}
		}
#define HEX(N) std::setw (N) << std::setfill ('0') << std::hex << std::uppercase
#define DEC std::dec
#define READ(BT, BYTES, RT, CNT, TYPE)\
{\
	if (CNT > 0 && BYTES > 0) {\
		BT *buf;\
		buf = (BT *)NifAlloc (BYTES);\
		if (!buf) {\
			ERR("R: Out of memory")\
			return 0;\
		}\
		NIFint rr = s.Read##RT (&buf[0], CNT);\
		if (rr != BYTES) {\
			ERR("R: Read"#RT" failed")\
			NifRelease (buf);\
			return 0;\
		}\
		AddNode (field, AddField (field, (char *)&buf[0], BYTES, TYPE), n);\
		POS += rr;\
		NifRelease (buf);\
	}\
}
		NifLib::Attr *ARGsentinel = NULL;
		for (int i = 0; i < t->Tags.Count (); i++) {// its kinda CS:IP :)
			if (ARGsentinel) ARG = ARGsentinel;
			NifLib::Tag *field = t->Tags[i];// a field
			NifLib::Attr *ftype = field->AttrById (ATYPE);// field type
			if (!ftype) {// field must have a type
				INFO("R: Unknown type for L2 tag in L1 tag #" << t->Id)
				return 0;
			}
			if (!V12Check (field))// "AVER1" "AVER2"
				continue;
			NifLib::Attr *userver = field->AttrById (AUSERVER);
			if (userver) {// can be a const
				NIFuint uv =
					str2<NIFint> (std::string (userver->Value.buf, userver->Value.len));
				if (uv != nUserVersion)
					continue;
			}
			// ARG
			NifLib::Attr *tmp = field->AttrById (AARG);
			ARGsentinel = ARG;
			if (tmp) {
				ARG = field->AttrById (AARG);
			}
			tmp = field->AttrById (ATEMPLATE);
			if (tmp && !tmp->Value.Equals ("TEMPLATE", 8))
				TEMPLATE = tmp;// keep last template type
			NifLib::Attr *acond = field->AttrById (ACOND);
			if (acond) {
				int eval_result = Evaluate (acond);
				if (!eval_result)
					continue;
			}
			NifLib::Attr *vcond = field->AttrById (AVERCOND);
			if (vcond) {
				int eval_result = Evaluate (vcond);
				if (!eval_result)
					continue;
			}
			i2j = NULL;
			// AARR1
			// can be const uint, field, expression
			NIFint i1 = InitArr (field->AttrById (AARR1));// 1d size
			if (i1 <= 0)
				continue;// nothing to read
			// AARR1
			// can be const, field, field what is an array (jagged)
			NIFint i2 = InitArr (field->AttrById (AARR2));// 1d size
			if (i2 <= 0)
				continue;// nothing to read
			// TODO:AARR3
			// can be const
			NIFint izise = i1 * i2;// fixed array size

			// type="TEMPLATE"
			if (ftype->Value.Equals ("TEMPLATE", 8)) {
				if (TEMPLATE)
					ftype = TEMPLATE;
				else {
					ERR("R: uknown TEMPLATE")
					return 0;
				}
			}

			NifLib::Tag *tt = GetBasicType (ftype);
			if (tt) {// its a TBASIC type
#define READJBASIC(BT, SZ, TYPE)\
{\
	BT *lengths = (BT *)&(i2j->Value.buf[0]);\
	int total_size = 0;\
	for (int idx = 0; idx < (int)i1; idx++)\
		total_size += (SZ*(int)lengths[idx]);\
	READ(NIFbyte, total_size, Byte, total_size, TYPE)\
}
#define READJNONBASIC(BT, SZ, TYPE)\
{\
	BT *lengths = (BT *)&(i2j->Value.buf[0]);\
	int total_size = 0;\
	if (SZ > 0) {\
		for (int idx = 0; idx < i1; idx++)\
			total_size += (SZ * (int)lengths[idx]);\
		READ(NIFbyte, total_size, Byte, total_size, TYPE)\
	} else\
		for (int idx = 0; idx < i1; idx++) {\
			for (int j = 0; j < (int)lengths[idx]; j++) {\
				if (!ReadObject (s, tt, newnode))\
					return 0;\
			}\
		}\
}
#define READJBASICALL(SZ, RPROC, TYPE)\
{\
	if ((i2j->NLType & NIFT_BT) == BtnType (BTN_UINT))\
		RPROC(NIFuint, SZ, TYPE)\
	else if ((i2j->NLType & NIFT_BT) == BtnType (BTN_INT))\
		RPROC(NIFint, SZ, TYPE)\
	else if ((i2j->NLType & NIFT_BT) == BtnType (BTN_BYTE))\
		RPROC(NIFbyte, SZ, TYPE)\
	else if ((i2j->NLType & NIFT_BT) == BtnType (BTN_USHORT))\
		RPROC(NIFushort, SZ, TYPE)\
	else if ((i2j->NLType & NIFT_BT) == BtnType (BTN_SHORT))\
		RPROC(NIFshort, SZ, TYPE)\
	else {\
		ERR("R: unknown jagged array type: " << NIFT2Str (i2j->NLType))\
		return 0;\
	}\
}
				NifLib::Attr *ta = tt->AttrById (ANIFLIBTYPE);
				NifLib::Attr *taname = tt->AttrById (ANAME);
				if (!taname) {
					ERR("R: TBASIC is missing ANAME")
					return 0;
				}
				if (!ta) {
					ERR("R: TBASIC is missing ANIFLIBTYPE")
					return 0;
				}
				int btypeid = BType (taname->Value.buf, taname->Value.len);
				//INFO ("btypeid: " << HEX(8) << btypeid << DEC
				//	<< " FixedSize: " << tt->FixedSize
				//	<< ", t: " << std::string (ta->Value.buf, ta->Value.len))
				if (tt->FixedSize > 0) {
					if (i2j)
						READJBASICALL(tt->FixedSize, READJBASIC, btypeid)
					else
						READ(NIFbyte, tt->FixedSize * izise, Byte, tt->FixedSize * izise,
							btypeid)
				} else
				if (NIFT(btypeid, BTN_BOOL)) {
					if (nVersion > 0x04010001)
						READ(NIFbyte, 1*izise, Byte, izise,
							((BTN_BOOL << 8) | BtnType (BTN_BYTE)))
					else
						READ(NIFint, 4*izise, Int, izise,
							((BTN_BOOL << 8) | BtnType (BTN_INT)))
				} else
				if (NIFT(btypeid, BTN_HEADERSTRING) ||
					NIFT(btypeid, BTN_LINESTRING)) {
					const int MAX = 1024;// max length
					NIFchar buf[MAX];
					NIFint rr = s.ReadCharCond (&buf[0], MAX, '\n');
					if (rr <= MAX) {
						AddNode (field, AddField (
							field, &buf[0], rr, btypeid), n);
					} else {// file format not supported
						ERR("R: ReadCharCond failed")
						return 0;
					}
					nVersion = HeaderString2Version (&buf[0], rr - 1);
					POS += rr;
				} else {
					ERR("R: Unknown basic type")
					return 0;
				}
			} else {// 'tt' is not a basic type
				tt = Find (TCOMPOUND, ftype->Value.buf, ftype->Value.len);
				if (!tt) // it is not compund
					tt = Find (TNIOBJECT, ftype->Value.buf, ftype->Value.len);
				if (!tt) { // it is not anything "known"
					ERR("R: Uknown tag")
					return 0;// can not continue - its sequential file format
				}
				NifLib::TreeNode<NifLib::Field *> *newnode = AddNode (field, NULL, n);
				if (!i2j) {
					if (tt->FixedSize > 0) {
						NifLib::TreeNode<NifLib::Field *> *nn = n;
						n = newnode;
						READ(NIFbyte, tt->FixedSize * izise, Byte, tt->FixedSize * izise,
							NIFT_T)
						n = nn;
					} else
						for (int idx = 0; idx < izise; idx++) {// 1d/2d/3d array
							if (!ReadObject (s, tt, newnode))
								return 0;
						}
				}
				else // jagged array of basic type (lengths), of non-basic type
					READJBASICALL(tt->FixedSize, READJNONBASIC, NIFT_T)
			}// not basic type
		}// for
#undef READJBASICALL
#undef READJNONBASIC
#undef READJBASIC
#undef READ
		return 1;
	}

	int
	Compiler::ReadNifBlock(int i, NifStream &s, const char *name, int nlen)
	{
		NifLib::Tag *t = Find (TCOMPOUND, name, nlen);
		if (!t) {
			t = Find (TNIOBJECT, name, nlen);
			if (!t) {
				ERR("R: Unknown block \"" << std::string (name, nlen) << "\"")
				return 0;
			}
		}

		Reset_FieldViewAName ();
		blockTag = t;
		AT(9)
		if (!ReadObject (s, t, AddNode (t, NULL, &ftree))) {
			B(9)
			return 0;
		}
		B(9)
		ARG = NULL;
		return 1;
	}

	int
	Compiler::ReadNif(const char *fname)
	{
		Reset ();
		NifLib::Tag *t = Find (TCOMPOUND, "Header", 6);
		if (!t) {
			ERR("R: Missing TCOMPOUND \"Header\"")
			return 0;
		}
		POS = 0;
		NifStream s(fname, 1*1024*1024);// 1Mb buffer
		INFO("Opened \"" << fname << "\"")
		blockTag = t;
		AT(9)
		if (!ReadObject (s, t, AddNode (t, NULL, &ftree))) {
			B(9)
			return 0;
		}
		B(9)
		ARG = NULL;
		//
		fUserVersion = FFBackwards ("User Version", 12);
		if (fUserVersion)
			nUserVersion = fUserVersion->AsNIFuint ();
		fVersion = FFBackwards ("Version", 7);
		if (!fVersion) {
			ERR("R: Version is missing from header")
			return 0;
		}
		fUserVersion2 = FFBackwards ("User Version 2", 14);
		if (fUserVersion2)
			nUserVersion2 = fUserVersion2->AsNIFuint ();
		INFO ("v: "
			<< HEX(8) << nVersion << DEC << ", uv: "
			<< nUserVersion << ", uv2: " << nUserVersion2)
		int i;
		NifLib::Field *f = FFBackwards ("Num Blocks", 10);
		if (!f) {
			ERR("R: \"Num Blocks\" lookup failed")
			return 0;
		}
		int num_blocks = (int)f->AsNIFuint ();
		INFO("Num Blocks: " << num_blocks)
			if (nVersion < 0x030300D) {
			INFO ("Version not supported yet: " << HEX(8) << nVersion << DEC)
			return 0;
		}
		else if (nVersion < 0x05000001) {
			NifLib::Tag *t1 = Find (TBASIC, "uint", 4);
			NifLib::Tag *t2 = Find (TBASIC, "char", 4);
			for (i = 0; i < num_blocks; i++) {
				int slen;
				if (s.ReadInt(&slen, 1) == 4 && slen > 0) {
					POS += 4;
					char *bname = (char *)NifAlloc (slen);
					if (!bname)
						return 0;
					if (s.ReadChar (bname, slen) == slen) {
						POS += slen;
						blockTag = NULL;
						AddField (t1, (char *)&slen, 4, NIFT_U | NIFT_4);
						AddField (t2, bname, slen, NIFT_U | NIFT_1);
						if (!ReadNifBlock(i, s, bname, slen)) {
							NifRelease (bname);
							return 0;// block read failed
						}
					} else {
						NifRelease (bname);
						return 0;// block name read file
					}
					NifRelease (bname);
				} else
					return 0;// block name length read failed
			}
			// those seem to have footer too
			if (!ReadNifBlock (i, s, "Footer", 6))
				return 0;
		}
		else if (nVersion > 0x0A000100/*"10.0.1.0"*/) {
			f = FFBackwards ("Block Type Index", 16);
			if (!f) {
				ERR("R: \"Block Type Index\" lookup failed")
				return 0;
			}
			NIFushort *block_type_index = (NIFushort *)&(f->Value.buf[0]);
			int bti_len = f->Value.len / 2;
			//INFO("Block Type Index len: " << bti_len)
			f = FFBackwards ("Num Block Types", 15);
			if (!f) {
				ERR("R: \"Num Block Types\" lookup failed")
				return 0;
			}
			int num_block_types = (int)f->AsNIFuint ();
			//INFO("Num Block Types: " << num_block_types)
			//int btIdx = FFBackwardsIdx (ANAME, "Block Types", 11);
			int btIdx = FFBackwardsIdx (ANAME, "Num Block Types", 15);
			if (btIdx < 0) {
				ERR("R: \"Block Types\" lookup failed")
				return 0;
			}
			NifLib::Tag *tz = Find (TBASIC, "uint", 4);
			for (i = 0; i < num_blocks; i++) {
				if (nVersion >= 0x05000001 && nVersion <= 0x0A01006A) {
					uint zero;
					if (s.ReadUInt(&zero, 1) == 4 && zero == 0) {
						blockTag = NULL;
						AddField (tz, (char *)&zero, 4, NIFT_U | NIFT_4);
					}
					else
						return 0;
				}
				if (i < 0 || i >= bti_len) {
					ERR("R: Assertion failed: i can not be outside bti")
					return 0;
				}
				int bt = block_type_index[i];
				//INFO("block type is: " << bt)
				if (bt < 0 || bt >= num_block_types) {
					ERR("R: Assertion failed: invald block type: " << bt)
					return 0;
				}
				int fbname_idx = btIdx + (2 * (bt + 1));
				if (fbname_idx < 0 || fbname_idx >= flist.Count ()) {
					ERR("R: Assertion failed: invald block name field index: " << bt)
					return 0;
				}
				f = flist[fbname_idx];
				if (!f) {
					ERR("E: Invalid block name field")
					return 0;
				}
				//INFO("Block #" << i << " \"" << STDSTR (f->Value) << "\"")
				//INFO("Block #" << i << " \"" << STDSTR (f->Value) << "\", pos: " << HEX(8) << POS << DEC)
				if (!ReadNifBlock (i, s, f->Value.buf, f->Value.len))
					return 0;
			}// for
			if (!ReadNifBlock (i, s, "Footer", 6))
				return 0;
		}//
		else {
			INFO ("Version not supported yet: " << HEX(8) << nVersion << DEC)
			return 0;
		}
		INFO("FP :" << HEX(8) << POS << DEC)
#undef DEC
#undef HEX
		//for (int i = 0; i < MLEN; i++)
		//	INFO("c: " << M[i].c << ", s: " << M[i].s / 1000 << " ms "
		//		<< M[i].n << " ()")
		/*NifLib::List<NifLib::Field *> flist;
		NifLib::TreeNode<NifLib::Field *> ftree;
		std::map<std::string, NIFuint> strversion_cache;
		std::map<std::string, int> tagnames_cache[TAGS_NUML1];
		std::map<std::string, int> btypes_cache;
		std::map<std::string, NifLib::List<int> *> fview_aname;*/
		INFO("flist: " << flist.Count ())
		INFO("ftree[0]: " << ftree.Nodes.Count ())
		INFO("strversion_cache: " << strversion_cache.size ())
		for (int i = 0; i < TAGS_NUML1; i++)
		INFO("tagnames_cache[" << i << "]: " << tagnames_cache[i].size ())
		INFO("btypes_cache: " << btypes_cache.size ())
		INFO("fview_aname: " << fview_aname.size ())
		return 1;
	}

	void
	Compiler::WriteNif(const char *fname)
	{
		if (flist.Count () <= 0) {
			ERR("WriteNif: flist empty")
			return;// nothing to write
		}
		FILE *fh = fopen (fname, "w");
		if (!fh) {
			ERR("WriteNif: can't open")
			return; // TODO: error handling
		}
		for (int i = 0; i < flist.Count (); i++) {
			NifLib::Field *f = flist[i];
			if (f->Value.len > 0) {
				if (!f) {
					ERR ("WriteNif: f is null")
					break;
				}
				if (!f->Value.buf) {
					ERR ("WriteNif: buf is null")
					break;
				}
				int rr = fwrite(f->Value.buf, 1, f->Value.len, fh);
				if (rr != f->Value.len) {
					ERR("WriteNif: writing " << f->Value.len << ",written " << rr)
					break;
				}
			}
		}
		fclose (fh);
		INFO("WriteNif: file closed")
	}

	void
	Compiler::Build()
	{
		int i, j;
		// Calculate TBASIC types fixed size
#define TAG(ID) "\"" << TagOpener (ID) << "\""
#define ATTR(ID) "\"" << AttrText (ID) << "\""
		//INFO("B: built " << TAG(TBASIC) << ": " << objs[TBASIC]->Count ())
		for (i = 0; i < objs[TBASIC]->Count (); i++) {
			NifLib::Tag *t = (*objs[TBASIC])[i];
			NifLib::Attr *ta = t->AttrById (ANIFLIBTYPE);// type attribute
			if (!ta) {
				ERR("B: " << TAG(TBASIC) << " without " << ATTR(ANIFLIBTYPE))
				continue;
			}
			NifLib::Attr *tn = t->AttrById (ANAME);
			if (!tn) {
				ERR("B: " << TAG(TBASIC) << " without " << ATTR(ANAME))
				continue;
			}
			t->FixedSize = BType (ta->Value.buf, ta->Value.len) & NIFT_SIZE;
			//if (t->FixedSize <= 0)
			//	INFO(" dynamic: " << TAG(TBASIC) << " \""
			//		<< STDSTR(ta->Value) << " " << STDSTR(tn->Value) << "\"")
		}
		// Calculate TENUM types fixed size
		//INFO("B: built " << TAG(TENUM) << ": " << objs[TENUM]->Count ())
		for (i = 0; i < objs[TENUM]->Count (); i++) {
			NifLib::Tag *t = (*objs[TENUM])[i];
			NifLib::Attr *ta = t->AttrById (ASTORAGE);// type attribute
			if (!ta) {
				ERR("B: " << TAG(TENUM) << " without " << ATTR(ASTORAGE))
				continue;
			}
			NifLib::Attr *tn = t->AttrById (ANAME);
			if (!tn) {
				ERR("B: " << TAG(TENUM) << " without " << ATTR(ANAME))
				continue;
			}
			NifLib::Tag *btag = Find(TBASIC, ta->Value.buf, ta->Value.len);
			if (!btag) {
				ERR("B: " << TAG(TENUM) << " with no "
					<< TAG(TBASIC) << " " << ATTR(ASTORAGE))
				continue;
			}
			t->FixedSize = btag->FixedSize;
			//if (t->FixedSize <= 0)
			//	INFO(" dynamic: " << TAG(TENUM) << " \""
			//		<< STDSTR(ta->Value) << " " << STDSTR(tn->Value) << "\"")
		}
		// Calculate TBITFLAGS types fixed size
		//INFO("B: built " << TAG(TBITFLAGS) << ": " << objs[TBITFLAGS]->Count ())
		for (i = 0; i < objs[TBITFLAGS]->Count (); i++) {
			NifLib::Tag *t = (*objs[TBITFLAGS])[i];
			NifLib::Attr *ta = t->AttrById (ASTORAGE);// type attribute
			if (!ta) {
				ERR("B: " << TAG(TBITFLAGS) << " without " << ATTR(ASTORAGE))
				continue;
			}
			NifLib::Attr *tn = t->AttrById (ANAME);
			if (!tn) {
				ERR("B: " << TAG(TBITFLAGS) << " without " << ATTR(ANAME))
				continue;
			}
			NifLib::Tag *btag = Find(TBASIC, ta->Value.buf, ta->Value.len);
			if (!btag) {
				ERR("B: " << TAG(TBITFLAGS) << " with no "
					<< TAG(TBASIC) << " " << ATTR(ASTORAGE))
				continue;
			}
			t->FixedSize = btag->FixedSize;
			//if (t->FixedSize <= 0)
			//	INFO(" dynamic: " << TAG(TBITFLAGS) << " \""
			//		<< STDSTR(ta->Value) << " " << STDSTR(tn->Value) << "\"")
		}
		// Calculate TCOMPOUND sizes where possible
		//int fcount = 0;
		//INFO("B: built " << TAG(TCOMPOUND) << ": " << objs[TCOMPOUND]->Count ())
		for (int iter = 0; iter < 2; iter++)
		for (i = 0; i < objs[TCOMPOUND]->Count (); i++) {
			NifLib::Tag *t = (*objs[TCOMPOUND])[i];
			NifLib::Attr *tname = t->AttrById (ANAME);
			if (!tname) {
				ERR("B: " << TAG(TCOMPOUND) << " without " << ATTR(ANAME))
				continue;
			}
			t->FixedSize = 0;
			int fs = 0;
			for (j = 0; j < t->Tags.Count (); j++) {// TCOMPOUND fields
				NifLib::Tag *t1 = t->Tags[j];
				NifLib::Attr *name = t1->AttrById (ANAME);
				NifLib::Attr *type = t1->AttrById (ATYPE);
				NifLib::Attr *def = t1->AttrById (ADEFAULT);
				NifLib::Attr *ttemplate = t1->AttrById (ATEMPLATE);
				NifLib::Attr *arr1 = t1->AttrById (AARR1);// handle when its const
				int arr1v = 1;
				if (arr1) {
					if (IsUInt (arr1->Value.buf, arr1->Value.len))
						arr1v =
							str2<NIFint> (std::string (arr1->Value.buf, arr1->Value.len));
				}
				if (name && type) {
					if (def) {// allow 'name', 'type' and 'default'
						if (t1->Attr.Count () != 3)
							break;
					} else if (ttemplate) {// allow 'name', 'type' and 'template'
						if (t1->Attr.Count () != 3)
							break;
					} else if (arr1 && arr1v > 1) {// allow 'name', 'type' and const 'arr1'
						if (t1->Attr.Count () != 3)
							break;
					} else {// allow 'name' and 'type'
						if (t1->Attr.Count () != 2)
							break;
					}
				}
				NifLib::Tag *tt =
					Find(TBASIC, type->Value.buf, type->Value.len);
				if (!tt)
					tt = Find(TENUM, type->Value.buf, type->Value.len);
				if (!tt)
					tt = Find(TBITFLAGS, type->Value.buf, type->Value.len);
				if (!tt)
					tt = Find(TCOMPOUND, type->Value.buf, type->Value.len);
				if (!tt)
					break;
				if (tt->FixedSize <= 0)
					break;
				else
					fs += tt->FixedSize * arr1v;
			}
			if (j != t->Tags.Count ())
				continue;
			//fcount++;
			//INFO(" fixed: " << TAG(TCOMPOUND) << " \""
			//	<< STDSTR(tname->Value) << "\": " << fs)
			t->FixedSize = fs;
		}
		//INFO(" with fixed size: " << fcount/2 << "/" << objs[TCOMPOUND]->Count ())
		// Calculate TNIOBJECT sizes where possible
		//fcount = 0;
		//INFO("B: built " << TAG(TNIOBJECT) << ": " << objs[TNIOBJECT]->Count ())
		for (int iter = 0; iter < 2; iter++)
		for (i = 0; i < objs[TNIOBJECT]->Count (); i++) {
			NifLib::Tag *t = (*objs[TNIOBJECT])[i];
			// check if empty
			if (t->Tags.Count () <= 0)
				continue;
			// check if parent(s) are empty too
			NifLib::Attr *tinh = t->AttrById (AINHERIT);
			int tagcount = 0;
			while (tinh) {
				NifLib::Tag *p = Find (TNIOBJECT, tinh->Value.buf, tinh->Value.len);
				if (!p) {
					ERR("B: " << TAG(TNIOBJECT) << " missing parent: "
						<< "\"" << STDSTR(tinh->Value) << "\"")
					break;
				}
				tagcount += p->Tags.Count ();
				tinh = p->AttrById (AINHERIT);
			}
			if (tagcount > 0)
				// Has parent(s) with field(s).
				// Those are recursively read by ReadObject ().
				continue;
			NifLib::Attr *tname = t->AttrById (ANAME);
			if (!tname) {
				ERR("B: " << TAG(TNIOBJECT) << " without " << ATTR(ANAME))
				continue;
			}
			t->FixedSize = 0;
			int fs = 0;
			for (j = 0; j < t->Tags.Count (); j++) {// TNIOBJECT fields
				NifLib::Tag *t1 = t->Tags[j];
				NifLib::Attr *name = t1->AttrById (ANAME);
				NifLib::Attr *type = t1->AttrById (ATYPE);
				NifLib::Attr *def = t1->AttrById (ADEFAULT);
				NifLib::Attr *ttemplate = t1->AttrById (ATEMPLATE);
				NifLib::Attr *arr1 = t1->AttrById (AARR1);// handle when its const
				int arr1v = 1;
				if (arr1) {
					if (IsUInt (arr1->Value.buf, arr1->Value.len))
						arr1v =
							str2<NIFint> (std::string (arr1->Value.buf, arr1->Value.len));
				}
				if (name && type) {
					if (def) {// allow 'name', 'type' and 'default'
						if (t1->Attr.Count () != 3)
							break;
					} else if (ttemplate) {// allow 'name', 'type' and 'template'
						if (t1->Attr.Count () != 3)
							break;
					} else if (arr1 && arr1v > 1) {// allow 'name', 'type' and const 'arr1'
						if (t1->Attr.Count () != 3)
							break;
					} else {// allow 'name' and 'type'
						if (t1->Attr.Count () != 2)
							break;
					}
				}
				NifLib::Tag *tt =
					Find(TBASIC, type->Value.buf, type->Value.len);
				if (!tt)
					tt = Find(TENUM, type->Value.buf, type->Value.len);
				if (!tt)
					tt = Find(TBITFLAGS, type->Value.buf, type->Value.len);
				if (!tt)
					tt = Find(TCOMPOUND, type->Value.buf, type->Value.len);
				if (!tt)
					tt = Find(TNIOBJECT, type->Value.buf, type->Value.len);
				if (!tt)
					break;
				if (tt->FixedSize <= 0)
					break;
				else
					fs += tt->FixedSize * arr1v;
			}
			if (j != t->Tags.Count ())
				continue;
			//fcount++;
			//INFO(" fixed: " << TAG(TNIOBJECT) << " \""
			//	<< STDSTR(tname->Value) << "\": " << fs)
			t->FixedSize = fs;
		}
		//INFO(" with fixed size: " << fcount/2 << "/" << objs[TNIOBJECT]->Count ())
#undef TAG
#undef ATTR
	}

	void
	Compiler::PrintNode(NifLib::TreeNode<NifLib::Field *> *node, std::string ofs)
	{
		static int bIdx = -1;
		for (int i = 0; i < node->Nodes.Count (); i++) {
			NifLib::Field *f = node->Nodes[i]->Value;
			if (node->Parent == NULL)
				bIdx = i - 1;
			INFO(ofs << "f (" << NIFT2Str (f->NLType) << ") #" << bIdx
			<< " (" << f->BlockName () << ")" << ": \""
			<< f->OwnerName () << "."
			<< f->Name () << "\": "
			<< f->Value.len << " \"" << f->AsString (this) << "\"")
			if (node->Nodes[i]->Nodes.Count () > 0)
				PrintNode (node->Nodes[i], ofs + " ");
		}
	}

	void
	Compiler::DbgPrintFields()
	{
		std::string ofs = "";;
		PrintNode (&ftree, ofs);
		return;
		int bIdx = -1;
		for (int i = 0; i < flist.Count (); i++) {
			NifLib::Field *f = flist[i];
			if (i > 0 && f->BlockTag != flist[i-1]->BlockTag)
				bIdx++;
			INFO("f #" << bIdx
				<< " (" << f->BlockName () << ")" << ": \""
				<< f->OwnerName () << "."
				<< f->Name () << "\": "
				<< f->Value.len << " \"" << f->AsString (this) << "\"")
		}
	}

	/*
	*	Returns tag ANAME attribute as a string
	*/
	std::string
	Compiler::TagName(NifLib::Tag *tag)
	{
		if (tag) {
			NifLib::Attr *tname = tag->AttrById (ANAME);
			if (tname)
				return std::string (tname->Value.buf, tname->Value.len);
			else
				return std::string ("ERROR: Missing tag name");
		} else
			return std::string ("NULL");
	}

	NifLib::TreeNode<NifLib::Field *> *Compiler::AsTree()
	{
		return &ftree;
	}

#undef STDSTR
}
