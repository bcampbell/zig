#include "icongen.h"

#ifdef ENABLE_ICONGEN_HACKERY

#include <assert.h>
#include <string>

#include "arena.h"
#include "background.h"
//#include "controller.h"
#include "display.h"
#include "drawing.h"
//#include "gameover.h"
//#include "gamestate.h"
//#include "highscorescreen.h"
//#include "level.h"
#include "mathutil.h"
#include "player.h"
//#include "optionsscreen.h"
//#include "soundmgr.h"
#include "texture.h"
#include "titlescreen.h"
#include "wobbly.h"
#include "zig.h"
//#include "dudes.h"

//#define OFFSCREEN


static void drawExhaust( vec2 pos, float theta, float f );

IconGenScreen::IconGenScreen() : m_Done(false)
{
    m_Arena = new Arena(220.0f);
		m_Background = new OriginalBackground();

}

IconGenScreen::~IconGenScreen()
{
    delete m_Arena;
    delete m_Background;
    // TODO: free up gl stuff
}


static const float heading = (15.0f * twopi) / 360.0f;

void IconGenScreen::RenderBG()
{
	glLoadIdentity();
	m_Background->Draw( vec2(100,30), heading);
}


void IconGenScreen::RenderTrail()
{

    glRotatef(-r2d(heading), 0,0,1);
    int i;
    for( i=0; i<160; ++i ) {
	    vec2 vel  = Rotate( vec2( 0.0f, Rnd(5.0f,8.0f) ),
    		pi + Rnd( -0.15f, 0.15f ) );
        vel *= 65;
        float life = Rnd(1.0f, 3.0f);
        float f = (life-1.0f) / (3.0f-1.0f);
        vec2 pos = vec2(0,0) + (f*vel);

        float a = Rnd(0,twopi);
        drawExhaust( pos, a, f);
    }
}


void IconGenScreen::RenderPlayer()
{
    glPushMatrix();
        glTranslatef(0,30,0);
        Player::StaticDraw(80.0f);
    glPopMatrix();
}

void IconGenScreen::Render()
{
    glClearColor(0,0,0,0);
	glClear( GL_COLOR_BUFFER_BIT );
    RenderBG();
    RenderTrail();
    m_Arena->Draw();
    RenderPlayer();
}


void IconGenScreen::DumpEm()
{
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Render to our framebuffer
    glClearColor(0,0,0,0);
	glClear( GL_COLOR_BUFFER_BIT );
	glLoadIdentity();
	m_Background->Draw( vec2(100,30), heading);
    DumpTGA("/tmp/bg.tga");


	glClear( GL_COLOR_BUFFER_BIT );
    m_Arena->Draw();
    DumpTGA("/tmp/ring.tga");


	glClear( GL_COLOR_BUFFER_BIT );
    RenderTrail();
    DumpTGA("/tmp/trail.tga");


/*    vec2 tl = g_Display->TopLeft();
    vec2 tr = g_Display->TopRight();
    vec2 bl = g_Display->BottomLeft();
*/
	glClear( GL_COLOR_BUFFER_BIT );
    glPushMatrix();
        glTranslatef(0,30,0);
        Player::StaticDraw(80.0f);
    glPopMatrix();

    DumpTGA("/tmp/icon.tga");


    // screen
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void IconGenScreen::DumpTGA(const char* filename)
{
    int w= m_W;
    int h=m_H;
    uint8_t *buf = new uint8_t[w*h*4];

    glReadBuffer( GL_COLOR_ATTACHMENT0);
    glReadPixels(0,0,w,h, GL_BGRA, GL_UNSIGNED_BYTE, buf);

    uint8_t hdr[3+5+10] = {
        0,0,2,
        0,0,0,0,0,
        0,0,        // xoffset
        0,0,        // yoffset
        (uint8_t)(w&0xff),(uint8_t)(w>>8),
        (uint8_t)(h&0xff),(uint8_t)(h>>8),
        32,
        0x08,
    };

    FILE* fp =  fopen(filename, "wb");
    if( !fp ) {
        delete [] buf;
        throw Wobbly("fopen failed\n");
    }
    fwrite(hdr, 1,3+5+10, fp);
    fwrite(buf, 1,w*h*4,fp);
    fclose(fp);
    delete [] buf;
}


void IconGenScreen::HandleKeyDown( SDL_Keysym& keysym )
{
    SDL_Keycode code = keysym.sym;   // as opposed to hw scancode

	if (code == SDLK_ESCAPE) {

        initOffscreen(512,512);
        DumpEm();
        ditchOffscreen();



        m_Done=true;
    }
}

void IconGenScreen::Tick()
{
    m_Arena->Tick();
}


Scene* IconGenScreen::NextScene()
{
    if( m_Done ) {
        delete this;
        return NULL;    //new TitleScreen();
    }

	return this;    // still running
}


void drawExhaust( vec2 pos, float theta, float f )
{
	glShadeModel(GL_SMOOTH);
	glEnable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );

	float s = 1 + f*3.5f;
	glPushMatrix();
		glTranslatef( pos.x, pos.y, 0.0f );
		glRotatef( r2d(theta), 0,0,1);
        glBlendFunc( GL_SRC_ALPHA, GL_ONE ); //GL_ONE_MINUS_SRC_ALPHA );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        //glBlendFunc( GL_ONE, GL_ONE ); //GL_ONE_MINUS_SRC_ALPHA );
        glBegin( GL_TRIANGLES );
            glColor4f( 1.0f, 0.8f, 0.0f, 1.0f-f); //f > 0.3f ? 0.3f : f );
            glVertex2f( 0.0f*s, 5.0f*s );
            glVertex2f( 8.0f*s, -3.0f*s );
            glVertex2f( -8.0f*s, -3.0f*s );
        glEnd();
	glPopMatrix();
}


void IconGenScreen::initOffscreen(int w,int h)
{
    m_W = w;
    m_H = h;


    // set up offscreen render and texture
    glGenFramebuffers(1, &m_FrameBuf);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuf);
    // and texture
    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, m_W, m_H, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // and depthbuffer
    glGenRenderbuffers(1, &m_Depth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_Depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_W, m_H);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_Depth);
    //glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_Texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture, 0);

    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw Wobbly("glCheckFramebufferStatus() failed\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuf);

    float xmag = m_W/2.0;
    float ymag = m_H/2.0;
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

    glOrtho( -xmag, xmag, -ymag,ymag, -1.0, 1.0 );
	glMatrixMode( GL_MODELVIEW );
	glViewport( 0,0, m_W, m_H );
}

void IconGenScreen::ditchOffscreen()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // TODO: free all the GL gubbins
    // TODO: restore the viewport/projection etc...
    // but not big deal as we'll bail out anyway
}

#endif // ENABLE_ICONGEN_HACKERY
