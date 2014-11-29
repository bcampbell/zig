
#include "zig.h"



#include "agent.h"
#include "agentmanager.h"
#include "bullets.h"
#include "completionscreen.h"
#include "controller.h"
#include "display.h"
#include "drawing.h"
#include "dudes.h"
#include "gameover.h"
#include "highscores.h"
#include "highscorescreen.h"
#include "image.h"
#include "level.h"
#include "leveldef.h"
#include "mathutil.h"
#include "optionsscreen.h"
#include "player.h"
#include "proceduraltextures.h"
#include "resources.h"
#include "soundmgr.h"
#include "texture.h"
#include "titlescreen.h"
#include "util.h"
#include "wobbly.h"

#include <SDL.h>

#ifdef ZIG_INSTALL_DIR
#include <unistd.h>
#endif


#ifdef CRIPPLED
	#include "crippleclock.h"
	static bool s_LaunchWeb = false;
#endif // CRIPPLED

ZigConfig g_Config;
Player* g_Player=0;
Level* g_CurrentLevel=0;
Texture* g_Font=0;
Texture* g_InvaderTexture=0;
Texture* g_BlueGlow=0;
AgentManager* g_Agents = 0;
Display* g_Display=0;
ControllerMgr* g_ControllerMgr = 0;

bool g_KeepYourSectorTidy = false;


bool g_BigHeadMode=false;
bool g_NoExtraLives=false;

static std::vector<LevelDef> s_LevelDefs;
static std::string s_ZigUserDir;

static void PlayGame( HighScores& highscores );
static void InitZigUserDir();


int main( int argc, char*argv[] )
{
	try
	{
		// set up memory pooling system for agents
		Agent_Startup();

		InitZigUserDir();

		g_Config.Init( argc, argv );

		Resources::Init();

		if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK|SDL_INIT_GAMECONTROLLER) != 0)
		{
			Wobbly e("SDL_Init() failed");	//: %s\n", SDL_GetError() );
			throw e;
		}

//		SDL_WM_SetCaption( "Zig", 0 );
// TODO: SDL_SetWindowTitle(

#ifdef ZIG_INSTALL_DIR
		if( chdir( ZIG_INSTALL_DIR ) < 0 )
		{
			throw Wobbly( "Couldn't change to " ZIG_INSTALL_DIR );
		}
#endif
        g_ControllerMgr = new ControllerMgr();

		g_Display = new Display( g_Config.fullscreen );

		g_Font = new FileTexture( Resources::Map( "font.png" ).c_str() );
		g_Display->AddTexture( g_Font );

		g_InvaderTexture = new FileTexture( Resources::Map( "invaders.png").c_str() );
		g_Display->AddTexture( g_InvaderTexture );

		g_BlueGlow = new BlueGlow( 64,64 );
		g_Display->AddTexture( g_BlueGlow );

		//----------------------------------------------
		// Sound Init
		if( !g_Config.nosound )
		{
			try
			{
				RealSoundMgr::Create();
			}
			catch( Wobbly& e )
			{
				fprintf( stderr, "Sound error:\n%s\n",e.what() );
			}
		}

		if( !SoundMgr::Running() )
			NullSoundMgr::Create();

		//----------------------------------------------


		HighScores highscores;

		srand( SDL_GetTicks() );


		//----------------------------------------------
		{
			std::string levelfile = Resources::Map( "levels" );
			LevelParser parser( levelfile, s_LevelDefs );
		}

#ifdef CRIPPLED
		CrippleClock::Init();
#endif	// CRIPPLED

		//----------------------------------------------
		// MAIN
		//----------------------------------------------
		bool quit = false;
		while( !quit )
		{
#ifdef CRIPPLED
			if( CrippleClock::Expired() )
			{
				quit = true;
			}
#endif	// CRIPPLED
			// looping title sequence
			bool go=false;
			while( !quit && !go )
			{
				TitleScreen t;
				t.Run();

				if( t.Result() == TitleScreen::QUIT )
					quit=true;
				else if( t.Result() == TitleScreen::PLAY )
					go=true;
				else if( t.Result() == TitleScreen::TIMEOUT )
				{
					{
						// Demo mode
						Player player(true);
						int num = (int)(Rnd(0.0f,(float)(s_LevelDefs.size()-2))+0.5f);
						Level l( s_LevelDefs[num], num+1, true);
						l.Run();
					}

					HighScoreScreen scorescreen( highscores );
					scorescreen.Run();
				}
				else if( t.Result() == TitleScreen::CONFIG )
				{
					OptionsScreen o;
					o.Run();
				}
			}

			if( !quit )
				PlayGame( highscores );
		}
