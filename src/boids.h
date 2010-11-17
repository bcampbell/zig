#ifndef BOIDS_H
#define BOIDS_H

#include "dudes.h"

class Boid : public Dude
{
public:
	Boid();
	Boid( vec2 const& pos );
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new Boid() ); }
private:
	vec2 m_Vel;
	vec2 CalcGroupingAcc( std::vector< const Boid* > neighbours );
	vec2 CalcVelMatchAcc( std::vector< const Boid* > neighbours );
};

#endif // BOIDS_H


