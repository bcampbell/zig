#include <SDL.h>
#include <SDL_opengl.h>
#include <assert.h>
#include <string>

#include "colours.h"
#include "drawing.h"
#include "texture.h"
#include "titlescreen.h"
#include "optionsscreen.h"


void OptionsScreen::Render()
{
	glClear( GL_COLOR_BUFFER_BIT );
	glLoadIdentity();

	m_Menu.Draw();
}


void OptionsScreen::Tick()
{
	m_Menu.Tick();
}
	
Scene* OptionsScreen::NextScene()
{
	if (m_Menu.IsDone())
    {
        delete this;
        return new TitleScreen();
    }
    return this;
}




