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

    // returns full path for first existing match found
    // returns empty string if not found 
    virtual std::string ResolveForRead(std::string const& filename);

    // will create the output dir if if doesn't exist
    virtual std::string ResolveForWrite(std::string const& filename);
private:
    bool m_WriteDirChecked;
    std::vector<std::string> m_Dirs;
};

PathResolver* BuildConfigResolver( std::string const& appname);
PathResolver* BuildDataResolver( std::string const& appname);


#endif // PATHS_H

