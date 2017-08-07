// front end for stand-alone retromat app.

#include "retromat.h"

#include <Stk.h>
#include <RtWvOut.h>
#include <FileWvOut.h>
#include <vector>
#include <cstdio>


struct SoundEntry
{
	const char* name;
	RetromatFn fn;
};

SoundEntry Sounds[] = 
{
	{ "drone",			GenerateDrone },
	{ "laser",			GenerateLaser },
	{ "baiteralert",	GenerateBaiterAlert },
	{ "playertoast",	GeneratePlayerToast },
	{ "levelintro",		GenerateLevelIntro },
	{ "bigexplosion",	GenerateBigExplosion },
	{ "gameover",		GenerateGameOver },
	{ "dullblast",		GenerateDullBlast },
	{ "wibblepop",		GenerateWibblePop },
	{ 0, 0 }
};



SoundEntry* FindSound( std::string const& name )
{
	SoundEntry* p=Sounds;
	while( p->name )
	{
		if( p->name == name )
			return p;
		++p;
	}
	return 0;
}


int main( int argc, char* argv[] )
{
	bool writewav = false;
	bool list = false;
	std::vector< std::string > soundlist;

	SetRetromatFreq( 44100 );

	int i;
	for( i=1; i<argc; ++i )
	{
		if( 0 == strcmp( argv[i], "-writewav" ) )
			writewav = true;
		else if( 0 == strcmp( argv[i], "-list" ) )
			list = true;
		else
			soundlist.push_back( argv[i] );
	}

	if( list )
	{
		SoundEntry* p=Sounds;
		while( p->name )
		{
			printf( "%s\n", p->name );
			++p;
		}
		return 0;
	}


	if( soundlist.empty() )
	{
		SoundEntry* p=Sounds;
		while( p->name )
		{
			soundlist.push_back( p->name );
			++p;
		}
	}


	for( i=0; i<soundlist.size(); ++i )
	{

		SoundEntry* snd = FindSound( soundlist[i] );
		if( !snd )
		{
			printf("ERROR: couldn't find sound %s!\n", soundlist[i].c_str() );
			return -1;
		}


		std::vector< float > buf;
		buf.reserve( 10 * (int)stk::Stk::sampleRate() );
		(*snd->fn)( buf );
		if( writewav )
		{
            std::string filename = soundlist[i] + ".wav";
			printf("writing %s...", filename.c_str() );
            stk::FileWvOut out( filename );
			std::vector<float>::const_iterator it;
			for( it=buf.begin(); it!=buf.end(); ++it )
				out.tick( *it );
			printf("ok\n");
		}
		else
		{
			printf("playing %s...", soundlist[i].c_str() );
			fflush( stdout );
            stk::RtWvOut out(1);
			std::vector<float>::const_iterator it;
			for( it=buf.begin(); it!=buf.end(); ++it )
				out.tick( *it );
			printf("ok\n");
		}
	}
	
	return 0;	
}



