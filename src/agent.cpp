


#include "agent.h"
#include "zig.h"
#include "level.h"


// memory management to allow us to create and destroy agents at a great rate...
static MemPool* s_AgentPool = 0;


void Agent_Startup()
{
	s_AgentPool = new MemPool( 256, 500 );
}

void Agent_Shutdown()
{
	delete s_AgentPool;
}


void* Agent::operator new( size_t numbytes )
{
	assert( s_AgentPool != 0 );
	return s_AgentPool->Allocate( numbytes );
}

void Agent::operator delete( void* mem)
{
	assert( s_AgentPool != 0 );
	s_AgentPool->Free( mem );
}


void Agent::Unlock()
{
	assert( m_LockCount > 0 );	// not locked!
	--m_LockCount;
	if( m_LockCount == 0 )
		delete this;			// looks evil, but really should be fine
}


//virtual
Agent::~Agent()
{
	assert( m_LockCount == 0 );	// uh-oh - we've got an outstanding lock...
}


void Agent::TurnToward( vec2 const& target, float turnspd )
{
	vec2 v = target-Pos();
	float theta = atan2(v.x,v.y);

	float d = theta-Heading();

	// reduce to range (-pi..pi)
	while( d < -pi )
		d+= twopi;
	while( d > pi )
		d-= twopi;

	// clip
	if( d>turnspd )
		d=turnspd;
	if( d<-turnspd )
		d=-turnspd;
	TurnBy(d);
}




void Agent::RandomPos()
{
	float maxdist = g_CurrentLevel->ArenaRadius() - Radius();
	float theta = Rnd( 0.0f, (float)(M_PI*2.0f) );
	// don't spawn in inner 1/3rd (wouldn't be cricket)
	float r = Rnd( maxdist/3.0f, maxdist );
	MoveTo( vec2( r*sin(theta), r*cos(theta) ) );

	// head toward 0,0
	TurnTo( (float)fmod( theta+M_PI, M_PI*2.0f ) );
}



bool Agent::MoveWithinArena( Agent& a, vec2& vel )
{
	bool boing=false;
	float r = g_CurrentLevel->ArenaRadius()-a.Radius();

	// if already outside arena, warp inside (no boing)
	// this allows arena to push dudes about.
	if( a.Pos().LenSq() > r*r )
	{
		vec2 corrected( a.Pos() );
		corrected.Normalise();
		corrected *= r;
		a.MoveTo( corrected );
	}

	// apply velocity, check for collisions
	vec2 newpos=a.Pos() + vel;
	if( newpos.LenSq() > r*r )
	{
		boing=true;
		// boing.
		vec2 normal( -a.Pos() );
		normal.Normalise();
		vec2 incident = vel;
		incident.Normalise();

		vec2 reflect =
			2.0f*Dot(-incident,normal)*normal + incident;
		vel = vel.Len() * reflect;

		// reposition within boundary
		newpos = -normal * r;
		//a.Pos() * (a.Pos().Len()/r);
	}
	a.MoveTo(newpos);
	return boing;
}


void Agent::PositionRelative( Agent const& parent, vec2 const& relpos, float relheading )
{
	MoveTo( parent.Pos() + Rotate( relpos, parent.Heading() ) );
	TurnTo( parent.Heading() + relheading );
}

