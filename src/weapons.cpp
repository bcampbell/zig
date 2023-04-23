

#include "weapons.h"

#include "agentmanager.h"
#include "bullets.h"
#include "drawing.h"
#include "effects.h"
#include "soundmgr.h"
#include "zig.h"

#include <SDL_opengl.h>


SingleLaser::SingleLaser( Player& owner ) :
	Weapon( owner ),
	m_FireTimer( 0 )
{
}

void SingleLaser::Tick( bool firebutton )
{
	const int firedelay = 6;

	++m_FireTimer;
	if( firebutton )
	{
		if( m_FireTimer >= firedelay )
		{
			m_FireTimer = 0;

			Bullet* b;
			b = new Laser( Owner(), vec2(0.0f, 8.0f), -0.0f );
			g_Agents->AddBullet(b);

			SoundMgr::Inst().Play( SFX_PLAYERFIRE );
		}
	}
}


//-----------------------------------------


DoubleLaser::DoubleLaser( Player& owner ) :
	Weapon( owner ),
	m_FireTimer( 0 ),
	m_RemainingTicks( TARGET_FPS * 15 )
{
}

void DoubleLaser::Tick( bool firebutton )
{
	--m_RemainingTicks;

	const int firedelay = 6;

	++m_FireTimer;
	if( firebutton )
	{
		if( m_FireTimer >= firedelay )
		{
			m_FireTimer = 0;

			g_Agents->AddBullet(
				new Laser( Owner(), vec2(-8.0f, 8.0f), -0.0f ) );
			g_Agents->AddBullet(
				new Laser( Owner(), vec2(8.0f, 8.0f), -0.0f ) );
			SoundMgr::Inst().Play( SFX_PLAYERFIRE );
		}
	}
}

bool DoubleLaser::Expired()
{
	return m_RemainingTicks <= 0;
}

//-----------------------------------------

TripleLaser::TripleLaser( Player& owner ) :
	Weapon( owner ),
	m_FireTimer( 0 ),
	m_RemainingTicks( TARGET_FPS * 15 )
{
}

void TripleLaser::Tick( bool firebutton )
{
	--m_RemainingTicks;

	const int firedelay = 6;

	++m_FireTimer;
	if( firebutton )
	{
		if( m_FireTimer >= firedelay )
		{
			m_FireTimer = 0;

			g_Agents->AddBullet(
				new Laser( Owner(), vec2(0.0f, 8.0f), -0.0f ) );

			g_Agents->AddBullet(
				new Laser( Owner(), vec2(-8.0f, 8.0f), -0.3f ) );
			g_Agents->AddBullet(
				new Laser( Owner(), vec2(8.0f, 8.0f), 0.3f ) );
			SoundMgr::Inst().Play( SFX_PLAYERFIRE );
		}
	}
}

bool TripleLaser::Expired()
{
	return m_RemainingTicks <= 0;
}


//-----------------------------------------

SuperNashwanPower::SuperNashwanPower( Player& owner ) :
	Weapon( owner ),
	m_FatTimer( 0 ),
	m_FrontTimer( 0 ),
	m_RearTimer( 0 ),
	m_RemainingTicks( TARGET_FPS * 10 )
{
}

void SuperNashwanPower::Tick( bool firebutton )
{
	--m_RemainingTicks;
	
	++m_FatTimer;
	++m_FrontTimer;
	++m_RearTimer;
	if( firebutton )
	{
		if( m_FatTimer >= 20 )
		{
			m_FatTimer = 0;

			g_Agents->AddBullet(
				new FatLaser( Owner(), vec2(-30.0f, -10.0f), -0.0f ) );
			g_Agents->AddBullet(
				new FatLaser( Owner(), vec2(30.0f, -10.0f), -0.0f ) );

			g_Agents->AddBullet(
				new FatLaser( Owner(), vec2(-30.0f, -10.0f), -pi/2 ) );
			g_Agents->AddBullet(
				new FatLaser( Owner(), vec2(30.0f, -10.0f), pi/2 ) );

			SoundMgr::Inst().Play( SFX_PLAYERFIRE );
		}
/*
		if( m_FrontTimer >= 6 )
		{
			m_FrontTimer = 0;

			g_Agents->AddBullet(
				new Laser( Owner(), vec2(-8.0f, 8.0f), -0.0f ) );
			g_Agents->AddBullet(
				new Laser( Owner(), vec2(8.0f, 8.0f), -0.0f ) );
			SoundMgr::Inst().Play( SFX_PLAYERFIRE );
		}
*/
		if( m_RearTimer >= 10 )
		{
			m_RearTimer = 0;

			g_Agents->AddBullet( new Laser( Owner(), vec2(0.0f, -8.0f), pi ) );

			g_Agents->AddBullet( new Laser( Owner(), vec2(0.0f, 0.0f), pi/6 ) );
			g_Agents->AddBullet( new Laser( Owner(), vec2(0.0f, 0.0f), pi - pi/6 ) );
			g_Agents->AddBullet( new Laser( Owner(), vec2(0.0f, 0.0f), -pi/6 ) );
			g_Agents->AddBullet( new Laser( Owner(), vec2(0.0f, 0.0f), pi + pi/6 ) );
			SoundMgr::Inst().Play( SFX_PLAYERFIRE );
		}
	}
}

bool SuperNashwanPower::Expired()
{
	return m_RemainingTicks <= 0;
}

void SuperNashwanPower::Draw()
{
	glEnable( GL_BLEND );
	glBlendFunc( GL_ONE, GL_ONE );
	glDisable( GL_TEXTURE_2D );
	glShadeModel(GL_FLAT);
	glColor3f( 0.1f, 0.1f, 0.2f );

	float vx[6] = { -35.0f, -10.0f, -35.0f,	35.0f, 10.0f, 35.0f };
	float vy[6] = { -5.0f,  -5.0f,  -20.0f,	-5.0f, -5.0f, -20.0f };
	
	glBegin( GL_TRIANGLES );

		int i;
		for( i=0; i<7; ++i )
		{
			int j;
			for( j=0; j<6; ++j )
			{
				glVertex2f( vx[j] + Rnd(-3.0f,3.0f), vy[j] + Rnd(-3.0f,3.0f) );
			}
		}
	glEnd();

	int remainingsecs = m_RemainingTicks / TARGET_FPS;
	if( remainingsecs<= 5)
	{

		char buf[16];
		sprintf( buf,"%d", remainingsecs );

		glPushMatrix();
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		glTranslatef( -0.0f, -22.0f, 0.0f );
		PlonkText( *g_Font, buf, true, 8.0f, 8.0f );
		glPopMatrix();
	}
}


