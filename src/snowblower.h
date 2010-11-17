#ifndef SNOWBLOWER_H
#define SNOWBLOWER_H

#include "dudes.h"


class SnowBlower : public Dude
{
public:
	SnowBlower();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new SnowBlower() ); }
private:
	static const float s_OffTime;
	static const float s_OnTime;

	SnowBlower( SnowBlower const& );	// disable copy ctor
	int		m_Life;
	float	m_Flash;
	float	m_Timer;
	enum { OFF, ON } m_State;
};



#endif // SNOWBLOWER_H

