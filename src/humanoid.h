

#ifndef HUMANOID_H
#define HUMANOID_H

#include "dudes.h"

class Humanoid : public Dude
{
public:
	Humanoid();
	virtual void Draw();
	virtual void Tick();
	virtual void OnHitPlayer( Player& plr );
	virtual void Respawn();
private:
	vec2 m_Spd;
};

#endif // HUMANOID_H

