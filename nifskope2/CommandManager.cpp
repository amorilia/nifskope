#include "CommandManager.h"

#include "nifskope.h"
#include "NifSkopeException.h"

namespace NifSkope
{
	CommandManager::CommandManager(MainWindow *mainwindow)
		: IMainWindow (mainwindow)
	{
		mw = mainwindow;
	}

	Command *
	CommandManager::Add(const std::string name)
	{
		if (Exists (name))
			throw NifSkopeException (NS_ALREADYTHERE);
		Command *cmd = new Command (name);
		map.insert (std::pair<std::string, Command *> (name, cmd));
		return cmd;
	}

	bool
	CommandManager::Exists(const std::string &name) const
	{
		return map.find (name) != map.end ();
	}

	CommandManager::~CommandManager()
	{
		INFO("~CommandManager()")
		std::map<std::string, Command *>::iterator i;
		for (i = map.begin(); i != map.end(); i++)
			delete (*i).second;
	}
}
