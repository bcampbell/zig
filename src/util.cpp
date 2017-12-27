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


std::vector<std::string> Split( std::string const& s, char delim)
{
    std::vector<std::string> out;
	std::string::const_iterator it=s.begin();
    while( it!=s.end()) {
        auto a = it;
        while (it!=s.end()) {
            if(*it++ == delim) {
                break;
            }
		}

		out.push_back(std::string(a,it));
	}
    return out;
}


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

bool Mkdir( std::string const& dir )
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

bool Mkdir( std::string const& dir )
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



#ifdef _WIN32
bool FileExists(std::string const& filename)
{
    return GetFileAttributesA(filename.c_str()) != INVALID_FILE_ATTRIBUTES;
    // TODO: check it's a file
}

bool Stat( std::string const& filename, FileInfo& fi)
{
    DWORD attrs = GetFileAttributesA(filename.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }

    fi.attrs = 0;
    if (attrs&FILE_ATTRIBUTE_DIRECTORY) {
        fi.attrs |= FileInfo::ATTR_DIR;
    }
    return true;
}

bool IsPathSeparator( char c)
    { return c=='/' || c=='\\'; }

#else

bool FileExists( std::string const& filename )
{
    struct stat st = {0};
    return (stat(filename.c_str(), &st) == -1) ? false:true;
    // TODO: check it's a file
}

bool Stat( std::string const& filename, FileInfo& fi)
{
    struct stat st = {0};
    if (stat(filename.c_str(), &st) == -1) {
       return false;
    }

    fi.attrs = 0;
    if (S_ISDIR(st.st_mode)) {
        fi.attrs |= FileInfo::ATTR_DIR;
    }
    return true;
    
}

bool IsPathSeparator( char c)
    { return c=='/'; }

#endif


bool MkdirAll( std::string const& path )
{
    FileInfo fi;
	if (Stat(path, fi)) {
        if(fi.IsDir()) {
            return true;
        }
        return false;   // exists, but not a dir
    }

    // skip trailing separator
    std::string::const_iterator i,j;
    i = path.end();
  
	while(i!=path.begin() && IsPathSeparator(*(i-1))) {
		--i;
	}

    // get parent
	while (i!=path.begin() && !IsPathSeparator(*(i-1))) {
		--i;
	}

	if (i!=path.begin()) {
		// Create parent
        std::string parent(path.begin(),i);
        if (!MkdirAll(parent) ) {
            return false;
        };
	}

	return Mkdir(path);
}




