#include "zig.h"
#include "mainloop.h"
#include "display.h"
#include "controller.h"
#include "player.h"
#include "level.h"
#include "leveldef.h"
#include "highscores.h"
#include "zigconfig.h"
#include "scene.h"
#include "gameover.h"
#include "titlescreen.h"
#include "highscorescreen.h"

#include <SDL.h>
#include <SDL_keyboard.h>
#include <SDL_events.h>

// Minimal statemachine
static Scene* s_Scene = 0;



static void execframe();

void mainloop()
{
	Uint32 prevtime;

    s_Scene = new TitleScreen();
	while( 1 )
	{
		prevtime = SDL_GetTicks();

		execframe();
        if( s_Scene== 0  ) {
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
    s_Scene = s_Scene->NextScene();

    if( s_Scene== 0  ) {
        // TODO: cleanup here
        return;
    }

    Scene* scene = s_Scene;

    g_ControllerMgr->Tick();
    
    scene->Tick();
    scene->Render();

    g_Display->Present();

    SDL_PumpEvents();
    SDL_Event event;
    while( SDL_PollEvent( &event ) == 1 )
    {
        switch( event.type )
        {
            case SDL_QUIT:
                {
                    // Hard exit
                    delete s_Scene;
                    s_Scene = 0;
                    // TODO: cleanup here, as above
                    return;
                }
            case SDL_TEXTINPUT:
                scene->HandleTextInput(event.text);
                break;
            case SDL_KEYDOWN:
                switch( event.key.keysym.sym )
                {
                    default:
                        scene->HandleKeyDown( event.key.keysym );
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
                        scene->HandleFocusLost();
                    }
                }
                break;
        }
    }
}




