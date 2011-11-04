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
	NifSkopeApp::NifSkopeApp()
	{
	}

	NifLib::List< NifLib::List<NifLib::Field *> *> *
	NifSkopeApp::AsBlocks()
	{
		NSINFO("Requested Block View")
		if (!File.Loaded ()) {
			NSINFO(" Block View: no file loaded")
			return NULL;
		}
		for (int i = 0; i < vBlock.Count (); i++) {
			vBlock[i]->Clear ();
			delete vBlock[i];
		}
		vBlock.Clear ();
		NifLib::Compiler *nif = File.NifFile;
		int bIdx = -1;
		for (int i = 0; i < nif->FCount (); i++) {
			NifLib::Field *f = (*nif)[i];
			if (i > 0 && f->BlockTag != ((*nif)[i-1])->BlockTag)
				bIdx++;
			// BlockIndex is sequiential. -1 is compound name="header"
			int block = bIdx + 1;
			if (vBlock.Count () <=  block)
				vBlock.Add (new NifLib::List<NifLib::Field *>);
			vBlock[block]->Add (f);
		}
		NSINFO(" Block View: contains " << vBlock.Count () << " blocks")
		return &vBlock;
	}

	NifLib::TreeNode<NifLib::Field *> *
	NifSkopeApp::AsTree()
	{
		NSINFO("Requested Block View")
		if (!File.Loaded ()) {
			NSINFO(" Block View: no file loaded")
			return NULL;
		}
		return File.NifFile->AsTree ();
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
