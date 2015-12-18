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
    m_Snd.Start(SFX_CHARGEUP,100);
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
                m_Snd.Start(SFX_ELECTRIC,10);
				SetFlags( flagCanHitPlayer );
			}
			break;
		case ON:
			if( m_Timer > m_Params.ontime )
            {
                m_Timer = 0;
                m_State = DONE;
                m_Snd.Stop(500);
            }
			break;
        case DONE:  // allow sound to fade away
            if(m_Timer > 0.5f)
                Die();
            break;
	}
}



void Beam::Draw()
{
	glEnable( GL_BLEND );
	glEnable( GL_TEXTURE_2D );
	glShadeModel( GL_FLAT );
	glBlendFunc( GL_ONE, GL_ONE );
	float w;
	Colour c;
    float l = m_Params.length;
	if( m_State == WARMUP )
	{
		float factor = Rnd();
		w = 2.0f * (1.0f-factor);
        l = m_Timer * 1000.0f;
        if(l>m_Params.length)
            l = m_Params.length;

        glColor3f( 0.2f, 0.0f, 0.0f);
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D,g_Textures[TX_WIDEBEAMGRADIENT]->ID() );
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        const int n_segs = 16;
        glBegin( GL_QUAD_STRIP );
        {
            int i;
            for(i=0; i<=n_segs; ++i)
            {
                float t=(float)i/(float)n_segs;
                float y = t*l;
                float x = w*2;

                glTexCoord2f(0.2f,0.0f);
                glVertex2f( -x, y );
                glTexCoord2f(0.8f,0.0f);
                glVertex2f( x, y );
            }
        }
        glEnd();
	}
	else
    {
            // ON
            w = m_Params.width + Rnd(-2.0f,2.0f);
            c = Colour( 1.0f,1.0f,1.0f,1.0f);


        // draw as long segmented rectangle upwards
        glColor3f( 1.0f, 0.0f, 0.0f);
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D,g_Textures[TX_WIDEBEAMGRADIENT]->ID() );
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        const int n_segs = 16;
        glBegin( GL_QUAD_STRIP );
        {
            int i;
            for(i=0; i<=n_segs; ++i)
            {
                float t=(float)i/(float)n_segs;
                float y = t*l;
                float x = w*2;

                float theta = 8*t - m_Timer*2;

                x += (sin(theta*3)*sin(theta*5))*14.0f;  //Rnd(-8.0f,8.0f);

                glTexCoord2f(0.2f,0.0f);
                glVertex2f( -x, y );
                glTexCoord2f(0.8f,0.0f);
                glVertex2f( x, y );
            }
        }
        glEnd();

        glColor3f( 1.0f, 1.0f, 1.0f);
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D,g_Textures[TX_NARROWBEAMGRADIENT]->ID() );
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        glBegin( GL_QUAD_STRIP );
        {
            int i;
            for(i=0; i<=n_segs; ++i)
            {
                float y = i*(l/n_segs);
                float x = w/2;

    //            x += Rnd(-1.0f,1.0f);

                glTexCoord2f(0.0f,(1.0f/n_segs)*i);
                glVertex2f( -x, y );
                glTexCoord2f(1.0f,(1.0f/n_segs)*i);
                glVertex2f( x, y );
            }
        }
        glEnd();
    }
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




