

#include "agentmanager.h"
#include "beam.h"
#include "boids.h"
#include "bonuses.h"
#include "boss.h"
#include "colours.h"
#include "display.h"
#include "dudes.h"
#include "edgepuller.h"
#include "effects.h"
#include "fatzapper.h"
#include "bullets.h"
#include "drawing.h"
#include "image.h"
#include "level.h"
#include "player.h"
#include "snow.h"
#include "snowblower.h"
#include "spiker.h"
#include "texture.h"
#include "wobbly.h"
#include "zig.h"

#include <assert.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <algorithm>


struct DudeCreationInfo
{
	const char* type;
	DudeCreatorFn creator;
};

static const DudeCreationInfo s_DudeCreationTable[] =
{
	{ "Amoeba",		Amoeba::Create },
	{ "Flanker",	Flanker::Create },
	{ "Grunt",		Grunt::Create },
	{ "HeavyThing",	HeavyThing::Create },
	{ "Obstacle",	Obstacle::Create },
	{ "Pacman",		Pacman::Create },
	{ "Rock",		Rock::Create },
	{ "Snowflake",	Snowflake::Create },
	{ "SnowBlower",	SnowBlower::Create },
	{ "Tank",		Tank::Create },
	{ "UFO",		UFO::Create },
	{ "WallHugger",	WallHugger::Create },
	{ "Wiggler",	Wiggler::Create },
	{ "Zipper",		Zipper::Create },
	{ "ZipperMat",	ZipperMat::Create },
	{ "Invader",	Invader::Create },
	{ "Snake",		Snake::Create },
	{ "MineLayer",	MineLayer::Create },
	{ "Boss",		Boss::Create },
	{ "FatZapper",	FatZapper::Create },
	{ "EdgePuller",	EdgePuller::Create },
	{ "Spiker",		Spiker::Create },
	{ "Agitator",	Agitator::Create },
	{ "Boid",		Boid::Create },
	{ "Divider",	Divider::Create },
	{ "Puffer",     Puffer::Create },
};


DudeCreatorFn GetDudeCreator( std::string const& dudetype )
{
	const int numtypes = sizeof(s_DudeCreationTable) / sizeof(DudeCreationInfo);

	int i;
	for( i=0; i<numtypes; ++i )
	{
		if( dudetype == s_DudeCreationTable[i].type )
			return s_DudeCreationTable[i].creator;
	}

	throw Wobbly( "Unknown Dude type!" );
}





Dude::Dude( bool worldspace ) : Agent(worldspace), m_Flags(0)
{
}

void Dude::Respawn()
{
	// default behaviour - don't respawn upon level restart
	Die();
}

void Dude::OnHitPlayer( Player& player )
{
	// default behaviour - kill player
	player.FatalDudeCollision( *this );
}

void Dude::OnHitBullet( Bullet& bullet )
{
	// default behaviour - do nothing
}


void Dude::StandardDeath( Bullet& b, int points, int fragcount )
{
    StandardDeath(b.Owner(), points, fragcount);
}

void Dude::BigArseDeath( Bullet& b, int points)
{
    BigArseDeath(b.Owner(), points);
}

void Dude::StandardDeath( Player& causeofdeath, int points, int fragcount )
{
	char scoretext[32];
	sprintf(scoretext,"%d",points);
	causeofdeath.GivePoints( points );

	g_Agents->AddUnderlay( new FadeText( Pos(), scoretext ) );
	int i;
	for(i=0;i<fragcount;++i)
		g_Agents->AddDude(new Frag(Pos()));

	g_Agents->AddDude( new BlastWave(Pos()) );
	SpawnBonusMaybe(Pos());
}

void Dude::BigArseDeath( Player& causeofdeath, int points )
{
	char scoretext[32];
	sprintf(scoretext,"%d",points);
	causeofdeath.GivePoints( points );

	g_Agents->AddUnderlay( new FadeText( Pos(), scoretext ) );
	int i;
	for(i=0;i<1000;++i)
		g_Agents->AddDude(new Frag(Pos()));

	SoundMgr::Inst().Play( SFX_BIGEXPLOSION );

	g_Agents->AddDude( new BlastWave(Pos()) );
	SpawnBonusMaybe(Pos());
}



//--------------------
// Baiter
//--------------------

Baiter::Baiter() : m_Spd( vec2::ZERO )
{
	SetRadius(10.0f);
	SetFlags( flagCanHitBullet| flagCanHitPlayer| flagLocksLevel );
	RandomPos();
}

Baiter::Baiter( vec2 const& pos ) : m_Spd( vec2::ZERO )
{
	SetRadius(10.0f);
	SetFlags( flagCanHitBullet| flagCanHitPlayer| flagLocksLevel );
	RandomPos();
	MoveTo(pos);
}

void Baiter::Draw()
{
    const float verts[8*3*2] = {
		-4.0f,4.0f, 0.0f,14.0f, 4.0f,4.0f,
		 4.0f,4.0f, 14.0f,0.0f, 4.0f,-4.0f,
		 -4.0f,-4.0f, 0.0f,-14.0f, 4.0f,-4.0f,
		 -4.0f,4.0f, -14.0f,0.0f, -4.0f,-4.0f,

		-2.0f,2.0f, 0.0f,10.0f, 2.0f,2.0f,
		 2.0f,2.0f, 10.0f,0.0f, 2.0f,-2.0f,
		 -2.0f,-2.0f, 0.0f,-10.0f, 2.0f,-2.0f,
		 -2.0f,2.0f, -10.0f,0.0f, -2.0f,-2.0f,
    };
    const float colours[8*3*3] = {
        0.6f,0.0f,0.0f, 0.6f,0.0f,0.0f, 0.6f,0.0f,0.0f,
        0.6f,0.0f,0.0f, 0.6f,0.0f,0.0f, 0.6f,0.0f,0.0f,
        0.6f,0.0f,0.0f, 0.6f,0.0f,0.0f, 0.6f,0.0f,0.0f,
        0.6f,0.0f,0.0f, 0.6f,0.0f,0.0f, 0.6f,0.0f,0.0f,

        1.0f,0.8f,0.0f, 1.0f,0.8f,0.0f, 1.0f,0.8f,0.0f,
        1.0f,0.8f,0.0f, 1.0f,0.8f,0.0f, 1.0f,0.8f,0.0f,
        1.0f,0.8f,0.0f, 1.0f,0.8f,0.0f, 1.0f,0.8f,0.0f,
        1.0f,0.8f,0.0f, 1.0f,0.8f,0.0f, 1.0f,0.8f,0.0f,
    };


	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );
	glBegin( GL_TRIANGLES );
    int i;
    const float* pc = colours;
    const float* pv = verts;
    for(i=0;i<8*3;++i)
    {
        glColor3f( pc[0], pc[1], pc[2]);
        pc += 3;
        glVertex2f( pv[0], pv[1] );
        pv += 2;
    }
	glEnd();
}

void Baiter::Tick()
{
	MoveBy( m_Spd );
	TurnBy( pi/16.0f );

	vec2 accel = g_Player->Pos() - Pos();
	accel.Normalise();
	accel *= 0.2f;

	m_Spd += accel;
	m_Spd *= 0.98f;

}

void Baiter::OnHitBullet( Bullet& bullet )
{
	StandardDeath( bullet, 100 );
	Die();
	bullet.ReducePower(1);
}

void Baiter::Respawn()
{
	// remove baiters to give player a chance :-)
	Die();
}


//--------------------
// Obstacle
//--------------------

Obstacle::Obstacle() : m_Cyc(0.0f)
{
	SetRadius(10.0f);
	SetFlags( flagCanHitBullet| flagCanHitPlayer );
	RandomPos();
}

Obstacle::Obstacle( vec2 const& pos ) : m_Cyc(0.0f)
{
	SetRadius(10.0f);
	SetFlags( flagCanHitBullet| flagCanHitPlayer );
	RandomPos();
	MoveTo(pos);
}

void Obstacle::Draw()
{
	// evil, menacing colours.
	static const Colour evilcolours[] =
	{
		Colour( 0.6f, 0.0f, 0.0f),
		Colour( 0.0f, 0.0f, 1.0f),
	};

	static const ColourRange evilrange( evilcolours, 2 );

	m_Cyc += 0.01f;

	Colour c=evilrange.Get(m_Cyc,false);
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );
	glColor3f( c.r, c.g, c.b );
	glBegin( GL_TRIANGLES );
		glVertex2f( -10.0f, -10.0f );
		glVertex2f( 10.0f, -10.0f );
		glVertex2f( 10.0f, 10.0f );

		glVertex2f( -10.0f, -10.0f );
		glVertex2f( 10.0f, 10.0f );
		glVertex2f( -10.0f, 10.0f );
	glEnd();
}

void Obstacle::Tick()
{
}

void Obstacle::OnHitBullet( Bullet& bullet )
{
	StandardDeath( bullet, 20 );
	Die();
	bullet.ReducePower(1);
}


void Obstacle::Respawn()
{
	RandomPos();
}

//--------------------
// Grunt
//--------------------

Grunt::Grunt()
{
	SetRadius(10.0f);
	SetFlags( flagCanHitBullet|
		flagCanHitPlayer|
		flagLocksLevel );
	Respawn();
}

void Grunt::Respawn()
{
	RandomPos();
	m_Cyc = 0.0f;
}

