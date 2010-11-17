#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>


// assorted utility functions that don't fit anywhere else

void SplitLine( std::string const& line, std::vector<std::string>& args );
std::string BaseName( std::string const& fullpath );
std::string DirName( std::string const& fullpath );

std::string JoinPath( std::string const& a, std::string const& b );

std::string PerUserDir();
bool MakeDir( std::string const& dir );

// number of elements in array
#define arraylen(x) (sizeof(x)/sizeof((x)[0]))

#endif // UTIL_H

