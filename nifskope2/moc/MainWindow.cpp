#include "MainWindow.h"

namespace NifSkopeQt4
{
	void
	MainWindow::LoadStyle(QApplication &app)
	{
		QDir qssDir (QApplication::applicationDirPath ());
		// Check app dir, relative from nifskope/release, linux data dir
		QStringList qssList (QStringList ()
			<< "style.qss"
			<< "../style.qss"
			<< "/usr/share/nifskope/style.qss" );
		QString qssName;
		foreach (QString str, qssList) {
			if (qssDir.exists (str)) {
				qssName = qssDir.filePath (str);
				break;
			}
		}
		// load the style sheet if present
		if (!qssName.isEmpty ()) {
			QFile style (qssName);
			if (style.open (QFile::ReadOnly)) {
				app.setStyleSheet (style.readAll());
				style.close ();
			}
		}
	}

	int
	MainWindow::Run(int argc, char **argv)
	{
		QApplication app (argc, argv);
		app.setOrganizationName (NS_ORG);
		app.setApplicationName (NS_NAME);
		app.setOrganizationDomain (NS_URL);
		LoadStyle (app);
		QWidget mainWindow;
		mainWindow.setWindowTitle (
			QApplication::translate("windowlayout", NS_VERSION));
		mainWindow.show();
		return app.exec();
	}

/**
*  @brief Qt UI action handler: Help->About
*/
/*void NifSkope::about()
{
	QString text = tr(
	"<p style='white-space:pre'>NifSkope is a tool for analyzing and editing NetImmerse/Gamebryo '.nif' files.</p>"
	"<p>NifSkope is based on NifTool's XML file format specification. "
	"For more information visit our site at <a href='http://niftools.sourceforge.net'>http://niftools.sourceforge.net</a></p>"
	"<p>NifSkope is free software available under a BSD license. "
	"The source is available via <a href='http://niftools.git.sourceforge.net/git/gitweb.cgi?p=niftools/nifskope'>git</a> "
	"(<a href='git://niftools.git.sourceforge.net/gitroot/niftools/nifskope'>clone</a>) on <a href='http://sourceforge.net'>SourceForge</a>. "
	"Instructions on compiling NifSkope are available on the <a href='http://niftools.sourceforge.net/wiki/NifSkope/Compile'>NifTools wiki</a>.</p>"
	"<p>The most recent version of NifSkope can always be downloaded from the <a href='https://sourceforge.net/projects/niftools/files/nifskope/'>"
	"NifTools SourceForge Project page</a>.</p>"
// only the windows build uses havok
// (Q_OS_WIN32 is also defined on win64)
#ifdef Q_OS_WIN32
	"<center><img src=':/img/havok_logo' /></center>"
	"<p>NifSkope uses Havok(R) for the generation of mopp code. "
	"(C)Copyright 1999-2008 Havok.com Inc. (and its Licensors). "
	"All Rights Reserved. "
	"See <a href='http://www.havok.com'>www.havok.com</a> for details.</p>"
#endif
	"<center><img src=':/img/qhull_logo' /></center>"
	"<p>NifSkope uses Qhull for the generation of convex hulls. "
	"Copyright(c) 1993-2010  C.B. Barber and The Geometry Center. "
	"Qhull is free software and may be obtained from <a href='http://www.qhull.org'>www.qhull.org</a>. "
	"See Qhull_COPYING.txt for details."
	);

	QMessageBox mb( tr("About NifSkope %1 (revision %2)").arg(NIFSKOPE_VERSION).arg(NIFSKOPE_REVISION), text, QMessageBox::Information,
		QMessageBox::Ok + QMessageBox::Default, 0, 0, this);
	mb.setIconPixmap( QPixmap( ":/res/nifskope.png" ) );
	mb.exec();
}*/
}
