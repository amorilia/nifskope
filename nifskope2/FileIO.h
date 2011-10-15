#ifndef __FILEIO_H__
#define __FILEIO_H__

#include <string>
#include <vector>

namespace NifSkope
{
	class FileIO
	{
		std::vector<std::string> fmt_load;
	public:
		FileIO();
		std::string FileName;
		virtual void Load();
		std::string	GetLoadFormats(std::string pairSeparator, std::string listSeparator);
		/*virtual void SaveAs();
		virtual void Import();
		virtual void Export();
		virtual void ReloadXML();// reloads nif.xml and kfm.xml
		virtual void ReloadXMLAndNif(); // ReloadXML() + current .nif*/
	};
}

#endif /*__FILEIO_H__*/
