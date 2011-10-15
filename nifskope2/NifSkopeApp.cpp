#include "NifSkopeApp.h"

#include "nifskope.h"
#include "NifSkopeException.h"

namespace NifSkope
{
	NifSkopeApp::NifSkopeApp()
	{
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
		INFO("~NifSkopeApp()")
	}
}
