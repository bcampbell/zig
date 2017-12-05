

#include <SDL_opengl.h>
#include <assert.h>
#include <string>

#include "controller.h"
#include "display.h"
#include "drawing.h"
#include "gameover.h"
#include "gamestate.h"
#include "highscorescreen.h"
#include "level.h"
#include "mathutil.h"
#include "optionsscreen.h"
#include "soundmgr.h"
#include "texture.h"
#include "titlescreen.h"
#include "zig.h"
#include "dudes.h"

// time before highscore/demo/whatever
static float s_TimeOut = 5.0f;

static const char* facetiouscrap[] = {
	"WHEN GAMES GO BAD!",
	"MMMMM... BRAIN CANDY...",
	"A KINDER, GENTLER SHOOT-EM-UP",
	"NOW WITH 50 PERCENT MORE INNOVATION!",
	"HAND CRAFTED FOR YOUR GAMING ENJOYMENT",
	"A NEW PARADIGM FOR INTERACTIVE ELECTRONIC ENTERTAINMENT",
	"LET'S TAKE OFF AND NUKE THE SITE FROM ORBIT",
	"DARING. THOUGHT-PROVOKING. PROVOCATIVE.",
	"NOW IN COLOUR",
	"FULL OF RETRO GOODNESS",
	"MADE FROM THE FINEST NATURAL INGREDIENTS",
	"HOME-STYLE... JUST LIKE MOM USED TO CODE!",
	"25 PERCENT MORE IRONY THAN THE NEXT LEADING BRAND!",
	"ONE MORE STEP DOWN THE DARK PATH",
	"YOU KNOW... FOR THE KIDS!",
	"HOURS OF FUN FOR THE WHOLE FAMILY!",
	"THERE IS NO SPOON.",
	"SHARE AND ENJOY.",
	"MUST... DESTROY... MANKIND...",
	"WE'RE ALL GOING TO DIE.",
	"JUST ONE MICROSCOPIC COG IN A CATASTROPHIC PLAN",
	"HAPPINESS IS BACK IN STYLE",
	"ENTERPRISE EDITION"
};


TitleScreen::TitleScreen()
{
	m_CommentNum = (int)Rnd( 0.0f,
		(float)(sizeof(facetiouscrap)/sizeof(const char*)) );
}

TitleScreen::~TitleScreen()
{
}


void TitleScreen::Render()
{
	glClear( GL_COLOR_BUFFER_BIT );
	glLoadIdentity();

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glPushMatrix();
		glTranslatef( 0.0f, 150.0f, 0.0f );
		PlonkText( *g_Font, "ZIG", true, 64, 64 );
	glPopMatrix();

    vec2 tl = g_Display->TopLeft();
    vec2 tr = g_Display->TopRight();
	glColor4f( 0.4f, 0.4f, 0.4f, 1.0f );
	glPushMatrix();
		glTranslatef( tl.x + 8.0f, tl.y-12.0f, 0.0f );
		PlonkText( *g_Font, "ITSAGAMEWHEREYOUSHOOTSTUFF.COM", false, 8, 10 );
	glPopMatrix();
	glPushMatrix();
		glTranslatef( tr.x-40.0f, tr.y-12.0f, 0.0f );
		PlonkText( *g_Font, "V" ZIGVERSION, false, 8, 10 );
	glPopMatrix();
/*
	glColor4f( 0.4f, 0.4f, 0.4f, 1.0f );
	glPushMatrix();
		glTranslatef( 0.0f, -150.0f, 0.0f );
		PlonkText( *g_Font, "INSTRUCTIONS:", true, 8, 8 );
		glTranslatef( 0.0f, -16.0f, 0.0f );
		PlonkText( *g_Font, "CURSOR KEYS TO MOVE", true, 8, 8 );
		glTranslatef( 0.0f, -10.0f, 0.0f );
		PlonkText( *g_Font, "CTRL TO FIRE", true, 8, 8 );
	glPopMatrix();
*/
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glPushMatrix();
		glTranslatef( 0.0f, 100.0f, 0.0f );
		PlonkText( *g_Font, facetiouscrap[m_CommentNum], true, 11, 12 );
	glPopMatrix();

	
	m_Menu.Draw();

	glPushMatrix();
		glTranslatef( -200.0f, 50.0f, 0.0f );
        Baiter::StaticDraw();
		glTranslatef( 30.0f, 0.0f, 0.0f );
        Obstacle::StaticDraw();
		glTranslatef( 30.0f, 0.0f, 0.0f );
        Grunt::StaticDraw(50.0f);
		glTranslatef( 30.0f, 0.0f, 0.0f );
        Swarmer::StaticDraw();
		glTranslatef( 30.0f, 0.0f, 0.0f );
        Puffer::StaticDraw(sqrtf((Puffer::s_MinArea*2.0f)/pi), g_Time);
		glTranslatef( 30.0f, 0.0f, 0.0f );
        Bomber::StaticDraw(0.0f,0.2f);
	glPopMatrix();
}


void TitleScreen::Tick()
{
	m_Menu.Tick();
}


Scene* TitleScreen::NextScene()
{

	if( m_Menu.IsDone() )
	{
		switch( m_Menu.GetResult() )
		{
		case TitleMenu::ID_PLAY:
            {
			    delete this;
                g_GameState->StartNewGame();
                return new Level();
            }
		case TitleMenu::ID_OPTIONS:
            {
			    delete this;
                return new OptionsScreen();
            }
		case TitleMenu::ID_EXIT:
            {
                delete this;
                return 0;
            }
		}
	}
    else if (m_Menu.InactivityTime() > s_TimeOut)
    {
        delete this;
        g_GameState->StartNewDemo();
        return new Level();
    }

	return this;    // still running
}







TitleMenu::TitleMenu() :
	m_Done(false)
{
	AddItem( new MenuItem( ID_PLAY, vec2(0.0f,-20.0f), "PLAY", true, CTRL_BTN_START ) );
	AddItem( new MenuItem( ID_OPTIONS, vec2(0.0f,-50.0f), "OPTIONS" ) );
	AddItem( new MenuItem( ID_EXIT, vec2(0.0f,-80.0f), "EXIT", true, CTRL_BTN_ESC ) );
}

TitleMenu::ResultID TitleMenu::GetResult() const
{
	assert( m_Done );
	return m_Result;
}

void TitleMenu::OnSelect( int id )
{
	m_Done=true;
	m_Result=(ResultID)id;
}

