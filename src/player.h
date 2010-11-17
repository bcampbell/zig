

#ifndef PLAYER_H
#define PLAYER_H

#include "agent.h"


class Controller;
class Dude;
class Weapon;

class Player : public Agent
{
public:
	Player( bool autopilot=false );
	virtual ~Player();
	virtual void Draw();
	virtual void Tick();

	void FatalDudeCollision( Dude& dude );

	void GivePoints( int pts );
	void GiveExtraLife();
	void GiveWeaponPowerup();
	void GiveSuperNashwan();

	int Score() const;
	int SpareLives() const;

	void Reset();		// for new level
	void Resurrect();	// after losing a life

	enum { MAX_WEAPON_LEVEL=2 };

	int WeaponNum() const
		{ return m_WeaponNum; }
private:
	
	void SetWeapon( int w );

	void ApplyVelocity();
	
	Controller*	m_Controller;

	float m_RotSpd;
	vec2 m_Vel;

	int m_Score;
	int m_SpareLives;
	
	int m_FireTimer;
	int m_WeaponNum;
	Weapon*	m_Weapon;
	Weapon* m_Nashwan;
};


#endif


