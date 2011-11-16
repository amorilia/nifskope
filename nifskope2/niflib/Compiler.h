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

#ifndef __COMPILER_H__
#define __COMPILER_H__

#include "niflib.h"
#include "Parser.h"
#include "niff.h"
#include "Tag.h"
#include "Attr.h"
#include "List.h"
#include "Field.h"
#include <string>
#include <sstream>
#include <map>
#include "TreeNode.h"

namespace NifLib
{
	/*
	*	Runtime parser generator for reading, editing and writing of
	*	"nif.xml" and .nif files.
	*
	*	Usage (read):
	*		// Load the XML document:
	*		NifLib::Compiler foo ("nif.xml");
	*		if (!foo.Loaded ()) {
	*			INFO("nif.xml load failed")
	*			return false;
	*		}
	*		// The XML document has been changed for "foo", so build it:
	*		foo.Build ();
	*		// Parse a .nif file:
	*		int result = foo.ReadNif ("Bar.nif");
	*		if (result)
	*			INFO("Bar.nif was parsed successfully")
	*
	*	The .nif data can be accessed either as a tree:
	*		NifLib::TreeNode<NifLib::Field *> *AsTree (),
	*	or as a list:
	*		NifLib::Field *operator[](int index), int FCount ()
	*
	*	"NifLib::Field" contains the file bytes as a Buffer instance - a
	*	lightweight "wrapper" around "int len; char *buf;".
	*
	*	For the XML document data see "Parser.h".
	*/
	class Compiler: public Parser
	{
		int POS;// debug: current position in the binary file being read

		/*
		*
		*	Core data containers.
		*
		*	flist
		*		- owns all fields what contain the file data;
		*	ftree
		*		- owns only fields not present in the list - root node fields;
		*		- has references to all fields in "flist";
		*
		*/
		NifLib::List<NifLib::Field *> flist;
		NifLib::TreeNode<NifLib::Field *> ftree;

		NifLib::Tag *blockTag;// state - current .nif block Tag
		NifLib::Field *i2j;// state - jagged array Field
		NifLib::Attr *ARG;// state - current ARG attribute
		NifLib::Attr *TEMPLATE;// state - current TEMPLATE attribute
		NifLib::Field *fVersion;// state - fast access for "Version"
		NifLib::Field *fUserVersion;// state - fast access for "User Version"
		NifLib::Field *fUserVersion2;// state - fast access for "User Version 2"
		NIFuint nVersion;// state - global nif file variable(s)
		NIFuint nUserVersion;// state - global nif file variable(s)
		NIFuint nUserVersion2;// state - global nif file variable(s)
		NIFint i1;// state - AARR1 size

		std::map<std::string, NIFuint> strversion_cache;//HeaderString2Version
		std::map<std::string, int> tagnames_cache[TAGS_NUML1];//Find
		std::map<std::string, int> btypes_cache;//BType

		/*
		*	A "view" for the ANAME attribute - fast lookup by ANAME.
		*	"FFBackwards" returns the last in the list and
		*	"AddField" adds newest field index there, so
		*	"FFBackwards" always returns the first found by that ANAME
		*	from flist.Count () - 1 backwards (towards 0).
		*	That is required because of AINHERIT. A field in "derived"
		*	may depend on a field in "parent" or "parent" "parent", etc.
		*	so instead of tracing back AINHERIT in a complex tree-like
		*	structures, I use a "view" to a "list" thus effectively
		*	detaching the core data container structure from "algorithms".
		*/
		std::map<std::string, NifLib::List<int> *> fview_aname;

		/*
		*	Add a field to the list view
		*/
		NifLib::Field *AddField(NifLib::Tag *field, char *buf, int bl, int type);

		/*
		*	Add a field to the tree view
		*/
		NifLib::TreeNode<NifLib::Field *> *AddNode(
			NifLib::Tag *t,
			NifLib::Field *f,
			NifLib::TreeNode<NifLib::Field *> *pnode);

		/*
		*	Starts from current count-1 and scans untill the first field.
		*	Returns NULL when not found.
		*/
		NifLib::Field *FFBackwards(const char *val, int len);

		/*
		*	FindField Version. Special proc to look for "Version",
		*	"User Version" and "User Version 2" since they're the only
		*	two fields that are not in the block currently being read
		*	except for TCOMPOUND "Header".
		*/
		NifLib::Field *FFVersion(const char *val, int len);

		/*
		*	Same as "FFBackwards ()", but return the index instead.
		*	Used by the main loop of "ReadNif ()" to handle block types.
		*/
		int	FFBackwardsIdx(int attrid, const char *val, int len);

		/*
		*	Clear the view. Use before each block read.
		*	Automatically used in "ReadNifBlock ()".
		*/
		void Reset_FieldViewAName();

		/*
		*	Check if a field is within version requirements
		*/
		bool V12Check(NifLib::Tag *field);
		
