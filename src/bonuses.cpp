

#include "agentmanager.h"
#include "bonuses.h"
#include "drawing.h"
#include "zig.h"
#include "effects.h"

#include <assert.h>
#include <SDL_opengl.h>


void SpawnBonusMaybe( vec2 const& pos )
{
	const float bonusprob = 0.008f;

	// if we completely cleared the last sector then definitely
	// spawn a bonus
	if( !g_GameState->KeepYourSectorTidy )
	{
		if( Rnd( 0, 1.0f ) > bonusprob )
			return;
	}

	g_GameState->KeepYourSectorTidy = false;

	// OK, now what _kind_ of bonus?
	float nashwanprob = 0.01f;
	float extralifeprob = 0.24f;
	float powerupprob = 0.25f;
	float pointsprob = 0.5f;

	if( g_GameState->NoExtraLives )
		extralifeprob = 0.0f;
	if( g_Player->WeaponNum() == 1 )
		powerupprob *= 0.5f;
	if( g_Player->WeaponNum() == 2 )
		powerupprob = 0.0f;

	float total = nashwanprob + extralifeprob + powerupprob + pointsprob;

	float r=Rnd( 0.0f, total );

	if( r < nashwanprob )
		g_Agents->AddDude( new SuperNashwanBonus(pos) );
	else if( r < nashwanprob + extralifeprob )
		g_Agents->AddDude( new ExtraLife(pos) );
	else if( r < nashwanprob + extralifeprob + powerupprob )
		g_Agents->AddDude( new WeaponPowerup(pos) );
	else if( r < nashwanprob + extralifeprob + powerupprob + pointsprob )
		g_Agents->AddDude( new PointsBonus(pos) );
}

//--------------------

Bonus::Bonus( vec2 const& pos ) : m_Timeout(BONUSTIMEOUT)
{
	MoveTo(pos);
	SetFlags( flagCanHitPlayer | flagLocksLevel );
	SetRadius(8.0f);
}

//virtual
void Bonus::Tick()
{
	vec2 mov = g_Player->Pos() - Pos();
	mov.Normalise();
	mov *= 2.0f;
	MoveBy(mov);
	TurnBy( pi/64.0f );

	if( --m_Timeout < 0 )
		Die();
}

//virtual
void Bonus::Respawn()
{
	Die();
}





//--------------------
// PointsBonus
//--------------------

PointsBonus::PointsBonus( vec2 const& pos ) : Bonus(pos)
{
}

void PointsBonus::Draw()
{
	glShadeModel( GL_FLAT );
	glPushMatrix();

	glColor4f( 0.0f, 0.0f, 1.0f, 1.0f );
	DrawCircle( vec2::ZERO, 12.0f );

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glTranslatef( -1.0f, 0.0f, 0.0f );
	PlonkText( *g_Font, "1000", true, 7.0f, 10.0f );
	glPopMatrix();
}


void PointsBonus::OnHitPlayer( Player& player )
{
	g_Player->GivePoints( 1000 );
	vec2 offset = Rotate( vec2(0.0f,40.0f), g_Player->Heading() );
	g_Agents->AddUnderlay(
		new FadeText( g_Player->Pos()+offset, "1000 POINTS!", 16.0f, 1.2f ) );
	SoundMgr::Inst().Play( SFX_WIBBLEPOP );
	Die();
}

//--------------------
// ExtraLife
//--------------------

ExtraLife::ExtraLife( vec2 const& pos ) : Bonus(pos)
{
}

void ExtraLife::Draw()
{
	glShadeModel( GL_FLAT );
	glPushMatrix();

	glColor4f( 0.0f, 1.0f, 0.0f, 1.0f );
	DrawCircle( vec2::ZERO, 12.0f );

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glTranslatef( -8.0f, -8.0f, 0.0f );
	PlonkText( *g_Font, "L" );
	glPopMatrix();
}


void ExtraLife::OnHitPlayer( Player& player )
{
	player.GiveExtraLife();
	vec2 offset = Rotate( vec2(0.0f,40.0f), g_Player->Heading() );
	g_Agents->AddUnderlay(
		new FadeText( g_Player->Pos()+offset, "EXTRA LIFE!" ) );
	SoundMgr::Inst().Play( SFX_WIBBLEPOP );
	Die();
}


//--------------------
// WeaponPowerup
//--------------------

WeaponPowerup::WeaponPowerup( vec2 const& pos ) : Bonus(pos)
{
}

void WeaponPowerup::Draw()
{
	glShadeModel( GL_FLAT );
	glPushMatrix();

	glColor4f( 1.0f, 0.7f, 0.0f, 1.0f );
	DrawCircle( vec2::ZERO, 12.0f );

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	PlonkText( *g_Font, "F", true);
	glPopMatrix();
}

void WeaponPowerup::OnHitPlayer( Player& player )
{
	player.GiveWeaponPowerup();
	vec2 offset = Rotate( vec2(0.0f,40.0f), g_Player->Heading() );
	g_Agents->AddUnderlay( new FadeText(
		g_Player->Pos()+offset, "POWER UP!" ));
	SoundMgr::Inst().Play( SFX_WIBBLEPOP );
	Die();
}

//--------------------
// SuperNashwanBonus
//--------------------

SuperNashwanBonus::SuperNashwanBonus( vec2 const& pos ) : Bonus(pos)
{
}

void SuperNashwanBonus::Draw()
{
	glShadeModel( GL_FLAT );
	glPushMatrix();

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	DrawCircle( vec2::ZERO, 12.0f );

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	PlonkText( *g_Font, "Z", true);
	glPopMatrix();
}

void SuperNashwanBonus::OnHitPlayer( Player& player )
{
	player.GiveSuperNashwan();
	vec2 offset = Rotate( vec2(0.0f,40.0f), g_Player->Heading() );
	g_Agents->AddUnderlay( new FadeText(
		g_Player->Pos()+offset, "SUPERNASHWAN!!!" ));
	SoundMgr::Inst().Play( SFX_WIBBLEPOP );
	Die();
}



//--------------------
// Banana
//--------------------

Banana::Banana( vec2 const& pos )
{
	MoveTo(pos);
	SetFlags( flagCanHitPlayer | flagLocksLevel );
	SetRadius(8.0f);
}


void Banana::Tick()
{
}


void Banana::Draw()
{
	glColor4f( 1.0f, 1.0f, 0.0f, 1.0f );
	PlonkText( *g_Font, "B", true);
}


void Banana::OnHitPlayer( Player& player )
{
	Die();
}



