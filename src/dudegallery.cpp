#include <SDL_opengl.h>
#include <assert.h>
#include <string>

#include "display.h"
#include "drawing.h"
#include "gameover.h"
#include "effects.h"
#include "fatzapper.h"
#include "dudegallery.h"
#include "titlescreen.h"
#include "zig.h"
#include "dudes.h"
#include "spiker.h"



DudeGallery::DudeGallery() : m_Done(false), m_Time(0.0f)
{
    m_SnowTexture = new SnowflakeTexture();
	g_Display->AddTexture( m_SnowTexture );
}

DudeGallery::~DudeGallery()
{
	g_Display->RemoveTexture( m_SnowTexture );
    delete m_SnowTexture;
}


void DudeGallery::Render()
{
	glClear( GL_COLOR_BUFFER_BIT );
	glLoadIdentity();

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

//    vec2 tl = g_Display->TopLeft();
//    vec2 tr = g_Display->TopRight();

    //float w = tr.x - tl.x;


	glColor4f( 1,1,1,1);
	glPushMatrix();
		glTranslatef( 0, 170.0f, 0.0f );
        PlonkText(*g_Font, "THE CAST", true, 12,12);
	glPopMatrix();

    const float speedup = 2.0f; // spawn effect looks better faster here
    int i;
    for(i=0;i<12;++i) {
        glPushMatrix();
        float dx = (float)((i%4)-1.5f) * 100.0f;
        float dy = (float)((i/4)-1.0f) * -100.0f;

		glTranslatef( dx,dy, 0.0f );
        float t = 1+(float)i/4.0f;
        float spawnt = t-DudeSpawnEffect::s_SpawnTime/speedup;
        if( m_Time > spawnt)
        {
            glPushMatrix();
            glRotatef( 90, 0.0f, 0.0f, 1.0f );
            DudeSpawnEffect::StaticDraw((m_Time - spawnt) * speedup);
            glPopMatrix();
        }


        if(m_Time>t) {
            DrawDude(i);
        }
        glPopMatrix();
    }

}


void DudeGallery::DrawDude(int n)
{
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

    std::string name;

	glPushMatrix();
    glRotatef( r2d(g_Time), 0.0f, 0.0f, 1.0f );
    switch(n)
    {
        case 0: Baiter::StaticDraw(); name="BAITER"; break;
        case 1: Obstacle::StaticDraw(); name="OBSTACLE"; break;
        case 2: Grunt::StaticDraw(50.0f); name="GRUNT"; break;
        case 3: Swarmer::StaticDraw(); name="SWARMER"; break;
        case 4:
            Puffer::StaticDraw(sqrtf((Puffer::s_MinArea*2.0f)/pi), g_Time);
            name="PUFFER";
            break;
        case 5: Bomber::StaticDraw(0.0f,0.2f); name = "BOMBER"; break;
        case 6: Tank::StaticDraw(0.0f); name = "TANK"; break;
        case 7: FatZapper::StaticDraw(14.0f, 0.5f + cosf(g_Time)*0.5f, 0.0f); name = "FATZAPPER"; break;
        case 8: Snowflake::StaticDraw(14.0f, m_SnowTexture); name = "SNOWFLAKE"; break;
        case 9: Flanker::StaticDraw(false); name = "FLANKER"; break;
        case 10: Spiker::StaticDraw(0.0f); name = "SPIKER"; break;
        case 11: Agitator::StaticDraw(0.5f); name = "AGITATOR"; break;
        default:
            assert(false);
            return;
    }
	glPopMatrix();

    Colour c = rawrange.Get(g_Time/4,false);
	glColor4f( c.r, c.g, c.b,1);
	glPushMatrix();
    glTranslatef(0,-32.0f, 0.0f);
    PlonkText(*g_Font,name,true,10,10);

	glPopMatrix();
}

void DudeGallery::Tick()
{
    m_Time += 1.0f/TARGET_FPS;
}

void DudeGallery::HandleKeyDown( SDL_Keysym& keysym )
{
    m_Done = true;
}

Scene* DudeGallery::NextScene()
{
    if (m_Time > 20.0f) {
        m_Done = true;
    }

    if( m_Done )
    {
        delete this;
        return new TitleScreen();
    }

	return this;    // still running
}





