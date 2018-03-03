

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


static void drawControls( float t );

// time before highscore/demo/whatever
static float s_TimeOut = 8.0f;

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
    vec2 bl = g_Display->BottomLeft();
	glColor4f( 0.4f, 0.4f, 0.4f, 1.0f );
	glPushMatrix();
		glTranslatef( tl.x + 8.0f, tl.y-12.0f, 0.0f );
		PlonkText( *g_Font, "ITSAGAMEWHEREYOUSHOOTSTUFF.COM", false, 8, 10 );
	glPopMatrix();
	glPushMatrix();
		glTranslatef( tr.x-40.0f, tr.y-12.0f, 0.0f );
		PlonkText( *g_Font, "V" ZIGVERSION, false, 8, 10 );
	glPopMatrix();

    float idle = m_Menu.InactivityTime();
    if( idle > 1.0f ) {
        float fade = (idle-1.0f)*2.0f;
        fade = zfmin(fade,1.0f);

    	glColor4f( fade*0.4f, fade*0.4f, fade*0.4f, 1.0f );

    	glPushMatrix();
	    	glTranslatef( 0, bl.y + 12.0f, 0.0f );
            drawControls(idle-1.0f);
        glPopMatrix();
    }
/*
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
//        return new InstructionScreen();
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


static void drawControls( float t )
{
    const char* wasdKeys[4] = {"W","A","S","D"};
    const char* cursorKeys[4] = {
        "\xe2\x86\x91", // UP
        "\xe2\x86\x90", // LEFT
        "\xe2\x86\x93", // DOWN"
        "\xe2\x86\x92"  // RIGHT
    };


    const float sz = 12.0f;     // font size

    const float kw=sz + (sz/4.0f);   // arrow key width
    const float kpad = sz/2;   // pad between keys

    const float movew = kw+kpad+kw+kpad+kw;

    const char** keys = ( fmod(t,2.0f) < 1.0f ) ? cursorKeys:wasdKeys;

    const float pad = sz;
    float totalw = movew + pad + 1*sz + pad + sz*5;

    glPushMatrix();
        glTranslatef( 0, sz*5 , 0.0f );
        PlonkText(*g_Font, "CONTROLS", true,12,12);
	glPopMatrix();

	glPushMatrix();
        const vec2 offsets[4] = {
            vec2(kw+kpad,kw+kpad),
            vec2(0,0),
            vec2(kw+kpad,0),
            vec2((kw+kpad)*2,0)
        };
        glTranslatef( -totalw/2, 0.0f, 0.0f );

        int i;
        for (i=0; i<4; ++i)
        {
            glPushMatrix();
            glTranslatef( offsets[i].x, offsets[i].y, 0.0f );
            PlonkTextKeyCapped( *g_Font, keys[i], false, sz,sz );
            glPopMatrix();
        }
        glTranslatef( movew + pad, 0.0f, 0.0f );

        PlonkText(*g_Font, "&", false, sz, sz);
        glTranslatef( 1*sz + pad, 0.0f, 0.0f );

        //const char* fireKeys[] = {"CTRL","\xe2\x87\xa7"" SHIFT","\xe2\x8f\x8e"" ENTER","SPACE"};
#if defined( __APPLE__ ) && defined( __MACH__ )
        const char* fireKeys[] = {"SHIFT","SPACE","\xe2\x8f\x8e"};
#else
        const char* fireKeys[] = {"CTRL","SHIFT","SPACE"};
#endif
        const int n = sizeof(fireKeys)/sizeof(const char*);

        int idx = (int)(t / 1.0f) % n;
        PlonkTextKeyCapped( *g_Font, fireKeys[idx], false, sz,sz );
    glPopMatrix();
}

