#include "scene.h"
#include "zig.h"
#include "display.h"
#include "controller.h"

#include "soundmgr.h"


Scene* Scene::ExecFrame()
{

    g_ControllerMgr->Tick();
    
    Tick();
    Render();

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
                    delete this;
                    return 0;
                }
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
    return NextScene();
}

