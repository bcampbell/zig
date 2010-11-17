

#ifndef BULLETS_H
#define BULLETS_H

#include "agent.h"
#include "player.h"

#include <list>

class Dude;


// bullet is base class for stuff fired from player weapons
class Bullet : public Agent
{
public:
	virtual ~Bullet()							{}
	virtual void Draw()=0;
	virtual void Tick()=0;
	Player& Owner()								{ return m_Owner; }

	int Power() const							{ return m_Power; }
	void ReducePower( int d )					{ m_Power -= d; if( m_Power<=0 ) Die(); }
protected:
	// only to be invoked by derived classes
	Bullet( Player& owner, int power, vec2 const& relpos, float relheading );
private:
	Player& m_Owner;
	int m_Power;
};


typedef std::list< Bullet* > BulletList;



// bog standard laser
class Laser : public Bullet
{
public:
	Laser( Player& owner, vec2 const& relpos, float relheading );
	virtual void Draw();
	virtual void Tick();
private:
	int m_Timer;
};



//
class FatLaser : public Bullet
{
public:
	FatLaser( Player& owner, vec2 const& relpos, float relheading );
	virtual void Draw();
	virtual void Tick();
private:
	int m_Timer;

	static const float s_FwdSpd;
	static const int s_Duration;
};

// debris left behind laser
class LaserCrud : public Agent
{
public:
	LaserCrud( vec2 const& pos, float heading );
	virtual void Draw();
	virtual void Tick();
private:
	float m_Life;
	float m_LifeDecay;
};


#endif	// BULLETS_H

