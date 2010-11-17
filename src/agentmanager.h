

#ifndef AGENTMANAGER_H
#define AGENTMANAGER_H

#include <list>
#include "zig.h"

#include "agent.h"
#include "bullets.h"
#include "dudes.h"


class AgentManager
{
public:
	AgentManager();
	~AgentManager();
	void AddDude( Dude* dude );
	void AddBullet( Bullet* bullet );
	void AddUnderlay( Agent* effect );

	void Tick();
	void Render();

	void Restart();

	DudeList& Dudes()					{ return m_Dudes; }

	// only valid after Tick()
	int LockCount() const				{ return m_LockCount; }
	int DestructibleCount() const		{ return m_DestructibleCount; }
private:
	void DrawAgent( Agent& a );

	DudeList	m_Dudes;
	AgentList	m_Underlays;
	BulletList	m_Bullets;

	int m_LockCount;
	int m_DestructibleCount;
};





inline void AgentManager::AddBullet( Bullet* bullet )
{
	bullet->Lock();
	m_Bullets.push_back( bullet );
}

inline void AgentManager::AddUnderlay( Agent* effect )
{
	effect->Lock();
	m_Underlays.push_back( effect );
}


#endif // AGENTMANAGER_H


