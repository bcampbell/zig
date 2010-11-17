#include "resources.h"
#include "util.h"	// for JoinPath()

//static
std::string Resources::s_ResourcePath;


std::string Resources::Map( std::string const& name )
{
	return JoinPath( s_ResourcePath, name );
}



#if defined( __APPLE__ ) && defined( __MACH__ )

// OSX

//#include <CFBundle.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Carbon/Carbon.h>


static bool AmIBundled()
{
	ProcessSerialNumber psn = { 0, kCurrentProcess };
	FSRef processref;
	GetProcessBundleLocation( &psn, &processref );
	FSCatalogInfo processinfo;
	FSGetCatalogInfo( &processref, kFSCatInfoNodeFlags, &processinfo,
		NULL, NULL, NULL );
	if( processinfo.nodeFlags & kFSNodeIsDirectoryMask )
		return true;
	else
		return false;
}



void Resources::Init()
{
	if( !AmIBundled() )
	{
		// not running from a bundle (development)
		s_ResourcePath = "data";
		return;
	}

	// Locate the resource subdir within our bundle.
	// A bit shite - we should really use CFBundleCopyResourceURL() instead and
	// get proper transparent per-resource localisation and that kind of stuff,
	// but hey.

	CFBundleRef mainbundle = CFBundleGetMainBundle();
	assert( mainbundle );

	char resdir[1024];
	CFURLRef resdirurl = CFBundleCopyResourcesDirectoryURL( mainbundle );
	assert( resdirurl );

	CFURLGetFileSystemRepresentation(
		resdirurl,
		TRUE,
		(UInt8*)resdir,
		sizeof(resdir) );

	CFRelease( resdirurl );

	s_ResourcePath = resdir;

//	printf("ResourcePath: '%s'\n", s_ResourcePath.c_str() );
}


#else

// Windows, Linux

void Resources::Init()
{
	s_ResourcePath = "data";
}

#endif

