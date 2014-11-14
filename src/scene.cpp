

#include <SDL_opengl.h>
#include <time.h>

//#ifdef CRIPPLED
#include "crippleclock.h"
//#endif // CRIPPLED
#include "scene.h"
#include "zig.h"
#include "display.h"
#include "controller.h"



void Scene::Run()
{
	Uint32 prevtime;

	while( !IsFinished() )
	{
		prevtime = SDL_GetTicks();

		Tick();

		Render();

#ifdef CRIPPLED
		CrippleClock::Render();
#endif //CRIPPLED

		glFlush();

        SDL_GL_SwapWindow(g_Display->m_Window);

		SDL_PumpEvents();
		SDL_Event event;
		while( SDL_PollEvent( &event ) == 1 )
		{
			switch( event.type )
			{
				case SDL_QUIT:
					throw QuitNotification();
					break;
				case SDL_KEYDOWN:
					switch( event.key.keysym.sym )
					{
						default:
							KeyDown( event.key.keysym );
							break;
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



