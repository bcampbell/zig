#include "util.h"
#include <ctype.h>
#include <stdlib.h>

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <shlobj.h>
#else
	// for mkdir, stat, opendir, readdir etc...
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <errno.h>
#endif //!_WIN32

#if defined( __APPLE__ ) && defined( __MACH__ )
    #include <sys/syslimits.h>  // for PATH_MAX
    #include "osx_glue.h"
#endif



// Splits a line of text into whitespace-delimited parts.
// Quoted strings are handled
// Unix-style comments are handled - any text after a '#' is ignored.
//
// Resultant parts returned in args.
void SplitLine( std::string const& line,
	std::vector<std::string>& args )
{
	std::string::const_iterator it=line.begin();

	while( true ) 	//it != line.end() )
	{
		// skip whitespace
		while( it!=line.end() && isspace( *it ) )
			++it;

		if( it==line.end() )
			break;

		// comment?
		if( *it == '#' )
			break;		// skip rest of line

		// collate arg
		args.push_back("");
		std::string& a = args.back();

		if( *it == '\"' || *it == '\'' )
		{
			// quoted string
			char q = *it++;
			while( it!=line.end() && *it != q )
				a += *it++;

			if( it != line.end() )
				++it;	// skip final quote
		}
		else
		{
			// unquoted string
			while( it!=line.end() && !isspace(*it) )
				a += *it++;
		}
	}
}



// Given a path, returns just the file part.
// eg "/foo/bar/wibble.txt" will return "wibble.txt"
// note: results undefined if there is no file part (ie is fullpath ends with
// a trailing slash).
std::string BaseName( std::string const& fullpath )
{
	std::string::size_type pos = fullpath.find_last_of( "/\\:" );
	if( pos == std::string::npos )
		return fullpath;
	++pos;
	if( pos > fullpath.length() )
		return "";
	return fullpath.substr( pos );
}

// returns the directory part of fullpath
// eg "/foo/bar/wibble.txt" will return "/foo/bar"
std::string DirName( std::string const& fullpath )
{
	std::string::size_type pos = fullpath.find_last_of( "/\\:" );
	if( pos == std::string::npos )
		return ".";
	return fullpath.substr( 0, pos );
}

// Build a filename from two parts.
// eg JoinPath( "/foo/bar", "wibble.txt" )  -> "/foo/bar/wibble.txt"
// Trailing slashes on first part are optional.
std::string JoinPath( std::string const& a, std::string const& b )
{
	if( a.empty() )
		return b;

	std::string ret(a);
	char c = a[a.size()-1];

#ifdef _WIN32
	if( c != '/' && c != '\\' && c != ':' )
		ret += '/';
#else
	if( c != '/' )
		ret += '/';
#endif

	ret+=b;
	return ret;
}






#ifdef _WIN32

// Windows version

std::string PerUserDir()
{
	char buf[ MAX_PATH ];
	HRESULT result = SHGetFolderPathA( 0, CSIDL_APPDATA, 0, 0, buf );
	// |CSIDL_FLAG_CREATE
	if( !SUCCEEDED( result ) )
		return "";

	return buf;
}

#elif defined( __APPLE__ ) && defined( __MACH__ )

// OSX version
// Use the Application Support dir (this should handle sandboxed and non-sandboxed)

std::string PerUserDir()
{
    char buf[PATH_MAX];
    if(!osx_get_app_support_path(buf,PATH_MAX))
        return "";
    return buf;
}

#else

// standard unix version

std::string PerUserDir()
{
	const char* home = getenv( "HOME");
	if( !home )
		return "";

	return home;
}

#endif


#ifdef _WIN32

bool MakeDir( std::string const& dir )
{
	if( !CreateDirectoryA( dir.c_str(), 0 ) )
	{
		DWORD err = GetLastError();
		if( err != ERROR_ALREADY_EXISTS )
		{
			return false;
		}
	}

	return true;
}

#else

bool MakeDir( std::string const& dir )
{
	int r = mkdir( dir.c_str(),
		S_IRUSR | S_IWUSR | S_IXUSR |
		S_IRGRP | S_IXGRP |
		S_IROTH | S_IXOTH);
	if( r<0 )
	{
		if( errno != EEXIST )
			return false;
		// TODO: make sure it's a dir!
	}

	return true;
}

#endif