		/*
		*	Converts a string to anything supported by "std::stringstream"
		*/
		template <typename T> static T str2(const std::string &val)
		{
			std::stringstream aa;
			aa << val;
			T k;
			aa >> k;
			return k;
		}

		/*
		*	Checks if "buf" contains "unsigned int" only
		*/
		inline bool IsUInt(const char *buf, int len)
		{
			if (len <= 0)
				return false;
			int x;
			for (x = 0; x < len; x++)
				if (buf[x] < '0' || buf[x] > '9' )
					return false;
			return true;
		}

		/*
		*	Return NifLib Type by string name. See "niflib.h".
		*	Returns NIFT_T if unknown.
		*/
		int BType(const char *buf, int bl);

		/*
		*	Evaluates expressions. Searches for fields up in "flist".
		*	Not all-purposes evaluator.
		*	- if you have more than one condition always put brackets:
		*		() && () || () ...
		*		brackets can be omitted only when you have exactly one condition:
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
		int Evaluate(NifLib::Attr* cond);

		/*
		*	Find out whats in "val[0;len)"
		*	Returns EVAL_TYPE_UNKNOWN when it can not recognize the contents.
		*	See "niflib.h" for the types it "recognizes".
		*/
		int EvalDeduceType(const char *val, int len);

		/*
		*	Handles brackets.
		*	Works with: uint, EVAL_OPB_OR, EVAL_OPB_AND and "!"
		*	Example: "((1)&&(!((0)&&(1))))"
		*	Note: "(Has Normals) && (TSpace Flag & 240)", so
		*		  here we'll receive something like:
		*		  "(1)&&(240)"
		*/
		int EvaluateL2(NifLib::List<NIFuint> &l2);

		/*
		*	Initialize AARR attribute. Set *i2j to the array field
		*	if any, to indicate jagged array
		*/
		NIFint InitArr(NifLib::Attr *aarr);

		/*
		*	Read one object from the .nif
		*/
		int ReadObject(NifStream &s, NifLib::Tag *t,
			NifLib::TreeNode<NifLib::Field *> *n);

		/*
		*	Reads a single block from a .nif file.
		*	See "nif.xml" for details.
		*	Args:
		*	 - "s" - a stream used to read data from the file
		*	 - "n" - "compound" or "niobject" name
		*	 - nlen - "n" length
		*/
		int ReadNifBlock(NifStream &s, const char *n, int nlen);
	public:
		Compiler(const char *fname);

		/*
		*	Clean up buffers, cache and the likes. Automatically called
		*	on "ReadNif ()".
		*/
		void Reset();

		/*
		*	Sequential field access - for fast operations like texture
		*	paths checking, etc. The fields accessed here are the fields
		*	stored by "WriteNif ()".
		*/
		NifLib::Field *operator[](int index);

		/*
		*	Sequential field access - field count
		*/
		int FCount();

		/*
		*	Converts a string version like "20.0.0.5" into NIFuint
		*/
		NIFuint HeaderString2Version(const char *buf, int bl);

		/*
		*	Returns TBASIC for that ATYPE attribute if there is TBASIC
		*	reachable for it.
		*/
		NifLib::Tag *GetBasicType(NifLib::Attr *type);

		/*
		*	Returns tag "tagid" with ANAME "attrvalue".
		*	Returns NULL if there is no such tag.
		*/
		NifLib::Tag *Find(int tagid, const char *attrvalue, int len);

		/*
		*	Parse a file using the currently loaded and "Build ()"
		*	XML document
		*/
		int ReadNif(const char *fname);

		/*
		*	Store the sequential field list to a file
		*/
		void WriteNif(const char *fname);

		/*
		*	Prepares the Compiler for parsing binary files using the
		*	currently loaded XML document. Also, call this method when the
		*	XML document was edited and you want to "apply the changes".
		*/
		void Build();

		/*
		*	debug: print the fields
		*/
		void DbgPrintFields();

		/*
		*	debug: print the fields - "ftree" "walker"
		*/
		void PrintNode(NifLib::TreeNode<NifLib::Field *> *node, std::string ofs);

		/*
		*	Returns tag ANAME attribute as a string
		*/
		static std::string TagName(NifLib::Tag *tag);

		/*
		*	Return the fields as a tree structure.
		*	It is not automatically synchronized with the list:
		*	- if you modify the tree, changes wont reflect in the list;
		*	- if you modify the list, changes wont reflect in the tree;
		*	- if you modify field data, changes will reflect in both:
		*		- when a field is pointed by a pointer in both structures;
		*	The tree contains Fields what are not present in the list.
		*	All list fields are present in the tree.
		*	The tree is useful for UI tools, like "NifSkope".
		*	The list is useful for fast tools - scanners, checkers, optimizers,
		*	etc.
		*/
		NifLib::TreeNode<NifLib::Field *> *AsTree();

		~Compiler();
	};
}

#endif /*__COMPILER_H__*/
