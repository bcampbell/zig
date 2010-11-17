#ifndef EDGEPULLER_H
#define EDGEPULLER_H

#include "dudes.h"
#include "vec2.h"


class EdgePuller : public Dude
{
public:
	EdgePuller();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new EdgePuller() ); }
private:
	EdgePuller( EdgePuller const& );	// disable copy ctor
//	float	m_Timer;
	int		m_Life;
	float	m_Flash;
	float	m_TargetHeading;
	bool	m_Attached;
};



#endif // EDGEPULLER_H

