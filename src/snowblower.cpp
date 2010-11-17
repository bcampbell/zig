#include <SDL_opengl.h>

#include "agentmanager.h"
#include "colours.h"
#include "snowblower.h"
#include "effects.h"
#include "level.h"
#include "zig.h"




const float SnowBlower::s_OffTime = 4.0f;
const float SnowBlower::s_OnTime = 0.2f;

SnowBlower::SnowBlower() :
	m_Life( 20 )
{
	SetFlags( flagCanHitBullet| flagCanHitPlayer| flagLocksLevel );
	Respawn();
}

void SnowBlower::Respawn()
{
	SetRadius( 20.0f );
	m_Flash = 0.0f;
	m_Timer = Rnd( 0.0f, s_OffTime );
	m_State = OFF;
	RandomPos();
}


void SnowBlower::Tick()
{
	TurnBy( pi/64.0f );
	m_Flash *= 0.8f;
	m_Timer += 1.0f/TARGET_FPS;
	if( m_State == ON )
	{
		vec2 vel = Rotate(
			vec2( 0.0f, Rnd( 8.0f, 10.0f ) ),
			Heading() );	//+ Rnd( -pi/32.0f, pi/32.0f ) );
		Dude* d = new Snowflake( Pos(), vel );
		g_Agents->AddDude( d );

		if( m_Timer > s_OnTime )
		{
			m_State = OFF;
			m_Timer = 0.0f;
		}
	}
	else
	{
		if( m_Timer > s_OffTime )
		{
			m_State = ON;
			m_Timer = 0.0f;
		}
	}
}



void SnowBlower::Draw()
{
	Colour c = ColourLerp(
		Colour(0.5f,0.5f,0.5f,1.0f),
		Colour::WHITE,
		m_Flash );

	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_SMOOTH );
	glBegin( GL_TRIANGLES );
		glColor3f( c.r, c.g, c.b );
		glVertex2f( -10.0f, 16.0f );

		glColor3f( c.r, c.g, c.b );
		glVertex2f( 10.0f, 16.0f );

		glColor3f( c.r, c.g, c.b );
		glVertex2f( 0.0f, -16.0f );
	glEnd();
}


void SnowBlower::OnHitBullet( Bullet& bullet )
{
	--m_Life;
	if( m_Life < 0 )
	{
		BigArseDeath( bullet, 100 );
		Die();
	}
	else
	{
		int i;
		for(i=0;i<5;++i)
			g_Agents->AddDude(new Frag(Pos()));
		m_Flash = 1.0f;
	}
}

