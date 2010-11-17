

#ifndef BONUSES_H
#define BONUSES_H

#include "dudes.h"

#define BONUSTIMEOUT 200


extern void SpawnBonusMaybe( vec2 const& pos );


class Bonus : public Dude
{
protected:
	Bonus( vec2 const& pos );
public:
	virtual void Tick();
	virtual void Respawn();
private:
	Bonus();
	int m_Timeout;
};


class PointsBonus : public Bonus
{
public:
	PointsBonus( vec2 const& pos );
	virtual void Draw();
	virtual void OnHitPlayer( Player& player );
private:
	PointsBonus();
};

class ExtraLife : public Bonus
{
public:
	ExtraLife( vec2 const& pos );
	virtual void Draw();
	virtual void OnHitPlayer( Player& player );
private:
	ExtraLife();
};


class WeaponPowerup : public Bonus
{
public:
	WeaponPowerup( vec2 const& pos );
	virtual void Draw();
	virtual void OnHitPlayer( Player& player );
private:
	WeaponPowerup();
};


class SuperNashwanBonus : public Bonus
{
public:
	SuperNashwanBonus( vec2 const& pos );
	virtual void Draw();
	virtual void OnHitPlayer( Player& player );
private:
	SuperNashwanBonus();
};


class Banana : public Dude
{
public:
	Banana( vec2 const& pos );
	virtual void Tick();
	virtual void Draw();
	virtual void OnHitPlayer( Player& player );
};


#endif	// BONUSES_H


