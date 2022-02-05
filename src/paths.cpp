#include "paths.h"
#include "util.h"

#include <cassert>

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <shlobj.h>
#else
/*
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <errno.h>
    */
#endif //!_WIN32

#if defined( __APPLE__ ) && defined( __MACH__ )
    #include <sys/syslimits.h>  // for PATH_MAX
    #include "osx_glue.h"
#endif


GenericPathResolver::GenericPathResolver( std::vector<std::string> const& dirs ) :
        m_WriteDirChecked(false),
        m_Dirs(dirs)
{
    assert(!m_Dirs.empty());
}

std::string GenericPathResolver::ResolveForRead(std::string const& filename)
{
    for(const std::string& dir : m_Dirs)
    {
        std::string f = JoinPath(dir,filename);
        if (FileExists(f)) {
            //printf("FOUND %s\n",f.c_str());
            return f;
        }
        //printf("NOPE %s\n",f.c_str());
    }
    //printf("%s NOT FOUND\n",filename.c_str());
    return "";
}

std::string GenericPathResolver::ResolveForWrite(std::string const& filename)
{
    std::string dir = m_Dirs[0];
    if (!m_WriteDirChecked) {
        if(MkdirAll(dir)) {
            m_WriteDirChecked = true;
        } else {
            return "";  //uhoh.
        }
    }
    return JoinPath(dir,filename);
}



//-----------------------------


#ifdef _WIN32

// Windows versions

// get the APPDATA dir (eg C:/users/<username>/AppData/Local)
static std::string getAPPDATA()
{
	char buf[MAX_PATH];
	HRESULT result = SHGetFolderPathA( 0, CSIDL_APPDATA, 0, 0, buf );
	// |CSIDL_FLAG_CREATE
	if(!SUCCEEDED(result) )
    {
        buf[0] = '\0';
    }
    return std::string(buf);
}

PathResolver* BuildConfigResolver( std::string const& appname)
{
    // per-user APPDATA dir for both reading and writing.
    std::string appdata = getAPPDATA();
    if(appdata.empty()) {
        return nullptr;
    }
    std::vector<std::string> dirs;
    dirs.push_back(JoinPath(appdata, appname));
    return new GenericPathResolver(dirs);
}

PathResolver* BuildDataResolver( std::string const& appname)
{
    // order is:
    // 1) per-user APPDATA dir (also for writing).
    // 2) ./data (readonly).
    std::vector<std::string> dirs;
    std::string appdata = getAPPDATA();
    if(!appdata.empty()) {
        dirs.push_back(JoinPath(appdata, appname));
    }
    dirs.push_back("data");
    return new GenericPathResolver(dirs);
}



#elif defined( __APPLE__ ) && defined( __MACH__ )

// OSX version

PathResolver* BuildConfigResolver( std::string const& appname)
{
    // Use the Application Support dir (this should handle sandboxed
    // and non-sandboxed)
    std::vector<std::string> dirs;
    char buf[PATH_MAX];
    if(!osx_get_app_support_path(buf,PATH_MAX))
        return nullptr;
    dirs.push_back(JoinPath(buf,appname));
    return new GenericPathResolver(dirs);
}

// config and data files in same location
PathResolver* BuildDataResolver( std::string const& appname)
{
    // ordering:
    // 1) local per-user Application Support dir (read/write)
    // 2) data dir inside bundle (resource path)
    std::vector<std::string> dirs;

    char buf[PATH_MAX];
    if(!osx_get_app_support_path(buf,PATH_MAX))
        return nullptr;
    dirs.push_back(JoinPath(buf, appname));

    if(osx_get_resource_path(buf,PATH_MAX) ) {
        dirs.push_back(buf);
    } else {
        // We're not running in a bundle maybe? Try current dir.
        dirs.push_back(".");
    }
    return new GenericPathResolver(dirs);
}

#else

#include "config.h" // for ZIG_DATA_DIR

// unix version
// Use the XDG standards:
// https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html

static std::string xdg_conf_dir()
{
    const char *xdg_conf = std::getenv("XDG_CONFIG_HOME");
    if (xdg_conf && xdg_conf[0]!='\0') {
        return xdg_conf;
    }
    // missing or empty - fall back to default
    const char *home = std::getenv("HOME");
    if (!home) {
        return "";  // uhoh.
    }
    return std::string(home) + "/.config";
}

static std::vector<std::string> xdg_conf_dirs()
{
    const char* configDirs = std::getenv("XDG_CONFIG_DIRS");
    if (!configDirs || configDirs[0]=='\0') {
        configDirs = "/etc/xdg";
    }

    std::vector<std::string> dirs = Split(configDirs,':');
    return dirs;
}


static std::string xdg_data_dir()
{
    const char *xdg_data = std::getenv("XDG_DATA_HOME");
    if (xdg_data && xdg_data[0]!='\0') {
        return xdg_data;
    }
    // missing or empty - fall back to default
    const char *home = std::getenv("HOME");
    if (!home) {
        return "";  // uhoh.
    }
    return std::string(home) + "/.local/share";
}

static std::vector<std::string> xdg_data_dirs()
{
    const char* v = std::getenv("XDG_DATA_DIRS");
    if (!v || v[0]=='\0') {
        v = "/usr/local/share/:/usr/share/";
    }

    std::vector<std::string> dirs = Split(v,':');
    return dirs;
}


PathResolver* BuildConfigResolver( std::string const& appname)
{
    // default ordering (if no XDG env vars set):
    // 1) $HOME/.config/<appname> (read/write)
    // 2) /etc/xdg/<appname>
    std::vector<std::string> dirs;

    std::string primary = xdg_conf_dir();
    if (primary.empty()) {
        return 0; // uhoh
    }
    dirs.push_back(JoinPath(primary, appname));

    std::vector<std::string> extraDirs = xdg_conf_dirs();
    for (auto const& d : extraDirs) {
        dirs.push_back(JoinPath(d,appname));
    }

    return new GenericPathResolver(dirs);
}

PathResolver* BuildDataResolver( std::string const& appname)
{
    // default ordering (if no XDG env vars set):
    // 1) $HOME/.local/share/<appname> (read/write)
    // 2) /usr/local/share/<appname>
    // 3) /usr/share/<appname>
    std::vector<std::string> dirs;

    std::string primary = xdg_data_dir();
    if (primary.empty()) {
        return 0; // uhoh
    }
    primary = JoinPath(primary,appname);
    dirs.push_back(primary);

    std::string hardwired = JoinPath(ZIG_DATA_DIR, appname);
    dirs.push_back(hardwired);

    std::vector<std::string> extraDirs = xdg_data_dirs();
    for (auto const& d : extraDirs) {
        dirs.push_back(JoinPath(d,appname));
    }

    return new GenericPathResolver(dirs);
}

#endif