void Grunt::Draw()
{
	static const Colour colours[] =
	{
		Colour( 1.0f, 0.0f, 0.0f),
		Colour( 1.0f, 1.0f, 0.0f),
		Colour( 0.0f, 1.0f, 0.0f),
		Colour( 0.0f, 1.0f, 1.0f),
		Colour( 0.0f, 0.0f, 1.0f),
		Colour( 1.0f, 0.0f, 1.0f),
		Colour( 1.0f, 0.0f, 0.0f),
	};
	static const ColourRange range( colours, sizeof(colours)/sizeof(Colour) );

	const int segcount = 8;
	const float totallen = 15.0f;
	const float segw0 = 20.0f;
	const float segw1 = 10.0f;
	const float glowzonemin = 50.0f;
	const float glowzonemax = 150.0f;

	Colour base( 0.5f, 0.5f, 0.5f );
	float dist = ((g_Player->Pos() - Pos()).Len() - glowzonemin) / (glowzonemax-glowzonemin);
	if( dist > 1.0f )
		dist = 1.0f;
	if( dist < 0.0f )
		dist = 0.0f;

	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_SMOOTH );
	glBegin( GL_TRIANGLE_STRIP );
		int seg;

		for( seg=0; seg<=segcount; ++seg )
		{
			float f = (float)seg/(float)segcount;
			float ss = 1.0f + f*sin( m_Cyc*10.0f )*0.4f;
			float cs = 1.0f + f*cos( m_Cyc*10.0f )*0.4f;
			
			const float w = (1.0f-f)*segw0 + f*segw1;

			Colour c = range.Get( m_Cyc+(1.0f-f)*0.2f, false );
			c = ColourLerp( c, base, dist );
			
			glColor3f( c.r, c.g, c.b );
			glVertex2f( ss* -w/2.0f, -cs*(f-0.5f)*totallen);
			glColor3f( c.r, c.g, c.b );
			glVertex2f( ss* w/2.0f, -cs*(f-0.5f)*totallen);
		}
	glEnd();
}

void Grunt::Tick()
{
	Forward( 1.0f );
	m_Cyc += 0.01f;
	TurnToward( g_Player->Pos(), pi/16.0f );
}

void Grunt::OnHitBullet( Bullet& bullet )
{

	StandardDeath( bullet, 100 );
	Die();
	bullet.ReducePower(1);
}




//--------------------
// Wiggler
//--------------------

Wiggler::Wiggler()
{
	Respawn();
}

Wiggler::Wiggler( vec2 const& pos )
{
	Respawn();
	MoveTo(pos);
}


void Wiggler::Respawn()
{
	m_fuz = Rnd( 0.0f, twopi );
	SetRadius(5.0f);
	SetFlags( flagCanHitBullet|
		flagCanHitPlayer|
		flagLocksLevel );
	RandomPos();
}

void Wiggler::Draw()
{
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );
    glColor3f( 0.0f, 1.0f, 0.0f );
	glBegin( GL_TRIANGLES );
		glVertex2f( 0.0f, 10.0f );
		glVertex2f( 5.0f, -10.0f );
		glVertex2f( -5.0f, -10.0f );
	glEnd();
}

void Wiggler::Tick()
{
	Forward( 2.0f );

	vec2 v = g_Player->Pos()-Pos();
	float theta = atan2(v.x,v.y);
//	TurnTo( theta );

	float a = theta-Heading();
	while( a<-pi )
		a+=twopi;
	while( a>pi )
		a-=twopi;

	if( a>0.1f )
		a=0.1f;
	else if (a<-0.1f)
		a=-0.1f;

	a += sin(m_fuz) * 0.2f;

	m_fuz += pi/16.0f;

	TurnBy(a);
}

void Wiggler::OnHitBullet( Bullet& bullet )
{
	StandardDeath( bullet, 100 );
	Die();
	bullet.ReducePower(1);
}




//--------------------
// Flanker
//--------------------

Flanker::Flanker() : m_Mode(flank)
{
	m_FlankSpd = Rnd( 3.0f, 4.0f );
	SetRadius(10.0f);
	SetFlags( flagCanHitBullet|
		flagCanHitPlayer |
		flagLocksLevel );
	RandomPos();
}

void Flanker::Draw()
{
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );
	if( m_Mode == flank )
	{
		glBegin( GL_TRIANGLES );

		glColor3f( 0.6f, 0.2f, 0.85f );
		glVertex2f( 0.0f, 10.0f );
		glColor3f( 0.6f, 0.2f, 0.85f );
		glVertex2f( 0.0f, -10.0f );
		glColor3f( 0.6f, 0.2f, 0.85f );
		glVertex2f( -10.0f, -4.0f );
		
		glColor3f( 0.5f, 0.0f, 0.7f );
		glVertex2f( 0.0f, 10.0f );
		glColor3f( 0.5f, 0.0f, 0.7f );
		glVertex2f( 10.0f, -4.0f );
		glColor3f( 0.5f, 0.0f, 0.7f );
		glVertex2f( 0.0f, -10.0f );

		glEnd();
	}
	else
	{
		// charge!
		glBegin( GL_TRIANGLES );
		glColor3f( 0.9f, 0.2f, 0.2f );
		glVertex2f( 0.0f, 11.0f );
		glColor3f( 0.9f, 0.2f, 0.2f );
		glVertex2f( 0.0f, -11.0f );
		glColor3f( 0.9f, 0.2f, 0.2f );
		glVertex2f( -9.0f, -4.0f );
		
		glColor3f( 0.5f, 0.0f, 0.5f );
		glVertex2f( 0.0f, 11.0f );
		glColor3f( 0.5f, 0.0f, 0.5f );
		glVertex2f( 9.0f, -4.0f );
		glColor3f( 0.5f, 0.0f, 0.5f );
		glVertex2f( 0.0f, -11.0f );
		glEnd();
	}
}

void Flanker::Tick()
{
	const float chargespdinitial=1.0f;
	const float chargespdmax=16.0f;
	const float chargeaccel=0.075f;
	if( m_Mode == flank )
	{
		vec2 v = g_Player->Pos()-Pos();
		float angletoplayer = atan2(v.x,v.y);

		float a = fmod( g_Player->Heading() - angletoplayer, twopi);

		if( a > pi/32.0f )
		{
			TurnTo(angletoplayer - pi/2.5f );
			Forward( m_FlankSpd );
		}
		else if (a < -pi/32.0f )
		{
			TurnTo(angletoplayer + pi/2.5f );
			Forward( m_FlankSpd );
		}
		else
		{
			TurnTo(angletoplayer);
			m_Mode = charge;
			m_ChargeDist = v.Len() * 2.0f;	//Rnd( 1.2f, 2.0f );
			m_ChargeSpd = chargespdinitial;
		}
	}
	else
	{
		Forward( m_ChargeSpd );
		m_ChargeDist -= m_ChargeSpd;

		m_ChargeSpd += chargeaccel;
		if( m_ChargeSpd > chargespdmax )
			m_ChargeSpd = chargespdmax;
		
		if( m_ChargeDist < 0.0f )
		{
			m_Mode = flank;
		}
	}
}

void Flanker::OnHitBullet( Bullet& bullet )
{
	StandardDeath( bullet, 150 );
	Die();
	bullet.ReducePower(1);
}


void Flanker::Respawn()
{
	RandomPos();
	m_Mode = flank;
}


//--------------------
// WallHugger
//--------------------

WallHugger::WallHugger() : m_Life(10), m_Flash(0.0f), m_FireTimer(0.0f)
{
	SetRadius(15.0f);
	SetFlags( flagCanHitBullet|
		flagLocksLevel );
	m_AngularAccel = Rnd( 0.0009f, 0.0011f );
	m_MaxSpd = Rnd( 0.01f, 0.015f );

	Respawn();
}



void WallHugger::Respawn()
{
	m_Angle = Rnd( -pi,pi );
	m_AngularSpd = 0.0f;
    m_FireTimer = 0.0f;
    m_Flash = 0.0f;
    Orient();
}

void WallHugger::Orient()
{
	TurnTo( - m_Angle - (pi/2.0f) );
	float r = g_CurrentLevel->ArenaRadius();
	MoveTo( vec2(r*cosf(m_Angle), r*sinf(m_Angle)) );
}


// custom mod function to handle -ve values as expected
static float zigmod(float a, float n)
{
    return a - floorf(a/n) * n;
}


