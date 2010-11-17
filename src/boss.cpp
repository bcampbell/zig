#include "agentmanager.h"
#include "beam.h"
#include "bonuses.h"
#include "boss.h"
#include "bullets.h"
#include "drawing.h"
#include "effects.h"
#include "player.h"
#include "zig.h"

#include "fatzapper.h"
#include "soundmgr.h"

#include <assert.h>
#include <SDL.h>
#include <SDL_opengl.h>



Boss::Boss() :
	m_State(NONE),
	m_Timer(0.0f),
	m_Life(50),
	m_AnimTime( 0.0f ),
	m_AddedPods(false)
{
	SetRadius(50.0f);
	SetFlags( flagCanHitBullet| flagCanHitPlayer | flagLocksLevel );
	int i;
	for( i=0; i<4; ++i )
	{
		m_BeamPods[i] = new BeamPod();
		m_BeamPods[i]->Lock();
	}

	Respawn();
}

Boss::~Boss()
{
	int i;
	for( i=0; i<4; ++i )
	{
		assert( m_BeamPods[i] );
		m_BeamPods[i]->Unlock();
	}
}


void Boss::Respawn()
{
	RandomPos();
	PositionBeamPods();
	m_State = NONE;
	m_Timer = 0.0f;
	m_NextAttack = BEAMS;
	m_Flash = 0.0f;
	m_AnimTime = 0.0f;
}

void Boss::Draw()
{
	static const Colour outercolours[] =
	{
//		Colour( 0.7f, 0.2f, 0.0f),
//		Colour( 0.4f, 0.0f, 0.0f),
		Colour( 1.0f, 0.5f, 0.0f),
		Colour( 0.2f, 0.0f, 0.0f),
	};

	static const Colour innercolours[] =
	{
		Colour( 0.3f, 0.0f, 0.3f),
		Colour( 0.1f, 0.0f, 0.3f),
		Colour( 0.3f, 0.0f, 0.3f),
	};

	ColourRange innerrange( innercolours, sizeof(innercolours)/sizeof(Colour) );
	ColourRange outerrange( outercolours, sizeof(outercolours)/sizeof(Colour) );

	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_SMOOTH );

	Colour outercolour = ColourLerp( outerrange.Get( m_AnimTime/2.0f, false), Colour::WHITE, m_Flash );
	Colour innercolour = ColourLerp( innerrange.Get( m_AnimTime/4.0f, false), Colour::WHITE, m_Flash );

	glBegin( GL_TRIANGLE_FAN );
		glColor3f( innercolour.r, innercolour.g, innercolour.b );

		glVertex2f( 0.0f, 0.0f );
		const float step = twopi / 64.0f;
		float theta;
		glBegin( GL_LINE_LOOP );

		for( theta=0.0f; theta<twopi; theta += step )
		{
			float r = Radius() + Rnd( -20.0f,0.0f );
			const float x = r*sin(theta);
			const float y = r*cos(theta);

			glColor3f( outercolour.r, outercolour.g, outercolour.b );
			glVertex2f( x,y );
		}

	glEnd();


	if( m_State != BERSERK )
	{
		const float m=3.0f;
		glColor3f( 0.0f, 0.0f, 0.2f );
		DrawCircle( vec2( Rnd(-m,m), Rnd(-m,m) ), Radius() );
		glColor3f( 0.2f, 0.2f, 0.7f );
		DrawCircle( vec2( Rnd(-m,m), Rnd(-m,m) ), Radius() );
		glColor3f( 1.0f, 1.0f, 1.0f );
		DrawCircle( vec2( 0.0f, 0.0f ), Radius() );
	}
}


