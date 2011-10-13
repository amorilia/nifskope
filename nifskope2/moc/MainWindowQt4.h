#ifndef __MAINWINDOWQT4_H__
#define __MAINWINDOWQT4_H__

#include <MainWindow.h>

#include <QObject>
#include <QtGui>

namespace NifSkopeQt4
{
	/**
	*   @brief Main window - Qt style
	*/
	class MainWindowQt4: public QObject, public NifSkope::MainWindow
	{
	Q_OBJECT
	protected:
		void LoadStyle(QApplication &app);
	public:
		virtual int Run(int argc, char **argv);
	};
}

#endif /*__MAINWINDOWQT4_H__*/