void WallHugger::Tick()
{
    m_Flash *= 0.8f;
    if( g_Player->Pos().LenSq() > 4.0f ) {

    	float pa = atan2f(g_Player->Pos().y, g_Player->Pos().x);

        float d = pa - m_Angle;
        d = zigmod(d+pi, twopi) - pi;

        const float accel = m_AngularAccel;
        const float maxSpd = m_MaxSpd;
        const float thresh = 0.5f/twopi;
        if( d>thresh) {
            m_AngularSpd +=accel;
            if(m_AngularSpd>maxSpd) {
                m_AngularSpd = maxSpd;
            }
        } else if (d< -thresh) {
            m_AngularSpd -= accel;
            if(m_AngularSpd<-maxSpd) {
                m_AngularSpd = -maxSpd;
            }
        } else {
            m_AngularSpd *= 0.9f;
        }

    }

    m_Angle = m_Angle+m_AngularSpd;
    m_Angle = zigmod(m_Angle, twopi);
    /*
    while( m_Angle<-pi) {
        m_Angle += twopi;
    }
    while( m_Angle>pi) {
        m_Angle -= twopi;
    }
    */


	TurnTo( - m_Angle - (pi/2.0f) );
	float r = g_CurrentLevel->ArenaRadius();
	MoveTo( vec2(r*cosf(m_Angle), r*sinf(m_Angle)) );

    m_FireTimer += 1.0f/TARGET_FPS;
    if( m_FireTimer > 1.0f )
    {
        m_FireTimer = 0.0f;
        vec2 pos = Pos() + Rotate(vec2(0,10.0f),Heading());

		g_Agents->AddDude( new Missile( pos, Heading() ) );
	}

#if 0
    m_FireTimer += 1.0f/TARGET_FPS;
    if( m_FireTimer > 8.0f )
    {
        m_FireTimer = 0.0f;
		Beam::Params beamparams;
		beamparams.warmuptime = 2.0f;
		beamparams.ontime = 2.0f;
		beamparams.width = 28.0f;
		beamparams.length = 1200.0f;
		Beam* b1 = new Beam( *this, vec2( 0.0f, 8 ), 0.0f, &beamparams );
		g_Agents->AddDude( b1 );
    }
#endif
}

void WallHugger::OnHitBullet( Bullet& bullet )
{
	int absorbed = std::min( bullet.Power(), m_Life );
	m_Life -= absorbed;
	if( m_Life <= 0 )
	{
		BigArseDeath( bullet, 500 );
		Die();
	}
	else
	{
		SoundMgr::Inst().Play( SFX_SMALLTHUD );
        m_Flash = 1.0f;
		int i;
		for(i=0;i<5;++i)
			g_Agents->AddDude(new Frag(Pos()));
	}
	bullet.ReducePower( absorbed );
}

void WallHugger::Draw()
{
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );

	Colour c1 = ColourLerp( Colour( 0.8f, 0.8f, 1.0f ), Colour::WHITE, m_Flash ); 
	Colour c2 = ColourLerp( Colour( 0.5f, 0.5f, 8.0f ), Colour::WHITE, m_Flash ); 
	Colour c3 = ColourLerp( Colour( 0.2f, 0.2f, 6.0f ), Colour::WHITE, m_Flash ); 

	glColor3f( c2.r, c2.g, c2.b );
	glBegin( GL_TRIANGLES );
        glVertex2f( 8.0f, 0.0f );
        glVertex2f( 8.0f, -5.0f );
        glVertex2f( -8.0f, -5.0f );
        glVertex2f( -8.0f, -5.0f );
        glVertex2f( -8.0f, 0.0f );
        glVertex2f( 8.0f, 0.0f );
	glEnd();
	glBegin( GL_TRIANGLES );
		// light
        glColor3f( c1.r, c1.g, c1.b );
		glVertex2f( -16.0f, 8.0f );
        glColor3f( c1.r, c1.g, c1.b );
		glVertex2f( -8.0f, 8.0f );
        glColor3f( c1.r, c1.g, c1.b );
		glVertex2f( -4.0f, 0.0f );

        glColor3f( c2.r, c2.g, c2.b );
		glVertex2f( -8.0f, 8.0f );
        glColor3f( c2.r, c2.g, c2.b );
		glVertex2f( 8.0f, 8.0f );
        glColor3f( c2.r, c2.g, c2.b );
		glVertex2f( -4.0f, 0.0f );

        glColor3f( c2.r, c2.g, c2.b );
		glVertex2f( 8.0f, 8.0f );
        glColor3f( c2.r, c2.g, c2.b );
		glVertex2f( 4.0f, 0.0f );
        glColor3f( c2.r, c2.g, c2.b );
		glVertex2f( -4.0f, 0.0f );

        glColor3f( c3.r, c3.g, c3.b );
		glVertex2f( 8.0f, 8.0f );
        glColor3f( c3.r, c3.g, c3.b );
		glVertex2f( 16.0f, 8.0f );
        glColor3f( c3.r, c3.g, c3.b );
		glVertex2f( 4.0f, 0.0f );
	glEnd();

}


//--------------------
// Rock
//--------------------

Rock::Rock() : m_Size(2)
{
	CommonInit();
	RandomPos();
}

Rock::Rock(vec2 const& pos, int size) : m_Size(size)
{
	assert( m_Size >=0 && m_Size <= 2 );
	CommonInit();
	MoveTo(pos);
}


void Rock::CommonInit()
{
	float f = 10.0f * ((float)m_Size+1);
	SetRadius( sqrt(2.0f*f*f) );

	SetFlags( flagCanHitBullet|
		flagCanHitPlayer|
		flagLocksLevel );
	float r = Rnd(1.0f,2.0f);
	float theta = Rnd(0,twopi);
	m_Spd.x = r*sin(theta);
	m_Spd.y = r*cos(theta);

	m_TurnRate = Rnd(-pi/128.0f, pi/128.0f ) * (float)(3-m_Size);

	int vertexCount=6+m_Size*2;
	m_VertexList.reserve(vertexCount);
	float t=0;
	float dt=6.283185307f / vertexCount;
	int i;
	for (i=0;i<vertexCount;++i,t+=dt)
	{
		float d = Radius() + Rnd( -5.0f, 5.0 );	//((rand()%30)-10)/5.0f;
		m_VertexList.push_back( vec2(  sin(t)*d, cos(t)*d ) );
	}
}


void Rock::Draw()
{
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );
	glColor3f( 1.0f, 1.0f, 1.0f );
	glBegin( GL_LINE_LOOP );
		VertexList::const_iterator i = m_VertexList.begin();
		VertexList::const_iterator end = m_VertexList.end();
		for (;i!=end;++i)
			glVertex2f( i->x, i->y );
	glEnd();
}




void Rock::Tick()
{
	TurnBy( m_TurnRate );
	MoveWithinArena( *this, m_Spd );
}


void Rock::OnHitBullet( Bullet& bullet )
{
	StandardDeath( bullet, 100 );

	if( m_Size == 1 )
	{
		g_Agents->AddDude( new Rock( Pos(), 0 ) );
		g_Agents->AddDude( new Rock( Pos(), 0 ) );
		g_Agents->AddDude( new Rock( Pos(), 0 ) );
		g_Agents->AddDude( new Rock( Pos(), 0 ) );
	}
	else if( m_Size > 1 )
	{
		g_Agents->AddDude( new Rock( Pos(), 1 ) );
		g_Agents->AddDude( new Rock( Pos(), 1 ) );
	}
	Die();
	bullet.ReducePower(1);
}


void Rock::Respawn()
{
	RandomPos();
}


//--------------------
// UFO
//--------------------

int UFO::s_NumInstances = 0;
int UFO::s_Warble = -1;

UFO::UFO() : m_Timer( (int)Rnd( 100,200 ) )
{
//	if( s_NumInstances == 0 )
//	{
//		assert( s_Warble == -1 );
//		s_Warble = SoundMgr::Inst().PlayLooped( SFX_PULSE1 );
//	}
	++s_NumInstances;

	SetRadius( 8.0f );
	SetFlags( flagCanHitBullet|
		flagCanHitPlayer|
		flagLocksLevel );
	Respawn();
}

UFO::~UFO()
{
	--s_NumInstances;
//	if( s_NumInstances == 0 && s_Warble != -1 )
//	{
//		SoundMgr::Inst().StopLooped( s_Warble );
//		s_Warble = -1;
//	}
}

void UFO::Respawn()
{
	m_Timer = (int)Rnd( 100,200 );

	RandomPos();
	TurnBy( Rnd( -halfpi, halfpi ) );
	m_Spd = Rotate( vec2(0.0f, 2.0f), Heading()+halfpi );
}

void UFO::Draw()
{
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );

	glColor3f( 1.0f, 1.0f, 1.0f );
	glBegin( GL_LINE_LOOP );
		glVertex2f( 0, 8 );
		glVertex2f( 5, 2 );
		glVertex2f( 12, -2 );
		glVertex2f( 0, -8 );
		glVertex2f( -12, -2 );
		glVertex2f( -5, 2 );
	glEnd();
}

void UFO::Tick()
{
	MoveWithinArena( *this, m_Spd );

	if( --m_Timer <= 0 )
	{
		m_Timer = (int)Rnd( 100,200 );
		TurnBy( Rnd( -halfpi, halfpi ) );
		m_Spd = Rotate( vec2(0.0f, 2.0f), Heading()+halfpi );

	}

	if( Rnd() < 0.03 )	// 3% chance of firing
	{
		vec2 spd = g_Player->Pos() - Pos();
		spd.Normalise();
		spd *= 4.0f;
		g_Agents->AddDude( new UFOBullet( Pos(), spd ) );
	}
}

void UFO::OnHitBullet( Bullet& bullet )
{
	StandardDeath( bullet, 100 );
	Die();
	bullet.ReducePower(1);
}


//--------------------
// UFOBullet
//--------------------

UFOBullet::UFOBullet( vec2 const& pos, vec2 const& spd ) :
	m_Spd( spd ),
	m_Timer( 40 )
{
	MoveTo(pos);
	SetRadius( 2.0f );
	SetFlags(
		flagCanHitPlayer|
		flagLocksLevel );
}

void UFOBullet::Respawn()
{
	Die();
}

