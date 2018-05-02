

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


static const char* stickchars = " ABCDEFGHIJKLMNOPQRSTUVWXYZ*!$";

HighScoreScreen::HighScoreScreen() :
	m_Time( 0.0f ),
	m_EntryTarget( -1 ),
    m_StickIdx( 0 )
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
			    if( name.size() < HighScores::MAX_NAME_SIZE) {
                   
                    if (fmodf(g_Time,flash) <flash/2.0f) {
                        Colour c = focusrange.Get(g_Time, false);
                        glColor3f( c.r, c.g, c.b);
                        glBegin(GL_QUADS);
                            glVertex2f(x,y+ch);
                            glVertex2f(x+cw,y+ch);
                            glVertex2f(x+cw,y);
                            glVertex2f(x,y);
                        glEnd();
                    } else {
                        // show currently-selected char
                        char buf[2] = {0};
                        buf[0] = stickchars[m_StickIdx];
				        glTranslatef( x, y, 0.0f );
                        if( buf[0] == '$' ) {
    				        PlonkText( *g_Font, "\xe2\x8f\x8e", false, cw, ch );
                        } else {
                            PlonkText( *g_Font, buf, false, cw, ch );
                        }
                    }
                } else {
                    glTranslatef( x, y, 0.0f );
    		        PlonkText( *g_Font, "\xe2\x8f\x8e", false, cw, ch );
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


static const float s_Timeout = 8.0f;

void HighScoreScreen::Tick()
{
    m_Time += (1.0f/TARGET_FPS);
	if( m_EntryTarget == -1 )
	{
        // force exit if button pressed
		if (m_Time> 0.5f && g_ControllerMgr->MenuController().Buttons() ) {
			m_Time = s_Timeout;
        }
        return;
	}

    // TODO: hacked in. This needs a good overhaul!
    // update input via gamepad

    // TODO: KILLKILLKILL!!!
    // latched controller uses WASD keys, so we need this.
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if( keys[SDL_SCANCODE_W] ||
        keys[SDL_SCANCODE_A] ||
        keys[SDL_SCANCODE_S] ||
        keys[SDL_SCANCODE_D] )
    {
        // bail out, to avoid clashing with keyboard entry...
        return;
    }
    int n = strlen(stickchars);
    float x = g_ControllerMgr->MenuController().XAxis();
    float y = g_ControllerMgr->MenuController().YAxis();
    int b = g_ControllerMgr->MenuController().Pressed();
    if( y>0.0f )
    {
        m_StickIdx++;
        if (m_StickIdx>=n) {
            m_StickIdx = 0;
        }
    }
    else if( y<0.0f )
    {
        m_StickIdx--;
        if (m_StickIdx<0) {
            m_StickIdx = n-1;
        }
    }
   
    std::string name = g_HighScores->Name( m_EntryTarget );
    if ( b & CTRL_BTN_ESC || x<0.0f)
    {
        // delete last char
        if( !name.empty() )
        {
            name = name.substr( 0, name.size()-1 );
            g_HighScores->SetName( m_EntryTarget, name );
        }
    }
	if( b & CTRL_BTN_FIRE )
    {
        char c= stickchars[m_StickIdx];
        if( c=='$' || name.size() >= HighScores::MAX_NAME_SIZE ) {

            m_Time = 0.0f;    // reset timeout and bask in glory for a bit
            EntryMode(-1);
        } else {
            name += c;
            m_StickIdx=0;
            g_HighScores->SetName( m_EntryTarget, name );
        }
    }
}

Scene* HighScoreScreen::NextScene()
{
	if( m_EntryTarget == -1 && m_Time >= s_Timeout )
    {
        // timed out
        delete this;
        return new DudeGallery();
    }

    return this;    // still running
}



void HighScoreScreen::HandleKeyDown( SDL_Keysym& keysym )
{

	if( m_EntryTarget == -1 )
	{
		// not in edit mode
		m_Time = s_Timeout;
		return;
	}

    SDL_Keycode code = keysym.sym;   // as opposed to hw scancode

	if (code == SDLK_RETURN)
	{
        m_Time = 0.0f;    // reset timeout and bask in glory for a bit
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



