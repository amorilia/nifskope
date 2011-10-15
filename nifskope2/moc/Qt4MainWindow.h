#ifndef __QT4MAINWINDOW_H__
#define __QT4MAINWINDOW_H__

#include <NifSkopeApp.h>
#include <QtGui>

namespace NifSkopeQt4
{
	class MainWindow: public QMainWindow
	{
		void createMainMenu();
	Q_OBJECT
	public:
		MainWindow();
		NifSkope::NifSkopeApp *App;// TODO: init by Qt4App because of NewWindow() only
	protected slots:
		void mFileLoad();
	};
}

#endif /*__QT4MAINWINDOW_H__*/