void UFOBullet::Draw()
{
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );

	glColor3f( 1.0f, 1.0f, 1.0f );
	glBegin( GL_QUADS );
		glVertex2f( 0, 2 );
		glVertex2f( -2, 0 );
		glVertex2f( 0, -2 );
		glVertex2f( 2, 0 );
	glEnd();
}

void UFOBullet::Tick()
{
	if( --m_Timer > 0 )
		MoveBy( m_Spd );
	else
		Die();
}

//--------------------
// Snowflake
//--------------------

class SnowflakeTexture : public Texture
{
public:
	virtual void UploadToGL()
	{
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_Flake.width(), m_Flake.depth(),
			0, GL_RGBA, GL_UNSIGNED_BYTE, m_Flake.buffer() );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
private:
	UniqueSnowFlake m_Flake;
};



const float Snowflake::s_MinSpd = 0.5f;
const float Snowflake::s_MaxSpd = 1.2f;

Snowflake::Snowflake() :
	m_Texture(0),
	m_Size( Rnd( 8.0f, 20.0f ) )
{
	CommonInit();
}

Snowflake::Snowflake( vec2 const& pos, vec2 const& vel ) :
	m_Texture(0),
	m_Size( Rnd( 8.0f, 20.0f ) )
{
	CommonInit();
	MoveTo( pos );
	m_Vel = vel;
}


void Snowflake::CommonInit()
{
	SetRadius(m_Size);	// should take sqrt...
	SetFlags( flagCanHitBullet|
		flagCanHitPlayer|
		flagLocksLevel );
	
	m_Texture = new SnowflakeTexture();
	g_Display->AddTexture( m_Texture );
	Respawn();
	m_Colour = Colour::WHITE;
}



void Snowflake::Respawn()
{
	RandomPos();
	m_Vel = Rotate( vec2(0.0f, Rnd(s_MinSpd, s_MaxSpd) ), Rnd(0.0f,twopi) );
	m_RotSpd = Rnd( -pi/64, pi/64 );
}

Snowflake::~Snowflake()
{
	g_Display->RemoveTexture( m_Texture );
	delete m_Texture;
}

void Snowflake::Draw()
{
	glEnable( GL_BLEND );
	glEnable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );
	glBindTexture( GL_TEXTURE_2D,m_Texture->ID() );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	Colour const& c = m_Colour;

	glColor3f( c.r, c.g, c.b );
	glBegin( GL_QUADS );
		glTexCoord2f(0.0f,0.0f);
		glVertex2f( -m_Size, -m_Size );

		glTexCoord2f(1.0f,0.0f);
		glVertex2f( m_Size, -m_Size );

		glTexCoord2f(1.0f,1.0f);
		glVertex2f( m_Size, m_Size );

		glTexCoord2f(0.0f,1.0f);
		glVertex2f( -m_Size, m_Size );
	glEnd();

	glDisable( GL_TEXTURE_2D );
	glDisable( GL_BLEND );
}

void Snowflake::Tick()
{
	MoveWithinArena( *this, m_Vel );
	TurnBy( m_RotSpd );

	if( m_Vel.LenSq() > s_MaxSpd*s_MaxSpd )
		m_Vel *= 0.99f;
}

void Snowflake::OnHitBullet( Bullet& bullet )
{
	StandardDeath( bullet, 100 );
	Die();
	bullet.ReducePower(1);
}



//--------------------
// Missile
//--------------------

Missile::Missile( vec2 const& pos, float heading ) : m_Timer(100)
{
	SetRadius(5.0f);
	SetFlags( flagCanHitBullet|
		flagCanHitPlayer|
		flagLocksLevel );
	MoveTo(pos);
	TurnTo(heading);
}


void Missile::Draw()
{
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glShadeModel(GL_SMOOTH);
	glBegin( GL_TRIANGLES );
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		glVertex2f( 0.0f, 5.0f );
		glColor4f( 1.0f, 1.0f, 1.0f, 0.0f );
		glVertex2f( -5.0f, -25.0f );
		glColor4f( 1.0f, 1.0f, 1.0f, 0.0f );
		glVertex2f( 5.0f, -25.0f );
	glEnd();
	glDisable( GL_BLEND );
}

void Missile::Tick()
{
	Forward( 4.0f );
	if(--m_Timer<0)
		Die();
}

void Missile::OnHitBullet( Bullet& bullet )
{
	StandardDeath( bullet, 10 );
	Die();
	bullet.ReducePower(1);
}

void Missile::Respawn()
{
	Die();
}



//--------------------
// HeavyThing
//--------------------

HeavyThing::HeavyThing()
{
	SetRadius(10.0f);
	SetFlags( flagCanHitBullet | flagCanHitPlayer | flagIndestructible );
	Respawn();
}


void HeavyThing::Respawn()
{
	m_Spd = vec2::ZERO;
	RandomPos();
}


void HeavyThing::Draw()
{
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );
	glBegin( GL_TRIANGLE_FAN );
		glColor3f( 0.5f, 0.5f, 0.5f );
		glVertex2f( 0.0f, 0.0f );
		glColor3f( 1.0f, 1.0f, 1.0f );
		glVertex2f( 8.0f, 16.0f );
		glColor3f( 1.0f, 1.0f, 1.0f );
		glVertex2f( 16.0f, 8.0f );
		glColor3f( 0.8f, 0.8f, 0.8f );
		glVertex2f( 16.0f, -8.0f );
		glColor3f( 0.6f, 0.6f, 0.6f );
		glVertex2f( 8.0f, -16.0f );
		glColor3f( 0.4f, 0.4f, 0.4f );
		glVertex2f( -8.0f, -16.0f );
		glColor3f( 0.2f, 0.2f, 0.2f );
		glVertex2f( -16.0f, -8.0f );
		glColor3f( 0.4f, 0.4f, 0.4f );
		glVertex2f( -16.0f, 8.0f );
		glColor3f( 0.6f, 0.6f, 0.6f );
		glVertex2f( -8.0f, 16.0f );
		glColor3f( 0.8f, 0.8f, 0.8f );
		glVertex2f( 8.0f, 16.0f );
	glEnd();
}

void HeavyThing::Tick()
{
	MoveWithinArena( *this, m_Spd );
//	m_Spd *= 0.99f;
}

void HeavyThing::OnHitBullet( Bullet& bullet )
{
	vec2 impulse = Rotate( vec2(0,1), bullet.Heading() );

	m_Spd += impulse * 4.0f;

	bullet.ReducePower( bullet.Power() );
}


#ifdef BLIP_DUDE

//--------------------
// Blip
//--------------------

Blip::Blip()
{
        SetRadius(10.0f);
        SetFlags( flagCanHitBullet|
                flagCanHitPlayer|
                flagLocksLevel );
        RandomPos();
}

void Blip::Draw()
{
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );
        glBegin( GL_TRIANGLES );
                glColor3f( 0.0f, 1.0f, 0.0f );
                glVertex2f( -5.0f, -5.0f );
                glVertex2f( 5.0f, -5.0f );
                glVertex2f( 5.0f, 5.0f );

        glColor3f( 1.0f, 0.0f, 0.0f );
                glVertex2f( -5.0f, -5.0f );
                glVertex2f( 5.0f, 5.0f );
                glVertex2f( -5.0f, 5.0f );
        glEnd();
}

void Blip::Tick()
{
//        Forward( 0.0f );

	vec2 v = g_Player->Pos()-Pos();
	float dist = ((v.x) * (v.x)) + ((v.y) * (v.y));
	float theta = atan2(v.x,v.y);
	TurnBy( 0.1f );
	m_Tick = m_Tick + 1;
	if (m_Tick > 10) 
		m_Tick = 0;
	if (m_Tick == 1) //Rnd() < 0.05)
	{
		g_Agents->AddDude( new BlastWave(Pos()) );
		if( dist < 50000.0f )
		{
			TurnTo (theta);
			Forward( -2.0f );
			g_Agents->AddDude( new Missile( Pos(), theta ) );
		}
		//defense
		BulletList::iterator it;
		for( it = g_Bullets.begin(); it!=g_Bullets.end(); ++it )
		{
			vec2 v2 = (*it)->Pos()-Pos();
//			float dist2 = ((v.x) * (v.x)) + ((v.y) * (v.y));
			float theta = atan2(v2.x,v2.y);
			if( dist < 50000.0f )
			{
				TurnTo (theta);
				Forward( -2.0f );
				g_Agents->AddDude( new Missile( Pos(),
				theta ) );
			}
		}
	}
}


void Blip::OnHitBullet( Bullet& bullet )
{
	StandardDeath( bullet, 100 );
	Die();
	bullet.ReducePower(1);
}


void Blip::Respawn()
{
        RandomPos();
}


#endif // BLIP_DUDE

//--------------------------------------
// EvilHoverDude (A bit like a flanker)
//--------------------------------------

EvilHoverDude::EvilHoverDude()
{
	SetRadius(5.0f);
	SetFlags( flagCanHitBullet|
		flagCanHitPlayer |
		flagLocksLevel );
	RandomPos();
}

void EvilHoverDude::Draw()
{
	//!!! TODO !!!
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );

	glBegin( GL_TRIANGLES );
	glColor3f( 0.5f, 0.0f, 0.7f );
	glVertex2f( 0.0f, 10.0f );
	glVertex2f( 5.0f, -10.0f );
	glVertex2f( -5.0f, -10.0f );
	glEnd();
}

