#ifndef __NIFSKOPEAPP_H__
#define __NIFSKOPEAPP_H__

#include <string>
#include "FileIO.h"
#include "XMLChecker.h"
#include "ResourceFiles.h"

namespace NifSkope
{
	/*
	*	@brief main application class
	*   State engine
	*/
	class NifSkopeApp
	{
	public:
		NifSkopeApp();
		// arguments
		std::string FileName;
		bool SanitizeBeforeSave;
		// commands - File
		virtual int Run(int argc, char **argv);
		virtual void NewWindow();// creates new mainwindow
		NifSkope::FileIO File;
		NifSkope::XMLChecker XmlChecker;
		NifSkope::ResourceFiles ResourceFiles;
		virtual void Quit();
		//
		~NifSkopeApp();
	};
}
#endif /*__NIFSKOPEAPP_H__*/
