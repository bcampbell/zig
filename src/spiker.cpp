#include <SDL_opengl.h>
#include "agentmanager.h"
#include "colours.h"
#include "player.h"
#include "spiker.h"
#include "zig.h"

//--------------------
// Spiker
//--------------------

Spiker::Spiker() : m_Cyc(0.0f), m_Life(2)
{
	SetRadius(16.0f);
	SetFlags( flagCanHitBullet|
		flagCanHitPlayer|
		flagLocksLevel );
	Respawn();
	RandomPos();
}

void Spiker::Respawn()
{
	RandomPos();
	m_Spd = vec2::ZERO;
	m_Acceleration = Rnd( 0.02f, 0.04f );
	m_Flash = 0.0f;
}


void Spiker::Draw()
{
	static const Colour pulsecolours[] =
	{
		Colour( 0.3f, 0.0f, 0.0f ),
		Colour( 0.2f, 0.0f, 0.0f ),
		Colour( 0.2f, 0.0f, 0.0f ),
		Colour( 0.3f, 0.0f, 0.0f ),
		Colour( 0.4f, 0.0f, 0.0f ),
		Colour( 0.8f, 0.5f, 0.0f ),
		Colour( 0.4f, 0.0f, 0.0f ),
	};
	static const ColourRange pulserange( pulsecolours, sizeof(pulsecolours)/sizeof(Colour) );

	Colour c1 = ColourLerp( Colour( 0.6f, 0.0f, 0.0f ), Colour::WHITE, m_Flash );
	Colour c2 = ColourLerp( Colour( 1.0f, 0.7f, 0.3f ), Colour::WHITE, m_Flash );

	m_Cyc += 0.01f;
	Colour c3 = ColourLerp( pulserange.Get( m_Cyc, false ), Colour::WHITE, m_Flash );

	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );
	glBegin( GL_TRIANGLES );
		glColor3f( c1.r, c1.g, c1.b );
		glVertex2f( -4.0f, 4.0f );
		glColor3f( c2.r, c2.g, c2.b );
		glVertex2f( 0.0f, 16.0f );
		glColor3f( c1.r, c1.g, c1.b );
		glVertex2f( 4.0f, 4.0f );

		glColor3f( c1.r, c1.g, c1.b );
		glVertex2f( 4.0f, 4.0f );
		glColor3f( c2.r, c2.g, c2.b );
		glVertex2f( 16.0f, 0.0f );
		glColor3f( c1.r, c1.g, c1.b );
		glVertex2f( 4.0f, -4.0f );
		
		glColor3f( c1.r, c1.g, c1.b );
		glVertex2f( -4.0f, -4.0f );
		glColor3f( c2.r, c2.g, c2.b );
		glVertex2f( 0.0f, -16.0f );
		glColor3f( c1.r, c1.g, c1.b );
		glVertex2f( 4.0f, -4.0f );

		glColor3f( c1.r, c1.g, c1.b );
		glVertex2f( -4.0f, 4.0f );
		glColor3f( c2.r, c2.g, c2.b );
		glVertex2f( -16.0f, 0.0f );
		glColor3f( c1.r, c1.g, c1.b );
		glVertex2f( -4.0f, -4.0f );
	
		// centre
		glColor3f( c3.r, c3.g, c3.b );
		glVertex2f( -4.0f, -4.0f );
		glColor3f( c3.r, c3.g, c3.b );
		glVertex2f( 4.0f, -4.0f );
		glColor3f( c3.r, c3.g, c3.b );
		glVertex2f( 4.0f, 4.0f );

		glColor3f( c3.r, c3.g, c3.b );
		glVertex2f( -4.0f, -4.0f );
		glColor3f( c3.r, c3.g, c3.b );
		glVertex2f( 4.0f, 4.0f );
		glColor3f( c3.r, c3.g, c3.b );
		glVertex2f( -4.0f, 4.0f );

	glEnd();
}

void Spiker::Tick()
{
	MoveBy( m_Spd );
	TurnBy( pi/128.0f );

	vec2 accel = g_Player->Pos() - Pos();
	accel.Normalise();
	accel *= m_Acceleration;	//0.02f;

	m_Spd += accel;
	m_Spd *= 0.98f;

	m_Flash *= 0.8f;
}

void Spiker::OnHitBullet( Bullet& bullet )
{
	--m_Life;
	if( m_Life <= 0 )
	{
	
		StandardDeath( bullet, 100, 0 );

		g_Agents->AddDude( new Missile( Pos(), Heading() ) );
		g_Agents->AddDude( new Missile( Pos(), Heading() + pi/2.0f ) );
		g_Agents->AddDude( new Missile( Pos(), Heading() + pi ) );
		g_Agents->AddDude( new Missile( Pos(), Heading() + 3.0f*pi/2.0f ) );
		Die();
	}
	else
	{
		m_Flash = 1.0f;
	}
}


