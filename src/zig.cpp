#include "zig.h"

#include "agent.h"
#include "agentmanager.h"
#include "bullets.h"
#include "controller.h"
#include "display.h"
#include "drawing.h"
#include "dudes.h"
#include "gamestate.h"
#include "highscores.h"
#include "image.h"
#include "leveldef.h"
#include "mathutil.h"
#include "player.h"
#include "proceduraltextures.h"
#include "resources.h"
#include "soundmgr.h"
#include "texture.h"
#include "titlescreen.h"
#include "dudegallery.h"
#include "util.h"
#include "wobbly.h"
#include "log.h"
#include "paths.h"
#include <SDL.h>


#include "soundexplore.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

#ifdef ZIG_INSTALL_DIR
#include <unistd.h>
#endif


PathResolver* g_ConfigPath = 0;      // for config files
PathResolver* g_DataPath = 0;        // for generated data (highscores)
//PathResolver* g_ResourcePath = 0;    // for read-only data (textures etc)

ZigConfig g_Config;
std::vector<LevelDef> g_LevelDefs;

Player* g_Player=0;
float g_Time=0; // in seconds
Level* g_CurrentLevel=0;
Texture* g_Font=0;

Texture* g_Textures[TX_NUMTEXTURES] = {0};

AgentManager* g_Agents = 0;
Display* g_Display=0;
ControllerMgr* g_ControllerMgr = 0;
HighScores* g_HighScores = 0;

GameState* g_GameState = 0;


static void InitTextures();
static void FreeTextures();
static void startup( int argc, char*argv[] );
static void shutdown();
static void mainloop();


void startup( int argc, char*argv[] )
{
    g_ConfigPath = BuildConfigResolver("zig");
    if (!g_ConfigPath) {
        Wobbly e("couldn't set up config path resolver\n" );
        throw e;
    }
    g_DataPath = BuildDataResolver("zig");
    if (!g_DataPath) {
        Wobbly e("couldn't set up data path resolver\n" );
        throw e;
    }
    

#ifdef __EMSCRIPTEN__
    log_open("-");
#else
    {
        std::string logfile = g_DataPath->ResolveForWrite("log.txt");
        if (!logfile.empty()) {
            log_open(logfile.c_str());
        }
    }
#endif
    log_infof("Started\n");

    // load options file, if available
    std::string optsFile = g_ConfigPath->ResolveForRead("options");
    if (!optsFile.empty() ) {
        g_Config.Read(optsFile);    // load() is OK even if file missing
    }
    // commandline args override file settings
    g_Config.ApplyArgs( argc-1, &argv[1] );

#ifdef __EMSCRIPTEN__
    g_Config.fullscreen = false;
    g_Config.nosound = true;
#endif

    // set up memory pooling system for agents
    Agent_Startup();

    //log_open("-");
    Resources::Init();

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK|SDL_INIT_GAMECONTROLLER) != 0)
    {
        Wobbly e("SDL_Init() failed: %s\n", SDL_GetError() );
        throw e;
    }

#ifdef ZIG_INSTALL_DIR
    if( chdir( ZIG_INSTALL_DIR ) < 0 )
    {
        throw Wobbly( "Couldn't change to " ZIG_INSTALL_DIR );
    }
#endif
    g_ControllerMgr = new ControllerMgr();
    g_HighScores = new HighScores();
    g_Display = new Display( g_Config.fullscreen );

    InitTextures();

    //----------------------------------------------
    // Sound Init
#if !defined(DISABLE_SOUND)
    if( !g_Config.nosound )
    {
        try
        {
            RealSoundMgr::Create();
        }
        catch( Wobbly& e )
        {
            log_errorf("Error starting sound: %s - running silent\n",e.what() );
        }
    }
#endif
    if( !SoundMgr::Running() )
        NullSoundMgr::Create();

    //----------------------------------------------

    srand( SDL_GetTicks() );

    //----------------------------------------------
    {
        std::string levelfile = Resources::Map( "levels" );
        LevelParser parser( levelfile, g_LevelDefs );
    }

    g_GameState = new GameState();

}




void shutdown()
{
    delete g_GameState;
    delete g_HighScores;
    delete g_ControllerMgr;

	SoundMgr::Destroy();

	// clean up global textures
	if( g_Display )
        FreeTextures();

	delete g_Display;

	SDL_Quit();
	Agent_Shutdown();
    log_close();
    delete g_DataPath;
    delete g_ConfigPath;
}



// create/load global textures
void InitTextures()
{
    g_Font = new FileTexture( Resources::Map( "font.png" ).c_str() );
    g_Textures[TX_FONT] = g_Font;
    g_Textures[TX_INVADER] = new FileTexture( Resources::Map( "invaders.png").c_str() );
    g_Textures[TX_BLUEGLOW] = new BlueGlow( 64,64 );
    g_Textures[TX_NARROWBEAMGRADIENT] = new NarrowBeamGradient( 64,64 );
    g_Textures[TX_WIDEBEAMGRADIENT] = new WideBeamGradient( 64,64 );
    int i;
    for(i=0; i<TX_NUMTEXTURES; ++i)
    {
        assert(g_Textures[i]);
        g_Display->AddTexture(g_Textures[i]);
    }
}


// clean up global textures
void FreeTextures()
{
    int i;
    for(i=0; i<TX_NUMTEXTURES; ++i)
    {
        if(!g_Textures[i])
            continue;
        g_Display->RemoveTexture(g_Textures[i]);
        delete g_Textures[i];
        g_Textures[i] = 0;
    }
    g_Font = 0;
}




#ifdef __EMSCRIPTEN__

static Scene* s_Scene = 0;

static void execframe()
{
    s_Scene = s_Scene->ExecFrame();
    if( s_Scene == 0  ) {
        emscripten_cancel_main_loop();
        log_errorf("byebye!\n" );
        shutdown();
    }
}


int main( int argc, char*argv[] )
{
    int retCode = 0;
	try
	{
        startup(argc, argv);
        s_Scene = new TitleScreen();
        emscripten_set_main_loop(execframe,0,1); // never returns
		log_errorf("shouldn't see this...\n" );
	}
	catch( Wobbly& e )
	{
		// uhoh...
		log_errorf("ERROR: %s\n", e.what() );
        retCode = 1;
	}
    shutdown();
	return retCode;
}


#else
int main( int argc, char*argv[] )
{

    int retCode = 0;
	try
	{
        startup(argc, argv);
        mainloop();
	}
	catch( Wobbly& e )
	{
		// uhoh...
		log_errorf("ERROR: %s\n", e.what() );
        retCode = 1;
	}
    shutdown();
	return retCode;
}

void mainloop()
{
	Uint32 prevtime;

    // Minimal statemachine
    Scene* scene;
    if( g_Config.soundexplore) {
        scene = new SoundExplore();
    } else {
        //scene = new DudeGallery();
        scene = new TitleScreen();
    }

	while( 1 )
	{
		prevtime = SDL_GetTicks();

        scene = scene->ExecFrame();
        if( scene== 0  ) {
            break;
        }

        g_Time += (1.0f / TARGET_FPS);

		Uint32 currtime = SDL_GetTicks();	// currytime? mmmmmmmm
		Uint32 elapsed = currtime-prevtime;

		if( !g_Config.flatout )
		{
			if( elapsed < TICK_INTERVAL )
				SDL_Delay( TICK_INTERVAL-elapsed );
		}
	}
}
#endif