void Boss::Tick()
{
	if( !m_AddedPods )
	{
		int i;
		for( i=0; i<4; ++i )
		{
			g_Agents->AddDude( m_BeamPods[i] );
		}
		m_AddedPods = true;
	}

	m_Flash *= 0.9f;
	
	m_Timer += 1.0f/TARGET_FPS;
	m_AnimTime += 1.0f/TARGET_FPS;
	switch( m_State )
	{
		case NONE:
			EnterState( WANDER );
			break;
		case WANDER:
			{
				TurnToward( g_Player->Pos(), 0.015f );
//				TurnBy( 0.01f );
				Forward( 1.0f );
				if( m_Timer > 5.0f )
					EnterState( m_NextAttack );
			}
			break;
		case BEAMS:
			{
				TurnBy( 0.01f );
				if( m_Timer > 5.0f )
					EnterState( WANDER );
			}
			break;
		case SPIN:
		{
			const float spinupend = 2.0f;
			const float spinholdend = spinupend + 1.0f;
			const float spindownend = spinholdend + 1.0f;
			const float spinmaxspd = twopi/32.0f;

			if( m_Timer < spinupend )
			{
				float f = m_Timer/spinupend;
				TurnBy( spinmaxspd * f );
			}
			else if( m_Timer < spinholdend )
			{
				TurnBy( spinmaxspd );

				static int foo;
				if( ++foo >=2 )
				{
					foo=0;
					g_Agents->AddDude( new Missile( Pos(), Heading() ) );
				}
			}
			else if( m_Timer < spindownend )
			{
				float f = (m_Timer-spinholdend)/(spindownend-spinholdend);
				TurnBy( spinmaxspd * (1.0f-f) );
			}
			else
				EnterState( WANDER );
		}
		break;
	case BERSERK:
		{
			TurnToward( g_Player->Pos(), 0.04f );
			Forward( 2.0f );
			MoveBy( vec2( Rnd(-1.0f,1.0f), Rnd(-1.0f,1.0f) ) );

			if( m_Timer > 1.0f )
			{
				m_Timer = 0.0f;
				g_Agents->AddDude( new Baiter( Pos() ) );
			}
		}
		break;
	}

	PositionBeamPods();

	if( m_State != BERSERK )
	{
		bool alldead = true;
		int i;
		for( i=0; i<4; ++i )
		{
			if( m_BeamPods[i]->Alive() )
				alldead = false;
		}

		if( alldead )
		{
			EnterState( BERSERK );
		}
	}
}

static const vec2 s_BeamPodPos[4] =
{
	vec2( 40.0f,40.0f ),
	vec2( 40.0f,-40.0f ),
	vec2( -40.0f,-40.0f ),
	vec2( -40.0f,40.0f )
};

static const float s_BeamPodHeading[4] =
{
	1*pi/4,
	3*pi/4,
	5*pi/4,
	7*pi/4
};

void Boss::PositionBeamPods()
{
	int i;
	for( i=0; i<4; ++i )
	{
		BeamPod* pod = m_BeamPods[i];
		if( pod->Alive() )
		{
			pod->PositionRelative( *this, s_BeamPodPos[i], s_BeamPodHeading[i] );
		}
	}
}


void Boss::EnterState( State newstate )
{
	// exit old state first
	switch( m_State )
	{
		case NONE: break;
		case WANDER: break;
		case BEAMS:
			m_NextAttack = SPIN;
			break;
		case SPIN:
			m_NextAttack = BEAMS;
			break;
		case BERSERK: break;
	}

	// now enter new state
	m_Timer = 0.0f;
	m_State = newstate;
	switch( m_State )
	{
		case NONE:
			assert( false );	// should never happen!
			break;
		case WANDER: break;
		case SPIN: break;
		case BEAMS:
		{
			int i;
			for( i=0; i<4; ++i )
			{
				if( m_BeamPods[i]->Alive() )
					m_BeamPods[i]->Fire();
			}
		}
		case BERSERK:
			break;
	}
}


