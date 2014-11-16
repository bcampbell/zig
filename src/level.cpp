#include "level.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <assert.h>
#include <math.h>
#include <cstdio>
#include <string>

#include "agent.h"
#include "agentmanager.h"
#include "arena.h"
#include "background.h"
#include "bonuses.h"
#include "controller.h"
#include "display.h"
#include "drawing.h"
#include "effects.h"
#include "humanoid.h"
#include "leveldef.h"
#include "mathutil.h"
#include "pausemenu.h"
#include "soundmgr.h"
#include "texture.h"
#include "zig.h"
#ifdef CRIPPLED
	#include "crippleclock.h"
#endif // CRIPPLED

// This template doesn't seem to work in visual c!!!

// helper
template< typename T > void MultiDude( int number )
{
	while( number-- )
		g_Agents->AddDude( new DudeSpawnEffect( new T() ) );
}


#define MULTI(TYPE,num) \
{ \
	int i=num; \
	while(i--) \
		g_Agents->AddDude( new DudeSpawnEffect( new TYPE() ) ); \
}

Level::Level( LevelDef const& details, int levelnum, bool demomode ) :
	m_State(intro),
	m_StateTimer(0.0f),
	m_ArenaShrinkage(0.0f),
	m_Arena( 0 ),
	m_LevelDef(details),
	m_LevelNum(levelnum),
	m_Background( 0 ),
	m_AttractMode( demomode ),
	m_PauseMenu( 0 ),
	m_Attempt( 0 ),
	m_BaiterDelay( details.m_BaiterStart ),
	m_BaiterCount(0),
	m_VictoryDuration( 0.0f ),
	m_ScreenshotFlag( false )
{
	assert( g_CurrentLevel == 0 );	// only one level at a time please.
	g_CurrentLevel = this;

	if( m_LevelDef.m_Retro )
		m_Background = new RetroBackground();
	else
		m_Background = new OriginalBackground();

	m_Arena = new Arena( details.m_ArenaRadius );

	g_Agents = new AgentManager();
	
	g_Player->Reset();

	// create a heap of dudes
	Populate();

	if( m_LevelDef.m_Retro )
		g_Agents->AddUnderlay( new FadeText( vec2( 0.0f, 100.0f), "RETRO LEVEL!", 16.0f, 0.5f ) );
	else
		g_Agents->AddUnderlay( new FadeText( vec2( 0.0f, 100.0f), "GAME ON!", 16.0f, 0.5f ) );
		
	char buf[64];
	sprintf( buf, "LEVEL %d", m_LevelNum );
	g_Agents->AddUnderlay( new FadeText( vec2( 0.0f, 80.0f), buf, 10.0f, 0.5f ) );
}


Level::~Level()
{
#ifdef CRIPPLED
	if( !m_AttractMode )
		CrippleClock::WriteRemainingTime();
#endif // CRIPPLED

	assert(g_CurrentLevel==this);

	if( m_PauseMenu )
	{
		delete m_PauseMenu;
		m_PauseMenu = 0;
	}

	
	delete g_Agents;
	g_Agents = 0;

	delete m_Arena;
	delete m_Background;

	g_CurrentLevel = 0;
}


bool Level::IsFinished()
{
	if( m_State == completed ||
		m_State == thatsitmangameovermangameover ||
		m_State == quit ||
		m_State == demofinished )
	{
		return true;
	}
	else
		return false;
}

bool Level::WasCompleted() const
	{ return m_State == completed; }

bool Level::WasGameOver() const
	{ return m_State == thatsitmangameovermangameover; }

bool Level::WasQuit() const
	{ return m_State == quit; }


float Level::ArenaRadius() const
{
	return m_Arena->Radius();
}


void Level::ShrinkArenaBy( float d )
{
	// Just accumulate.
	// Tick() will apply it.
	m_ArenaShrinkage += d;
}



void Level::Tick()
{
	if( m_AttractMode && g_Controller->Button() )
	{
		m_State = demofinished;
		return;
	}

	if( m_PauseMenu )
	{
		m_PauseMenu->Tick();
		if( m_PauseMenu->IsDone() )
		{
			if( m_PauseMenu->GetResult() == PauseMenu::AbortGame )
				m_State = quit;

			delete m_PauseMenu;
			m_PauseMenu = 0;
		}
		return;
	}


	m_StateTimer += 1.0f / TARGET_FPS;




	int prevdestructibles = g_Agents->DestructibleCount();

	if( m_State != intro )
	{
		g_Agents->Tick();
	}

	DoArenaShrinkage();

	if( m_State == inplay )
	{
		DoBaiters();
		DoUFOs();
	}

	int destructibles = g_Agents->DestructibleCount();
	if( destructibles == 0 && prevdestructibles > 0 )
		GiveKeepYourSectorTidyBonus();

	EvalState();

#ifdef CRIPPLED
	if( !m_AttractMode )
	{
		CrippleClock::ReduceRemainingTime( 1.0f/TARGET_FPS );
		if( CrippleClock::Expired() )
			m_State = thatsitmangameovermangameover;
	}
#endif // CRIPPLED
}



