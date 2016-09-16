#include "zig.h"
#include "mainloop.h"
#include "display.h"
#include "controller.h"
#include "zigconfig.h"
#include "scene.h"
#include "titlescreen.h"

#include <SDL.h>
#include <SDL_keyboard.h>
#include <SDL_events.h>

static enum {
   INIT,
   TITLESCREEN,
   HIGHSCORE,
   DEMO,
   LEVEL,
   GAMEOVER,
   QUIT
} s_State = INIT;

static Scene* s_CurrentScene = 0;

static void execframe();

void mainloop()
{
	Uint32 prevtime;

	while( 1 )
	{
		prevtime = SDL_GetTicks();


		execframe();
        if( s_State==QUIT ) {
            return;
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



void NextState()
{
    if (s_CurrentScene)
    {
        delete s_CurrentScene;
        s_CurrentScene = 0;
    }

    if (s_State==INIT)
    {
        // initial startup
        s_CurrentScene = new TitleScreen();
        s_State = TITLESCREEN;
    }
    else
    {
        s_State = QUIT;
    }

}




// perform a single iteration of the mainloop
void execframe()
{

    if (!s_CurrentScene || s_CurrentScene->IsFinished() )
    {
        NextState();
    }

    Scene* scene = s_CurrentScene;
    if (!scene)
    {
        return;
    }

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
                // TODO: kill.
                throw Scene::QuitNotification();
                break;
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






