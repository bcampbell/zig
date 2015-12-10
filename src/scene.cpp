

#include <SDL_opengl.h>
#include <time.h>

//#ifdef CRIPPLED
#include "crippleclock.h"
//#endif // CRIPPLED
#include "scene.h"
#include "zig.h"
#include "display.h"
#include "controller.h"

#include "soundmgr.h"
#include "drawing.h"


void Scene::Run()
{
	Uint32 prevtime;

	while( !IsFinished() )
	{
		prevtime = SDL_GetTicks();
        g_ControllerMgr->Tick();
		Tick();

		Render();
/*
	glPushMatrix();
	glLoadIdentity();
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    vec2 tl = g_Display->TopLeft();
    glTranslatef(tl.x, tl.y-16,0.0f);
		PlonkText( *g_Font, SoundMgr::Inst().DebugString(), false, 8, 8 );
	glPopMatrix();
*/

#ifdef CRIPPLED
		CrippleClock::Render();
#endif //CRIPPLED


        g_Display->Present();
    
		SDL_PumpEvents();
		SDL_Event event;
		while( SDL_PollEvent( &event ) == 1 )
		{
			switch( event.type )
			{
				case SDL_QUIT:
                    // TODO: kill.
					throw QuitNotification();
					break;
				case SDL_TEXTINPUT:
                    HandleTextInput(event.text);
                    break;
				case SDL_KEYDOWN:
					switch( event.key.keysym.sym )
					{
						default:
							HandleKeyDown( event.key.keysym );
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
                            HandleFocusLost();
                        }
                    }
                    break;
			}
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



