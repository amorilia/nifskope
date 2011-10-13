#include "IMainWindow.h"

#include <cstdlib>

namespace NifSkope
{
	IMainWindow::IMainWindow()
	{
	}

	IMainWindow::IMainWindow(IMainWindow &)
	{
	}

	IMainWindow &
	IMainWindow::operator=(const IMainWindow &param)
	{
		return const_cast<IMainWindow &>(param);
	}

	IMainWindow::IMainWindow(MainWindow *mainwindow)
	{
		mw = mainwindow;
	}

	int
	IMainWindow::Run(int argc, char **argv)
	{
		return mw->Run (argc, argv);
	}
}
