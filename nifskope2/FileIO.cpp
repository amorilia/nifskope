#include "FileIO.h"

#include "nifskope.h"
#include <sstream>

namespace NifSkope
{
	FileIO::FileIO()
	{
		fmt_load.push_back ("Nif Files:");
		fmt_load.push_back ("");
		fmt_load.push_back ("");
		fmt_load.push_back ("*.nif");
		fmt_load.push_back ("");
		fmt_load.push_back ("*.kf");
		fmt_load.push_back ("");
		fmt_load.push_back ("*.kfa");
		fmt_load.push_back ("");
		fmt_load.push_back ("*.kfm");
		fmt_load.push_back ("");
		fmt_load.push_back ("*.nifcache");
		fmt_load.push_back ("");
		fmt_load.push_back ("*.texcache");
		fmt_load.push_back ("");
		fmt_load.push_back ("*.pcpatch");
		fmt_load.push_back ("");
		fmt_load.push_back ("*.jmi");
		// set [0] as All
		if (fmt_load.size () < 3)
			return;
		std::stringstream tmp;
		tmp << fmt_load[3];
		for (int i = 5; i < (int)fmt_load.size (); i+=2)
			tmp << " " << fmt_load[i];
		fmt_load[1] = tmp.str();
	}

	std::string
	FileIO::GetLoadFormats(std::string pairSeparator, std::string listSeparator)
	{
		if (fmt_load.size () < 2)
			return "";
		std::stringstream tmp;
		tmp << fmt_load[0] << pairSeparator
			<< fmt_load[1];
		for (int i = 2; i < (int)fmt_load.size (); i += 2)
			tmp << listSeparator
				<< fmt_load[i] << pairSeparator << fmt_load[i+1];
		return tmp.str ();
	}

	void
	FileIO::Load()
	{
		INFO("Loading \"" << FileName << "\"")
	}
}