void Level::Render()
{
	glLoadIdentity();
//	glClear( GL_COLOR_BUFFER_BIT );
	m_Background->Draw( g_Player->Pos(), g_Player->Heading() );

	glPushMatrix();
	
	glRotatef( r2d( g_Player->Heading() ), 0.0f, 0.0f, 1.0f );
	glTranslatef( -g_Player->Pos().x, -g_Player->Pos().y, 0.0f );

	m_Arena->Draw();

	g_Agents->Render();


	glPopMatrix();

	DrawHUD();

	if( m_PauseMenu && !m_ScreenshotFlag )
		m_PauseMenu->Draw();

	if( m_ScreenshotFlag )
	{
		m_ScreenshotFlag = false;
		g_Display->TakeScreenshot();
	}
}

// helper for RenderAll()
void Level::DrawAgent( Agent& a )
{
	glPushMatrix();
	glTranslatef( a.Pos().x, a.Pos().y, 0.0f );
	glRotatef( r2d( -a.Heading() ), 0.0f, 0.0f, 1.0f );
	a.Draw();
	glPopMatrix();
}


void Level::DrawHUD()
{
	char buf[128];

    vec2 tl = g_Display->TopLeft();

	glPushMatrix();
	sprintf( buf, "LEVEL: %d  SCORE: %d  LIVES: %d",
		m_LevelNum,
		g_Player->Score(),
		g_Player->SpareLives() );
	glTranslatef( tl.x+20.0f, tl.y-20.0f, 0.0f );

	glColor4f( 1.0f, 1.0f, 1.0f, 0.8f );
	PlonkText( *g_Font, buf, false, 10.f, 10.0f );

	glPopMatrix();

	if( m_AttractMode )
	{
		glPushMatrix();
		glTranslatef( 0.0f, -40.0f, 0.0f );
		glColor4f( 1.0f, 1.0f, 1.0f, 0.4f );
		PlonkText( *g_Font, "-- DEMO --", true, 32.f, 32.0f );
		glPopMatrix();
	}
}


void Level::EvalState()
{

	switch( m_State )
	{
		case intro:
			if( m_StateTimer > 1.0f )
			{
				m_State = inplay;	//GO!
				if( m_Attempt == 0 )
					SoundMgr::Inst().Play( SFX_LEVELINTRO );
			}
			break;
		case inplay:
			if( !g_Player->Alive() )
			{
				m_State = dying;
				m_StateTimer = 0.0f;
			}
			else
			{
				// see how many dudes are locking the level
				int lockcount = g_Agents->LockCount();
				if( lockcount == 0 )
				{

					m_State = victorydance;
					m_StateTimer = 0.0f;

					// hang round so long if stuff left to shoot
					if( g_Agents->DestructibleCount() > 0 )
						m_VictoryDuration = 3.0f;
					else
						m_VictoryDuration = 1.0f;

					vec2 offset = Rotate(
						vec2(0.0f,40.0f),
						g_Player->Heading() );

					g_Agents->AddUnderlay( new FadeText(
						g_Player->Pos() + offset,
						"CLEAR!" , 20.0f, 3.0f ) );

				}
			}
			break;
		case dying:
			if( m_StateTimer >= 1.0f )
			{
				if( m_AttractMode )
				{
					m_State = demofinished;
				}
				else
				{
					if( g_Player->SpareLives() > 0 )
					{
						Restart();

						// level still got some mileage?
						int lockcount = g_Agents->LockCount();
						if( lockcount > 0 )
						{
							// yep - restart the level
							m_State = intro;
							m_StateTimer = 0.0f;
						}
						else
						{
							// nope, we died after shooting everything...
							m_State = completed;
						}
					}
					else
					{
						m_State = thatsitmangameovermangameover;
					}
				}
			}
			break;
		case victorydance:
			if( m_AttractMode )
			{
				m_State = demofinished;
			}
			else
			{
				if( !g_Player->Alive() )
				{
					m_State = dying;
					m_StateTimer = 0.0f;
				}
				else
				{
					if( m_StateTimer > m_VictoryDuration )
						m_State = completed;
				}
			}
			break;
		case completed:
		case thatsitmangameovermangameover:
		case quit:
		case demofinished:
			break;
	}
}

