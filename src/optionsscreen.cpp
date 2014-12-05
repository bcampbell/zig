#include "drawing.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <assert.h>
#include <string>

#include "texture.h"
#include "optionsscreen.h"
#include "colours.h"


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
	
bool OptionsScreen::IsFinished()
{
	return ( m_Menu.IsDone() );
}




