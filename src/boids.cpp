#include "agentmanager.h"
#include "boids.h"
#include "bullets.h"
#include "zig.h"
#include "level.h"

#include <SDL_opengl.h>

Boid::Boid()
{
	SetRadius(10.0f);
	SetFlags( flagCanHitBullet | flagCanHitPlayer | flagLocksLevel );
	RandomPos();
	m_Vel = vec2::ZERO;
}


void Boid::Draw()
{
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );
	glColor3f( 0.2f, 0.2f, 1.0f );
	glBegin( GL_TRIANGLES );
		glVertex2f( 0.0f, 8.0f );
		glVertex2f( 8.0f, -8.0f );
		glVertex2f( -8.0f, -8.0f );
	glEnd();
}


void Boid::OnHitBullet( Bullet& bullet )
{
	StandardDeath( bullet, 20 );
	Die();
	bullet.ReducePower(1);
}


void Boid::Respawn()
{
	RandomPos();
}


void Boid::Tick()
{
	const float rangesq = 50.0f*50.0f;

	std::vector< const Boid* > neighbours;
	neighbours.reserve(100);
	DudeList& dudes = g_Agents->Dudes();
	DudeList::const_iterator it;
	vec2 const& pos = Pos();
	for( it=dudes.begin(); it!=dudes.end(); ++it )
	{
		if( *it == this )
			continue;
		vec2 d = (*it)->Pos()-pos;
		if( d.LenSq() < rangesq )
		{
			// DANGER DANGER!!! (need to sort out better casting!)
			neighbours.push_back( (const Boid*)*it );
		}
	}

	vec2 groupingacc = CalcGroupingAcc( neighbours );
	vec2 velmatchacc = CalcVelMatchAcc( neighbours );
	// TODO: separation!

	vec2 attackacc = g_Player->Pos() - Pos();
	if( attackacc.LenSq() < 80*80 )
		attackacc.Normalise();
	else
		attackacc = vec2::ZERO;

	vec2 avoidboundaryacc = vec2::ZERO;
	float bound = g_CurrentLevel->ArenaRadius()-20.0f;
	if( Pos().LenSq() > bound*bound )
	{
		avoidboundaryacc = -Pos();
		avoidboundaryacc.Normalise();
		avoidboundaryacc *= 0.1f;
	}

	vec2 acc = 0.3f*groupingacc + 0.6f*velmatchacc + 0.1f*attackacc + 0.4f*avoidboundaryacc;
	m_Vel += acc;

	const float maxvel=1.0f;
	if( m_Vel.LenSq() > maxvel )
	{
		m_Vel.Normalise();
		m_Vel *= maxvel;
	}

	MoveBy(m_Vel);

	TurnTo( atan2(m_Vel.x,m_Vel.y) );
}


vec2 Boid::CalcGroupingAcc( std::vector< const Boid* > neighbours )
{
	vec2 centre = vec2::ZERO;
	if( neighbours.size() <= 0 )
		return centre;

	std::vector< const Boid* >::const_iterator it;
	vec2 const& pos = Pos();
	for( it=neighbours.begin(); it!=neighbours.end(); ++it )
		centre += (*it)->Pos()-pos;

	centre /= (float)neighbours.size();

	vec2 acc = centre;
	if( acc.LenSq() > 0.1f*0.1f )	// avoid divide-by-zero
	{
		acc.Normalise();
		acc *= 0.1f;
	}

	return acc;
}


vec2 Boid::CalcVelMatchAcc( std::vector< const Boid* > neighbours )
{
	if( neighbours.size() <= 0 )
		return vec2::ZERO;

	vec2 vel = vec2::ZERO;
	std::vector< const Boid* >::const_iterator it;
	for( it=neighbours.begin(); it!=neighbours.end(); ++it )
	{
		vel += ((Boid*)(*it))->m_Vel;
	}
	vel /= (float)neighbours.size();

	vec2 acc = vel;
	if( acc.LenSq() > 0.1f*0.1f )	// avoid divide-by-zero
	{
		acc.Normalise();
		acc *= 0.1f;
	}

	return acc;
}