// restart level after player has died
void Level::Restart()
{
	++m_Attempt;
	m_Arena->SetRadius( m_LevelDef.m_ArenaRadius );
	m_BaiterDelay = m_LevelDef.m_BaiterStart;
	m_BaiterCount = 0;

	g_Agents->Restart();

	g_Agents->AddUnderlay( new FadeText( vec2( 0.0f, 100.0f), "GAME ON!" ) );
}


void Level::KeyDown( SDL_Keysym& keysym )
{
	if( m_AttractMode )
	{
		m_State = demofinished;
		return;
	}

	if( !m_PauseMenu )
	{
		if( keysym.sym == SDLK_ESCAPE ||
			keysym.sym == SDLK_PAUSE )
			m_PauseMenu = new PauseMenu();
	}


	// screenshot?
#if 0
	if( keysym.sym == SDLK_PRINT ||
		keysym.sym == SDLK_SYSREQ ||
		keysym.sym == SDLK_F12 )
	{
		m_ScreenshotFlag = true;
	}
#endif

#ifdef ENABLE_CHEATS
	// CHEAT KEYS
	if( keysym.sym == SDLK_x )
	{
		g_Player->GiveExtraLife();
	}
	if( keysym.sym == SDLK_z )
	{
		m_State = victorydance;
		m_StateTimer = 0.1f;
	}
	if( keysym.sym == SDLK_c )
	{
		g_Player->GiveWeaponPowerup();
	}
#endif // ENABLE_CHEATS
}



void Level::Populate()
{
	std::list< Dude* > newdudes;

	int i;
	for( i=0; i<m_LevelDef.NumSpawnEntries(); ++i )
	{
		int quant = m_LevelDef.GetSpawnEntry(i).quantity;
		DudeCreatorFn creatorfn = GetDudeCreator( m_LevelDef.GetSpawnEntry(i).dudetype );

		while( quant-- )
		{
			creatorfn( newdudes );
		}

	}
	std::list<Dude*>::const_iterator it;
	for( it=newdudes.begin(); it!=newdudes.end(); ++it )
		g_Agents->AddDude( new DudeSpawnEffect( *it ) );

	// special handling for particular types of dudes...
	Invader::Callibrate();
}



// throw baiters at the player if they're taking too long to
// finish the level!
void Level::DoBaiters()
{
	m_BaiterDelay -= 1.0f/TARGET_FPS;
	if( m_BaiterDelay <= 0 )
	{
		if( m_BaiterCount == 0 )
		{
		// first one is just a warning!	
			g_Agents->AddUnderlay( new FadeText( g_Player->Pos() + vec2( 0.0f, 40.0f), "HURRY UP!" ) );
		}
		else
		{
			SoundMgr::Inst().Play( SFX_BAITERALERT );
			MULTI( Baiter, m_BaiterCount )
			g_Agents->AddUnderlay( new FadeText( g_Player->Pos() + vec2( 0.0f, 40.0f), "LOOK OUT!!!" ) );
		}
		++m_BaiterCount;
		m_BaiterDelay = m_LevelDef.m_BaiterInterval;
	}
}


// maybe spawn ufos
void Level::DoUFOs()
{
	if( m_LevelDef.m_UFOProbability > 0.0f )
	{
		if( UFO::NumInstances()==0 &&
			Rnd() <= m_LevelDef.m_UFOProbability )
		{
			SoundMgr::Inst().Play( SFX_BAITERALERT );
			MULTI( UFO, 1 );
		}
	}
}

// update the arena size
void Level::DoArenaShrinkage()
{
	float target = m_LevelDef.m_ArenaRadius - m_ArenaShrinkage;
	if( target < (float)ARENA_RADIUS_MIN )
		target = (float)ARENA_RADIUS_MIN;
	float r = m_Arena->Radius();
	r = r + 0.01f*(target-r);
	m_Arena->SetRadius( r );
	m_ArenaShrinkage = 0.0f;
}


void Level::GiveKeepYourSectorTidyBonus()
{
	vec2 offset1 = Rotate( vec2(0.0f,-20.0f), g_Player->Heading() );
	vec2 offset2 = Rotate( vec2(0.0f,-40.0f), g_Player->Heading() );

//	g_Agents->AddUnderlay( new FadeText(
//		g_Player->Pos() + offset1,
//		"KEEP YOUR SECTOR TIDY!" , 8.0f, 1.5f ) );

	g_KeepYourSectorTidy = true;

	// make the victory state hang round a little longer
	if( m_VictoryDuration-m_StateTimer < 1.5f )
	{
		m_VictoryDuration = m_StateTimer + 1.5f;
	}
}


