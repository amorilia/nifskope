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
struct timeval ta1, tb1; int c1 = 0; long s1 = 0;
struct timeval ta2, tb2; int c2 = 0; long s2 = 0;
struct timeval ta3, tb3; int c3 = 0; long s3 = 0;
struct timeval ta4, tb4; int c4 = 0; long s4 = 0;
struct timeval ta5, tb5; int c5 = 0; long s5 = 0;
struct timeval ta6, tb6; int c6 = 0; long s6 = 0;
struct timeval ta7, tb7; int c7 = 0; long s7 = 0;
struct timeval ta8, tb8; int c8 = 0; long s8 = 0;
struct timeval ta9, tb9; int c9 = 0; long s9 = 0;
struct timeval ta10, tb10; int c10 = 0; long s10 = 0;
struct timeval ta11, tb11; int c11 = 0; long s11 = 0;
struct timeval ta12, tb12; int c12 = 0; long s12 = 0;
#define A(N)\
{gettimeofday (&ta##N, NULL);\
c##N++;}

#define B(N)\
{gettimeofday (&tb##N, NULL);\
s##N += time_interval (&ta##N, &tb##N) / (1);}

#define STDSTR(BUF) std::string (BUF.buf, BUF.len)

	NifLib::Field *
	Compiler::AddField(NifLib::Tag *field, char *buf, int bl)
	{
		A(1)
		NifLib::Field *f = new NifLib::Field();
		f->BlockIndex = blockIndex;
		f->BlockTag = blockTag;
		f->JField = i2j;
		f->Tag = field;
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

		B(1)
		return f;
	}

	NifLib::TreeNode<NifLib::Field *> *
	Compiler::AddNode(
			NifLib::Tag *t,
			NifLib::Field *f,
			NifLib::TreeNode<NifLib::Field *> *pnode)
	{
		NifLib::TreeNode<NifLib::Field *> *node = new NifLib::TreeNode<NifLib::Field *>;
		node->Parent = pnode;
		if (!f) {
			f = new NifLib::Field ();
			f->BlockIndex = blockIndex;
			f->BlockTag = blockTag;
			f->JField = i2j;
			f->Tag = t;
			f->Value.CopyFrom ("", 1);
		}
		node->Value = f;
		pnode->Nodes.Add (node);
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
			if (it->second == -1)
				return NULL;
			else
				return (*objs[tagid])[it->second];
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
		//INFO("Block #" << blockIndex
		//	<< ", E p1: \"" << std::string (buf, len) << "\"")

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
				//if (ARG)
				//	INFO ("Block #" << blockIndex << ", ARG: " << STDSTR(ARG->Value))
				//else
				//	INFO ("Block #" << blockIndex << ", ARG: null")
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
		DETAILEDLOG = 0;
		blockIndex = 0;
		blockTag = NULL;
		TEMPLATE = NULL;
		fVersion = NULL;
		fUserVersion = NULL;
		fUserVersion2 = NULL;
		i2j = NULL;
	}

	Compiler::~Compiler()
	{
		Reset ();
		strversion_cache. clear ();
		for (int i = 0; i < TAGS_NUML1; i++)
			tagnames_cache[i].clear ();
	}

	void
	Compiler::Reset()
	{
		nVersion = 0;
		nUserVersion = 0;
		nUserVersion2 = 0;
		DETAILEDLOG = 0;
		blockIndex = 0;
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
		if ((it = strversion_cache.find (std::string (buf, bl))) != strversion_cache.end ())
			return it->second;
		else {
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
			B(6)
			strversion_cache[std::string (buf, bl)] = r;
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
					//INFO("Block #" << blockIndex
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
		A(9)
		if (!t) {
			ERR("R: Missing tag")
			B(9)
			return 0;
		}
		NifLib::Attr *tname = t->AttrById (ANAME);
		if (!tname) {
			ERR("R: A tag should have a name")
			B(9)
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
				ERR("Unknown parent block for \""
					<< std::string (tname->Value.buf, tname->Value.len) << "\"")
				B(9)
				return 0;
			}
			B(9)
			if (!ReadObject (s, tp, AddNode(tp, NULL, n))) {
				return 0;
			}
			A(9)
		}
#define SFIELD(L1, L2)\
	"\"" << std::string (L1->Value.buf, L1->Value.len)\
	<< "." << std::string (L2->Value.buf, L2->Value.len) << "\""
#define HEX(N) std::setw (N) << std::setfill ('0') << std::hex << std::uppercase
#define DEC std::dec
#define READ(BT, BYTES, RT, CNT)\
{\
	if (CNT > 0 && BYTES > 0) {\
		BT *buf;\
		buf = (BT *)NifAlloc (BYTES);\
		if (!buf) {\
			ERR("READ: Out of memory")\
			B(9)\
			return 0;\
		}\
		NIFint rr = s.Read##RT (&buf[0], CNT);\
		if (rr != BYTES) {\
			ERR("Read##RT failed")\
			NifRelease (buf);\
			B(9)\
			return 0;\
		}\
		AddNode (field, AddField (field, (char *)&buf[0], BYTES), n);\
		POS += rr;\
		NifRelease (buf);\
	}\
}
		NifLib::Attr *ARGsentinel = NULL;
		for (int i = 0; i < t->Tags.Count (); i++) {// its kinda CS:IP :)
			if (ARGsentinel) ARG = ARGsentinel;
			NifLib::Tag *field = t->Tags[i];// a field
			NifLib::Attr *ftype = field->AttrById (ATYPE);// field type
			NifLib::Attr *fname = field->AttrById (ANAME);// field name
			if (!ftype) {// field must have a type
				INFO("R: Unknown type for L2 tag in L1 tag #" << t->Id)
				B(9)
				return 0;
			}
			if (!V12Check (field)) {// "AVER1" "AVER2"
				//INFO("R: " << SFIELD(tname, fname)	<< ": *** V12Check")
				continue;
			}
			NifLib::Attr *userver = field->AttrById (AUSERVER);
			if (userver) {// can be a const
				NIFuint uv =
					str2<NIFint> (std::string (userver->Value.buf, userver->Value.len));
				if (uv != nUserVersion) {
					//INFO("R: " << SFIELD(tname, fname) << ": *** userver check")
					continue;
				}
			}
			// ARG
			NifLib::Attr *tmp = field->AttrById (AARG);
			ARGsentinel = ARG;
			if (tmp) {
				ARG = field->AttrById (AARG);
			//if (ARG)
			//	INFO("Block #" << blockIndex << ", ARG: \"" << STDSTR (ARG->Value) << "\"")
			}
			tmp = field->AttrById (ATEMPLATE);
			if (tmp && !tmp->Value.Equals ("TEMPLATE", 8))
				TEMPLATE = tmp;// keep last template type
			NifLib::Attr *acond = field->AttrById (ACOND);
			if (acond) {
				int eval_result = Evaluate (acond);
				//INFO("*R cond:" << "\""
				//	<< STDSTR(acond->Value) << "\": "
				//	<< eval_result)
				if (!eval_result)
					continue;
			}
			NifLib::Attr *vcond = field->AttrById (AVERCOND);
			if (vcond) {
				int eval_result = Evaluate (vcond);
				//INFO("*R vercond:" << "\""
				//	<< STDSTR (vcond->Value) << "\": "
				//	<< eval_result)
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
				if (TEMPLATE) {
					//INFO("TQ: " << STDSTR(TEMPLATE->Value))
					ftype = TEMPLATE;
				}
				else {
					ERR("TQ: uknown")
					return 0;
				}
			}

			NifLib::Tag *tt = GetBasicType (ftype);
			if (tt) {// its a TBASIC type
				NifLib::Attr *ta = tt->AttrById (ANIFLIBTYPE);
				// behaviour is hard-coded
				if (ta->Value.Equals ("HeaderString", 12) ||
					ta->Value.Equals ("LineString", 10)) {
					// A variable length string that ends with a newline character (0x0A)
					const int MAX = 1024;// max length
					NIFchar buf[MAX];
					NIFint rr = s.ReadCharCond (&buf[0], MAX, '\n');
					if (rr <= MAX) {
						AddNode (field, AddField (field, &buf[0], rr), n);
					} else {// file format not supported
						ERR("R: ReadCharCond failed")
						B(9)
						return 0;
					}
					nVersion = HeaderString2Version (&buf[0], rr - 1);
					POS += rr;
				} else
#define READJBASIC(BT, SZ)\
	{\
		BT *lengths = (BT *)&(i2j->Value.buf[0]);\
		int total_size = 0;\
		for (int idx = 0; idx < (int)i1; idx++)\
			total_size += (SZ*(int)lengths[idx]);\
		READ(NIFbyte, total_size, Byte, total_size)\
	}
#define READJNONBASIC(BT, SZ)\
	{\
		BT *lengths = (BT *)&(i2j->Value.buf[0]);\
		int total_size = 0;\
		if (SZ > 0) {\
			for (int idx = 0; idx < i1; idx++)\
				total_size += (SZ * (int)lengths[idx]);\
			READ(NIFbyte, total_size, Byte, total_size)\
		} else\
			for (int idx = 0; idx < i1; idx++) {\
				for (int j = 0; j < (int)lengths[idx]; j++) {\
					B(9)\
					if (!ReadObject (s, tt, newnode)) {\
						return 0;\
					}\
					A(9)\
				}\
			}\
	}
#define READJBASICALL(SZ, RPROC)\
	{\
		NifLib::Tag *_tag = i2j->Tag;\
		NifLib::Attr *_ta1 = _tag->AttrById (ATYPE);\
		NifLib::Attr *_ta = _tag->AttrById (ATYPE);\
		NifLib::Tag *_bt = Find (TBASIC, _ta->Value.buf, _ta->Value.len);\
		if (_bt)\
			_ta = _bt->AttrById (ANIFLIBTYPE);\
		if (!_ta) {\
			ERR("R: cant't handle non basic types jagged array lengths: "\
			<< "\"" << std::string (_ta1->Value.buf, _ta1->Value.len) << "\"")\
			B(9)\
			return 0;\
		}\
		else if (_ta->Value.Equals ("unsigned int", 12))\
			RPROC(NIFuint, SZ)\
		else if (_ta->Value.Equals ("int", 3))\
			RPROC(NIFint, SZ)\
		else if (_ta->Value.Equals ("byte", 4))\
			RPROC(NIFbyte, SZ)\
		else if (_ta->Value.Equals ("unsigned short", 14))\
			RPROC(NIFushort, SZ)\
		else if (_ta->Value.Equals ("short", 5))\
			RPROC(NIFshort, SZ)\
		else {\
			ERR("R: can't handle that jagged array type: "\
			<< "\"" << std::string (_ta->Value.buf, _ta->Value.len) << "\"")\
			B(9)\
			return 0;\
		}\
	}
				// behaviour is hard-coded
				if (ta->Value.Equals ("bool", 4)) {
					// A boolean; 32-bit from 4.0.0.2, and 8-bit from 4.1.0.1 on.
					if (nVersion > 0x04010001)
						READ(NIFbyte, 1*izise, Byte, izise)
					else
						READ(NIFint, 4*izise, Int, izise)
				} else
				if (tt->FixedSize > 0) {
					if (i2j)
						READJBASICALL(tt->FixedSize, READJBASIC)
					else
						READ(NIFbyte, tt->FixedSize * izise, Byte, tt->FixedSize * izise)
				}
				else
					ERR("R: " << SFIELD(tname, fname) << ": *** Unknown basic type"
					<< " (" << std::string (ta->Value.buf, ta->Value.len) << ")")
			} else {// 'tt' is not a basic type
				tt = Find (TCOMPOUND, ftype->Value.buf, ftype->Value.len);
				if (!tt) // it is not compund
					tt = Find (TNIOBJECT, ftype->Value.buf, ftype->Value.len);
				if (!tt) { // it is not anything "known"
					ERR("R: Uknown tag")
					B(9)
					return 0;// can not continue - its sequential file format
				}
				//AddField (field, "", 1); // struct marker
				NifLib::TreeNode<NifLib::Field *> *newnode = AddNode (field, NULL, n);
				if (!i2j) {
					if (tt->FixedSize > 0) {
						NifLib::TreeNode<NifLib::Field *> *nn = n;
						n = newnode;
						READ(NIFbyte, tt->FixedSize * izise, Byte, tt->FixedSize * izise)
						n = nn;
					}
					else
						for (int idx = 0; idx < izise; idx++) {// 1d/2d/3d array
							B(9)
							if (!ReadObject (s, tt, newnode)) {
								return 0;
							}
							A(9)
						}
				}
				else // jagged array of basic type (lengths) of non-basic type
					READJBASICALL(tt->FixedSize, READJNONBASIC)
			}// not basic type
		}// for
#undef READJBASICALL
#undef READJNONBASIC
#undef READJBASIC
#undef READ
#undef DEC
#undef HEX
#undef SFIELD
		B(9)
		return 1;
	}

	int
	Compiler::ReadNifBlock(int i, NifStream &s, const char *name, int nlen)
	{
		NifLib::Tag *t = Find (TCOMPOUND, name, nlen);
		if (!t) {
			t = Find (TNIOBJECT, name, nlen);
			if (!t) {
				ERR("Unknown block \"" << std::string (name, nlen) << "\"")
				return 0;
			}
		}

		Reset_FieldViewAName ();
		blockIndex = i;
		blockTag = t;
		if (!ReadObject (s, t, AddNode (t, NULL, &ftree)/*&ftree*/))
			return 0;
		ARG = NULL;
		return 1;
	}

	int
	Compiler::ReadNif(const char *fname)
	{
		Reset ();
		NifLib::Tag *t = Find (TCOMPOUND, "Header", 6);
		if (t) {
			//INFO("Found header structure")
			POS = 0;
			NifStream s(fname, 1*1024*1024);
			INFO("Opened \"" << fname << "\"")

			//INFO("R: Header")
			blockIndex = -1;
			blockTag = t;
			if (!ReadObject (s, t, AddNode (t, NULL, &ftree)/*&ftree*/))
				return 0;
			ARG = NULL;
			//argstack.Clear ();
			fUserVersion = FFBackwards ("User Version", 12);
			if (fUserVersion)
				nUserVersion = fUserVersion->AsNIFuint ();
			fVersion = FFBackwards ("Version", 7);
			if (!fVersion) {
				ERR("Version is missing from header")
				return 0;
			}
			fUserVersion2 = FFBackwards ("User Version 2", 14);
			if (fUserVersion2)
				nUserVersion2 = fUserVersion2->AsNIFuint ();
#define HEX(N) std::setw (N) << std::setfill ('0') << std::hex << std::uppercase
#define DEC std::dec
			INFO ("v: "
				<< HEX(8) << nVersion << DEC << ", uv: "
				<< nUserVersion << ", uv2: " << nUserVersion2)
			//INFO("FP :" << HEX(8) << pos << DEC)
			//return;

			int i;
			NifLib::Field *f = FFBackwards ("Num Blocks", 10);
			if (!f) {
				ERR("\"Num Blocks\" lookup failed")
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
							AddField (t1, (char *)&slen, 4);
							AddField (t2, bname, slen);
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
					ERR("\"Block Type Index\" lookup failed")
					return 0;
				}
				NIFushort *block_type_index = (NIFushort *)&(f->Value.buf[0]);
				int bti_len = f->Value.len / 2;
				//INFO("Block Type Index len: " << bti_len)
				f = FFBackwards ("Num Block Types", 15);
				if (!f) {
					ERR("\"Num Block Types\" lookup failed")
					return 0;
				}
				int num_block_types = (int)f->AsNIFuint ();
				//INFO("Num Block Types: " << num_block_types)
				//int btIdx = FFBackwardsIdx (ANAME, "Block Types", 11);
				int btIdx = FFBackwardsIdx (ANAME, "Num Block Types", 15);
				if (btIdx < 0) {
					ERR("\"Block Types\" lookup failed")
					return 0;
				}
				NifLib::Tag *tz = Find (TBASIC, "uint", 4);
				for (i = 0; i < num_blocks; i++) {
					if (nVersion >= 0x05000001 && nVersion <= 0x0A01006A) {
						uint zero;
						if (s.ReadUInt(&zero, 1) == 4 && zero == 0) {
							blockTag = NULL;
							AddField (tz, (char *)&zero, 4);
						}
						else
							return 0;
					}
					if (i < 0 || i >= bti_len) {
						ERR("Assertion failed: i can not be outside bti")
						return 0;
					}
					int bt = block_type_index[i];
					//INFO("block type is: " << bt)
					if (bt < 0 || bt >= num_block_types) {
						ERR("Assertion failed: invald block type: " << bt)
						return 0;
					}
					int fbname_idx = btIdx + (2 * (bt + 1));
					if (fbname_idx < 0 || fbname_idx >= flist.Count ()) {
						ERR("Assertion failed: invald block name field index: " << bt)
						return 0;
					}
					f = flist[fbname_idx];
					if (!f) {
						ERR("Invalid block name field")
						return 0;
					}
					//INFO("Block #" << i	<< " \"" << STDSTR (f->Value) << "\"")
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
			INFO("c: " << c1 << ", s: " << s1 / 1000 << " ms AddField ()")
			INFO("c: " << c2 << ", s: " << s2 / 1000 << " ms Evaluate ()")
			INFO("c: " << c3 << ", s: " << s3 / 1000 << " ms EvalDeduceType ()")
			INFO("c: " << c4 << ", s: " << s4 / 1000 << " ms EvaluateL2 ()")
			INFO("c: " << c5 << ", s: " << s5 / 1000 << " ms FFBackwards ()")
			INFO("c: " << c6 << ", s: " << s6 / 1000 << " ms HeaderString2Version ()")
			INFO("c: " << c7 << ", s: " << s7 / 1000 << " ms GetBasicType ()")
			INFO("c: " << c8 << ", s: " << s8 / 1000 << " ms InitArr ()")
			INFO("c: " << c9 << ", s: " << s9 / 1000 << " ms ReadObject ()")
			INFO("c: " << c10 << ", s: " << s10 / 1000 << " ms V12Check ()")
			INFO("c: " << c11 << ", s: " << s11 / 1000 << " ms Find ()")
			INFO("c: " << c12 << ", s: " << s12 / 1000 << " ms FFVersion ()")
		}
		return 1;
	}

	void
	Compiler::WriteNif(const char *fname)
	{
		//return;
		if (flist.Count () <= 0)
			return;// nothing to write
		FILE *fh = fopen (fname, "w");
		if (!fh)
			return; // TODO: error handling
		for (int i = 0; i < flist.Count (); i++) {
			NifLib::Field *f = flist[i];
			if (f->Value.len > 0)
				fwrite(f->Value.buf, f->Value.len, 1, fh);
		}
		fclose (fh);
		INFO("file closed")
	}

	void
	Compiler::Build()
	{
		int i, j;
		// calculate basic types fixed size
		// calculate enum types fixed size
		// calculate bitlflags types fixed size
		//INFO("BUILD: 'basic'")
		for (i = 0; i < objs[TBASIC]->Count (); i++) {
			NifLib::Tag *t = (*objs[TBASIC])[i];
			NifLib::Attr *ta = t->AttrById (ANIFLIBTYPE);// type attribute
			if (!ta) {
				ERR("BUILD: 'basic' without 'niflibtype'")
				continue;
			}
			NifLib::Attr *tn = t->AttrById (ANAME);
			if (!tn) {
				ERR("BUILD: 'basic' without 'name'")
				continue;
			}
			if (ta->Value.Equals ("unsigned int", 12) ||
				ta->Value.Equals ("IndexString", 11) ||
				ta->Value.Equals ("int", 3) ||
				ta->Value.Equals ("*", 1) ||
				ta->Value.Equals ("Ref", 3) ||
				ta->Value.Equals ("float", 5))
				t->FixedSize = 4;
			else if ( ta->Value.Equals ("unsigned short", 14) ||
				ta->Value.Equals ("short", 5))
				t->FixedSize = 2;
			else if (ta->Value.Equals ("byte", 4))
				t->FixedSize = 1;
			else {
				//INFO("BUILD: 'basic' \""
				//	<< STDSTR(ta->Value) << " " << STDSTR(tn->Value) << "\""
				//	<< " has dynamic size")
			}
		}
		//INFO("BUILD: 'enum'")
		for (i = 0; i < objs[TENUM]->Count (); i++) {
			NifLib::Tag *t = (*objs[TENUM])[i];
			NifLib::Attr *ta = t->AttrById (ASTORAGE);// type attribute
			if (!ta) {
				ERR("BUILD: 'enum' without 'storage'")
				continue;
			}
			NifLib::Attr *tn = t->AttrById (ANAME);
			if (!tn) {
				ERR("BUILD: 'enum' without 'name'")
				continue;
			}
			NifLib::Tag *btag = Find(TBASIC, ta->Value.buf, ta->Value.len);
			if (!btag) {
				ERR("BUILD: 'enum' with no 'basic' 'storage'")
				continue;
			}
			t->FixedSize = btag->FixedSize;
			if (t->FixedSize <= 0) {
				//INFO("BUILD: 'enum' \""
				//	<< STDSTR(ta->Value) << " " << STDSTR(tn->Value) << "\""
				//	<< " has dynamic size")
			}
		}
		//INFO("BUILD: 'bitflags'")
		for (i = 0; i < objs[TBITFLAGS]->Count (); i++) {
			NifLib::Tag *t = (*objs[TBITFLAGS])[i];
			NifLib::Attr *ta = t->AttrById (ASTORAGE);// type attribute
			if (!ta) {
				ERR("BUILD: 'bitflags' without 'storage'")
				continue;
			}
			NifLib::Attr *tn = t->AttrById (ANAME);
			if (!tn) {
				ERR("BUILD: 'bitflags' without 'name'")
				continue;
			}
			NifLib::Tag *btag = Find(TBASIC, ta->Value.buf, ta->Value.len);
			if (!btag) {
				ERR("BUILD: 'bitflags' with no 'basic' 'storage'")
				continue;
			}
			t->FixedSize = btag->FixedSize;
			if (t->FixedSize <= 0) {
				//INFO("BUILD: 'bitflags' \""
				//	<< STDSTR(ta->Value) << " " << STDSTR(tn->Value) << "\""
				//	<< " has dynamic size")
			}
		}
		//INFO("BUILD: 'compound'")
		// calculate structure sizes where possible
		for (i = 0; i < objs[TCOMPOUND]->Count (); i++) {
			NifLib::Tag *t = (*objs[TCOMPOUND])[i];
			NifLib::Attr *tname = t->AttrById (ANAME);
			if (!tname) {
				ERR("BUILD: 'compound' without 'name'")
				continue;
			}
			t->FixedSize = 0;
			int fs = 0;
			for (j = 0; j < t->Tags.Count (); j++) {// compound fields
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
			//INFO("BUILD: 'compound' \""
			//	<< STDSTR(tname->Value) << "\""
			//	<< " has fixed size: " << fs)
			t->FixedSize = fs;
		}

		//INFO("BUILD: 'niobject'")
		// calculate structure sizes where possible
		for (i = 0; i < objs[TNIOBJECT]->Count (); i++) {
			NifLib::Tag *t = (*objs[TNIOBJECT])[i];
			// check if empty
			if (t->Tags.Count () <= 0)
				continue;
			// check if parent(s) are empty too
			NifLib::Attr *tinh = t->AttrById (AINHERIT);
			int tagcount = 0;
			//int non_fixed = 0;
			//int psize = 0;
			while (tinh) {
				NifLib::Tag *p = Find (TNIOBJECT, tinh->Value.buf, tinh->Value.len);
				if (!p) {
					ERR("BUILD: 'niobject' missing parent: "
					<< "\"" << STDSTR(tinh->Value) << "\"")
					break;
				}
				tagcount += p->Tags.Count ();
				/*if (p->Tags.Count () > 0) {// parent has fields
					non_fixed = (p->FixedSize <= 0);// parent has no fixed size
					if (non_fixed)
						break;
					psize += p->FixedSize;
				} see below *** */
				tinh = p->AttrById (AINHERIT);
			}
			if (tagcount > 0/* && non_fixed*/)
				// has parent(s) with fields and one of them is with non-fixed size
				continue;
			// *** works but useless for now, so
			// allow those with no parent(s) field(s) for reading
			//if (psize)
			//	INFO("BUILD: 'niobject', parent size: " << psize)

			NifLib::Attr *tname = t->AttrById (ANAME);
			if (!tname) {
				ERR("BUILD: 'niobject' without 'name'")
				continue;
			}
			t->FixedSize = 0;
			int fs = 0;
			for (j = 0; j < t->Tags.Count (); j++) {// compound fields
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
			/*if (!tinh)
			INFO("BUILD: 'niobject' \""
				<< STDSTR(tname->Value) << "\""
				<< " has fixed size: " << fs
				<< ", parent(s) tagcount = " << tagcount)
			else
			INFO("BUILD: 'niobject' "
				<< "\"" << STDSTR(tname->Value) << "\":"
				<< "\"" << STDSTR(tinh->Value) << "\""
				<< " has fixed size: " << fs
				<< ", parent(s) tagcount = " << tagcount)*/
			t->FixedSize = fs;
		}
		return;
		/*std::map<std::string, int (Compiler::*) (NifStream *, char *)> r;
		r["foo"] = &Compiler::Read_bool;
		int (Compiler::*f) (NifStream *, char *)  = r["foo"];
		(this->*f)(NULL, NULL);*/
	}

	int
	Compiler::Read_bool(NifStream *s, char *b)
	{
		INFO("hi")
		return 1;
	}

	void
	Compiler::PrintNode(NifLib::TreeNode<NifLib::Field *> *node, std::string ofs)
	{
		//INFO(ofs << "-- " << node->Nodes.Count ())
		for (int i = 0; i < node->Nodes.Count (); i++) {
			NifLib::Field *f = node->Nodes[i]->Value;
			INFO(ofs << "f #" << f->BlockIndex
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

		for (int i = 0; i < flist.Count (); i++) {
			NifLib::Field *f = flist[i];
			std::string structure = "";
			if (f->IsStruct (this))
				structure = " [struct]";
			INFO("f #" << f->BlockIndex
				<< " (" << f->BlockName () << ")" << ": " << structure << " \""
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