void EvilHoverDude::Tick()
{
	vec2 v1 = g_Player->Pos();
	vec2 v2 = Pos();

	v1.x += g_CurrentLevel->ArenaRadius()/2 * (float)sin( g_Player->Heading()+0.78 );
	v1.y += g_CurrentLevel->ArenaRadius()/2 * (float)cos( g_Player->Heading()+0.78 );

	vec2 v((v1.x * 0.1f) + (v2.x * 0.9f), (v1.y * 0.1f) + (v2.y * 0.9f));

	MoveTo( v );

	TurnTo( g_Player->Heading()*0.1f + Heading()*0.9f );
}

void EvilHoverDude::OnHitBullet( Bullet& bullet )
{
	StandardDeath( bullet, 100 );
	Die();
	bullet.ReducePower(1);
}


void EvilHoverDude::Respawn()
{
	RandomPos();
}




//--------------------
// Amoeba
//--------------------


//static
const float Amoeba::InitialRadius = 20.0f;

Amoeba::Amoeba() : m_Timer(0.0f), m_Spd( vec2::ZERO )
{
	RandomPos();
	CommonInit( InitialRadius );
}


Amoeba::Amoeba( vec2 const& pos, vec2 const& spd, float radius ) :
	m_Timer(0.0f), m_Spd(spd)
{
	MoveTo(pos);
	CommonInit( radius );
}


void Amoeba::CommonInit( float radius )
{
	SetRadius( radius );
	SetFlags( flagCanHitBullet|
		flagCanHitPlayer|
		flagLocksLevel );

	const int numverts = 16;
	m_Verts.reserve(numverts);
	int i;
	for( i=0; i<numverts; ++i )
	{
		float theta = (float)i * (twopi/(float)numverts);

		AmoebaVert v;

		v.Heading.x = sin(theta);
		v.Heading.y = cos(theta);
		v.Phase = theta + Rnd( -0.5,0.5 );
		m_Verts.push_back(v);
	}
}


void Amoeba::Respawn()
{
	RandomPos();
}


void Amoeba::Draw()
{
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_SMOOTH );
	glBegin( GL_TRIANGLE_FAN );
		glColor4f( 0.2f, 0.0f, 0.7f, 0.2f );
		glVertex2f( 0.0f, 0.0f );



		float theta = m_Timer;
		const float r=Radius();
//		const float var=r*0.4f;
		const float var=r*0.6f;

		std::vector< AmoebaVert >::const_iterator it;

		it = m_Verts.begin();
		vec2 first = it->Heading * (r + sin(theta + it->Phase)*var);
		glColor4f( 0.6f, 0.0f, 0.0f, 0.8f );
		glVertex2f( first.x, first.y );
		++it;
		
		while( it!=m_Verts.end() )
		{
			vec2 v = it->Heading * (r + sin(theta + it->Phase)*var);
            glColor4f( 0.6f, 0.0f, 0.0f, 0.8f );
			glVertex2f( v.x, v.y );
			++it;
		}
		glColor4f( 0.6f, 0.0f, 0.0f, 0.8f );
		glVertex2f( first.x, first.y );
	glEnd();
}

void Amoeba::Tick()
{
	m_Timer += 0.08f;


	MoveBy( m_Spd );

	m_Spd *= 0.9f;


	const float m = 0.3f;
	m_Spd.x += Rnd(-m,m);
	m_Spd.y += Rnd(-m,m);


	vec2 homing = g_Player->Pos() - Pos();
	homing.Normalise();
	homing *= 0.1f;

	m_Spd += homing;
}

void Amoeba::OnHitBullet( Bullet& bullet )
{
	StandardDeath( bullet, 50 );

	float r=Radius();

	const float childprop=0.6f;	// child proportion
	if( r > InitialRadius * childprop * childprop )	// 2 generations
	{
		int i;
		for(i=0;i<3;++i)
		{
			float dir=Rnd(0,twopi);
			vec2 spd = Rotate( vec2( 1.0f, 2.0f ), dir );
		
			g_Agents->AddDude(
				new Amoeba( Pos()+(spd*4.0f), spd, Radius()*childprop) );
		}
	}
	
	Die();
	bullet.ReducePower(1);
}

//--------------------------------------
// Zipper
//--------------------------------------
Zipper::Zipper()
{
	SetRadius(10.0f);
	SetFlags( flagCanHitBullet|
		flagCanHitPlayer |
		flagLocksLevel );
	Respawn();
}

Zipper::Zipper(vec2 const& pos, float heading ) : m_Timer(0.0f), m_Spd(2.0f)
{
	SetRadius(16.0f);
	SetFlags( flagCanHitBullet|
		flagCanHitPlayer |
		flagLocksLevel );
	MoveTo(pos);
    TurnTo(heading); 
//	NewDirection();
}


void Zipper::Respawn()
{
    Die();
    return;
}

void Zipper::Tick()
{
    /*
	if( m_Spd.LenSq() < 0.5f*0.5f )
		NewDirection();

	if( MoveWithinArena( *this, m_Spd ) )
		NewDirection();

	m_Spd *= 0.99f;
*/
    Forward(m_Spd);
	TurnToward( g_Player->Pos(), m_Timer / 40.0f );
    m_Spd += 0.05f;
    if (m_Timer>4.0f)
    {
        Die();
    }

}



void Zipper::Draw()
{
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );

	glBegin( GL_TRIANGLES );
	glColor3f( 1.0f, 0.2f, 0.2f );
	glVertex2f( 0.0f, 16.0f );
	glColor3f( 1.0f, 0.2f, 0.2f );
	glVertex2f( 0.0f, 3.0f );
	glColor3f( 1.0f, 0.2f, 0.2f );
	glVertex2f( 10.0f, -3.0f );
	
	glColor3f( 0.7f, 0.0f, 0.0f );
	glVertex2f( 0.0f, 16.0f );
	glColor3f( 0.7f, 0.0f, 0.0f );
	glVertex2f( -10.0f, -3.0f );
	glColor3f( 0.7f, 0.0f, 0.0f );
	glVertex2f( 0.0f, 3.0f );
	glEnd();
}


void Zipper::OnHitBullet( Bullet& bullet )
{
	StandardDeath( bullet, 100 );
	Die();
	bullet.ReducePower(1);
}

//--------------------------------------
// ZipperMat
//--------------------------------------


const int ZipperMat::HitPoints = 20;
const int ZipperMat::NumShots = 20;
const float ZipperMat::FireTime = 2.0f;
const float ZipperMat::ChargeTime = 2.0f;

ZipperMat::ZipperMat() : m_Life( HitPoints )
{
	SetRadius(10.0f);
	SetFlags( flagCanHitBullet| flagCanHitPlayer | flagLocksLevel );
	Respawn();
}


void ZipperMat::Respawn()
{
	RandomPos();
	m_Spd = Rotate( vec2(0.0f,2.0f), Rnd( -pi, pi ) );
	m_Timer = 0.0f;
	m_Flash = 0.0f;
    m_State = charging;
    m_Inflation = 0.0f;
    m_FireCnt = 0;
}

void ZipperMat::Tick()
{
	MoveWithinArena( *this, m_Spd );
	TurnBy( twopi/256.0f );
	m_Flash *= 0.95f;

    m_Spd *= 0.99f;
	m_Timer += 1.0f/TARGET_FPS;
    switch(m_State) {
        case charging:
            m_Inflation = (m_Timer / ChargeTime);
        	if( m_Timer >= ChargeTime )
            {
                m_State = firing;
                m_FireCnt = 0;
                m_Timer = 0;
                m_Inflation = 1.0f;
            }
            break;
        case firing:
            m_Inflation = ((float)(NumShots-m_FireCnt) / (float)NumShots);
        	if( m_Timer >= FireTime/NumShots )
        	{
                m_Spd += Rotate(vec2(0,-0.75f),Heading());  // thrust!

                vec2 pos = Pos() + Rotate(vec2(0,16.0f),Heading());
        		g_Agents->AddDude( new Missile( pos,Heading() ) );
        		m_Timer = 0.0f;
                if(++m_FireCnt >= NumShots ) {
                    m_State = charging;
                }
        	}
            break;
    }
    SetRadius(8.0f + (4.0f * m_Inflation));
}




void ZipperMat::Draw()
{
    Colour basec;
    Colour chargecolour(0,0,1);
    Colour firecolour(1.0f,0,0);
    float s = Radius();
    basec = ColourLerp( chargecolour, firecolour, m_Inflation );

	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	//glShadeModel( GL_FLAT );
	glShadeModel( GL_SMOOTH );

//	Colour c1 = ColourLerp( chargecolour, Colour::WHITE, m_Flash );
	Colour c2 = ColourLerp( basec, Colour::WHITE, m_Flash );
	Colour c1 = c2;
	glBegin( GL_QUADS );
	glColor3f( c1.r, c1.g, c1.b );
	glVertex2f( -s, -s*0.5f  );
	glColor3f( c1.r, c1.g, c1.b );
	glVertex2f( s, -s*0.5f );
	glColor3f( c2.r, c2.g, c2.b );
	glVertex2f( s/2, s );
	glColor3f( c2.r, c2.g, c2.b );
	glVertex2f( -s/2, s );
	glEnd();

    // draw glow
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, g_Textures[TX_BLUEGLOW]->ID() );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glEnable( GL_BLEND );
	glBlendFunc( GL_ONE, GL_ONE );

    float glowf = (m_State==firing) ? m_Inflation : (m_Inflation*m_Inflation*m_Inflation);
	glColor3f( glowf, glowf, glowf  );

    s = s * 1.5f * Rnd(0.9f,1.10f);
	glBegin( GL_QUADS );
		glTexCoord2f( 0.0f, 1.0f );
		glVertex2f( -s, s+10.0f );
		glTexCoord2f( 1.0f,1.0f);
		glVertex2f( s, s+10.0f );
		glTexCoord2f( 1.0f, 0.0f);
		glVertex2f( s, 10.0f-s );
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f( -s, 10.0f-s );
	glEnd();

}


