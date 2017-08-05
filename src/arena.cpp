#include "arena.h"

//#include <SDL.h>
#include <SDL_opengl.h>

#include "colours.h"
#include "display.h"
#include "mathutil.h"
#include "texture.h"
#include "vec2.h"
#include "zig.h"



class ArenaGlowTexture : public CalculatedTexture
{
public:
	ArenaGlowTexture( int pixw, int pixh ) : CalculatedTexture( pixw, pixh )
		{}
	virtual void Calc( float x, float y, Colour& out );
};

void ArenaGlowTexture::Calc( float x, float y, Colour& out )
{
	float my = y < 0.5f ? 2.0f*y : 2.0f*(1.0f-y);

	out.r = 0.3f*my*my*my*my;
	out.g = 0.3f*my*my*my*my;
	out.b = 0.5f*my*my;
	out.a = 1.0f;
}

Arena::Arena( float initialradius ) : m_Radius( initialradius ), m_Glow( 0 )
{
	m_Glow = new ArenaGlowTexture( 32,32 );
	g_Display->AddTexture( m_Glow );
}

Arena::~Arena()
{
	g_Display->RemoveTexture( m_Glow );
	delete m_Glow;
}

void Arena::Tick()
{
}

void Arena::Draw()
{
	float theta;

	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );

#if 0
    // marker at 'north' - handy for debugging
    
    glPushMatrix();
    glTranslatef(0.0f, m_Radius,0.0f);
	glColor3f( 1.0f, 1.0f, 1.0f );
    glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(8.0f, -4.0f);
        glVertex2f(-8.0f, -4.0f);
    glEnd();
    glPopMatrix();
#endif

	glColor3f( 0.0f, 0.0f, 1.0f );
	glBegin(GL_LINE_LOOP);
	for( theta=0.0f; theta<twopi; theta=theta + twopi/SEGMENTS)
	{
		float r=m_Radius + Rnd( -6.0f, 6.0f );
		glVertex2f( r*sin(theta), r*cos(theta));
	}
	glEnd();

	glColor3f( 0.5f, 0.5f, 1.0f );
	glBegin(GL_LINE_LOOP);
	for( theta=0.0f; theta<twopi; theta=theta + twopi/SEGMENTS)
	{
		float r=m_Radius + Rnd( -2.0f, 2.0f );
		glVertex2f( r*sin(theta), r*cos(theta));
	}
	glEnd();

	glColor3f( 0.9f, 0.9f, 1.0f );
	glBegin(GL_LINE_LOOP);
	for( theta=0.0f; theta<twopi; theta=theta + twopi/SEGMENTS)
	{
		float r=m_Radius + Rnd( -1.0f, 1.0f );
		glVertex2f( r*sin(theta), r*cos(theta));
	}
	glEnd();


	glEnable( GL_BLEND );
	glBlendFunc( GL_ONE, GL_ONE );
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D,m_Glow->ID() );
	glColor3f( 1.0f, 1.0f, 1.0f );
	glBegin( GL_TRIANGLE_STRIP );
	for( theta=0.0f; theta<twopi; theta=theta + twopi/SEGMENTS)
	{
		const float glowradius = 20.0f;
		float r1 = m_Radius-glowradius;
		float r2 = m_Radius+glowradius;

		vec2 p1( r1*sin(theta), r1*cos(theta) );
		vec2 p2( r2*sin(theta), r2*cos(theta) );

		glTexCoord2f( 0.0f, 0.0f );
		glVertex2f( p1.x, p1.y );

		glTexCoord2f( 0.0f, 1.0f );
		glVertex2f( p2.x, p2.y );
	}
	glEnd();
}

