#include <SDL_opengl.h>

#include "agentmanager.h"
#include "colours.h"
#include "edgepuller.h"
#include "effects.h"
#include "level.h"
#include "zig.h"



EdgePuller::EdgePuller() :
	m_Life( 5 )
{
	SetFlags( flagCanHitBullet| flagCanHitPlayer| flagLocksLevel );
	Respawn();
}

void EdgePuller::Respawn()
{
	SetRadius( 16.0f );
	RandomPos();
	m_Flash = 0.0f;
	m_Attached = false;
	m_TargetHeading = Rnd(-pi,pi);
}

void EdgePuller::Tick()
{
	if( m_Attached )
	{
		g_CurrentLevel->ShrinkArenaBy( 50.0f );

		// keep attached to arena edge
		vec2 pos = Pos();
		pos.Normalise();
		pos *= g_CurrentLevel->ArenaRadius();
		MoveTo(pos);

		// point inward
		TurnToward( vec2::ZERO, pi/16.0f );
	}
	else
	{
		// turn
		const float turnspd = pi/128.0f;
		float d = m_TargetHeading-Heading();
		while( d < -pi )
			d+= twopi;
		while( d > pi )
			d-= twopi;

		if( d>turnspd )
			d=turnspd;
		if( d<-turnspd )
			d=-turnspd;
		TurnBy(d);

		// move
		Forward( 1.0f );
		const float r = g_CurrentLevel->ArenaRadius();
		if( Pos().LenSq() >= r*r )
			m_Attached = true;	// hit edge!
	}

	m_Flash *= 0.8f;
}



void EdgePuller::Draw()
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
		glVertex2f( -16.0f, 0.0f );

		glColor3f( c.r, c.g, c.b );
		glVertex2f( 16.0f, 0.0f );

		glColor3f( c.r, c.g, c.b );
		glVertex2f( 0.0f, 16.0f );
	glEnd();
}


void EdgePuller::OnHitBullet( Bullet& bullet )
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

