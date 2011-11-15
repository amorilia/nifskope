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

#include "NifSkopeApp.h"

#include "nifskope.h"
#include "NifSkopeException.h"

// "NifLib"
#include "Compiler.h"

namespace NifSkope
{
	void NifSkopeApp::NifTreePrefixWalk(
		NifLib::TreeNode<NifLib::Field *> *node,
		int (NifSkopeApp::*actn)(NifLib::TreeNode<NifLib::Field *> *node))
	{
		if (!actn)
			return;
		if (!node)
			return;
		for (int i = 0; i < node->Nodes.Count (); i++) {
			if (!((this->*actn) (node->Nodes[i])))
				break;
			NifTreePrefixWalk (node->Nodes[i], actn);
		}
	}

	int
	NifSkopeApp::wFindFieldByName(NifLib::TreeNode<NifLib::Field *> *node)
	{
		if (node->Value->Name () == wName) {
			wField = node->Value;
			throw 1;// stop the recursion
		}
		else
			return 1;// continue searching
	}

	/*NifLib::List< NifLib::List<NifLib::Field *> *> *
	NifSkopeApp::AsBlocks()
	{
		NSINFO("Requested Block View")
		NSINFO(" Block View: contains " << vBlock.Count () << " blocks")
		return &vBlock;
	}*/

	NifSkopeApp::NifSkopeApp()
	{
	}

	NifLib::TreeNode<NifLib::Field *> *
	NifSkopeApp::AsTree()
	{
		//NSINFO("Requested Tree View")
		//if (!File.Loaded ())
		//	NSINFO(" no file loaded")
		return File.NifFile->AsTree ();
	}

	NifLib::Field *
	NifSkopeApp::ByName(std::string name, NifLib::TreeNode<NifLib::Field *> *node = NULL)
	{
		wField = NULL;
		wName = name;
		try {
			if (!node) {
				NifLib::TreeNode<NifLib::Field *> *nif = AsTree ();
				NifTreePrefixWalk (nif, &NifSkopeApp::wFindFieldByName);
			} else
				NifTreePrefixWalk (node, &NifSkopeApp::wFindFieldByName);
		} catch (int) {
		}
		return wField;
	}

	NifLib::Field *
	NifSkopeApp::ByName(std::string name, int idx)
	{
		NifLib::TreeNode<NifLib::Field *> *nif = AsTree ();
		if (!ValidRootNodeIdx (idx))
			return NULL;
		return ByName (name, nif->Nodes[idx]);
	}

	std::string
	NifSkopeApp::GetRootNodeValue(int idx)
	{
		NifLib::Field *f = ByName ("Value", idx);
		if (f)
			return f->AsString (File.NifFile);
		else
			return std::string ("");
	}

	std::string
	NifSkopeApp::GetRootNodeName(int idx)
	{
		NifLib::TreeNode<NifLib::Field *> *nif = AsTree ();
		if (!ValidRootNodeIdx (idx))
			return std::string ("[ERR: index out of range]");
		return nif->Nodes[idx]->Value->Name ();
	}

	bool
	NifSkopeApp::ValidRootNodeIdx(int idx)
	{
		NifLib::TreeNode<NifLib::Field *> *nif = AsTree ();
		return (idx >= 0) && (idx < nif->Nodes.Count ());
	}

	std::string
	NifSkopeApp::ToStrRef(NIFint ref)
	{
		std::stringstream s;
		if (!ValidRootNodeIdx (ref))
			s << "None";
		else {
			std::string value = GetRootNodeValue (ref + 1);
			if (value == "") // if empty - use node name
				s << ref << " [" << GetRootNodeName (ref + 1) << "]";
			else // otherwise - use node value
				s << ref << " (" << value << ")";
		}
		return s.str ();
	}

	std::string
	NifSkopeApp::ToStrBlockTypeIndex(NIFushort bti)
	{
		std::stringstream s;
		if (!ValidRootNodeIdx (bti))
			s << "[ERR: invalid BTN_BLOCKTYPEINDEX]";
		else
			s << GetRootNodeName (bti + 1) << " [" << bti << "]";
		return s.str ();
	}

	std::string
	NifSkopeApp::ToStrBool(NIFbyte value)
	{
		if (value)
			return std::string ("true");
		else
			return std::string ("false");
	}

	std::string
	NifSkopeApp::ToStrFixedSizeStruct(NifLib::Field *f)
	{
		// Handle fixed size complex fields - "Packed" fields of non-array
		// type what are not "auto-expanded" by the tree view in the
		// compiler as the other non-fixed size ones.
		NifLib::Tag *tt = f->TypeTag ();
		std::stringstream result;
		NIFfloat *fbuf = (NIFfloat *)&(f->Value.buf[0]);
		for (int i = 0; i < tt->Tags.Count (); i++) {
			NifLib::Tag *ct = tt->Tags[i];
			NifLib::Attr *name = ct->AttrById (ANAME);
			if ((ct->NLType & NIFT_BT) == BtnType (BTN_FLOAT) ) {
				result << std::string (name->Value.buf, name->Value.len) << ": ";
				result << fbuf[i];
			} else
				result << std::string (name->Value.buf, name->Value.len) << ": -";
			if (i < tt->Tags.Count () -1)
				result << ", ";
		}
		return result.str ();
	}

