#include "beam.h"
#include "colours.h"
#include "player.h"
#include "texture.h"
#include "zig.h"
#include <SDL_opengl.h>

struct vert_xyuv { float x,y,u,v; };

void drawWideBeam(float width, float length, float time);

void drawQuads_xyuv( const vert_xyuv* verts, const int* indices, int numQuads)
{
    glBegin( GL_QUADS );
    int i;
    for (i=0; i<numQuads*4; ++i)
    {
        const vert_xyuv& v = verts[indices[i]];
        glTexCoord2f( v.u, v.v );
        glVertex2f( v.x, v.y );
    }
    glEnd();
}



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


#define N_BEAM_SEGS 64

//static
void Beam::DrawBeamOn( float width, float length, float time)
{

        drawWideBeam(width*2,length,time);

        glColor3f( 1.0f, 1.0f, 1.0f);
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D,g_Textures[TX_NARROWBEAMGRADIENT]->ID() );
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        glBegin( GL_QUADS );
        {
            const int n_segs = N_BEAM_SEGS;
            float ystep = length/(float)n_segs;
            float y=0;
            int i;
            for(i=0; i<=n_segs; ++i)
            {
                float x = width/2;

    //            x += Rnd(-1.0f,1.0f);

                glTexCoord2f(0.0f,(1.0f/n_segs)*i);
                glVertex2f( -x, y );
                glTexCoord2f(1.0f,(1.0f/n_segs)*i);
                glVertex2f( x, y );

                glTexCoord2f(1.0f,(1.0f/n_segs)*(i+1));
                glVertex2f( x, y+ystep);
                glTexCoord2f(0.0f,(1.0f/n_segs)*(i+1));
                glVertex2f( -x, y+ystep );

                y += ystep;
            }
        }
        glEnd();
}

// draw as long segmented rectangle upwards
void drawWideBeam(float width, float length, float time)
{
    vert_xyuv verts[(N_BEAM_SEGS+1)*2 ];
    int indices[N_BEAM_SEGS*4];

    {
        vert_xyuv *p = verts;
        int i;
        for(i=0; i<(N_BEAM_SEGS+1); ++i)
        {
            float t= (float)i/(float)(N_BEAM_SEGS+1);
            float y = length*t;
            float theta = time + t*2.0f*3.14f;

            float wibble = width/2 + (sin(theta) * sin(theta*theta))*width/2 + sin(theta*8.0f)*width/5 + sin(theta*19.0f)*width/10;

            // -ve x
            p->x = -(width/2 + wibble);
            p->y = y;
            p->u = 0.2f;    // u
            p->v = 0.0f;    // v
            ++p;
            // +ve x
            p->x = (width/2 + wibble);
            p->y = y;
            p->u = 0.8f;    // u
            p->v = 0.0f;    // v
            ++p;
        }
    }


    {
        // quads
        int *p = indices;
        int i;
        for(i=0; i<N_BEAM_SEGS; ++i)
        {
            int base = i*2;
            *p++ = base + 0;
            *p++ = base + 1;
            *p++ = base + 3;
            *p++ = base + 2;
        }
    }


    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D,g_Textures[TX_WIDEBEAMGRADIENT]->ID() );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glColor3f( 0.8f, 0.0f, 0.0f);
    drawQuads_xyuv(verts,indices,N_BEAM_SEGS);
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
		w = 1.0f + (1.0f*factor);
        l = m_Timer * 1000.0f;
        if(l>m_Params.length)
            l = m_Params.length;

        drawWideBeam(w,l,m_Timer);
	}
	else
    {
        // ON
        DrawBeamOn(m_Params.width + Rnd(-2.0f, 2.0f), l, m_Timer);
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




