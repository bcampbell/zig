#ifndef SPIKER_H
#define SPIKER_H

#include "dudes.h"

class Spiker : public Dude
{
public:
	Spiker();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new Spiker() ); }
    static void StaticDraw(float flash);
private:
	vec2	m_Spd;
	float	m_Acceleration;
	int		m_Life;
	float	m_Flash;
};


#endif // SPIKER_H


