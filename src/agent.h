

#ifndef AGENT_H
#define AGENT_H

#include "mathutil.h"
#include "mempool.h"
#include "vec2.h"
#include <list>
#include <assert.h>


// Agent is the base class for all game objects.


class Player;
class Bullet;


void Agent_Startup();
void Agent_Shutdown();


class Agent
{
public:
	Agent( bool worldspace=false );
	virtual ~Agent();

	// reference counting
	void Lock();
	void Unlock();

	vec2 Pos() const;
	float Heading() const;

	void MoveTo( vec2 const& pos );
	void MoveBy( vec2 const& distance );
	void Forward( float dist );

	// position agent relative to another
	void PositionRelative( Agent const& parent, vec2 const& relpos, float relheading );
	
	// angles are in radians
	void TurnTo( float h );
	void TurnBy( float angle );
	void TurnToward( vec2 const& target, float turnspd );

	// Add vel onto position, taking into account the arena boundary.
	// If collision occured, true is returned and vel is revised.
	bool MoveWithinArena( Agent& a, vec2& vel );

	// these two must be overridden to provide agent behaviour and rendering
	virtual void Draw()=0;
	virtual void Tick()=0;

	// is agent still alive?
	bool Alive() const;

	// should this agent be rendered in world space?
	bool WorldSpace() const;

	// flag Agent as deceased
	void Die();


	// circle used for collision
	void SetRadius( float r );
	float Radius() const;	// should be protected/private?

	// is this agent touching the other one?
	bool Touching( Agent const& other ) const;

	// collision response (optional overrides)
//	virtual void OnHitPlayer( Player& player );
//	virtual void OnHitBullet( Bullet& bullet );

	
	// custom memory management to pool agents
	void *operator new( size_t numbytes );
	void operator delete( void* mem);

protected:
	// unkill the agent - used (for example) when the player respawns after losing a life
	void UnDie();

	// place agent at random position within arena, but with a safe-zone
	// right in the middle where the player can safely spawn...
	void RandomPos();

private:
	int m_LockCount;

	vec2 m_Pos;
	float m_Heading;	// 0=north, increasing clockwise
	float m_Radius;		// collision radius
	bool m_Alive;
	bool m_WorldSpace;	// render in worldspace?
};

typedef std::list< Agent* > AgentList;



//inlines

inline Agent::Agent( bool worldspace ) :
	m_LockCount( 0 ),
	m_Pos( vec2::ZERO ),
	m_Heading( 0.0f ),
	m_Radius( 0.0f ),
	m_Alive(true),
	m_WorldSpace( worldspace )
{
}

inline void Agent::Lock()
	{ ++m_LockCount; }

inline bool Agent::WorldSpace() const				{ return m_WorldSpace; }
inline bool Agent::Alive() const					{ return m_Alive; }
inline void Agent::Die()							{ m_Alive=false; }
inline void Agent::UnDie()							{ m_Alive=true; }
inline void Agent::MoveTo( vec2 const& pos )		{ m_Pos = pos; }
inline void Agent::MoveBy( vec2 const& distance )	{ m_Pos += distance; }
inline vec2 Agent::Pos() const						{ return m_Pos; }
inline void Agent::TurnTo( float h )				{ m_Heading = h; }
inline float Agent::Heading() const					{ return m_Heading; }
inline float Agent::Radius() const					{ return m_Radius; }
inline void Agent::SetRadius( float r )				{ m_Radius=r; }

inline void Agent::Forward( float dist )
{
	m_Pos.x += dist * (float)sin( m_Heading );
	m_Pos.y += dist * (float)cos( m_Heading );
}

inline void Agent::TurnBy( float angle )
{
	assert( angle < twopi && angle > -twopi );
//	m_Heading = (float)fmod( m_Heading+angle, (float)(2.0*M_PI) );
	m_Heading += angle;
	while( m_Heading > twopi )
		m_Heading -= twopi;
	while( m_Heading <0.0f )
		m_Heading += twopi;

}

inline bool Agent::Touching( Agent const& other ) const
{
	float distsq = ( other.Pos() - Pos() ).LenSq();
	float r=other.Radius()+Radius();
	return ( distsq <= r*r );
}



#endif // AGENT_H
