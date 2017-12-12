

#include "drawing.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <assert.h>
#include <string>
#include <ctype.h>

#include "controller.h"
#include "colours.h"
#include "dudegallery.h"
#include "texture.h"
#include "highscores.h"
#include "highscorescreen.h"
#include "titlescreen.h"

static const Colour focuscolours[] =
{
	Colour( 1.0f, 1.0f, 0.0f),
	Colour( 0.7f, 0.2f, 0.0f),
};

static const ColourRange focusrange( focuscolours, 2 );

static const Colour raw[] =
{
    Colour( 1.0f, 0.0f, 0.0f),
    Colour( 1.0f, 1.0f, 0.0f),
    Colour( 0.0f, 1.0f, 0.0f),
    Colour( 0.0f, 1.0f, 1.0f),
    Colour( 0.0f, 0.0f, 1.0f),
    Colour( 1.0f, 0.0f, 1.0f),
};

static const ColourRange rawrange( raw, 6 );



HighScoreScreen::HighScoreScreen() :
	m_TimeOut( 400 ),
	m_EntryTarget( -1 )
{
}

void HighScoreScreen::EntryMode( int scoreidx )
{
	m_EntryTarget = scoreidx;
}

HighScoreScreen::~HighScoreScreen()
{
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
	for( i=0; i<g_HighScores->NumScores(); ++i )
	{
		char buf[ 128 ];
        std::string const& name = g_HighScores->Name(i);
		int n = sprintf( buf, "%d %s", g_HighScores->Score(i), name.c_str() );
	    const float cw = 16.0f;
	    const float ch = 16.0f;
		if( i==m_EntryTarget )
		{
			glColor4f( 1.0f, 1.0f, 0.0f, 1.0f );
			glPushMatrix();
				glTranslatef( 0.0f, 110.0f - (float)(i*24), 0.0f );
				PlonkText( *g_Font, buf, true, cw, ch );

                glDisable(GL_TEXTURE_2D);
	            glDisable( GL_BLEND );
	            glShadeModel( GL_FLAT );

                float x=((float)n*cw)/2.0f;
                float y=-ch/2.0f;

                const float flash=0.5f;
			    if( name.size() < HighScores::MAX_NAME_SIZE && fmodf(g_Time,flash) <flash/2.0f)
                {
                    Colour c = focusrange.Get(g_Time, false);
			        glColor3f( c.r, c.g, c.b);
                    glBegin(GL_QUADS);
                        glVertex2f(x,y+ch);
                        glVertex2f(x+cw,y+ch);
                        glVertex2f(x+cw,y);
                        glVertex2f(x,y);
                    glEnd();
                }

			glPopMatrix();
		}
		else
		{
            Colour c = rawrange.Get(g_Time - i*0.05f,false);
        	glColor4f( c.r, c.g, c.b,1);
//			glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
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

		if( g_ControllerMgr->MenuController().Buttons() )
			m_TimeOut = 0;
	}
}

Scene* HighScoreScreen::NextScene()
{
	if( m_TimeOut > 0 )
    {
        return this;    // still running
    }

    delete this;
    return new DudeGallery();
}



void HighScoreScreen::HandleKeyDown( SDL_Keysym& keysym )
{

	if( m_EntryTarget == -1 )
	{
		// not in edit mode
		m_TimeOut = 0;
		return;
	}

    SDL_Keycode code = keysym.sym;   // as opposed to hw scancode

	if (code == SDLK_RETURN)
	{
        EntryMode(-1);
		return;
	}

	std::string name = g_HighScores->Name( m_EntryTarget );

	if (code==SDLK_LEFT || code==SDLK_BACKSPACE)
	{
		// delete last char
		if( !name.empty() )
			name = name.substr( 0, name.size()-1 );
	}
	else
	{
        // Cheesiness - SDL keycode values tend to correspond to
        // ascii(/unicode).
        // TODO: use utf-8, input via SDL_TextInputEvent
		char c = (char)code;

		// TODO: get more chars into the font!
		if ((c>='a' && c<='z') ||
            (c>='0' && c<='9') ||
			c=='-' || c=='\'' || c=='!' ||
			c==':' || c=='.' || c==',' || c==' ')
		{
            c = toupper(c);
			if( name.size() < HighScores::MAX_NAME_SIZE )
				name += c;
		}
	}
	g_HighScores->SetName( m_EntryTarget, name );
}



