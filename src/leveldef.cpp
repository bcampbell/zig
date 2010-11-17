

#ifdef _MSC_VER
    //identifier was truncated to '255' characters in the browser information
    #pragma warning (disable : 4786)
#endif

#include "leveldef.h"
#include "wobbly.h"

#include <assert.h>
#include <algorithm>
#include <cstdio>

using std::string;
using std::ifstream;
using std::transform;
using std::vector;


LevelParser::LevelParser( std::string const& filename,
	std::vector<LevelDef>& defs ) :
	m_Filename( filename ),
	m_LineNum(0),
	m_Stream( filename.c_str() ),
	m_Defs( defs )
{
	assert( m_Defs.empty() );

	if( !m_Stream.good() )
	{
		BailOut( "Couldn't open file" );
	}
	
	const int linemax=128;
	char linebuf[linemax];
	while( m_Stream.good() )
	{
		m_Stream.getline( linebuf, linemax );
		++m_LineNum;
		std::string line( linebuf );
//		transform( line.begin(), line.end(), line.begin(), tolower );

		vector<string> args;
		SplitLine( line, args );

		Process( args );
	}
}




void LevelParser::SplitLine( std::string const& line,
	std::vector<std::string>& args )
{
	string::const_iterator it=line.begin();

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
		while( it!=line.end() && !isspace(*it) )
			args.back() += *it++;
	}
}



void LevelParser::Process( std::vector<std::string> const& args )
{
//	int i;
//	printf( "%d: ", m_LineNum );
//	for( i=0; i<(int)args.size(); ++i )
//		printf("'%s' ", args[i].c_str() );
//	printf("\n");
		
	if( args.empty() )
		return;			// line is blank

	if( args[0] == "LEVEL" )
	{
		m_Defs.push_back( LevelDef() );
	}
	else if( args[0][0] == '@' )
	{
		// treat it as a level setting
		if( m_Defs.empty() )
			BailOut( "Missing LEVEL keyword" );

		if( args.size() < 2 )
		{
			if( args[0] == "@retro" )
				m_Defs.back().m_Retro = true;
			else
				BailOut( "Syntax error" );
		}
		else
		{
			float val = atof( args[1].c_str() );

			if( args[0] == "@ufo_probability" )
				m_Defs.back().m_UFOProbability = val;
			else if( args[0] == "@arena_radius" )
				m_Defs.back().m_ArenaRadius = val;
			else if( args[0] == "@baiter_start" )
				m_Defs.back().m_BaiterStart = val;
			else if( args[0] == "@baiter_interval" )
				m_Defs.back().m_BaiterInterval = val;
			else
				BailOut( "Unrecognised setting" );
		}
	}
	else
	{
		// treat as dude type
		if( m_Defs.empty() )
			BailOut( "Missing LEVEL keyword" );

		if( args.size() < 2 )
			BailOut( "Syntax error" );
		
		int quant = atoi( args[1].c_str() );

//		printf("level %d: %d x %s\n",m_Defs.size()-1, quant, args[0].c_str() );

		m_Defs.back().AddSpawnEntry( args[0], quant );
	}
}


void LevelParser::BailOut( const char* reason )
{
	throw Wobbly( "%s (line %d): %s",
		m_Filename.c_str(), m_LineNum, reason );
}



