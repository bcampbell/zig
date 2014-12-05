#include <SDL_opengl.h>
#include "pausemenu.h"
#include "controller.h"
#include "drawing.h"
//#include "zig.h"


PauseMenu::PauseMenu()
{
	m_Done = false;
	AddItem( new MenuItem( (int)ResumeGame, vec2(0.0f,15.0f), "RESUME", true, CTRL_BTN_START ) );
	AddItem( new MenuItem( (int)AbortGame, vec2(0.0f,-15.0f), "QUIT", true, CTRL_BTN_ESC ) );
}


PauseMenu::~PauseMenu()
{
}


void PauseMenu::Draw()
{
	glLoadIdentity();

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );
	glBegin( GL_QUADS );
		glColor4f( 0.0f, 0.0f, 0.0f, 0.6f );
		glVertex2f( -100.0f, 50.0f );
		glVertex2f( 100.0f, 50.0f );
		glVertex2f( 100.0f, -50.0f );
		glVertex2f( -100.0f, -50.0f );
	glEnd();

	Menu::Draw();
}

void PauseMenu::OnSelect( int id )
{
	m_Result = (Result)id;
	m_Done = true;
}

