#ifndef __NIFSKOPEEXCEPTION_H__
#define __NIFSKOPEEXCEPTION_H__

#include <string>

namespace NifSkope
{	
	class NifSkopeException
	{
	public:
		NifSkopeException(std::string msg);
		std::string Message;
	};
}

#endif /*__NIFSKOPEEXCEPTION_H__*/