void ZipperMat::OnHitBullet( Bullet& bullet )
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
	}
	bullet.ReducePower( absorbed );
}



//--------------------------------------
// Tank
//--------------------------------------

Tank::Tank() : m_Life( 20 ), m_FireTimer(0)
{
	SetRadius(10.0f);
	SetFlags( flagCanHitBullet| flagCanHitPlayer | flagLocksLevel );
	Respawn();
}


void Tank::Respawn()
{
	RandomPos();
	TurnTo( Rnd(-pi,pi) );
	m_Flash = 0.0f;
}



void Tank::Tick()
{
	TurnToward( g_Player->Pos(), 0.015f );
	Forward( 1.0f );

	if( ++m_FireTimer > 40 )
	{
        vec2 pos = Pos() + Rotate(vec2(0,16.0f),Heading());
		g_Agents->AddDude( new Missile( pos, Heading() ) );
//		SoundMgr::Inst().Play( SFX_LAUNCH );
		m_FireTimer = 0;
	}
	m_Flash *= 0.8f;
}



void Tank::Draw()
{
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );

	Colour c = ColourLerp(
		Colour( 0.5f, 0.1f, 0.0f ),
		Colour::WHITE,
		m_Flash );

	glColor3f( c.r, c.g, c.b );
	glBegin( GL_TRIANGLES );
	glVertex2f( 0.0f, 10.0f );
	glVertex2f( -20.0f, -10.0f );
	glVertex2f( 20.0f, -10.0f );
	
	glEnd();
}


void Tank::OnHitBullet( Bullet& bullet )
{
	int absorbed = std::min( bullet.Power(), m_Life );
	m_Life -= absorbed;
	if( m_Life <= 0 )
	{
		BigArseDeath( bullet, 500 );
		Die();
	}
	else
	{
		SoundMgr::Inst().Play( SFX_SMALLTHUD );
		int i;
		m_Flash = 1.0f;
		for(i=0;i<5;++i)
			g_Agents->AddDude(new Frag(Pos()));
	}
	bullet.ReducePower( absorbed );
}


//--------------------------------------
// Pacman
//--------------------------------------

Pacman::Pacman() : m_MouthAnim(0.0f), m_Velocity( vec2::ZERO )
{
	SetRadius(10.0f);
	SetFlags( flagCanHitBullet| flagCanHitPlayer | flagLocksLevel );
	Respawn();
}


void Pacman::Respawn()
{
	RandomPos();
	NewDirection();
}

void Pacman::Tick()
{
	if( --m_MoveTimer > 0 )
		MoveBy( m_Velocity );
	else
	{
		NewDirection();
	}

	m_MouthAnim += 0.04f;
	if( m_MouthAnim > 1.0f )
	{
		// wacka wacka
		m_MouthAnim = 0.0f;
	}
}


// pick new direction
void Pacman::NewDirection()
{
	const float spd=2.5f;
	vec2 v = g_Player->Pos()-Pos();
	float theta = atan2(v.x,v.y);

	// add some randomness
	theta += Rnd( -pi/3.0f, pi/3.0f );
	if( theta < -pi )
		theta += twopi;
	else if( theta > pi )
		theta -= twopi;

	// quantize to manhatten direction
	if( theta >= -(3.0f*pi)/4.0f && theta < -pi/4.0f )
		theta = twopi*0.75f;		// left
	else if( theta >= -pi/4.0f && theta < pi/4.0f )
		theta = 0.0f;			// up
	else if( theta >= pi/4.0f && theta < (3.0f*pi)/4.0f )
		theta = twopi*0.25f;	// right
	else
		theta = pi;	//down

	// go!
	m_Velocity = Rotate( vec2(0.0f,spd), theta );
	TurnTo( theta );
	m_MoveTimer = 20;
}



void Pacman::Draw()
{
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );

	glColor3f( 1.0f, 0.8f, 0.0f );
	glBegin( GL_TRIANGLE_FAN );
	glVertex2f( 0.0f, 0.0f );
	float theta;
	const float divisions = 32.0f;


	float openangle;
	if( m_MouthAnim > 0.5f )
	{
		openangle = (1.0f-m_MouthAnim) * pi/2.0f;
	}
	else
	{
		openangle = m_MouthAnim * pi/2.0f;
	}
	const float r=10.0f;
	for( theta=openangle;
		theta<=twopi-openangle;
		theta += twopi/divisions )
	{
		glVertex2f( r*sin(theta), r*cos(theta) );
	}
	
	glEnd();
}


void Pacman::OnHitBullet( Bullet& bullet )
{
	StandardDeath( bullet, 50 );
	Die();
	bullet.ReducePower( 1 );
}


//--------------------------------------
// Invader
//--------------------------------------

int Invader::s_CurrentCount=0;
int Invader::s_LevelStartCount;

Invader::Invader()
{
	m_AnimToggle = 0;
	m_Type = RndI( 0,2 );
	SetRadius(7.0f);
	SetFlags( flagCanHitBullet| flagCanHitPlayer | flagLocksLevel );
	Respawn();
	++s_CurrentCount;
}

Invader::~Invader()
{
	--s_CurrentCount;
}

void Invader::Respawn()
{
	m_Timer=0;
	RandomPos();
}

void Invader::Tick()
{
	assert( s_CurrentCount > 0 );
	assert( s_CurrentCount <= s_LevelStartCount );
	
	// turn toward player
	vec2 v = g_Player->Pos()-Pos();
	float theta = atan2(v.x,v.y);
	float d = theta-Heading();

	while( d < -pi )
		d+= twopi;
	while( d > pi )
		d-= twopi;

	if( d > 0.2f )
		TurnBy( 0.2f );
	else if( d < -0.2f )
		TurnBy( -0.2f );
	else
		TurnTo( theta );

	// delay between steps depends on how many invaders are left
	// (compared to the number at the start of the level)
	const int threshold = (40*(s_CurrentCount-1))/s_LevelStartCount;
	++m_Timer;
	if( m_Timer > threshold )
	{
		// do a step
		m_Timer=0;
		Forward( 7.0f );
		m_AnimToggle = m_AnimToggle ? 0:1;
	}
}


void Invader::Draw()
{
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, g_Textures[TX_INVADER]->ID() );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

//	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	glEnable( GL_BLEND );
	glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_COLOR );
	
	glShadeModel(GL_FLAT);
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

	float u = m_AnimToggle ? 0.0f:0.5f;
	float v=0.0f + 0.25f*(float)m_Type;

	glBegin( GL_QUADS );
		glTexCoord2f( u, v+0.25f );
		glVertex2f( -12.0f, 12.0f );
		glTexCoord2f( u+0.5f,v+0.25f);
		glVertex2f( 12.0f, 12.0f );
		glTexCoord2f( u+0.5f, v);
		glVertex2f( 12.0f, -12.0f );
		glTexCoord2f(u,v);
		glVertex2f( -12.0f, -12.0f );
	glEnd();
}


void Invader::OnHitBullet( Bullet& bullet )
{
	StandardDeath( bullet, 50 );
	Die();
	bullet.ReducePower( 1 );
}

//static
// called just before level starts
void Invader::Callibrate()
{
	s_LevelStartCount = s_CurrentCount;
}




//--------------------------------------
// Snake
//--------------------------------------


Snake::Snake( Snake* prev ) :
	m_HitPoints(4),
	m_Glow(0.0f),
	m_PrevSeg(prev),
	m_NextSeg(0)
{
	if( m_PrevSeg )
		m_PrevSeg->m_NextSeg = this;

	SetRadius(16.0f);
	SetFlags( flagCanHitBullet| flagCanHitPlayer | flagLocksLevel );

//	Respawn();
}


Snake::~Snake()
{
	if( m_PrevSeg )
		m_PrevSeg->m_NextSeg = 0;
	if( m_NextSeg )
		m_NextSeg->m_PrevSeg = 0;
}

void Snake::Respawn()
{
	m_Glow = 0.0f;

	if( m_PrevSeg )
		return;	// not the head

	RandomPos();
	TurnTo( Rnd( -pi, pi ) );

	Snake* s=this;
	while( s )
	{
		s->MoveTo( Pos() );
		s->TurnTo( Heading() );
		s->m_History.clear();
		s = s->m_NextSeg;
	}
}

void Snake::Tick()
{
	m_Glow *= 0.8f;

	if( m_PrevSeg )
		return;

	Snake* s = this;

	// find tail of snake
	while( s->m_NextSeg )
	{
		// push current position onto history deque
		if( s->m_History.size() >= HISTORY_COUNT )
			s->m_History.pop_back();
		s->m_History.push_front( HistoryEntry() );
		s->m_History.front().pos = s->Pos();
		s->m_History.front().heading = s->Heading();
		
		s = s->m_NextSeg;
	}

	// move back up snake setting positions
	while( s->m_PrevSeg )
	{
		s->MoveTo( s->m_PrevSeg->m_History.back().pos );
		s->TurnTo( s->m_PrevSeg->m_History.back().heading );
		s = s->m_PrevSeg;
	}

	// move the head
	TurnToward( g_Player->Pos(), twopi/128.0f );
	Forward( Radius() / 6.0f );
}


