#ifndef RESOURCES_H
#define RESOURCES_H


#include <string>


// static class to handle platform-specific data file locations.

class Resources
{
public:
	static void Init();

	// get the full filename for a resource
	static std::string Map( std::string const& name );

	//
	static std::string ResourcePath()
		{ return s_ResourcePath; }
private:
	static std::string s_ResourcePath;
};


#endif // RESOURCES_H

