
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
	if( o == "fullscreen" )
		cfg.fullscreen = true;
	else if( o == "window" )
		cfg.fullscreen = false;
	else if( o == "nosound" )
		cfg.nosound = true;
	else if( o == "sounddebug" )
		cfg.sounddebug = true;
	else if( o == "nomultitexture" )
		cfg.nomultitexture = true;
	else if( o == "flatout" )
		cfg.flatout = true;
	else if( o == "width" )
		cfg.width = atoi( opts[1].c_str() );
	else if( o == "height" )
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


// ac and av are commandline options passed along from main()
// Commandline options override config file options.
void ZigConfig::Init( int ac, char* av[] )
{
	// try to read in opts from file
	{
		std::string inname = JoinPath( ZigUserDir(), "options" );
		std::ifstream in( inname.c_str() );
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

	// now apply commandline opts on top.
	int i=1;
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


void ZigConfig::Save() const
{
	std::string outname = JoinPath( ZigUserDir(), "options" );

	std::ofstream out( outname.c_str() );

	if( !out.good() )
		return;

	if( fullscreen )
		out << "fullscreen" << std::endl;
	else
		out << "window" << std::endl;

	out << "width " << width << std::endl;
	out << "height " << height << std::endl;
	out << "depth " << depth << std::endl;
}


