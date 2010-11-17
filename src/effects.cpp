

//#include "bonuses.h"
#include "agentmanager.h"
#include "bullets.h"
#include "colours.h"
#include "drawing.h"
#include "effects.h"
#include "level.h"
#include "player.h"
#include "soundmgr.h"
//#include "texture.h"
#include "zig.h"

#include <assert.h>
#include <SDL.h>
#include <SDL_opengl.h>


//--------------------
// Frag
//--------------------
Frag::Frag( vec2 const& pos ) :
	m_Spd(Rnd(1.0f, 5.0f)),
	m_Life(1.0f),
	m_LifeDecay(Rnd(0.02f, 0.05f))
{
	SetFlags( flagIndestructible );
	MoveTo(pos);
	TurnTo(Rnd(0.0f, twopi));
}

void Frag::Draw()
{
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glColor4f( 1.0f, 1.0f, 0.0f, m_Life );
	glBegin( GL_LINES );
//		glColor3f( m_Life, m_Life, 0.0f );
		glVertex2f( 0.0f, 15.0f );
		glVertex2f( 0.0f, -15.0f );
	glEnd();
}

void Frag::Tick()
{
	Forward(m_Spd);
//	TurnBy( pi/8.0f );
	m_Life -= m_LifeDecay;
	if( m_Life < 0.0f )
		Die();
}



//--------------------
// BlastWave
//--------------------
// NEEDS TWEAKING!

BlastWave::BlastWave( vec2 const& pos ) :
	m_Radius(8.0f),
	m_ExpandRate(20.0f),
	m_InnerColours(0),
	m_OuterColours(0)
{
	static const Colour innercolours[] =
	{
		Colour( 0.5f, 0.0f, 0.0f, 0.2f ),
		Colour( 0.5f, 0.0f, 0.2f, 0.1f ),
		Colour( 0.0f, 0.0f, 0.4f, 0.0f )
	};
	static const Colour outercolours[] =
	{
		Colour( 1.0f, 0.8f, 0.0f, 0.5f ),
		Colour( 0.9f, 0.5f, 0.0f, 0.2f ),
		Colour( 0.8f, 0.0f, 0.5f, 0.0f )
	};

	m_InnerColours = new ColourCycle( innercolours,3 );
	m_OuterColours = new ColourCycle( outercolours,3 );
	MoveTo(pos);

	SoundMgr::Inst().Play( SFX_DULLBLAST );

	SetFlags( flagIndestructible );
}

BlastWave::~BlastWave()
{
	delete m_InnerColours;
	delete m_OuterColours;
}

void BlastWave::Draw()
{
	const float outer=m_Radius;
	const float inner=m_Radius/2.0f;
	const int segments = 32;

	glShadeModel(GL_SMOOTH);
	glEnable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );


	vec2 innerverts[ segments+1 ];
	vec2 outerverts[ segments+1 ];

	// calculate it
	float theta=0.0f;
	int i=0;
	while( i<segments+1 )
	{
		innerverts[ i ].x = inner*sin(theta);
		innerverts[ i ].y = inner*cos(theta);
		outerverts[ i ].x = outer*sin(theta);
		outerverts[ i ].y = outer*cos(theta);
		++i;
		theta += twopi/segments;
	}

	// draw it
	glBegin(GL_QUADS);
	for( i=0; i<segments; ++i )
	{
		Colour const& c1 = m_InnerColours->Current();
//		Colour const& c2 = c1;
		Colour const& c2 = m_OuterColours->Current();

		glColor4f( c1.r, c1.g, c1.b, c1.a );
		glVertex2f( innerverts[i].x, innerverts[i].y );
		glColor4f( c1.r, c1.g, c1.b, c1.a );
		glVertex2f( innerverts[i+1].x, innerverts[i+1].y );

		glColor4f( c2.r, c2.g, c2.b, c2.a );
		glVertex2f( outerverts[i+1].x, outerverts[i+1].y );
		glColor4f( c2.r, c2.g, c2.b, c2.a );
		glVertex2f( outerverts[i].x, outerverts[i].y );
	}
	glEnd();
}

void BlastWave::Tick()
{
	m_ExpandRate *= 0.9f;
	if( m_ExpandRate < 0.0f )
		m_ExpandRate = 0.0f;
	m_Radius += m_ExpandRate;

	m_InnerColours->Tick( 0.02f );
	bool done = m_OuterColours->Tick( 0.02f );
	if( done )
		Die();
}


//--------------------
// DudeSpawnEffect
//--------------------

DudeSpawnEffect::DudeSpawnEffect( Dude* d ) :
	m_Dude(d),
	m_Timer(0.0f)
{
	d->Lock();

	SetFlags( flagLocksLevel | flagIndestructible );
	MoveTo(m_Dude->Pos() );
	TurnTo( halfpi );	//Rnd(0.0,twopi) );
}

DudeSpawnEffect::~DudeSpawnEffect()
{
	if( m_Dude )
		m_Dude->Unlock();
}

