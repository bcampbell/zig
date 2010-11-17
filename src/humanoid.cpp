

#include "humanoid.h"

#include <assert.h>
#include <SDL.h>
#include <SDL_opengl.h>


Humanoid::Humanoid()
{
	SetRadius(5.0f);
	SetFlags( flagCanHitPlayer );
	Respawn();
}

void Humanoid::Respawn()
{
	RandomPos();
	m_Spd = vec2::ZERO;	
}


void Humanoid::Draw()
{
	glDisable( GL_BLEND );
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glBegin( GL_QUADS );
		glVertex2f( 5, 10 );
		glVertex2f( 5, 4 );
		glVertex2f( -5, 4 );
		glVertex2f( -5, 10 );
	glEnd();
	glBegin( GL_LINES );
		glVertex2f( 0, 4 );
		glVertex2f( 0, -4 );
		
		glVertex2f( 8, 0 );
		glVertex2f( -8, 0 );

		glVertex2f( 0, -4 );
		glVertex2f( 5, -10 );

		glVertex2f( 0, -4 );
		glVertex2f( -5, -10 );
	glEnd();
}

void Humanoid::Tick()
{
	TurnBy( pi/32.0f );
}

void Humanoid::OnHitPlayer( Player& plr )
{
	Die();
}

