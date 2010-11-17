

#include <SDL_opengl.h>
#include <assert.h>
#include <math.h>

#include "agentmanager.h"
#include "zig.h"


AgentManager::AgentManager() : m_LockCount(0), m_DestructibleCount(0)
{
}



AgentManager::~AgentManager()
{
	while( !m_Dudes.empty() )
	{
		m_Dudes.back()->Unlock();
		m_Dudes.pop_back();
	}
	
	while( !m_Bullets.empty() )
	{
		m_Bullets.back()->Unlock();
		m_Bullets.pop_back();
	}
	
	while( !m_Underlays.empty() )
	{
		m_Underlays.back()->Unlock();
		m_Underlays.pop_back();
	}
}


void AgentManager::AddDude( Dude* dude )
{
	dude->Lock();
	m_Dudes.push_back( dude);

	// update counts
	const int flags = dude->Flags();
	if( !( flags & Dude::flagIndestructible ) )
		++m_DestructibleCount;
	if( flags & Dude::flagLocksLevel )
		++m_LockCount;
}



void AgentManager::Tick()
{
	if( g_Player->Alive() )
		g_Player->Tick();

	BulletList::iterator bulletit;
	bulletit=m_Bullets.begin();
	while( bulletit!=m_Bullets.end() )
	{
		BulletList::iterator nextbullet = bulletit;
		++nextbullet;
		if( (*bulletit)->Alive() ) 
			(*bulletit)->Tick();
		else
		{
			(*bulletit)->Unlock();
			m_Bullets.erase(bulletit);
		}
		bulletit = nextbullet;
	}



	AgentList::iterator underlayit;
	underlayit=m_Underlays.begin();
	while( underlayit!=m_Underlays.end() )
	{
		AgentList::iterator next = underlayit;
		++next;
		if( (*underlayit)->Alive() ) 
			(*underlayit)->Tick();
		else
		{
			(*underlayit)->Unlock();
			m_Underlays.erase(underlayit);
		}
		underlayit = next;
	}


	DudeList::iterator dudeit;
	dudeit=m_Dudes.begin();
	while( dudeit!=m_Dudes.end() )
	{
		DudeList::iterator next = dudeit;
		++next;

		Dude& dude = *(*dudeit);
		if( dude.Alive() )
			dude.Tick();

		if( dude.Alive() )
		{
	
			// Also check for collisions while we're at it.
			if( (dude.Flags() & Dude::flagCanHitPlayer) &&
				g_Player->Alive() &&
				dude.Touching( *g_Player ) )
			{
				dude.OnHitPlayer( *g_Player );
				// dude will call Player::FatalDudeCollision() if needed.
			}

			if(dude.Flags() & Dude::flagCanHitBullet)
			{
				BulletList::const_iterator bulletit;
					for( bulletit= m_Bullets.begin();
					bulletit != m_Bullets.end() && dude.Alive();
					++bulletit )
				{
					if( (*bulletit)->Alive() )
					{
						if( dude.Touching( *(*bulletit) ) )
						{
							dude.OnHitBullet( *(*bulletit) );
						}
					}
				}
			}
		}

		if( !dude.Alive() )
		{	
			// update counts
			const int flags = dude.Flags();
			if( !( flags & Dude::flagIndestructible ) )
				--m_DestructibleCount;
			if( flags & Dude::flagLocksLevel )
				--m_LockCount;

			(*dudeit)->Unlock();
			m_Dudes.erase(dudeit);
		}
		dudeit = next;
	}
}



void AgentManager::Render()
{
	{
		AgentList::iterator it;
		for( it=m_Underlays.begin(); it!=m_Underlays.end(); ++it )
		{
			if( (*it)->Alive() )
				DrawAgent(*(*it));
		}
	}

	{
		BulletList::iterator it;
		for( it=m_Bullets.begin(); it!=m_Bullets.end(); ++it )
		{
			if( (*it)->Alive() )
				DrawAgent(*(*it));
		}
	}
	
	if( g_Player->Alive() )
		DrawAgent( *g_Player );

	{
		DudeList::iterator it;
		for( it=m_Dudes.begin(); it!=m_Dudes.end(); ++it )
		{
			if( (*it)->Alive() )
				DrawAgent(*(*it));
		}
	}
}



// helper for Render()
void AgentManager::DrawAgent( Agent& a )
{
	if( a.WorldSpace() )
	{
		a.Draw();
	}
	else
	{
		glPushMatrix();
		glTranslatef( a.Pos().x, a.Pos().y, 0.0f );
		glRotatef( r2d( -a.Heading() ), 0.0f, 0.0f, 1.0f );
		a.Draw();
		glPopMatrix();
	}
}

/*
int AgentManager::LockCount()
{
	int lockcount=0;

	DudeList::const_iterator dudeit;
	for( dudeit=m_Dudes.begin();
		dudeit!=m_Dudes.end();
		++dudeit )
	{
		if( (*dudeit)->Flags() & Dude::flagLocksLevel )
			++lockcount;
	}

	return lockcount;
}
*/

void AgentManager::Restart()
{
	g_Player->Resurrect();

	DudeList::iterator dudeit;
	for( dudeit=m_Dudes.begin(); dudeit!=m_Dudes.end(); ++dudeit )
		(*dudeit)->Respawn();

	while( !m_Bullets.empty() )
	{
		m_Bullets.back()->Unlock();
		m_Bullets.pop_back();
	}

	while( !m_Underlays.empty() )
	{
		m_Underlays.back()->Unlock();
		m_Underlays.pop_back();
	}

}

