#include "RudyPCH.h"
#include "FileSystem.h"

namespace Rudy {


	std::string& FileSystem::ConvertPath(std::string& path)
	{
		std::string result = path;
		//note it's \\ 
		std::replace(result.begin(), result.end(), '\\', '/');
		return result;

	}
	 
}
