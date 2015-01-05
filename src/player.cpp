

#include "agentmanager.h"
#include "colours.h"
#include "controller.h"
#include "dudes.h"
#include "drawing.h"
#include "effects.h"
#include "level.h"
#include "player.h"
#include "soundmgr.h"
#include "texture.h"
#include "weapons.h"
#include "zig.h"

#include <assert.h>
#include <SDL.h>
#include <SDL_opengl.h>


Player::Player( bool autopilot ) :
	Agent(),
	m_AutoController(0),
	m_Score(0),
	m_SpareLives(3),
	m_FireTimer(0),
	m_WeaponNum(0),
	m_Weapon(0),
	m_Nashwan(0)
{
	assert( g_Player == 0 );
	Reset();
	g_Player = this;

	if( autopilot )
    {
		m_AutoController = new Autopilot();
    }

	SetWeapon(0);
}

Player::~Player()
{
	assert( g_Player == this );
	g_Player = 0;
	delete m_AutoController;

	delete m_Weapon;
	delete m_Nashwan;
}

void Player::SetWeapon( int w )
{
	assert( w>=0 && w<=MAX_WEAPON_LEVEL );

	if( m_Weapon )
	{
		delete m_Weapon;
		m_Weapon = 0;
	}

	switch( w )
	{
		case 0: m_Weapon = new SingleLaser( *this );	break;
		case 1: m_Weapon = new DoubleLaser( *this );	break;
		case 2: m_Weapon = new TripleLaser( *this );	break;
	}

	m_WeaponNum = w;

	assert( m_Weapon != 0 );
}

//virtual
void Player::Draw()
{
	m_Weapon->Draw();
	if( m_Nashwan )
		m_Nashwan->Draw();

	Colour c(  0.0f, 0.0f, 0.8f );
	glColor3f( c.r, c.g, c.b);

	const float r = Radius();

	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );
	glBegin( GL_TRIANGLES );
		glVertex2f(-r, -r);
		glVertex2f(0.0f, r);
		glVertex2f(r, -r);
	glEnd();

}

//virtual
void Player::Tick()
{
	assert( m_Weapon != 0 );

    // ugly...
    if (m_AutoController) {
        m_AutoController->Tick();
    }

    Controller& ctrl = m_AutoController ? *m_AutoController:g_ControllerMgr->GameController();
	int buttons = ctrl.Buttons();
	m_Weapon->Tick( buttons & CTRL_BTN_FIRE );

	if( m_Nashwan )
		m_Nashwan->Tick( buttons & CTRL_BTN_FIRE );

	// time to degrade weapon?
	if( m_Weapon->Expired() && m_WeaponNum > 0 )
	{
		SetWeapon( m_WeaponNum-1 );
	}

	if( m_Nashwan && m_Nashwan->Expired() )
	{
		delete m_Nashwan;
		m_Nashwan = 0;
	}

	// turn?
	m_RotSpd += (0.02f*ctrl.XAxis());

	// forwards? backwards?
	float y = ctrl.YAxis();
	if( y < 0.0f )
	{
		// forward!
		y = (-y) * 0.75f;
		m_Vel += Rotate( vec2(0.0f,y), Heading() );
		// add exhaust trails
		int i;
		for( i=0; i<5; ++i )
			g_Agents->AddUnderlay( new Exhaust( *this, m_Vel ) );
	}
	else if( y > 0.0f )
	{
		// reverse
		y = (-y)*0.35f;
		m_Vel += Rotate( vec2(0.0f,y ), Heading() );
	}

	TurnBy(m_RotSpd);

//	MoveWithinArena( *this, m_Vel );
	ApplyVelocity();

	m_Vel *= 0.95f;	// drag
	m_RotSpd *= 0.8f;

}



// called by dude
void Player::FatalDudeCollision( Dude& dude )
{
	assert( Alive() );

	SoundMgr::Inst().Play( SFX_PLAYERTOAST );
	SoundMgr::Inst().Play( SFX_BIGEXPLOSION );
	Die();
	int i;
	for(i=0;i<80;++i)
		g_Agents->AddUnderlay(new Frag(Pos()));
	g_Agents->AddUnderlay( new BlastWave(Pos()));
}


void Player::GivePoints( int pts )
{
	m_Score += pts;
}

void Player::GiveExtraLife()
{
	++m_SpareLives;
}


void Player::GiveWeaponPowerup()
{
	int next = m_WeaponNum+1;

	if( next > MAX_WEAPON_LEVEL )
		next = MAX_WEAPON_LEVEL;

	SetWeapon( next );
}


int Player::Score() const
{
	return m_Score;
}

int Player::SpareLives() const
{
	return m_SpareLives;
}

void Player::Reset()
{
	if( g_BigHeadMode )
		SetRadius(20.0f);
	else
		SetRadius(10.0f);

	MoveTo( vec2::ZERO );
	TurnTo( 0.0f );
	m_RotSpd = 0.0f;
	m_Vel = vec2::ZERO;

	m_FireTimer = 0;
}

// called after losing a life.
void Player::Resurrect()
{
	UnDie();
	--m_SpareLives;
	Reset();
	SetWeapon(0);
}


void Player::ApplyVelocity()
{
	float r = g_CurrentLevel->ArenaRadius() - Radius();

	// if already outside arena, warp inside (no boing)
	// this allows arena to push dudes about.
	if( Pos().LenSq() > r*r )
	{
		vec2 corrected( Pos() );
		corrected.Normalise();
		corrected *= r;
		MoveTo( corrected );
	}

	vec2 newpos = Pos() + m_Vel;
	if( newpos.LenSq() > r*r )
	{
		// collided!
		//
		// Collision calculation similar to one in gamasutra article:
		// "Pool Hall Lessons: Fast, Accurate Collision Detection between
		// Circles or Spheres"
		// (http://www.gamasutra.com/features/20020118/vandenhuevel_01.htm)
		// 
		// Modified to keep circle _inside_ other circle rather than
		// outside!
		vec2 c = vec2::ZERO - Pos();
		float vlen = m_Vel.Len();
		vec2 vn = m_Vel/vlen;
		float d = Dot( c, vn );
		float dist = sqrt( r*r - c.LenSq() + d*d ) + d;

		// move to collision point
		vec2 colpoint = Pos() + (vn * dist);
		newpos = colpoint;

		// calc reflection vector
		vec2 normal( -colpoint );
		normal.Normalise();
		vec2 incident = vn;
		vec2 reflect =
			2.0f*Dot(-incident,normal)*normal + incident;

		// add on remaining movement after collision point
		float remain = vlen - dist;
		newpos += reflect*remain;
		m_Vel = vlen * reflect;
	}
	MoveTo( newpos );
}



void Player::GiveSuperNashwan()
{
	if( m_Nashwan )
	{
		delete m_Nashwan;
		m_Nashwan = 0;
	}
	m_Nashwan = new SuperNashwanPower( *this );
}


