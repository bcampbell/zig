#ifndef PATHS_H
#define PATHS_H

#include <string>
#include <vector>

class PathResolver
{
public:
    virtual ~PathResolver() {}
    virtual std::string ResolveForRead(std::string const& filename) = 0;
    virtual std::string ResolveForWrite(std::string const& filename) = 0;
protected:
    PathResolver() {}
};



class GenericPathResolver : public PathResolver
{
public:
    // dirs checked in order
    // 1st dir is used for writing.
    GenericPathResolver(std::vector<std::string> const& dirs );

    // Find filename in the list of directories to search.
    // Returns full path of first match, or empty string if file not found.
    virtual std::string ResolveForRead(std::string const& filename);

    // Return full name of filename, for writing.
    // (The first directory in the list is always the writable one).
    // Will create the output dir if it doesn't exist.
    virtual std::string ResolveForWrite(std::string const& filename);
private:
    bool m_WriteDirChecked;
    std::vector<std::string> m_Dirs;
};

// Resolver for reading/writing config files.
PathResolver* BuildConfigResolver( std::string const& appname);

// Resolver for reading/writing data files.
// Allows global data files to be overridden with local (per-user)
// versions, but that doesn't seem like a bad thing.
PathResolver* BuildDataResolver( std::string const& appname);

#endif // PATHS_H

