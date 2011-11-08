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

	/*
	*	Defines how the BTN_REF "looks" like
	*/
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

	/*
	*	Defines how the BTN_BLOCKTYPEINDEX "looks" like
	*/
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

	/*
	*	Returns string representation of a field
	*/
	std::string
	NifSkopeApp::ToStr(NifLib::Field *f, int ofs)
	{
		if (f->TypeId () == BTN_REF) {
			//todo: if ((f->NLType & (NIFT_S | NIFT_4)) == (NIFT_S | NIFT_4)) {
				NIFint *buf = (NIFint *)&(f->Value.buf[0]);
				return ToStrRef (buf[ofs]);
			//}
			//todo: else
			//todo: 	return "[ERR: wrong REF type]";
		} else
		// special handling for some "header" things
		if (f->TypeId () == BTN_BLOCKTYPEINDEX) {
			//"unsigned short"
			NIFushort *buf = (NIFushort *)&(f->Value.buf[0]);
			return ToStrBlockTypeIndex (buf[ofs]);
		}
		else
			return f->AsString (File.NifFile);

		/*NIFint *buf = (NIFint *)&(f->Value.buf[0]);
		int cnt = f->Value.len / sizeof(NIFint);
		if (f->Value.len < (int)sizeof(NIFint))
			s << "[ERR: invalid Ref field]";
		else if (ofs < 0 || ofs >= cnt)
			s << "[ERR: wrong offset]";*/
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
