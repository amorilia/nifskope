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

/*
*	The main file for NifSkope
*/

#include "nifskope.h"
#include "NifSkopeApp.h"
// windowing system selection
#ifdef NIFSKOPE_QT
#include "moc/Qt4App.h"
#endif

namespace NifSkope
{
	class NSC
	{
		/*bool LoadNif  (NifSkope::FileType *, std::string);
		bool SaveNifAs(NifSkope::FileType *, std::string);
		// These work on nif nodes - selected in the tree/list view
		bool Import   (NifSkope::FileType *, std::string);
		bool Export   (NifSkope::FileType *, std::string);

		bool SanitizeBeforeSave;
		bool NewWindow();// creates new mainwindow
		bool ReloadXML();// reloads nif.xml and kfm.xml
		bool ReloadXMLAndNif(); // ReloadXML() + current .nif
		bool XMLChecker(// opens a dialog
			std::string dir
			bool recursive
			bool *.nif
			bool *.kf(a)
			bool *.kfm
			NiNode block_match
			bool report_errors_only
			int threads = 2
			std::string version_match
			Run()
			ReloadXML()
			Close()
 		);// reports progress, is not modal
		bool ResourceFiles(// opens a modal dialog
			bool automatic_selection
			Add(std::tring *.bsa)
			Remove(std::tring *.bsa)
		);
		void Quit();*/
	};
}

/*
*   main
*/
int
main(int argc, char **argv)
{
	NifSkope::NifSkopeApp *app = NULL;
// windowing system selection
#ifdef NIFSKOPE_QT
	app = new NifSkopeQt4::Qt4App();
#endif
	if (app) {
		app->Run (argc, argv);
		delete app;
	} else ERR("No UI toolkit was selected")
	return EXIT_SUCCESS;
}
