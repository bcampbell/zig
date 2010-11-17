

#include "agentmanager.h"
#include "bullets.h"
#include "colours.h"
#include "dudes.h"
#include "mathutil.h"
#include "texture.h"

#include <SDL_opengl.h>
#include <cstdio>

Bullet::Bullet( Player& owner, int power, vec2 const& relpos, float relheading ) :
	m_Owner(owner),
	m_Power(power)
{
	MoveTo( owner.Pos() +  Rotate( relpos, owner.Heading() ) );
	float heading = relheading + owner.Heading();
	while( heading > twopi )
		heading -= twopi;
	TurnTo( heading );
}


Laser::Laser( Player& owner, vec2 const& relpos, float relheading ) :
	Bullet( owner, 1, relpos, relheading ),
	m_Timer( 30 )
{
	SetRadius(5.0f);
}

void Laser::Draw()
{
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel(GL_FLAT);
	glColor3f( 1.0f, 1.0f, 1.0f );
	glBegin( GL_LINES );
		glVertex2f( -0.0f, -5.0f );
		glVertex2f( 0.0f, 5.0f );
	glEnd();

	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, g_BlueGlow->ID() );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	glEnable( GL_BLEND );
	glBlendFunc( GL_ONE, GL_ONE );

	glColor3f( 0.3f, 0.3f, 0.3f  );
	glBegin( GL_QUADS );
		glTexCoord2f( 0.0f, 1.0f );
		glVertex2f( -12.0f, 12.0f );
		glTexCoord2f( 1.0f,1.0f);
		glVertex2f( 12.0f, 12.0f );
		glTexCoord2f( 1.0f, 0.0f);
		glVertex2f( 12.0f, -12.0f );
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f( -12.0f, -12.0f );
	glEnd();


}

void Laser::Tick()
{
	if( --m_Timer < 0 )
		Die();
	else
	{
		Forward( 10.0f );
	 	g_Agents->AddUnderlay( new LaserCrud( Pos(), Heading() ) );
	}
}




FatLaser::FatLaser( Player& owner, vec2 const& relpos, float relheading ) :
	Bullet( owner, 4, relpos, relheading ),
	m_Timer( 0 )
{
	SetRadius(20.0f);
}

const float FatLaser::s_FwdSpd = 30.0f;
const int FatLaser::s_Duration = 20;

void FatLaser::Draw()
{
	const float x0 = -20.0f;
	const float x1 = 20.0f;

	const float maxbod = 200.0f;
	float bod=(s_FwdSpd * m_Timer);
	if( bod > maxbod )
		bod = maxbod;
	const float cap=10.0f;

	glShadeModel(GL_FLAT);
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, g_BlueGlow->ID() );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	glEnable( GL_BLEND );
	glBlendFunc( GL_ONE, GL_ONE );


	glColor3f( 1.0f, 1.0f, 1.0f  );
	glBegin( GL_QUADS );

		// top cap
		glTexCoord2f( 0.0f, 0.0f );
		glVertex2f( x0, cap );
		glTexCoord2f( 1.0f,0.0f);
		glVertex2f( x1, cap );

		glTexCoord2f( 1.0f, 0.5f);
		glVertex2f( x1, 0.0f );
		glTexCoord2f(0.0f, 0.5f);
		glVertex2f( x0, 0.0f );

		// body
		glTexCoord2f( 0.0f, 0.5f);
		glVertex2f( x0, 0.0f );
		glTexCoord2f(1.0f, 0.5f);
		glVertex2f( x1, 0.0f );
		
		glTexCoord2f( 1.0f, 0.5f);
		glVertex2f( x1, -bod );
		glTexCoord2f(0.0f, 0.5f);
		glVertex2f( x0, -bod );

		// bottom cap
		glTexCoord2f( 0.0f, 0.5f );
		glVertex2f( x0, -bod );
		glTexCoord2f( 1.0f,0.5f);
		glVertex2f( x1, -bod );

		glTexCoord2f( 1.0f, 1.0f);
		glVertex2f( x1, -(bod+cap) );
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f( x0, -(bod+cap) );
	glEnd();
}


void FatLaser::Tick()
{
	++m_Timer;
	if( m_Timer >= s_Duration )
		Die();
	else
	{
		Forward( s_FwdSpd );
//	 	g_Agents->AddUnderlay( new LaserCrud( Pos(), Heading() ) );
	}
}



// evil, menacing colours.
static const Colour lasercrudcolours[] =
{
	Colour( 1.0f, 1.0f, 1.0f, 0.2f ),
	Colour( 1.0f, 0.7f, 0.0f, 0.2f ),
	Colour( 0.5f, 0.0f, 0.0f, 0.2f ),
	Colour( 0.5f, 0.0f, 0.5f, 0.2f ),
	Colour( 0.0f, 0.0f, 1.0f, 0.1f ),
	Colour( 0.0f, 0.0f, 1.0f, 0.0f ),
};

static const ColourRange lasercrudrange( lasercrudcolours, 6 );


LaserCrud::LaserCrud( vec2 const& pos, float heading ) :
	m_Life(1.0f),
	m_LifeDecay( Rnd( 0.05f, 0.3f ) )
{
	vec2 newpos(pos);
	newpos.x += Rnd(-2.0f,2.0f);
	newpos.y += Rnd(-2.0f,2.0f);
	MoveTo( newpos );

	heading += Rnd( -0.2f, 0.2f );
	TurnTo( heading );
}

void LaserCrud::Draw()
{
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	glShadeModel(GL_FLAT);


	Colour c = lasercrudrange.Get( 1.0f - m_Life, true );
	glColor4f( c.r, c.g, c.b, c.a );
	glBegin( GL_TRIANGLES );
		glVertex2f( 0.0f, 10.0f );
		glVertex2f( 2.0f, -20.0f );
		glVertex2f( -2.0f, -20.0f );
	glEnd();

}

void LaserCrud::Tick()
{
	m_Life -= m_LifeDecay;
	if( m_Life <= 0.0f )
		Die();
}


