#ifndef __COMMANDMANAGER_H__
#define __COMMANDMANAGER_H__

#include "IMainWindow.h"
#include "MainWindow.h"
#include "Command.h"

#include <map>
#include <string>

namespace NifSkope
{
	/**
	*	@brief Abstract parent of all UI Command handlers
	*	usecase:
	*	IMainWindow cm = new CommandManager(new NifSkopeQt4::MainWindow())
	*/
	class CommandManager: public IMainWindow
	{
		std::map<std::string, Command *> map;// owner
	public:
		CommandManager(MainWindow *mainwindow);
		Command *Add(const std::string name);
		bool Exists(const std::string &name) const;
		~CommandManager();
	};
}
#endif /*__COMMANDMANAGER_H__*/