void Snake::Draw()
{
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );

	const int divisions = 32;

	Colour c;
	if( !m_PrevSeg )
		c = Colour( 0.7f, 0.0f, 0.5f );	// head
	else
		c = Colour( 0.4f, 0.0f, 0.6f );	// body

	c = ColourLerp( c, Colour::WHITE, m_Glow );
	glColor3f( c.r, c.g, c.b );

	glBegin( GL_TRIANGLE_FAN );
	glVertex2f( 0.0f, 0.0f );

	float theta;
	int div;
	for( div=0; div<=divisions; ++div )
	{
		theta = - (float)div * twopi / (float)divisions;
		glVertex2f( Radius()*sin(theta), Radius()*cos(theta) );
	}
	glEnd();
}


void Snake::OnHitBullet( Bullet& bullet )
{

	if( m_PrevSeg )
	{
		// not the head
		if( Rnd() < 0.2f )
		{
			SoundMgr::Inst().Play( SFX_DULLBLAST );
			g_Agents->AddDude( new Wiggler( Pos() ) );
		}
		bullet.ReducePower( bullet.Power() );
		return;
	}

	// head.

	m_Glow = 1.0f;
	int absorbed = std::min( bullet.Power(), m_HitPoints );
	m_HitPoints -= absorbed;
	bullet.ReducePower( absorbed );
	if( m_HitPoints > 0 )
		return;

	// head destroyed!
	if( m_NextSeg )
	{
		m_NextSeg->m_PrevSeg = 0;
		m_NextSeg->TurnBy( Rnd( pi-twopi/64.0f, pi+twopi/64.0f ) );
		m_NextSeg = 0;
	}

	BigArseDeath( bullet, 100 );
	Die();
}


void Snake::Create( std::list<Dude*>& newdudes )
{
	const int length=15;
	
	int i;
	Snake* head = new Snake(0);
	newdudes.push_back( head );

	Snake* prev = head;
	for( i=0; i<length; ++i )
	{
		Snake* s = new Snake(prev);
		newdudes.push_back( s );
		prev = s;
	}

	// now snake is complete we can position the head (which also sets up all the
	// trailing segments)
	head->Respawn();
}

//--------------------
// MineLayer
//--------------------

const float MineLayer::s_DropInterval = 2.0f;

MineLayer::MineLayer() : m_Life(10)
{
	SetRadius(16.0f);
	SetFlags( flagCanHitBullet| flagCanHitPlayer| flagLocksLevel );
	Respawn();
}

void MineLayer::OnHitBullet( Bullet& bullet )
{
	m_Flash = 1.0f;
	int absorbed = std::min( m_Life, bullet.Power() );
	m_Life -= absorbed;
	if( m_Life > 0 )
	{
		int i;
		for(i=0;i<5;++i)
			g_Agents->AddDude(new Frag(Pos()));
	}
	else
	{
		StandardDeath( bullet, 50 );
		Die();
	}

	bullet.ReducePower( absorbed );
}

void MineLayer::Respawn()
{
	m_Spd = Rotate( vec2(0.0f, 2.0f ), Rnd(0.0f,twopi) );
//	m_Spd = vec2::ZERO;
	RandomPos();
	m_Timer = Rnd( 0.0f, s_DropInterval );
	m_Flash = 0.0f;
}


void MineLayer::Tick()
{
	MoveWithinArena( *this, m_Spd );
	TurnBy( pi/32.0f );
	Forward( 1.0f );
	m_Timer += 1.0f/TARGET_FPS;
	
//	if( m_Timer >= s_DropInterval )
//	{
		if( Rnd() < 0.01f )
		{
			g_Agents->AddDude( new Mine( Pos() ) );
		}
		if( m_Timer >= s_DropInterval*2.0f )
			m_Timer = 0.0f;
//	}
	m_Flash *= 0.95f;
}

void MineLayer::Draw()
{
	Colour c = ColourLerp(
		Colour(1.0, 1.0, 0.0),
		Colour::WHITE,
		m_Flash );

	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );

    glColor3f(c.r, c.g, c.b ); // yellow 
	glBegin(GL_TRIANGLE_FAN );
		glVertex2f( 0.0f, 0.0f );
		glVertex2f(-8.0,-8.0);
		glVertex2f(-16.0, 0.0);
		glVertex2f(-8.0, 8.0);
		glVertex2f( 8.0, 8.0);
		glVertex2f( 16.0, 0.0);
		glVertex2f( 8.0,-8.0);
		glVertex2f(-8.0,-8.0);
	glEnd();
}


//--------------------
// Mine
//--------------------

//static
const float Mine::s_FuseTime = 2.0f;	// seconds
const float Mine::s_ExplosionTime = 1.0f;	// seconds
const float Mine::s_ExplosionR0 = 5.0f;
const float Mine::s_ExplosionR1 = 100.0f;

Mine::Mine(vec2 const& pos) :
	m_Cyc(0.0f),
	m_Timer( 0.0f ),
	m_Exploding( false )
{
	SetRadius(6.0f);
	SetFlags( 0 );
	MoveTo(pos);
}

void Mine::Draw()
{
	static const Colour evilcolours[] =
	{
		Colour( 0.4f, 0.0f, 0.0f),
		Colour( 0.9f, 0.0f, 0.9f),
	};
	static const ColourRange evilrange( evilcolours, 2 );

	static const Colour explodecolours[] =
	{
		Colour( 1.0f, 1.0f, 1.0f, 1.0f),
		Colour( 1.0f, 1.0f, 0.0f, 1.0f),
		Colour( 1.0f, 0.0f, 0.0f, 1.0f),
		Colour( 0.0f, 0.0f, 0.5f, 0.0f),
	};
	static const ColourRange exploderange( explodecolours, 4 );

	m_Cyc += 0.1f;
	if( m_Exploding )
	{
		float factor = m_Timer/ s_ExplosionTime;
		Colour c=exploderange.Get( factor, true );
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glDisable( GL_TEXTURE_2D );
		glShadeModel( GL_SMOOTH );
		glBegin( GL_TRIANGLE_FAN );
		glColor4f( 1.0f,0.0f,0.0f, (1.0f-factor) * 0.5f );
		glVertex2f( 0.0f, 0.0f );
		float theta;
		const float step = pi/16.0f;
		for( theta=0.0f; theta<twopi+step; theta += step )
		{
			vec2 v = Rotate( vec2(0.0f,Radius()), theta );
			glColor4f( c.r, c.g, c.b, c.a );
			glVertex2f( v.x, v.y );
		}
		glEnd();
	}
	else
	{
		float factor = m_Timer / s_FuseTime;

		// not exploding
		glDisable( GL_BLEND );
		Colour c1=evilrange.Get(m_Cyc,false);
		c1 = ColourLerp( c1, Colour::WHITE, factor*factor*factor );

		Colour c2=evilrange.Get(m_Cyc+0.50f,false);
		c2 = ColourLerp( c2, Colour::WHITE, factor*factor*factor );
		glBegin( GL_LINES );
            glColor3f( c1.r, c1.g, c1.b );
			glVertex2f( 0.0f, 5.0f );
            glColor3f( c1.r, c1.g, c1.b );
			glVertex2f( 0.0f,-5.0f );

            glColor3f( c1.r, c1.g, c1.b );
			glVertex2f( 5.0f, 0.0f );
            glColor3f( c1.r, c1.g, c1.b );
			glVertex2f(-5.0f, 0.0f );

            glColor3f( c2.r, c2.g, c2.b );
			glVertex2f(-3.0f,-3.0f );
            glColor3f( c2.r, c2.g, c2.b );
			glVertex2f( 3.0f, 3.0f );

            glColor3f( c2.r, c2.g, c2.b );
			glVertex2f( 3.0f,-3.0f );
            glColor3f( c2.r, c2.g, c2.b );
			glVertex2f(-3.0f, 3.0f );
		glEnd();
	}
}


void Mine::Tick()
{
	m_Timer += 1.0f/TARGET_FPS;

	if( m_Exploding )
	{
		if( m_Timer >= s_ExplosionTime )
			Die();
		else
		{
			float factor = (s_ExplosionTime-m_Timer) / s_ExplosionTime;
			factor *= factor*factor;
			SetRadius( s_ExplosionR0*factor + (1.0f-factor)*s_ExplosionR1 );

			// last 50% of explosion doesn't hurt player
			if( m_Timer >= s_ExplosionTime*0.5f )
			{
				SetFlags(0);
			}
		}
	}
	else
	{
	
		TurnBy( pi/64.0f );

		if( m_Timer >= s_FuseTime )
		{
			// kaboom.
			SoundMgr::Inst().Play( SFX_BIGEXPLOSION );
			m_Timer = 0.0f;
			m_Exploding = true;
			SetFlags( flagCanHitPlayer );
		}
	}
}


void Mine::OnHitBullet( Bullet& bullet )
{
	if( !m_Exploding )
	{
		StandardDeath( bullet, 20 );
		Die();
	}
	bullet.ReducePower( 1 );
}

