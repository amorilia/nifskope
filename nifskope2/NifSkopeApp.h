/***** BEGIN LICENSE BLOCK *****

BSD License

Copyright (c) 2005-2012, NIF File Format Library and Tools
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

#ifndef __NIFSKOPEAPP_H__
#define __NIFSKOPEAPP_H__

#include <string>
#include "FileIO.h"
#include "XMLChecker.h"
#include "ResourceFiles.h"

// "NifLib"
#include "List.h"
#include "Field.h"
#include "TreeNode.h"
#include "Tag.h"

namespace NifSkope
{
	/*
	*	Same as Field, just owns the "Tag" field.
	*	Used for array dimensions > 1 where an artificial tag is needed
	*	for correct "display".
	*/
	class TagOwnerField: public NifLib::Field
	{
	public:
		~TagOwnerField()
		{
			delete Tag;
		}
	};

	/*
	*	@brief main application class
	*   State engine
	*/
	class NifSkopeApp
	{
		// .nif views - tree
		NifLib::Node nonif;// empty node to avoid NULL and a lot of "if"
		NifLib::Node nifview;
		// maps the nifview nodes to the real ones
		std::map<NifLib::Node *, NifLib::Node *> nvmap;

		// "args" for the walker actions.
		// All "walker"-related fields are prefixed with "w".
		NifLib::Field *wField;
		NifLib::Node *wNode;
		std::string wName;
		void NifTreePrefixWalk(
			NifLib::Node *node,
			int (NifSkopeApp::*actn) (NifLib::Node *node),
			int (NifSkopeApp::*filter)(NifLib::Node *node) = NULL);
		int wFilterArrays(NifLib::Node *node);
		int wFindFieldByName(NifLib::Node *node);
		int wFindNodeByName(NifLib::Node *node);

		/*
		*	Converts an ARR1 non-J field into a list of fields
		*	each containing a copy of each item of the array.
		*	Doesn't damage anything, the Compiler can handle it.
		*	Now the array can be edited, items modified, etc.
		*	"node->Value" contains the array as a block of bytes.
		*	"node->Nodes" contains the array as a list of fields.
		*	One can edit the list add/remove, or the values and
		*	then cancel or apply the changes for example.
		*/
		void ExpandToAArr1(NifLib::Node *node, int itemsize);

		/*
		*	Converts an ARR2 non-J field into a list of fields suitable for
		*	"ExpandToAArr1 ()".
		*	One can edit the list add/remove, or the values and
		*	then cancel or apply the changes for example. I.e. it does
		*	not destroy the src node data.
		*/
		void ExpandToAArr2(NifLib::Node *node, int itemsize);

		/*
		*	Converts an ARR2 J field into a list of fields suitable for
		*	"ExpandToAArr1 ()".
		*	One can edit the list add/remove, or the values and
		*	then cancel or apply the changes for example. I.e. it does
		*	not destroy the src nodes data.
		*/
		template <typename T> void ExpandJ(NifLib::Node *node, int itemsize)
		{
			if (itemsize <= 0)
				return;
			NifLib::Field *f = node->Value;
			if (f->JField->FixedSize () <= 0)
				return;
			char *buf = (char *)&(f->Value.buf[0]);
			T *lengths = (T *)&(f->JField->Value.buf[0]);
			int h = f->JField->Value.len / f->JField->FixedSize ();
			int base = 0;
			for (int i = 0; i < h; i++) {
				int w = lengths[i];
				NifLib::Node *n1 = new NifLib::OwnerNode;
				n1->Parent = node;
				NifLib::Field *nf1 = new TagOwnerField ();
				nf1->BlockTag = f->BlockTag;
				nf1->JField = NULL;
				// create artificial tag
				nf1->Tag = new NifLib::Tag;
				nf1->Tag->TypeTag = f->TypeTag ();
				nf1->Tag->NLType = nf1->Tag->TypeTag->NLType;
				NifLib::Attr *aname = new NifLib::Attr (*f->Tag->AttrById (ANAME));
				NifLib::Attr *atype = new NifLib::Attr (*f->Tag->AttrById (ATYPE));
				NifLib::Attr *aarr1 = new NifLib::Attr (AARR1);
				std::stringstream conv;
				conv << w;
				std::string asstr = conv.str ();
				aarr1->Value.CopyFrom (asstr.c_str (), asstr.length ());
				nf1->Tag->Attr.Add (aname);
				nf1->Tag->Attr.Add (atype);
				nf1->Tag->Attr.Add (aarr1);
				// copy portion
				nf1->Value.CopyFrom (&(buf[base]), itemsize*w);
				n1->Value = nf1;
				node->Nodes.Add (n1);
				base += (itemsize*w);
			}
		}

		NifLib::Node *GetFooter();

		/*
		*	Expands one node of the logical tree.
		*	"Ref" are "root", "Ptr" are leafs.
		*/
		void NifTreeNodeGen(NifLib::Node *src, NifLib::Node *dst);

		std::string GetNodeName(NifLib::Node *node);

		bool ValidRootNodeIdx(int idx);
	public:
		NifSkopeApp();

		/*
		*	Model provded by NifLib
		*	The file tree as produced by NifLib
		*/
		NifLib::Node *AsTree();

		/*
		*	Model
		*	The nif objects tree - a logical hierarchy defined
		*	by the values of the nif object fields.
		*/
		NifLib::Node *AsNifTree();

		/*
		*	Model
		*	Get strign representation of a node
		*/
		std::string	GetNodeValue(NifLib::Node *node);

		/*
		*	Model
		*	Get compiler tree node by a logical nifnode
		*/
		NifLib::Node *GetTreeNode(NifLib::Node *nifnode);

		/*
		*	Model
		*	Defines how the BTN_REF "looks" like
		*/
		std::string ToStrRef(NIFint ref);

		/*
		*	Model
		*	Defines how the BTN_BLOCKTYPEINDEX "looks" like
		*/
		std::string ToStrBlockTypeIndex(NIFushort bti);

		/*
		*	Model
		*	Defines how the BTN_BOOL "looks" like
		*/
		std::string ToStrBool(NIFbyte value);

		/*
		*	Model
		*	Defines how the packed structures "look" like
		*/
		std::string ToStrFixedSizeStruct(NifLib::Field *f);

		/*
		*	Model
		*	Returns string representation of a field
		*/
		std::string ToStr(NifLib::Field *f, NifLib::Node *node = NULL);

		/*
		*	Model
		*	Expand on demand. Expands a node into sub-fields.
		*	Supports fixed item size: AARR1, AARR2, AARR2 J
		*	TODO: AARR3
		*/
		void ExpandNode(NifLib::Node *node);

		NifLib::Field *ByName(std::string name, NifLib::Node *node = NULL);

		NifLib::Node *NodeByName(std::string name, NifLib::Node *node = NULL);

		// arguments
		bool SanitizeBeforeSave;
		// commands - File
		virtual int Run(int argc, char **argv);
		virtual void NewWindow();// creates new mainwindow

		/*
		*	"contract" - gives access to the file IO of the application.
		*/
		NifSkope::FileIO File;

		NifSkope::XMLChecker XmlChecker;
		NifSkope::ResourceFiles ResourceFiles;
		virtual void Quit();
		//
		~NifSkopeApp();
	};
}
#endif /*__NIFSKOPEAPP_H__*/
