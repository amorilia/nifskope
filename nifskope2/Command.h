#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <set>
#include <string>

namespace NifSkope
{
	/**
	*	@brief Multicast delegate
	*/
	class Command
	{
		std::set<Command *> targets;// not owner
		std::string id;
		Command();
		Command(Command &);
		Command &operator=(const Command &param);
	public:
		Command(std::string name);
		virtual void Execute();
		virtual Command *Subscribe(Command *target);
		virtual Command *Unsubscribe(Command *target);
		bool Exists(Command *target);
		bool Enabled;
		std::string Name;
	};
}

#endif /*__COMMAND_H__*/
