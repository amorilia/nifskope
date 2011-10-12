#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <nifskope.h>

#include <QObject>
#include <QtGui>

namespace NifSkopeQt4
{
	/**
	*   @brief Main window - Qt style
	*/
	class MainWindow: public QObject, public NifSkope::MainWindow
	{
	Q_OBJECT
	public:
		virtual int Run(int argc, char **argv);
	protected:
		void LoadStyle(QApplication &app);
	};
}

#endif /*__MAINWINDOW_H__*/
