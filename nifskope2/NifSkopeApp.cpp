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
		NifLib::Node *node,
		int (NifSkopeApp::*actn)(NifLib::Node *node),
		int (NifSkopeApp::*filter)(NifLib::Node *node))
	{
		if (!actn)
			return;
		if (!node)
			return;
		if (filter && (this->*filter) (node))
			return;
		for (int i = 0; i < node->Nodes.Count (); i++) {
			if (!((this->*actn) (node->Nodes[i])))
				break;
			NifTreePrefixWalk (node->Nodes[i], actn);
		}
	}

	int
	NifSkopeApp::wFilterArrays(NifLib::Node *node)
	{
		return node->Value &&
			(node->Value->IsArray1D () || node->Value->IsArray2D ());
	}

	int
	NifSkopeApp::wFindFieldByName(NifLib::Node *node)
	{
		if (node->Value->Name () == wName) {
			wField = node->Value;
			throw 1;// stop the recursion
		}
		else
			return 1;// continue searching
	}

	NifSkopeApp::NifSkopeApp()
	{
	}

	NifLib::Node *
	NifSkopeApp::AsTree()
	{
		return File.NifFile->AsTree ();
	}

	NifLib::Node *
	NifSkopeApp::GetFooter()
	{
		NifLib::Node *root = AsTree ();
		int idx = root->Nodes.Count () - 1;
		if (idx >= 0)
			return root->Nodes[idx];
		return NULL;
	}

	NifLib::Node *
	NifSkopeApp::AsNifTree()
	{
		if (nifview.Nodes.Count () > 0)
			return &nifview;
#define ASSERT(A) if (!(A))	{NSERR("Assertion failed: "#A) return NULL;}
#define DI(F1,F2,V) NSINFO(F1->Name () << "." << F2->Name () << ": " << V)
		NifLib::Node *root = AsTree ();
		// Header is always [0].
		// "Footer" has "root"s. TODO: What has no "Footer"? So far all
		// versions supported by NifLib do.
		//  - if no(Footer) use first after "Header".
		NifLib::Node *footer = GetFooter ();
		if (!footer) {
			if (root->Nodes.Count () > 1)
				nifview.Add (root->Nodes[1]->Value);// use first after "Header"
		} else {
			NSINFO(footer->Value->Name ())
			NifLib::Field *fNum_Roots = ByName ("Num Roots", footer);
			ASSERT(fNum_Roots != NULL)
			NifLib::Field *fRoots = ByName ("Roots", footer);
			ASSERT(fRoots != NULL)
			// Sanity check
			ASSERT(fRoots->FixedSize () > 0)
			NIFint vNum_Roots = fNum_Roots->AsNIFint ();
			NIFint size = fRoots->FixedSize ();
			ASSERT((size * vNum_Roots) == fRoots->Value.len)
			if (size > 4) {
				DI(footer->Value, fRoots, "Unsupported size: " << size)
				return NULL;
			}
			// Get roots
			NIFbyte *buf = (NIFbyte *)&(fRoots->Value.buf[0]);
			NIFbyte *buf2 = (NIFbyte *)&(fRoots->Value.buf[fRoots->Value.len]);
			DI(footer->Value, fNum_Roots, vNum_Roots)
			NIFint idx;
			for (; buf != buf2; buf += size) {
				idx = 0;
				memcpy (&idx, buf, size);// TODO: Endianness
				idx++;
				DI(footer->Value, fRoots, idx)
				ASSERT(idx > 0 && idx < root->Nodes.Count () - 1)
				nifview.Add (root->Nodes[idx]->Value);
			}
		}
#undef DI
#undef ASSERT
		return &nifview;
	}

	NifLib::Field *
	NifSkopeApp::ByName(std::string name, NifLib::Node *node)
	{
		wField = NULL;
		wName = name;
		try {
			if (!node) {
				NifLib::Node *nif = AsTree ();
				NifTreePrefixWalk (nif, &NifSkopeApp::wFindFieldByName,
					&NifSkopeApp::wFilterArrays);
			} else
				NifTreePrefixWalk (node, &NifSkopeApp::wFindFieldByName,
					&NifSkopeApp::wFilterArrays);
		} catch (int) {
		}
		return wField;
	}

	std::string
	NifSkopeApp::GetNodeValue(NifLib::Node *node)
	{
		// TODO: what if its not named "Value"?
		NifLib::Field *f = ByName ("Value", node);
		if (f)
			return f->AsString (File.NifFile);
		else
			return std::string ("");
	}

	std::string
	NifSkopeApp::GetNodeName(NifLib::Node *node)
	{
		if (node->Value)
			return node->Value->Name ();
		else
			return "";
	}

	bool
	NifSkopeApp::ValidRootNodeIdx(int idx)
	{
		NifLib::Node *nif = AsTree ();
		return (idx >= 0) && (idx < nif->Nodes.Count ());
	}

	std::string
	NifSkopeApp::ToStrRef(NIFint ref)
	{
		std::stringstream s;
		if (!ValidRootNodeIdx (ref))
			s << "None";// TODO: to options: text if no "ref" or "ptr"
		else {
			NifLib::Node *nif = AsTree ();
			NifLib::Node *rn = nif->Nodes[ref + 1];
			std::string value = GetNodeValue (rn);
			if (value == "") // if empty - use node name
				s << ref << " [" << GetNodeName (rn) << "]";
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
		else {
			NifLib::Node *nif = AsTree ();
			NifLib::Node *rn = nif->Nodes[bti + 1];
			s << GetNodeName (rn) << " [" << bti << "]";
		}
		return s.str ();
	}

	std::string
	NifSkopeApp::ToStrBool(NIFbyte value)
	{
		if (value)
			return std::string ("true");// TODO: to options: bool display
		else
			return std::string ("false");// TODO: to options: bool display
	}

	std::string
	NifSkopeApp::ToStrFixedSizeStruct(NifLib::Field *f)
	{
		// Handle fixed size complex fields
		NifLib::Tag *tt = f->TypeTag ();
		std::stringstream result;
		char *p = (char *)&(f->Value.buf[0]);
		for (int i = 0; i < tt->Tags.Count (); i++) {
			NifLib::Tag *ct = tt->Tags[i];
			int size = ct->FixedSize;
			if (size <= 0)
				return "[ERROR: FixedSize can not be <= 0 in Packed field]";
			NifLib::Attr *name = ct->AttrById (ANAME);
			result << std::string (name->Value.buf, name->Value.len) << ": ";
			NifLib::Field *air = new NifLib::Field;
			air->BlockTag = f->BlockTag;
			air->JField = NULL;
			air->Tag = ct;
			air->Value.CopyFrom (p, size);
			result << ToStr (air);
			delete air;
			p += size;
			if (i < tt->Tags.Count () -1)
				result << ", ";
		}
		return result.str ();
	}

	std::string
	NifSkopeApp::ToStr(NifLib::Field *f, NifLib::Node *node)
	{
		// TODO: optimizie: to method pointers
		// WHEN: when completed and this type of comm. is chosen
		// "QTreeView" calls "the model" and "the model" calls this quite
		// often - when a selection is changed, when a column is resized, etc.
		if (f->TypeId () == BTN_REF || f->TypeId () == BTN_PTR)
			return ToStrRef (f->AsNIFint ());
		else if (f->TypeId () == BTN_BLOCKTYPEINDEX)
			return ToStrBlockTypeIndex (f->AsNIFushort ());
		else if (f->TypeId () == BTN_BOOL)
			return ToStrBool (f->AsNIFbyte ());
		else if (f->TypeId () == NIFT_T && f->FixedSize () > 0)
			return ToStrFixedSizeStruct (f);
		// Try returning first-found "Value" field if "node" is provided.
		// "SizedString" and the likes display.
		if (node) {
			// TODO: what if its not named "Value"?
			NifLib::Field *tmp = ByName ("Value", node);
			if (tmp)
				f = tmp;
		}
		return f->AsString (File.NifFile);
	}

	void
	NifSkopeApp::ExpandToAArr1(NifLib::Node *node, int itemsize)
	{
		if (itemsize <= 0)
			return;// complex type - i.e. dynamic size items
		NifLib::Tag *tag = node->Value->TypeTag ();
		if (!tag)
			return;// unknown type
		char *buf = (char *)&(node->Value->Value.buf[0]);
		int cnt = node->Value->Value.len/itemsize;
		for (int i = 0; i < cnt; i++) {
			NifLib::Node *n = new NifLib::OwnerNode;
			n->Parent = node;
			NifLib::Field *f = new NifLib::Field ();
			f->BlockTag = node->Value->BlockTag;
			f->JField = NULL;
			f->Tag = tag;
			f->Value.CopyFrom ((const char *)&buf[itemsize*i], itemsize);// copy 1
			n->Value = f;
			node->Nodes.Add (n);
		}
	}

	void
	NifSkopeApp::ExpandToAArr2(NifLib::Node *node, int itemsize)
	{
		if (itemsize <= 0)
			return;// complex type - i.e. dynamic size items
		NifLib::Field *f = node->Value;
		if (!f->TypeTag ())
			return;// unknown type
		// compute size
		char *buf = (char *)&(f->Value.buf[0]);
		int totalcnt = f->Value.len/itemsize;
		int h = f->Arr1;
		int w = totalcnt / h;
		// create
		for (int i = 0; i < h; i++) {
			NifLib::Node *n1 = new NifLib::OwnerNode;
			n1->Parent = node;
			NifLib::Field *nf1 = new TagOwnerField ();
			nf1->BlockTag = f->BlockTag;
			nf1->JField = NULL;
			// create artificial tag
			NifLib::Tag *tag = new NifLib::Tag;
			tag->TypeTag = f->TypeTag ();
			tag->NLType = tag->TypeTag->NLType;
			NifLib::Attr *aname = new NifLib::Attr (*f->Tag->AttrById (ANAME));
			NifLib::Attr *atype = new NifLib::Attr (*f->Tag->AttrById (ATYPE));
			NifLib::Attr *aarr1 = new NifLib::Attr (AARR1);
			std::stringstream conv;
			conv << w;
			std::string asstr = conv.str ();
			aarr1->Value.CopyFrom (asstr.c_str (), asstr.length ());
			tag->Attr.Add (aname);
			tag->Attr.Add (atype);
			tag->Attr.Add (aarr1);
			nf1->Tag = tag;
			// copy portion
			nf1->Value.CopyFrom (&(buf[i*w]), itemsize*w);
			n1->Value = nf1;
			node->Nodes.Add (n1);
		}
	}

	void
	NifSkopeApp::ExpandNode(NifLib::Node *node)
	{
		if (node->Nodes.Count () > 0)
			return;// nothing to expand
		NifLib::Field *f = node->Value;
		if (!f)
			return;// nothing to expand
		if (f->IsArray1D () && !f->IsArrayJ () && !f->IsCharArray ()) {// AARR1
			ExpandToAArr1 (node, f->FixedSize ());
		} else
		if (f->IsArray2D () && !f->IsArrayJ () && !f->IsCharArray ()) {// AARR2
			ExpandToAArr2 (node, f->FixedSize ());
 		} else
		if (f->IsArray2D () && f->IsArrayJ () && !f->IsCharArray ()) {// AARR2 J
			if (f->JField->TypeId () == BTN_USHORT)
				ExpandJ<NIFushort> (node, f->FixedSize ());
			else if (f->JField->TypeId () == BTN_SHORT)
				ExpandJ<NIFshort> (node, f->FixedSize ());
			else if (f->JField->TypeId () == BTN_UINT)
				ExpandJ<NIFuint> (node, f->FixedSize ());
			else if (f->JField->TypeId () == BTN_INT)
				ExpandJ<NIFint> (node, f->FixedSize ());
			else if (f->JField->TypeId () == BTN_BYTE)
				ExpandJ<NIFbyte> (node, f->FixedSize ());
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
