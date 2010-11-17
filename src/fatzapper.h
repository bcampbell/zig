#ifndef FATZAPPER_H
#define FATZAPPER_H

#include "dudes.h"
#include "vec2.h"
#include <deque>


class FatZapper : public Dude
{
public:
	FatZapper();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new FatZapper() ); }
private:
	static const float ChargeTime;
	static const float WobbleTime;
	static const float MinRadius;
	static const float MaxRadius;
	
	FatZapper( FatZapper const& );	// disable copy ctor
	float m_Timer;
	int m_Life;
	vec2 m_Spd;
	float m_Flash;
};



class FatRay : public Dude
{
public:
	FatRay( vec2 const& pos, float heading );
	virtual void Draw();
	virtual void Tick();
	virtual void Respawn();
private:
	FatRay();					// disable
	FatRay( FatRay const& );	// disable copy ctor
	float m_Elapsed;

	static const unsigned int MaxLen;
	static const float LifeTime;
	static const float FadeTime;
	static const float BeamSpeed;
	static const float BeamWidth;

	struct Edge
	{
		vec2 p0;
		vec2 p1;
	};
	std::deque< Edge > m_Trail;

};


#endif // FATZAPPER_H