void DudeSpawnEffect::Draw()
{
	static const Colour rainbowcolours[] =
	{
		Colour( 1.0f, 0.0f, 0.0f),
		Colour( 0.5f, 0.5f, 0.0f),
		Colour( 0.0f, 1.0f, 0.0f),
		Colour( 0.0f, 0.5f, 0.5f),
		Colour( 0.0f, 0.0f, 1.0f),
		Colour( 0.5f, 0.0f, 0.5f),
	};

	static const ColourRange rainbowrange( rainbowcolours, 6 );

	glShadeModel(GL_SMOOTH);
	glEnable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );

	glBlendFunc( GL_SRC_ALPHA, GL_ONE ); //GL_ONE_MINUS_SRC_ALPHA );

	float s = (1.0f - m_Timer);
	Colour const c = rainbowrange.Get( m_Timer*4, false );
	float x1 = 5.0f + s*200.0f;
	float x2 = 10.0f + s*500.0f;
	glBegin(GL_TRIANGLES);
	
		glColor4f( c.r, c.g, c.b, m_Timer );
		glVertex2f( x2, 10.0f );
		glColor4f( c.r, c.g, c.b, m_Timer );
		glVertex2f( x2, -10.0f );
		glColor4f( c.r, c.g, c.b, m_Timer/4);
		glVertex2f( x1, 0.0f );

		glColor4f( c.r, c.g, c.b, m_Timer );
		glVertex2f( -x2, 10.0f );
		glColor4f( c.r, c.g, c.b, m_Timer );
		glVertex2f( -x2, -10.0f );
		glColor4f( c.r, c.g, c.b, m_Timer/4 );
		glVertex2f( -x1, 0.0f );
	glEnd();

}

void DudeSpawnEffect::Tick()
{
	const float step = 0.03f;
	m_Timer += step;
	if( m_Timer >= 1.0f )
	{
		g_Agents->AddDude(m_Dude);	// agentmanger will call Lock()
		m_Dude->Unlock();
		m_Dude = 0;
		Die();
	}
}


//--------------------
// FadeText
//--------------------

FadeText::FadeText(	vec2 const& pos, std::string const& text, float size, float life, float delay ) :
	m_Text(text),
	m_Delay(delay),
	m_Life(life),
	m_Size(size),
	m_Timer(0.0f)
{
	SetFlags( flagIndestructible );
	MoveTo( pos );
}

void FadeText::Draw()
{
	if( m_Timer < m_Delay )
		return;

	float a = 1.0f-(m_Timer-m_Delay)/m_Life;
	glColor4f( 1.0f, 1.0f, 1.0f, a );

	float s=1.0f;
	float t=1.0f;

	if(a<1.0f)
	{
		s = 1.0f + 1.0f * (1.0f-a);
		t = 1.0f + 1.0f * (1.0f-a);
	}
	PlonkText( *g_Font, m_Text, true, m_Size*s, m_Size*t );
}

void FadeText::Tick()
{
	TurnTo( g_Player->Heading() );
	m_Timer += 1.0f/TARGET_FPS;
	if( m_Timer > m_Delay + m_Life )
		Die();
}

//--------------------
// Exhaust 
//--------------------
Exhaust::Exhaust( Agent const& emitter, vec2 const& basespd ) :
	m_Life( Rnd( 0.3f, 1.0f) )
{
	SetFlags( flagIndestructible );
	MoveTo( emitter.Pos() );
	float heading = emitter.Heading() + (float)M_PI;
	m_Spd = Rotate( vec2( 0.0f, Rnd(5.0f,8.0f) ),
		heading + Rnd( -0.05f, 0.05f ) );
	m_Spd += basespd;

	TurnTo( Rnd(0.0f, 2.0f*(float)M_PI ) );
}

void Exhaust::Draw()
{
	glShadeModel(GL_SMOOTH);
	glEnable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );

	float s = m_Life*1.5f;

	glBlendFunc( GL_SRC_ALPHA, GL_ONE ); //GL_ONE_MINUS_SRC_ALPHA );
	glBegin( GL_TRIANGLES );
		glColor4f( 1.0f, 0.8f, 0.0f, m_Life > 0.3f ? 0.3f : m_Life );
		glVertex2f( 0.0f*s, 5.0f*s );
		glVertex2f( 8.0f*s, -3.0f*s );
		glVertex2f( -8.0f*s, -3.0f*s );
	glEnd();
}

void Exhaust::Tick()
{
	MoveBy( m_Spd );
	TurnBy( pi/32.0f );
	m_Life -= 0.02f;
	if( m_Life < 0.0f )
		Die();
}


//--------------------
// Streamer 
//--------------------


Streamer::Streamer( vec2 const& pos, float heading, float turnaccel ) :
	Agent(true),
	m_TurnSpd(0.0f),
	m_TurnAccel( turnaccel )
{
	MoveTo( pos );
	TurnTo( heading );
}

void Streamer::Draw()
{
	std::deque<vec2>::reverse_iterator it;

	if( m_Trail.size() < 2 )
		return;

	float c=1.0;
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_BLEND );
	glBegin( GL_LINE_STRIP );
	for( it=m_Trail.rbegin(); it!=m_Trail.rend(); ++it )
	{
		glColor4f( c,c,c,c );
		c *= 0.98f;
		vec2 const& v = *it;
		glVertex2f( v.x, v.y );		
	}
	glEnd();
}


void Streamer::Tick()
{
	TurnBy( m_TurnSpd+Rnd(-0.3, 0.3) );
	m_TurnSpd += m_TurnAccel;

	Forward( 10.0f );

	if( m_Trail.size() > 40 )
		Die();
	else
		m_Trail.push_back( Pos() );
}


