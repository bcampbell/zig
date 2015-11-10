#include "beam.h"
#include "colours.h"
#include "player.h"
#include "texture.h"
#include "zig.h"
#include <SDL_opengl.h>



Beam::Beam( Agent& owner, vec2 const& relpos, float relheading, Params const* params ):
	m_Owner( owner ),
	m_RelPos( relpos ),
	m_RelHeading( relheading ),
	m_State( WARMUP ),
	m_Timer( 0.0f )
{
	if( params )
		m_Params = *params;

	m_Owner.Lock();
	PositionRelative( m_Owner, m_RelPos, m_RelHeading );
	SetRadius( m_Params.length );	// hooo! only used for 1st-pass culling. Real work done in OnHitPlayer()
	SetFlags( 0 );
}

Beam::~Beam()
{
	m_Owner.Unlock();
}

void Beam::Respawn()
{
	Die();
}


void Beam::Tick()
{
	if( !m_Owner.Alive() )
	{
		Die();
		return;
	}

	PositionRelative( m_Owner, m_RelPos, m_RelHeading );
	m_Timer += 1.0f/(float)TARGET_FPS;
	
	switch( m_State )
	{
		case WARMUP:
			if( m_Timer > m_Params.warmuptime )
			{
				// zap.
				m_Timer = 0.0f;
				m_State = ON;
				SetFlags( flagCanHitPlayer );
			}
			break;
		case ON:
			if( m_Timer > m_Params.ontime )
				Die();
			break;
	}
}



void Beam::Draw()
{
	glEnable( GL_BLEND );
	glEnable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );
	glBindTexture( GL_TEXTURE_2D,g_Textures[TX_BEAMGRADIENT]->ID() );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glBlendFunc( GL_ONE, GL_ONE );
	float w;
	Colour c;
    float l = m_Params.length;
	if( m_State == WARMUP )
	{
		float factor = Rnd();
		w = 2.0f * (1.0f-factor);
		c = Colour( 0.5f, 0.0f, 0.0f, 1.0f );
        l = m_Timer * 4000.0f;
        if(l>m_Params.length)
            l = m_Params.length;
	}
	else
	{
		// ON
		w = m_Params.width + Rnd(-2.0f,2.0f);
		c = Colour( 1.0f, 1.0f, 1.0f, Rnd( 0.5f,0.7f) );
	}



	glColor4f( c.r, c.g, c.b, c.a );
	glBegin( GL_QUADS );
		glTexCoord2f(0.0f,0.0f);
		glVertex2f( -w/2.0f, -l );
		glTexCoord2f(1.0f,0.0f);
		glVertex2f( w/2.0f, -l );
		glTexCoord2f(1.0f,1.0f);
		glVertex2f( w/2.0f, 0.0f );
		glTexCoord2f(0.0f,1.0f);
		glVertex2f( -w/2.0f, 0.0f );
	glEnd();
}

// perform a distance-from-line check to see if the player
// actually hit the beam (as opposed to the _huge_ radius :-)
void Beam::OnHitPlayer( Player& player )
{
	vec2 v = Rotate( vec2(0.0f,1.0f), Heading() );
	vec2 p = player.Pos() - Pos();

	float d = Dot( v, p );
	if( d<0.0f || d>m_Params.length)
		return;	// too far foward or back

	// lsq = squared distance from line
	float lsq = fabs(p.LenSq() - d*d);
	if( lsq > (m_Params.width/2)*(m_Params.width/2) )
		return;	// too far out

	// call standard collision handling
	Dude::OnHitPlayer( player );
}




