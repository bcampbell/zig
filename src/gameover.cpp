

#include <SDL.h>
#include <SDL_opengl.h>
#include <assert.h>
#include <string>

#include "controller.h"
#include "drawing.h"
#include "texture.h"
#include "gameover.h"
#include "soundmgr.h"
#include "zig.h"


GameOver::GameOver( int score, int level ) :
	m_Timer( 0.0f ),
	m_Score( score ),
	m_Level( level ),
	m_Done( false )
{
	SoundMgr::Inst().Play( SFX_GAMEOVER );
}

void GameOver::Render()
{
	glClear( GL_COLOR_BUFFER_BIT );
	glLoadIdentity();

	glPushMatrix();
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		glTranslatef( 0.0f, 30.0f, 0.0f );
		PlonkText( *g_Font, "GAME OVER", true );

		glColor4f( 0.6f, 0.6f, 0.6f, 1.0f );
		glTranslatef( 0.0f, -30.0f, 0.0f );
		char buf[64];
		sprintf( buf, "LEVEL %d", m_Level );
		PlonkText( *g_Font, buf, true, 8.0f,8.0f );

		glTranslatef( 0.0f, -20.0f, 0.0f );
		sprintf( buf, "%d POINTS", m_Score );
		PlonkText( *g_Font, buf, true, 12.0f,12.0f );
	glPopMatrix();
}

void GameOver::Tick()
{
	const float timeout = 10.0f;
	m_Timer += 1.0f/TARGET_FPS;
	if( m_Timer > timeout )
		m_Done = true;

	if( g_MenuController->Button() && m_Timer > 0.5f )
		m_Done = true;
}

bool GameOver::IsFinished()
{
	return m_Done;
}

void GameOver::HandleKeyDown( SDL_Keysym& keysym )
{
    if (keysym.sym==SDLK_ESCAPE)
        m_Done = true;

	// bail out early if a any key is pressed
	if( m_Timer > 0.5f )
		m_Done = true;
}

