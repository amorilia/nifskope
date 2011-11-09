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

namespace NifSkope
{
	/*
	*	@brief main application class
	*   State engine
	*/
	class NifSkopeApp
	{
		// .nif views
		// as a list of blocks
		NifLib::List< NifLib::List<NifLib::Field *> *> vBlock;
		// "args" for the walker actions
		// all walker related fields are prefixed with "w"
		NifLib::Field *wField;
		std::string wName;
		void NifTreePrefixWalk(
			NifLib::TreeNode<NifLib::Field *> *node,
			int (NifSkopeApp::*actn) (NifLib::TreeNode<NifLib::Field *> *node));
		int wFindFieldByName(NifLib::TreeNode<NifLib::Field *> *node);

	public:
		NifSkopeApp();

		/* 
		*	Currently loaded .nif file grouped by blocks.
		*	Returns NULL if there is no currently loaded .nif file.
		*/
		//NifLib::List< NifLib::List<NifLib::Field *> *> *AsBlocks();
		NifLib::TreeNode<NifLib::Field *> *AsTree();

		NifLib::Field *ByName(std::string name, NifLib::TreeNode<NifLib::Field *> *node);
		NifLib::Field *ByName(std::string name, int index);
		std::string GetRootNodeValue(int idx);
		std::string GetRootNodeName (int idx);
		bool ValidRootNodeIdx(int idx);

		std::string ToStrRef(NIFint ref);
		std::string ToStrBlockTypeIndex(NIFushort bti);

		/*
		*	Returns string representation of a field
		*/
		std::string ToStr(NifLib::Field *f, int ofs = 0);

		template <typename T> void ExpandToAArr1(NifLib::TreeNode<NifLib::Field *> *node)
		{
			NifLib::Tag *tag = node->Value->TypeTag (File.NifFile);
			if (!tag)
				return;// unknown type
			T *buf = (T *)&(node->Value->Value.buf[0]);
			int cnt = node->Value->Value.len/sizeof(T);
			for (int i = 0; i < cnt; i++) {
				NifLib::TreeNode<NifLib::Field *> *n =
					new NifLib::TreeNode<NifLib::Field *>;
				n->Parent = node;
				NifLib::Field *f = new NifLib::Field ();
				f->BlockTag = node->Value->BlockTag;
				f->JField = NULL;
				f->Tag = tag;
				f->NLType = node->Value->NLType;
				f->Value.CopyFrom ((const char *)&buf[i], sizeof(T));
				n->OwnsValue = 1;
				n->Value = f;
				node->Nodes.Add (n);
				n->Index = node->Nodes.Count () - 1;
			}
		}

		/*
		*	Expands a node to subfields. AARR1
		*/
		void ExpandNode(NifLib::TreeNode<NifLib::Field *> *node);

		// arguments
		bool SanitizeBeforeSave;
		// commands - File
		virtual int Run(int argc, char **argv);
		virtual void NewWindow();// creates new mainwindow

		/*
		*	"contract" - gives access to file IO of the applicaiton.
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
