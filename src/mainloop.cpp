#include "zig.h"
#include "mainloop.h"
#include "display.h"
#include "controller.h"
#include "player.h"
#include "level.h"
#include "leveldef.h"
#include "zigconfig.h"
#include "scene.h"
#include "titlescreen.h"

#include <SDL.h>
#include <SDL_keyboard.h>
#include <SDL_events.h>

class State;

static State* s_State = 0;
static Scene* s_CurrentScene = 0;

class State
{
public:
    State() {}
    virtual ~State() {}
    // returns new state, or null to exit
    virtual State* Update() = 0;
};

class State_GAME : public State
{
public:
    State_GAME();
    virtual ~State_GAME();
    virtual State* Update();

    void InitLevel();
    Scene* m_Scene;
	Player* m_Player;
	int m_WrapCount;	// how many times all levels completed
	int m_LevelIndex;
};


class State_EXIT : public State
{
public:
    State_EXIT() {}
    virtual ~State_EXIT() { printf("byebye!\n"); }
    virtual State* Update() {
        delete this;
        return 0;
    }
};


class State_TITLE : public State
{
public:
    State_TITLE() : m_Scene(new TitleScreen())
    {
        s_CurrentScene = m_Scene;
    }

    virtual ~State_TITLE()
    {
        delete m_Scene;
        s_CurrentScene = 0;
    }

    virtual State* Update()
    {
        switch( m_Scene->Result() )
        {
        case DONE:
            delete this;
            return new State_EXIT();
        case STARTGAME:
            delete this;
            return new State_GAME();
        case CANCEL:
            delete this;
            return new State_EXIT();
        case NONE:
        default:
            return this;
        }
    }

    Scene* m_Scene;
};




State_GAME::State_GAME() : m_Scene(0),
    m_Player(0),
    m_WrapCount(0),
    m_LevelIndex(0)
{
    assert( g_GameState == 0 );
    g_GameState = new GameState();
    m_Player = new Player();
    InitLevel();
}

State_GAME::~State_GAME()
{
    delete m_Scene;
    s_CurrentScene = 0;
    delete g_GameState;
    g_GameState = 0;

    delete m_Player;
}


void State_GAME::InitLevel()
{
    switch( m_WrapCount )
    {
    case 0:
        g_GameState->BigHeadMode = false;
        g_GameState->NoExtraLives = false;
        break;
    case 1:
        g_GameState->BigHeadMode = true;
        g_GameState->NoExtraLives = false;
        break;
    case 2:
        g_GameState->BigHeadMode = false;
        g_GameState->NoExtraLives = true;
        break;
    }

    LevelDef* def;
    if( m_LevelIndex < (int)g_LevelDefs.size() )
        def = &g_LevelDefs[m_LevelIndex];
    else
        def = &g_LevelDefs.back();

    const int perceivedlevel =
        g_LevelDefs.size()*m_WrapCount + m_LevelIndex + 1;

    m_Scene = new Level( *def, perceivedlevel );
    s_CurrentScene = m_Scene;
}


State* State_GAME::Update()
{
    switch( m_Scene->Result() )
    {
    case DONE:
        ++m_LevelIndex;
        if( m_LevelIndex >= (int)g_LevelDefs.size() )
        {
            ++m_WrapCount;
            m_LevelIndex = 0;
            // TODO: how to show completion screen...
            // CompletionScreen c( m_WrapCount );
            // c.Run();
        }

        // ugh...
        delete m_Scene;
        m_Scene = 0;
        s_CurrentScene = 0;

        InitLevel();
        return this;
    case GAMEOVER:
        // TODO: go to gameover state + highscore...
        delete this;
        return new State_TITLE();
    case CANCEL:
        delete this;
        return new State_EXIT();
    case NONE:
    default:
        return this;
    }
}





static void execframe();

void mainloop()
{
	Uint32 prevtime;

    s_State = new State_TITLE();
	while( 1 )
	{
		prevtime = SDL_GetTicks();


		execframe();
        if( s_State== 0  ) {
            break;
        }

		Uint32 currtime = SDL_GetTicks();	// currytime? mmmmmmmm
		Uint32 elapsed = currtime-prevtime;

		if( !g_Config.flatout )
		{
			if( elapsed < TICK_INTERVAL )
				SDL_Delay( TICK_INTERVAL-elapsed );
		}
	}
}





// perform a single iteration of the mainloop
void execframe()
{
    s_State = s_State->Update();

    if( s_State== 0  ) {
        return;
    }

    Scene* scene = s_CurrentScene;

    g_ControllerMgr->Tick();
    
    if (scene)
    {
        scene->Tick();
        scene->Render();
    }

    g_Display->Present();

    SDL_PumpEvents();
    SDL_Event event;
    while( SDL_PollEvent( &event ) == 1 )
    {
        switch( event.type )
        {
            case SDL_QUIT:
                // TODO: kill.
                throw Scene::QuitNotification();
                break;
            case SDL_TEXTINPUT:
                if (scene)
                { 
                    scene->HandleTextInput(event.text);
                }
                break;
            case SDL_KEYDOWN:
                switch( event.key.keysym.sym )
                {
                    default:
                        if (scene)
                        {
                            scene->HandleKeyDown( event.key.keysym );
                        }
                        break;
                }
                break;
            case SDL_CONTROLLERDEVICEADDED:
                //printf("controller added\n");
                g_ControllerMgr->HandleControllerAdded(&event.cdevice);
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                //printf("controller Removed\n");
                g_ControllerMgr->HandleControllerRemoved(&event.cdevice);
                break;
            case SDL_JOYDEVICEADDED:
                //printf("joystick added\n");
                break;
            case SDL_JOYDEVICEREMOVED:
                //printf("joystick Removed\n");
                break;
            case SDL_WINDOWEVENT:
                {
                    SDL_WindowEvent& wev = event.window;
                    if ( wev.event==SDL_WINDOWEVENT_RESIZED)
                    {
                        int w=wev.data1;
                        int h=wev.data2;
                        g_Display->HandleResize(w,h);
                    }
                    else if( wev.event == SDL_WINDOWEVENT_FOCUS_LOST)
                    {
                        if (scene)
                        {
                            scene->HandleFocusLost();
                        }
                    }
                }
                break;
        }
    }
}





