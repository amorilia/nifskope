#include "Command.h"

#include "nifskope.h"
#include "NifSkopeException.h"
#include <sstream>

namespace NifSkope
{
	Command::Command()
	{
	}

	Command::Command(Command &)
	{
	}

	Command &
	Command::operator=(const Command &param)
	{
		return const_cast<Command &>(param);
	}

	Command::Command(std::string id)
	{
		this->id = id;
		Enabled = false;
		Name = id;
	}

	void
	Command::Execute()
	{
		std::set<Command *>::iterator i;
		for (i = targets.begin (); i != targets.end (); i++)
			try
			{
				(*i)->Execute ();
			}
			catch (...)
			{
				std::stringstream msg;
				msg << "Command \"" << id << "\"";
				msg << "->\"" << (*i)->id << "\"";
				ERR(msg << " threw an unhandled exception")
				throw;
			}
	}

	Command *
	Command::Subscribe(Command *target)
	{
		if (target == this)
			throw NifSkopeException("Recursion is not supported");
		if (Exists (target))
			throw NifSkopeException (NS_ALREADYTHERE);
		targets.insert (target);
		return target;
	}

	Command *
	Command::Unsubscribe(Command *target)
	{
		if (!Exists (target))
			throw NifSkopeException ("The item does not exist");
		targets.erase (target);
		return target;
	}

	bool
	Command::Exists(Command *target)
	{
		return targets.find (target) != targets.end ();
	}
}
