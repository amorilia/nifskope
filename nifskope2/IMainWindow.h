#ifndef __IMAINWINDOW_H__
#define __IMAINWINDOW_H__

#include "MainWindow.h"

namespace NifSkope
{
	/**
	*	@brief UI "bridge" - a single level "decorator"
	*/
	class IMainWindow
	{
		IMainWindow();
		IMainWindow(IMainWindow &);
		IMainWindow &operator=(const IMainWindow &param);
	protected:
		MainWindow *mw;
	public:
		IMainWindow(MainWindow *mainwindow);
		virtual int Run(int argc, char **argv);
	};
}

#endif /*__IMAINWINDOW_H__*/
