#ifndef BOSS_H
#define BOSS_H

#include "dudes.h"
#include <list>

class BeamPod;

class Boss : public Dude
{
public:
	Boss();
	virtual ~Boss();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	static void Create( std::list<Dude*>& newdudes )
		{ newdudes.push_back( new Boss() ); }
private:
	void PositionBeamPods();
	enum State { NONE, WANDER, BEAMS, SPIN, BERSERK };

	State m_State;
	float m_Timer;
	int m_Life;
	float m_Flash;

	float m_AnimTime;

	State m_NextAttack;	// BEAMS or SPIN
	bool m_AddedPods;
	
	void EnterState( State newstate );

	BeamPod* m_BeamPods[4];
};


class BeamPod : public Dude
{
public:
	BeamPod();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitBullet( Bullet& bullet );
	virtual void Respawn();
	void Fire();
private:
	int m_Life;
	float m_Flash;
};


#endif // BOSS_H

