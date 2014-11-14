

#include "drawing.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <assert.h>
#include <string>
#include <ctype.h>

#include "controller.h"
#include "texture.h"
#include "highscores.h"
#include "highscorescreen.h"



HighScoreScreen::HighScoreScreen( HighScores& scores ) :
	m_Scores( scores ),
	m_TimeOut( 400 ),
	m_EntryTarget( -1 )
{
}

void HighScoreScreen::EntryMode( int scoreidx )
{
	m_EntryTarget = scoreidx;
}

void HighScoreScreen::Render()
{
	glClear( GL_COLOR_BUFFER_BIT );
	glLoadIdentity();


//	glEnable( GL_BLEND );
//	glBlendFunc( GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR );
//	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glPushMatrix();
		glTranslatef( 0.0f, 160.0f, 0.0f );
		if( m_EntryTarget != -1 )
			PlonkText( *g_Font, "ENTER NAME", true, 16, 16 );
		else
			PlonkText( *g_Font, "HIGHSCORES", true, 32, 32 );
			
	glPopMatrix();


	int i;
	for( i=0; i<m_Scores.NumScores(); ++i )
	{
		char buf[ 128 ];
		sprintf( buf, "%d %s", m_Scores.Score(i), m_Scores.Name(i).c_str() );
	
		if( i==m_EntryTarget )
		{
			glColor4f( 1.0f, 1.0f, 0.0f, 1.0f );
			glPushMatrix();
				glTranslatef( 0.0f, 110.0f - (float)(i*24), 0.0f );
				PlonkText( *g_Font, buf, true, 16, 16 );
			glPopMatrix();
		}
		else
		{
			glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
			glPushMatrix();
				glTranslatef( 0.0f, 110.0f - (float)(i*24), 0.0f );
				PlonkText( *g_Font, buf, true, 16-i/2.0f, 16-i/2.0f );
			glPopMatrix();
		}
	}
}

void HighScoreScreen::Tick()
{
	if( m_EntryTarget == -1 )
	{
		--m_TimeOut;

		if( g_MenuController->Button() )
			m_TimeOut = 0;
	}
}

bool HighScoreScreen::IsFinished()
{
	if( m_TimeOut > 0 )
		return false;
	else
		return true;
}



void HighScoreScreen::KeyDown( SDL_Keysym& keysym )
{
	if( m_EntryTarget == -1 )
	{
		// not in edit mode
		m_TimeOut = 0;
		return;
	}

	if( keysym.sym == SDLK_RETURN )
	{
		m_EntryTarget = -1;
		return;
	}
    // TODO SDL2
#if 0
	// ignore international chars for now
	if( keysym.unicode & 0xFF80 )
		return;

	std::string name = m_Scores.Name( m_EntryTarget );

	if( keysym.sym == SDLK_LEFT || keysym.sym == SDLK_BACKSPACE )
	{
		// delete last char
		if( !name.empty() )
			name = name.substr( 0, name.size()-1 );
	}
	else
	{
		char c = keysym.unicode & 0x7f;

		// TODO: get more chars into the font!
		c = toupper(c);
		if( isalnum(c) ||
			c=='-' || c=='\'' || c=='!' ||
			c==':' || c=='.' || c==',' || c==' ' )
		{
			if( name.size() < HighScores::MAX_NAME_SIZE )
				name += c;
		}
	}
	m_Scores.SetName( m_EntryTarget, name );
#endif
}


