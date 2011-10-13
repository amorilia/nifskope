#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

namespace NifSkope
{
	/**
	*	@brief Abstract parent of all UI implementations
	*	QtMainWindow, GTKMainWindow, FooMainWindow, BarMainWindow, ...
	*/
	class MainWindow
	{
	public:
		virtual int Run(int argc, char **argv);
	};
}

#endif /*__MAINWINDOW_H__*/
