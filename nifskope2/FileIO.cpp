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

#include "FileIO.h"

#include "nifskope.h"
#include <sstream>

#include "nifskope.h"

namespace NifSkope
{
	FileIO::FileIO()
	{
		fmt_load.push_back ("Nif Files:");
		fmt_load.push_back ("");
		fmt_load.push_back ("");
		fmt_load.push_back ("*.nif");
		fmt_load.push_back ("");
		fmt_load.push_back ("*.kf");
		fmt_load.push_back ("");
		fmt_load.push_back ("*.kfa");
		fmt_load.push_back ("");
		fmt_load.push_back ("*.kfm");
		fmt_load.push_back ("");
		fmt_load.push_back ("*.nifcache");
		fmt_load.push_back ("");
		fmt_load.push_back ("*.texcache");
		fmt_load.push_back ("");
		fmt_load.push_back ("*.pcpatch");
		fmt_load.push_back ("");
		fmt_load.push_back ("*.jmi");
		// set [0] as All
		if (fmt_load.size () < 3)
			return;
		std::stringstream tmp;
		tmp << fmt_load[3];
		for (int i = 5; i < (int)fmt_load.size (); i+=2)
			tmp << " " << fmt_load[i];
		fmt_load[1] = tmp.str();
	}

	FileIO::~FileIO()
	{
		if (NifFile)
			delete NifFile;
	}

	std::string
	FileIO::GetLoadFormats(std::string pairSeparator, std::string listSeparator)
	{
		if (fmt_load.size () < 2)
			return "";
		std::stringstream tmp;
		tmp << fmt_load[0] << pairSeparator
			<< fmt_load[1];
		for (int i = 2; i < (int)fmt_load.size (); i += 2)
			tmp << listSeparator
				<< fmt_load[i] << pairSeparator << fmt_load[i+1];
		return tmp.str ();
	}

	void
	FileIO::Load()
	{
		NSINFO("Loading \"" << FileName << "\"")

		if (!NifFile) {
			NifLib::Compiler *tmp = new NifLib::Compiler ("niflib/nif.xml");
			if (tmp->Loaded ()) {
				NifFile = tmp;
				NifFile->Build ();
			}
		}
		if (NifFile) {
			try {
				if (!NifFile->ReadNif (FileName.c_str ())) {
					NSERR("ReadNif: failed")
					return;
				}
			} catch (...)
				NSERR("ReadNif: An exception was thrown")
			OnLoad.Exec (NULL);
			NSINFO("ReadNif: done")
		}
	}
}
