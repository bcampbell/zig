#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>


// assorted utility functions that don't fit anywhere else
std::vector<std::string> Split( std::string const& s, char delim);

void SplitLine( std::string const& line, std::vector<std::string>& args );
std::string BaseName( std::string const& fullpath );
std::string DirName( std::string const& fullpath );

std::string JoinPath( std::string const& a, std::string const& b );

bool Mkdir( std::string const& path );
bool MkdirAll( std::string const& path );



bool FileExists(std::string const& filename);


// number of elements in array
#define arraylen(x) (sizeof(x)/sizeof((x)[0]))



struct FileInfo
{

    bool IsDir() const { return (attrs & ATTR_DIR)?true:false; }

    enum {
        ATTR_DIR=1<<0
    };
    int attrs;
    // lots of other stuff to go in here...
};


bool Stat( std::string const& filename, FileInfo& fi);

#endif // UTIL_H