void Boss::OnHitBullet( Bullet& bullet )
{
	int absorbed = std::min( bullet.Power(), m_Life );
	if( m_State == BERSERK )
	{
/*
		vec2 target = Pos() + Rotate( vec2(0.0f, Radius() ), Heading() );
		if( (bullet.Pos() - target).LenSq() > 10.0f )
		{
			bullet.Die();
//			g_Agents->AddDude( new Wiggler( target ) );
			return;
		}
*/
		m_Life -= absorbed;
		if( m_Life <= 0 )
		{

			g_Agents->AddDude( new ExtraLife( Pos() ) );

			BigArseDeath( bullet, 5000 );
			Die();
		}
		else
		{
			m_Flash = 1.0f;
			int i;
			for(i=0;i<5;++i)
				g_Agents->AddDude( new Frag(bullet.Pos()) );
		}
	}

	bullet.ReducePower( absorbed );
}



//-----------------------------------------------------
// BeamPod
//-----------------------------------------------------

BeamPod::BeamPod() : m_Life( 20 )
{
	SetFlags( flagCanHitBullet );
	SetRadius( 20.0f );
}


void BeamPod::Respawn()
{
	m_Flash = 0.0f;
}

void BeamPod::Tick()
{
	m_Flash *= 0.8f;
}

void BeamPod::Fire()
{
	assert( Alive() );
	Beam* b = new Beam( *this, vec2( 0.0f, Radius() ), 0.0f );
	g_Agents->AddDude( b );

}


void BeamPod::OnHitBullet( Bullet& bullet )
{
	int absorbed = std::min( bullet.Power(), m_Life );
	m_Life -= absorbed;
	bullet.ReducePower( absorbed );
	if( m_Life <= 0 )
	{
		BigArseDeath( bullet, 200 );
		Die();
	}
	else
	{
		m_Flash = 1.0f;
		int i;
		for(i=0;i<5;++i)
			g_Agents->AddDude( new Frag(bullet.Pos()) );
	}
}


void BeamPod::Draw()
{
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );

	glShadeModel( GL_SMOOTH );

	Colour c1 = ColourLerp( Colour( 0.0f, 0.0f, 0.8f ), Colour::WHITE, m_Flash );
	Colour c2 = ColourLerp( Colour( 0.7f, 0.7f, 1.0f ), Colour::WHITE, m_Flash );
	Colour c3 = ColourLerp( Colour( 0.0f, 0.0f, 0.6f ), Colour::WHITE, m_Flash );
	Colour c4 = ColourLerp( Colour( 0.0f, 0.0f, 0.2f ), Colour::WHITE, m_Flash );


	glBegin( GL_TRIANGLES );
		glColor3f( c1.r, c1.g, c1.b );
		glVertex2f( -20.0f, 20.0f );
		glColor3f( c2.r, c2.g, c2.b );
		glVertex2f( -10.0f, 20.0f );
		glColor3f( c3.r, c3.g, c3.b );
		glVertex2f( -10.0f, -5.0f );

		glColor3f( c2.r, c2.g, c2.b );
		glVertex2f( -10.0f, 20.0f );
		glColor3f( c3.r, c3.g, c3.b );
		glVertex2f( 10.0f, 20.0f );
		glColor3f( c3.r, c3.g, c3.b );
		glVertex2f( -10.0f, -5.0f );

		glColor3f( c3.r, c3.g, c3.b );
		glVertex2f( 10.0f, 20.0f );
		glColor3f( c4.r, c4.g, c4.b );
		glVertex2f( 10.0f, -5.0f );
		glColor3f( c3.r, c3.g, c3.b );
		glVertex2f( -10.0f, -5.0f );


		glColor3f( c3.r, c3.g, c3.b );
		glVertex2f( 10.0f, 20.0f );
		glColor3f( c4.r, c4.g, c4.b );
		glVertex2f( 20.0f, 20.0f );
		glColor3f( c4.r, c4.g, c4.b );
		glVertex2f( 10.0f, -5.0f );

	glEnd();

}


