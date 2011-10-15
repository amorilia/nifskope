#ifndef __QT4APP_H__
#define __QT4APP_H__

#include <NifSkopeApp.h>

#include <QtGui>

namespace NifSkopeQt4
{
	/*
	*   @brief Main window - Qt style
	*/
	class Qt4App: public NifSkope::NifSkopeApp
	{
	protected:
		void LoadStyle(QApplication &app);
	public:
		virtual int Run(int argc, char **argv);
	};
}

#endif /*__QT4APP_H__*/