#ifdef CRIPPLED
		{
			NagScreen n;
			n.Run();
			s_LaunchWeb = n.LaunchWeb();
		}
#endif //CRIPPLED

	}
	catch( Wobbly& e )
	{
		// uhoh...
		fprintf(stderr,"Exception:\n%s\n", e.what() );
	}
	catch( Scene::QuitNotification& q )
	{
		// if we end up here, it means player has asked the game to close
		// (via alt-f4, window decoration, whatever).
		// Slight abuse of exceptions perhaps, but greatly simplifies the
		// main loop!
	}

	SoundMgr::Destroy();
    delete g_ControllerMgr;

	// clean up global textures
	if( g_Display )
	{
		if( g_Font )
			g_Display->RemoveTexture( g_Font );
		if( g_InvaderTexture )
			g_Display->RemoveTexture( g_InvaderTexture );
		if( g_BlueGlow )
			g_Display->RemoveTexture( g_BlueGlow );
	}
	delete g_Font;
	delete g_InvaderTexture;
	delete g_BlueGlow;

	delete g_Display;

	SDL_Quit();
	Agent_Shutdown();

#ifdef CRIPPLED
	if( s_LaunchWeb )
		CrippleClock::LaunchWebSite();
#endif	// CRIPPLED
	return 0;
}


static void PlayGame( HighScores& highscores )
{
	g_BigHeadMode = false;
	g_NoExtraLives = false;
	g_KeepYourSectorTidy = false;

	Player player;
	int wrapcount = 0;	// how many times all levels completed
	int levelindex=0;
	bool gameover=false;

	assert( s_LevelDefs.size() > 0 );

	while( !gameover )
	{
		switch( wrapcount )
		{
		case 0:
			g_BigHeadMode = false;
			g_NoExtraLives = false;
			break;
		case 1:
			g_BigHeadMode = true;
			g_NoExtraLives = false;
			break;
		case 2:
			g_BigHeadMode = false;
			g_NoExtraLives = true;
			break;
		}

		LevelDef* def;
		if( levelindex < (int)s_LevelDefs.size() )
			def = &s_LevelDefs[levelindex];
		else
			def = &s_LevelDefs.back();

		const int perceivedlevel =
			s_LevelDefs.size()*wrapcount + levelindex + 1;
		Level l( *def, perceivedlevel );
		l.Run();

#ifdef CRIPPLED
		if( CrippleClock::Expired() )
			return;
#endif	// CRIPPLED
		if( l.WasCompleted() )
		{
			++levelindex;
			if( levelindex >= (int)s_LevelDefs.size() )
			{
				++wrapcount;
				levelindex = 0;
				CompletionScreen c( wrapcount );
				c.Run();
			}
		}
		else
		{
			gameover = true;
			GameOver g( player.Score(), perceivedlevel );
			g.Run();
			if( !l.WasQuit() )
			{

				int scoreidx = highscores.Submit( player.Score() );
				HighScoreScreen scorescreen( highscores );
				if( scoreidx != -1 )
					scorescreen.EntryMode( scoreidx );

				// This makes sure that keypresses are correctly
				// mapped to characters (needed only for name entry).
//				SDL_EnableUNICODE(1);
				scorescreen.Run();
//				SDL_EnableUNICODE(0);
			}
		}
	}
}




static void InitZigUserDir()
{
#ifdef WIN32
	s_ZigUserDir = JoinPath( PerUserDir(), "Zig" );
#elif defined( __APPLE__ ) && defined( __MACH__ )
	s_ZigUserDir = JoinPath( PerUserDir(), "Zig" );
#else
	s_ZigUserDir = JoinPath( PerUserDir(), ".zig" );
#endif
	MakeDir( s_ZigUserDir );
}

std::string ZigUserDir()
{
	return s_ZigUserDir;
}

