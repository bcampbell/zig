
#include <algorithm>
#include <ctype.h>
#include <string>
#include <fstream>

#include "zigconfig.h"
#include "util.h"
#include "wobbly.h"
#include "zig.h"



// apply a single option to the config struct
static void ApplyOption( ZigConfig& cfg, std::vector<std::string> const& opts )
{
	std::string const& o = opts[0];
#if defined( __APPLE__ ) && defined( __MACH__ )
    // apple - ignore process serial number passed in by finder
    if (o.find("psn_")==0)
        return;
#endif

	if( o == "fullscreen" )
		cfg.fullscreen = true;
	else if( o == "window" )
		cfg.fullscreen = false;
	else if( o == "nosound" )
		cfg.nosound = true;
	else if( o == "sounddebug" )
		cfg.sounddebug = true;
	else if( o == "soundexplore" )
		cfg.soundexplore = true;
	else if( o == "nomultitexture" )
		cfg.nomultitexture = true;
	else if( o == "flatout" )
		cfg.flatout = true;
	else if( o == "w" )
		cfg.width = atoi( opts[1].c_str() );
	else if( o == "h" )
		cfg.height = atoi( opts[1].c_str() );
	else if( o == "depth" )
		cfg.depth = atoi( opts[1].c_str() );
	else if( o == "soundfreq" )
		cfg.soundfreq = atoi( opts[1].c_str() );
	else if( o == "soundbuffer" )
		cfg.soundbuffer = atoi( opts[1].c_str() );
	else
	{
		Wobbly e("Unknown option '%s'", o.c_str() );
		throw e;
	}
}


void ZigConfig::Read( std::string const& filename )
{
    std::ifstream in( filename.c_str() );
    int linenum = 0;
    while( in.good() )
    {
        std::string line;
        std::getline( in, line );
        ++linenum;

        std::vector< std::string > opts;
        SplitLine( line, opts );

        if( opts.empty() )		// ignore blank lines
            continue;

        ApplyOption( *this, opts );
    }
}

// apply argv options.
// ac and av are commandline options passed along from main(),
// but with argv[0] stripped off.
void ZigConfig::ApplyArgs( int ac, char* av[] )
{
	int i=0;
	while( i<ac )
	{
		std::vector< std::string > opt;
		if( av[i][0] != '-' )
			throw Wobbly( "Syntax error: '%s'", av[i] );
		opt.push_back( &av[i][1] );
		++i;
		// collect args for this option
		while( i<ac && av[i][0] != '-' )
		{
			opt.push_back( av[i] );
			++i;
		}
		ApplyOption( *this, opt );
	}
}


void ZigConfig::Write(std::string const& filename) const
{
	std::ofstream out( filename.c_str() );

	if( !out.good() )
		return;

	if( fullscreen )
		out << "fullscreen" << std::endl;
	else
		out << "window" << std::endl;

#if 0
	out << "width " << width << std::endl;
	out << "height " << height << std::endl;
	out << "depth " << depth << std::endl;
#endif
}


