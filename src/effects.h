

#ifndef EFFECTS_H
#define EFFECTS_H

#include <list>
#include <string>
#include "colours.h"
#include "dudes.h"

class Player;
class Bullet;
class ColourCycle;



class Frag : public Dude
{
public:
	Frag( vec2 const& pos );
	virtual void Draw();
	virtual void Tick();
private:
	float m_Spd;
	float m_Life;
	float m_LifeDecay;
};


class BlastWave : public Dude
{
public:
	BlastWave( vec2 const& pos );
	~BlastWave();
	virtual void Draw();
	virtual void Tick();
private:
	float m_Radius;
	float m_ExpandRate;
	ColourCycle* m_InnerColours;
	ColourCycle* m_OuterColours;
};


class DudeSpawnEffect : public Dude
{
public:
	DudeSpawnEffect( Dude* d );
	~DudeSpawnEffect();
	virtual void Draw();
	virtual void Tick();
private:
	Dude* m_Dude;
	float m_Timer;
};


class FadeText : public Dude
{
public:
	FadeText( vec2 const& pos, std::string const& text,float size=16.0f, float life=0.4f, float delay=0.0f );
	virtual void Draw();
	virtual void Tick();
private:
	std::string m_Text;
	float m_Delay;
	float m_Life;
	float m_Size;
	float m_Timer;
};


class Exhaust : public Dude
{
public:
	Exhaust( Agent const& emitter, vec2 const& basespd );
	virtual void Draw();
	virtual void Tick();
private:
	float m_Life;
	vec2 m_Spd;
};


class Streamer : public Agent
{
public:
	Streamer( vec2 const& pos, float heading, float turnaccel );
	virtual void Draw();
	virtual void Tick();
private:
	Streamer();
	Streamer( const Streamer& );
	std::deque< vec2 > m_Trail;
	float m_TurnSpd;
	float m_TurnAccel;
};

#endif	// EFFECTS_H


