

#include <SDL.h>
#include <SDL_opengl.h>
#include <assert.h>
#include <string>

#include "controller.h"
#include "drawing.h"
#include "texture.h"
#include "gameover.h"
#include "gamestate.h"
#include "highscores.h"
#include "highscorescreen.h"
#include "player.h"
#include "soundmgr.h"
#include "titlescreen.h"
#include "zig.h"


GameOver::GameOver() :
	m_Timer( 0.0f ),
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
		sprintf( buf, "LEVEL %d", g_GameState->PerceivedLevel() );
		PlonkText( *g_Font, buf, true, 8.0f,8.0f );

		glTranslatef( 0.0f, -20.0f, 0.0f );
		sprintf( buf, "%d POINTS", g_GameState->m_Player->Score() );
		PlonkText( *g_Font, buf, true, 12.0f,12.0f );
	glPopMatrix();
}

void GameOver::Tick()
{
	const float timeout = 10.0f;
	m_Timer += 1.0f/TARGET_FPS;
	if( m_Timer > timeout )
		m_Done = true;

    int pressed = g_ControllerMgr->MenuController().Pressed();
   
    if (m_Timer>0.5f && pressed)
    {
		m_Done = true;
    }
    if (pressed & CTRL_BTN_ESC)
    {
        m_Done = true;
    }

	// bail out early if a any key is pressed
	if( m_Timer > 5.0f )
    {
		m_Done = true;
    }
}

Scene* GameOver::NextScene()
{
	if( m_Done )
    {
        int scoreidx = g_HighScores->Submit( g_GameState->m_Player->Score(), g_GameState->m_Level + 1);

        delete this;
        HighScoreScreen* hs = new HighScoreScreen();
        if( scoreidx != -1 )
        {
            hs->EntryMode(scoreidx);
        }
        return hs;
    }
    return this;
}