	std::string
	NifSkopeApp::ToStr(NifLib::Field *f, int ofs)
	{
		// TODO: optimizie: to method pointers
		// WHEN: when completed and this type of comm. is chosen
		// "QTreeView" calls "the model" and "the model" calls this quite
		// often - when a selection is changed, when a column is resized, etc.
		if (f->TypeId () == BTN_REF) {
			NIFint *buf = (NIFint *)&(f->Value.buf[0]);
			return ToStrRef (buf[ofs]);
		} else
		if (f->TypeId () == BTN_BLOCKTYPEINDEX) {
			NIFushort *buf = (NIFushort *)&(f->Value.buf[0]);
			return ToStrBlockTypeIndex (buf[ofs]);
		} else
		if (f->TypeId () == BTN_BOOL) {
			NIFbyte *buf = (NIFbyte *)&(f->Value.buf[0]);
			return ToStrBool (buf[f->FixedSize ()*ofs]);
		} else
		if (f->TypeId () == NIFT_T && f->FixedSize () > 0) {
			return ToStrFixedSizeStruct (f);
		}
		else
			return f->AsString (File.NifFile);
	}

	void
	NifSkopeApp::ExpandToAArr1(NifLib::TreeNode<NifLib::Field *> *node, int itemsize)
	{
		NifLib::Tag *tag = node->Value->TypeTag ();
		if (!tag)
			return;// unknown type
		char *buf = (char *)&(node->Value->Value.buf[0]);
		int cnt = node->Value->Value.len/itemsize;
		for (int i = 0; i < cnt; i++) {
			NifLib::TreeNode<NifLib::Field *> *n =
				new NifLib::OwnerTreeNode<NifLib::Field *>;
			n->Parent = node;
			NifLib::Field *f = new NifLib::Field ();
			f->BlockTag = node->Value->BlockTag;
			f->JField = NULL;
			f->Tag = tag;
			f->Value.CopyFrom ((const char *)&buf[itemsize*i], itemsize);// copy 1
			n->Value = f;
			node->Nodes.Add (n);
			n->Index = node->Nodes.Count () - 1;
		}
	}

	void
	NifSkopeApp::ExpandNode(NifLib::TreeNode<NifLib::Field *> *node)
	{
		if (node->Nodes.Count () > 0)
			return;// nothing to expand
		NifLib::Field *f = node->Value;
		if (f->IsArray1D () && !f->IsArrayJ () && !f->IsCharArray ()) {// AARR1
			if (f->FixedSize () > 0)
				ExpandToAArr1 (node, f->FixedSize ());
		} else
		if (f->IsArray2D () && f->IsArrayJ () && !f->IsCharArray ()) {// AARR2 J
/*#define BTN_BYTE 1
#define BTN_UINT 2
#define BTN_USHORT 3
#define BTN_INT 4
#define BTN_SHORT 5*/
			if (f->FixedSize () <= 0)
				return;
			char *buf = (char *)&(f->Value.buf[0]);
			int itemsize = f->FixedSize ();
			//int cnt = f->Value.len/f-FixedSize ();
			if (f->JField->TypeId () == BTN_USHORT) {
				NIFushort *lengths = (NIFushort *)&(f->JField->Value.buf[0]);
				int cnt = f->JField->Value.len / f->JField->FixedSize ();
				NSINFO("cnt: " << cnt)
				int base = 0;
				for (int i = 0; i < cnt; i++) {
					int cnt2 = lengths[i];

					NifLib::TreeNode<NifLib::Field *> *n1 =
						new NifLib::OwnerTreeNode<NifLib::Field *>;
					n1->Parent = node;
					NifLib::Field *nf1 = new TagOwnerField ();
					nf1->BlockTag = f->BlockTag;
					nf1->JField = NULL;

					nf1->Tag = new NifLib::Tag;
					nf1->Tag->TypeTag = f->TypeTag ();
					nf1->Tag->NLType = nf1->Tag->TypeTag->NLType;
					NifLib::Attr *aname = new NifLib::Attr (*f->Tag->AttrById (ANAME));
					NifLib::Attr *atype = new NifLib::Attr (*f->Tag->AttrById (ATYPE));
					NifLib::Attr *aarr1 = new NifLib::Attr (AARR1);
					std::stringstream conv;
					conv << "\"" << f->JField->Name () << "\"[" << i << "]=" << cnt2;
					std::string asstr = conv.str ();
					aarr1->Value.CopyFrom (asstr.c_str (), asstr.length ());
					nf1->Tag->Attr.Add (aname);
					nf1->Tag->Attr.Add (atype);
					nf1->Tag->Attr.Add (aarr1);

					nf1->Value.CopyFrom (&(buf[base]), itemsize*cnt2);
					n1->Value = nf1;
					node->Nodes.Add (n1);
					n1->Index = node->Nodes.Count () - 1;

					base += (itemsize*cnt2);
				}
			}
		}
	}

	int
	NifSkopeApp::Run(int argc, char **argv)
	{
		return 0;
	}

	void
	NifSkopeApp::NewWindow()
	{
	}

	void
	NifSkopeApp::Quit()
	{
	}

	NifSkopeApp::~NifSkopeApp()
	{
		NSINFO("~NifSkopeApp()")
	}
}
