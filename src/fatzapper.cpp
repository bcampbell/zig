#include <SDL_opengl.h>

#include "agentmanager.h"
#include "colours.h"
#include "effects.h"
#include "fatzapper.h"
#include "zig.h"


const float FatZapper::ChargeTime = 4.0f;
const float FatZapper::WobbleTime = 2.0f;
const float FatZapper::MinRadius = 8.0f;
const float FatZapper::MaxRadius = 20.0f;


FatZapper::FatZapper() :
	m_Life( 20 )
{
	SetFlags( flagCanHitBullet| flagCanHitPlayer| flagLocksLevel );
	Respawn();
}

void FatZapper::Respawn()
{
	SetRadius(MinRadius);
	m_Timer = Rnd( 0.0f, ChargeTime/6.0f );	// to stagger multiple fatzappers
	m_Flash = 0.0f;
	m_Spd = Rotate( vec2(0.0f,1.0f), Rnd(0.0f,twopi) );
	RandomPos();
}

void FatZapper::Tick()
{
	float prevtime = m_Timer;
	m_Timer += 1.0f/(float)TARGET_FPS;


	
	float charge = m_Timer/ChargeTime;
	if( charge>=1.0f )
		charge = 1.0f;

	// calc radius
	float targetradius = MinRadius + (MaxRadius-MinRadius)*charge*charge;
	SetRadius( Radius() + (targetradius-Radius())*0.2f );

	if( m_Timer >= ChargeTime && prevtime < ChargeTime )
	{
		SoundMgr::Inst().Play( SFX_BAITERALERT );
	}
	
	if( m_Timer >= ChargeTime+WobbleTime )
	{
		//fire!
		m_Timer = 0.0f;
		g_Agents->AddDude( new FatRay(Pos(),Heading() ));
		SoundMgr::Inst().Play( SFX_PLAYERTOAST );
	}

	vec2 playerpos = g_Player->Pos();
	TurnToward( playerpos, twopi/256.0f );

	m_Flash *= 0.8f;

	
	MoveWithinArena( *this, m_Spd );
}



void FatZapper::Draw()
{

	float wobble = (m_Timer-ChargeTime) / WobbleTime;
	if( wobble < 0.0f )
		wobble = 0.0f;

	Colour c1 = ColourLerp(
			Colour(0.0f, 0.0f, 0.0f),
			Colour( 1.0f, 1.0f, 0.4f ),
			wobble );
	c1 = ColourLerp( c1, Colour::WHITE, m_Flash );

	Colour c2 = ColourLerp(
			Colour(0.0f, 0.8f, 0.0f),
			Colour( 0.6f, 1.0f, 0.4f ),
			wobble );
	c2 = ColourLerp( c2, Colour::WHITE, m_Flash );


	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_SMOOTH );
	glBegin( GL_TRIANGLE_FAN );
	glColor3f( c1.r, c1.g, c1.b );
	glVertex2f( 0.0f, 0.0f );
	float theta;
	float gap = wobble * twopi/4.0f;
//	if( gap < twopi/128.0f )
//		gap = twopi/128.0f;
	for( theta=gap/2.0f;
		theta<=(twopi-gap/2.0f)+0.001f;
		theta += twopi/128.0f )
	{
		float r = Radius();
		r += wobble*4.0f*sin( theta*wobble*16.0f);
		vec2 v = Rotate( vec2(0.0f,r), theta );
		glColor3f( c2.r, c2.g, c2.b );
		glVertex2f( v.x, v.y );
	}

	glEnd();
}


void FatZapper::OnHitBullet( Bullet& bullet )
{
	int absorbed = std::min( bullet.Power(), m_Life );
	m_Life -= absorbed;
	if( m_Life <= 0 )
	{
		BigArseDeath( bullet, 100 );
		Die();
	}
	else
	{
		int i;
		for(i=0;i<5;++i)
			g_Agents->AddDude(new Frag(Pos()));
		m_Flash = 1.0f;

		vec2 impulse = Rotate( vec2(0,0.1f), bullet.Heading() );
		m_Spd += impulse * 4.0f;
	}

	bullet.ReducePower( absorbed );
}


//---------------------------

const unsigned int FatRay::MaxLen = 16;
const float FatRay::LifeTime = 0.8f;
const float FatRay::FadeTime = 0.2f;
const float FatRay::BeamSpeed = 32.0f;
const float FatRay::BeamWidth = 16.0f;

FatRay::FatRay( vec2 const& pos, float heading ) :
	Dude( true ),
	m_Elapsed( 0.0f )
{
	MoveTo(pos);
	TurnTo(heading);

	SetRadius( BeamWidth*0.75 );
	SetFlags( flagCanHitPlayer );

	// start trail
	Edge e;
	e.p0 = Pos() + Rotate( vec2(BeamWidth/2.0f,0.0f), Heading() );
	e.p1 = Pos() + Rotate( vec2(-BeamWidth/2.0f,0.0f), Heading() );
	m_Trail.push_back( e );
}

void FatRay::Respawn()
{
	Die();
}

void FatRay::Draw()
{
	static const Colour raycolours[] =
	{
		Colour( 1.0f, 1.0f, 1.0f, 1.0f ),
		Colour( 0.0f, 0.0f, 1.0f, 1.0f ),
		Colour( 0.0f, 0.0f, 1.0f, 0.0f ),
	};
	static const ColourRange rayrange( raycolours, 3 );

	std::deque<Edge>::reverse_iterator it;

	if( m_Trail.size() < 2 )
		return;

	float fade = (m_Elapsed-(LifeTime-FadeTime))/FadeTime;
	if( fade < 0.0f )
		fade = 0.0;

	glDisable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glShadeModel(GL_SMOOTH);
	glBegin( GL_TRIANGLE_STRIP );
	int i=0;
	for( it=m_Trail.rbegin(); it!=m_Trail.rend(); ++it )
	{
		float factor = (float)i/(float)MaxLen;

		Colour c = rayrange.Get( factor,true );
		c.a*=(1.0f-fade);
		Edge const& e = *it;
		glColor4f( c.r,c.g,c.b,c.a );
		glVertex2f( e.p0.x, e.p0.y );
		glColor4f( c.r,c.g,c.b,c.a );
		glVertex2f( e.p1.x, e.p1.y );
		++i;
	}
	glEnd();
}


void FatRay::Tick()
{
	m_Elapsed += (1.0f/(float)TARGET_FPS);
	if( m_Elapsed >= LifeTime )
	{
		Die();
		return;
	}

	if( m_Elapsed < LifeTime-FadeTime )
	{
		Forward( BeamSpeed );
		TurnBy( Rnd( -twopi/64.0f, twopi/64.0f ) );

		// trim trail.
		while( m_Trail.size() >= MaxLen )
			m_Trail.pop_front();

		// add to trail
		Edge e;
		e.p0 = Pos() + Rotate( vec2(BeamWidth/2.0f,0.0f), Heading() );
		e.p1 = Pos() + Rotate( vec2(-BeamWidth/2.0f,0.0f), Heading() );
		m_Trail.push_back( e );
	}
}