void Mine::Respawn()
{
	Die();
}


//--------------------
// Agitator
//--------------------

const float Agitator::s_RadiusMin = 10.0f;
const float Agitator::s_RadiusMax = 16.0f;
const float Agitator::s_MaxJitter = 3.0f;
const float Agitator::s_MaxSpd = 5.0f;
const float Agitator::s_HitDamage = 0.25f;

Agitator::Agitator() :
	m_Agitation( 0.0f ),
	m_Flash( 0.0f )
{
	SetFlags( flagCanHitBullet|
		flagCanHitPlayer|
		flagLocksLevel );
	Respawn();
}

void Agitator::Respawn()
{
	RandomPos();
	m_Agitation = 0.0f;
	m_Vel = Rotate( vec2( 0.0f, 0.5f ), Rnd(0,twopi) );
	SetRadius( s_RadiusMin + m_Agitation*(s_RadiusMax-s_RadiusMin) );
}


void Agitator::Draw()
{

	float j = m_Agitation*8.0f;
	vec2 offset( Rnd(-j,j), Rnd(-j,j) );

	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );
	glColor3f( 1.0f, 0.3f, 0.3f );
	DrawCircle( offset, Radius() );

	Colour const c( 1.0f, 1.0f, 1.0f );
	glColor3f( c.r, c.g, c.b );
	DrawCircle( vec2::ZERO, Radius() );
	
}

void Agitator::Tick()
{
	if( m_Agitation > 0.0f )
	{
		// go toward player
		vec2 mv = ( g_Player->Pos() - Pos() );
		mv.Normalise();
		mv *= m_Agitation*s_MaxSpd;

		// jitter
		float j = m_Agitation*s_MaxJitter;
		mv += vec2( Rnd(-j,j), Rnd(-j,j) );
		MoveBy( mv );

//		m_Agitation *= 0.99f;
//		if( m_Agitation < 0.01f )
//		{
//			m_Agitation = 0.0f;
//			m_Vel = Rotate( vec2( 0.0f, 1.0f ), Rnd(0,twopi) );
//		}
	}
	else
	{
		// just drift...
		MoveWithinArena( *this, m_Vel );
	}
	SetRadius( s_RadiusMin + m_Agitation*(s_RadiusMax-s_RadiusMin) );

}

void Agitator::OnHitBullet( Bullet& bullet )
{
	bullet.ReducePower(1);

	m_Agitation += s_HitDamage;
	if( m_Agitation >= 1.0f )
	{
		StandardDeath( bullet, 100 );
		Die();
	}
}



//--------------------
// Divider
//--------------------

Divider::Divider()
{
	SetRadius(10.0f);
	SetFlags( flagCanHitBullet | flagCanHitPlayer | flagIndestructible );
	Respawn();
}


void Divider::Respawn()
{
	m_Spd = vec2::ZERO;
	RandomPos();
	m_RotSpd = twopi / 1024.0f;

	if( Rnd( -1,1 ) < 0 )
		m_RotSpd = -m_RotSpd;
	m_Timer = 2.0f;

	m_HappyOrbit = Rnd( 30.0f, 100.0f );
}


void Divider::Draw()
{
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );

//	float a=16.0f;
//	float b=8.0f;
	glBegin( GL_TRIANGLES );
		// light
		glColor3f( 0.8f, 0.8f, 1.0f );
		glVertex2f( -16.0f, 8.0f );
		glColor3f( 0.8f, 0.8f, 1.0f );
		glVertex2f( -8.0f, 8.0f );
		glColor3f( 0.8f, 0.8f, 1.0f );
		glVertex2f( -4.0f, 0.0f );

		glColor3f( 0.5f, 0.5f, 0.8f );
		glVertex2f( -8.0f, 8.0f );
		glColor3f( 0.5f, 0.5f, 0.8f );
		glVertex2f( 8.0f, 8.0f );
		glColor3f( 0.5f, 0.5f, 0.8f );
		glVertex2f( -4.0f, 0.0f );

		glColor3f( 0.5f, 0.5f, 0.8f );
		glVertex2f( 8.0f, 8.0f );
		glColor3f( 0.5f, 0.5f, 0.8f );
		glVertex2f( 4.0f, 0.0f );
		glColor3f( 0.5f, 0.5f, 0.8f );
		glVertex2f( -4.0f, 0.0f );

		glColor3f( 0.2f, 0.2f, 0.6f );
		glVertex2f( 8.0f, 8.0f );
		glColor3f( 0.2f, 0.2f, 0.6f );
		glVertex2f( 16.0f, 8.0f );
		glColor3f( 0.2f, 0.2f, 0.6f );
		glVertex2f( 4.0f, 0.0f );

		// dark
		glColor3f( 0.5f, 0.5f, 0.7f );
		glVertex2f( -16.0f, -8.0f );
		glColor3f( 0.5f, 0.5f, 0.7f );
		glVertex2f( -8.0f, -8.0f );
		glColor3f( 0.5f, 0.5f, 0.7f );
		glVertex2f( -4.0f, 0.0f );

		glColor3f( 0.2f, 0.2f, 0.5f );
		glVertex2f( -8.0f, -8.0f );
		glColor3f( 0.2f, 0.2f, 0.5f );
		glVertex2f( 8.0f, -8.0f );
		glColor3f( 0.2f, 0.2f, 0.5f );
		glVertex2f( -4.0f, 0.0f );

		glColor3f( 0.2f, 0.2f, 0.5f );
		glVertex2f( 8.0f, -8.0f );
		glColor3f( 0.2f, 0.2f, 0.5f );
		glVertex2f( 4.0f, 0.0f );
		glColor3f( 0.2f, 0.2f, 0.5f );
		glVertex2f( -4.0f, 0.0f );

		glColor3f( 0.0f, 0.0f, 0.3f );
		glVertex2f( 8.0f, -8.0f );
		glColor3f( 0.0f, 0.0f, 0.3f );
		glVertex2f( 16.0f, -8.0f );
		glColor3f( 0.0f, 0.0f, 0.3f );
		glVertex2f( 4.0f, 0.0f );

	
	glEnd();
}

void Divider::Tick()
{
	MoveWithinArena( *this, m_Spd );
	m_Spd *= 0.98f;
	TurnBy( m_RotSpd );

	m_Timer += 1.0f/TARGET_FPS;
	if( m_Timer > 6.0f )
	{
		Beam::Params beamparams;
		beamparams.warmuptime = 2.0f;
		beamparams.ontime = 2.0f;
		beamparams.width = 28.0f;
		beamparams.length = 1200.0f;
		
		Beam* b1 = new Beam( *this, vec2( 0.0f, 8 ), 0.0f, &beamparams );
		g_Agents->AddDude( b1 );

		Beam* b2 = new Beam( *this, vec2( 0.0f, -8 ), pi, &beamparams );
		g_Agents->AddDude( b2 );
		m_Timer = 0.0f;
	}

	// if outside happyorbit, move inward slowly
	if( Pos().LenSq() > m_HappyOrbit*m_HappyOrbit )
	{
		vec2 accel = -Pos();
		accel.Normalise();
		accel *= 0.01f;
		m_Spd += accel;
	}
}

void Divider::OnHitBullet( Bullet& bullet )
{
	vec2 impulse = Rotate( vec2(0,1), bullet.Heading() );
	m_Spd += impulse * 1.0f;
	bullet.ReducePower( bullet.Power() );
}


//--------------------
// Puffer
//--------------------


const float Puffer::s_RadiusMin = 8.0f;

Puffer::Puffer()
{
	SetFlags( flagCanHitBullet|
		flagCanHitPlayer|
		flagLocksLevel );
	Respawn();
}

void Puffer::Respawn()
{
    m_Hit=0;
    m_ExpansiveVel = 0.0f;
	RandomPos();
	m_Vel = Rotate( vec2( 0.0f, 0.5f ), Rnd(0,twopi) );
	SetRadius( s_RadiusMin );
}


void Puffer::Draw()
{
	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );

    float f = ((float)m_Hit)/3.0f;
	Colour const c( 1.0f, 1.0f-f, 1.0f-f );
	glColor3f( c.r, c.g, c.b );
	DrawCircle( vec2::ZERO, Radius() );
}


void Puffer::Tick()
{
    MoveWithinArena( *this, m_Vel );

    float r = Radius();
    r += m_ExpansiveVel;
    SetRadius(r);

    const float k=0.0075f;   // spring constant
    const float mass = 0.75f;
    m_ExpansiveVel *= 0.90f;    // friction
    m_ExpansiveVel += mass * -k * (r-s_RadiusMin);




    /*
    m_Puff += (m_TargetPuff - m_Puff) * 0.1f;
	SetRadius( s_RadiusMin + m_Puff*(s_RadiusMax-s_RadiusMin) );
    if (m_Puff> 0.95f)
    {
//		StandardDeath( bullet, 100 );
		Die();
    }
    */
}

void Puffer::OnHitBullet( Bullet& bullet )
{

    if (Radius()>35.0f) {
        m_Hit += bullet.Power();
        if( m_Hit > 4) {
            StandardDeath( bullet.Owner(), 100 );
            Die();
            return;
        }
    }
    else
    {
        m_Hit = 0;
    }

    // apply impulse
    m_ExpansiveVel += 1.0f * (float)bullet.Power();
	bullet.ReducePower(bullet.Power());


    

}

