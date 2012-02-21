/***** BEGIN LICENSE BLOCK *****

BSD License

Copyright (c) 2005-2012, NIF File Format Library and Tools
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the NIF File Format Library and Tools project may not be
   used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***** END LICENCE BLOCK *****/

#ifndef SPELL_DEBUG_H
#define SPELL_DEBUG_H

#include <QWidget>
#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QDateTime>
#include <QWaitCondition>

#include "message.h"

class QCheckBox;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QProgressBar;
class QPushButton;
class QSpinBox;
class QTextBrowser;
class QVBoxLayout;

class FileSelector;

class FileQueue
{
public:
	FileQueue() {}

	QString dequeue();
	
	bool isEmpty() { return count() == 0; }
	int count();
	
	void init( const QString & directory, const QStringList & extensions, bool recursive );
	void clear();

protected:
	QQueue<QString> make( const QString & directory, const QStringList & extensions, bool recursive );

	QMutex mutex;
	QQueue<QString> queue;
};

class TestThread : public QThread
{
	Q_OBJECT
public:
	TestThread( QObject * o, FileQueue * q );
	~TestThread();
	
	QString blockMatch;
	quint32 verMatch;
	bool reportAll;

signals:
	void sigStart( const QString & file );
	void sigReady( const QString & result );
	
protected:
	void run();
	
	QList<Message> checkLinks( const class NifModel * nif, const class QModelIndex & iParent, bool kf );
	
	FileQueue * queue;
	
	QMutex quit;
};

//! The XML checker widget.
class TestShredder : public QWidget
{
	Q_OBJECT
public:
	TestShredder();
	~TestShredder();
	
	static TestShredder * create();
	
protected slots:
	void chooseBlock();
	void run();
	void xml();
	
	void threadStarted();
	void threadFinished();
	
	void renumberThreads( int );
	
protected:
	void	closeEvent( QCloseEvent * );
	
	FileSelector * directory;
	QLineEdit    * blockMatch;
	QCheckBox    * recursive;
	QCheckBox    * chkNif, * chkKf, * chkKfm;
	QCheckBox    * repErr;
	QSpinBox     * count;
	QLineEdit    * verMatch;
	QTextBrowser * text;
	QProgressBar * progress;
	QLabel       * label;
	QPushButton  * btRun;
	
	FileQueue queue;

	QList<TestThread*> threads;
	
	QDateTime time;
};

#endif
