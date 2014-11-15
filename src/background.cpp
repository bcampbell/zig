

#include <SDL_opengl.h>
#include <assert.h>
#include <math.h>

#include "background.h"
#include "colours.h"
#include "drawing.h"
#include "image.h"
#include "perlinnoise.h"
#include "texture.h"
#include "vec2.h"
#include "zig.h"
#include "display.h"


class PerlinTexture : public CalculatedTexture
{
public:
	PerlinTexture( int pixw, int pixh, float bias, float repeat, Colour const& colour ) :
		CalculatedTexture( pixw,pixh ),
		m_Bias( bias ),
		m_Repeat( repeat ),
		m_Colour( colour )
	{
	}
	virtual void Calc( float x, float y, Colour& out );
private:
	float m_Bias;
	float m_Repeat;
	Colour m_Colour;
};


void PerlinTexture::Calc( float x, float y, Colour& out )
{
	const float r=m_Repeat;		// repeat factor

	float fy = r*y;
	float fx = r*x;

	// a nice seamless perlin noise texture
	float val =
		( (perlinNoise::noise( fx, fy ) * (r-fx) * (r-fy) ) +
		(perlinNoise::noise( fx-r, fy ) * (fx) * (r-fy) ) +
		(perlinNoise::noise( fx-r, fy-r ) * (fx) * (fy) ) +
		(perlinNoise::noise( fx, fy-r ) * (r-fx) * (fy) ) ) / (r*r);

	val = m_Bias + val*0.5f;

	out = m_Colour*val;
	out.a = 1.0f;
}



class RetroGrid : public CalculatedTexture
{
public:
	RetroGrid( int pixw, int pixh, int repeats, Colour const& brightness ) :
		CalculatedTexture( pixw, pixh ),
		m_Repeats( (float)repeats ),
		m_Brightness( brightness )
	{
	}
	virtual void Calc( float x, float y, Colour& out );
private:
	float m_Repeats;
	Colour m_Brightness;
};


void RetroGrid::Calc( float x, float y, Colour& out )
{
	x = fmodf( x*m_Repeats, 1.0f );
	y = fmodf( y*m_Repeats, 1.0f );

	float rx = x < 0.5f ? 2.0f*x : 2.0f*(1.0f-x);	// sawtooth
	float ry = y < 0.5f ? 2.0f*y : 2.0f*(1.0f-y);	// sawtooth

	out.r = (powf( rx, 128.0f ) + powf( ry, 128.0f ) );
	out.g = powf( rx, 8.0f ) + powf( ry, 8.0f );
	out.b = (powf( rx, 128.0f ) + powf( ry, 128.0f ) );

	out *= m_Brightness;
}







OriginalBackground::OriginalBackground() : m_Texture(0), m_TextureToo(0)
{
	if( g_GLMultitexture && g_NumTextureUnits > 1 )
	{
		m_Texture = new PerlinTexture( 128,128,0.5f, 4.0f, Colour( 0.5f, 0.0f, 1.0f, 0.0f ) );
		m_TextureToo = new PerlinTexture( 128,128,0.5f, 3.0f, Colour( 1.0f, 1.0f, 1.0f, 0.0f ) );

		g_Display->AddTexture( m_Texture );
		g_Display->AddTexture( m_TextureToo );
	}
	else
	{
		m_Texture = new PerlinTexture( 128,128,0.25f, 2.0f, Colour( 0.0f, 0.0f, 1.0f, 0.0f ) );
		g_Display->AddTexture( m_Texture );
	}
}


OriginalBackground::~OriginalBackground()
{
	g_Display->RemoveTexture( m_Texture );
	g_Display->RemoveTexture( m_TextureToo );
	delete m_Texture;
	delete m_TextureToo;
}


void OriginalBackground::Draw( vec2 const& pos, float orient )
{
	const vec2* xy = g_Display->m_Extent;
	vec2 uv[4];
	vec2 uvtoo[4];

	int i;
	for( i=0; i<4; ++i )
	{
		uv[i] = Rotate( xy[i], orient );
		uvtoo[i] = uv[i];

		uv[i] += pos / 2.0f;	// Parallax
		uv[i] /= 256.0f;				// texture scale

		uvtoo[i] += pos / 8.0f;
		uvtoo[i] /= 256.0f;
	}

	if( g_GLMultitexture && g_NumTextureUnits>1 )
	{
		g_glActiveTextureARB( GL_TEXTURE0_ARB );
		glBindTexture( GL_TEXTURE_2D,m_Texture->ID() );
		glEnable( GL_TEXTURE_2D );
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		g_glActiveTextureARB( GL_TEXTURE1_ARB );
		glBindTexture( GL_TEXTURE_2D,m_TextureToo->ID() );
		glEnable( GL_TEXTURE_2D );
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	
		glDisable( GL_BLEND );
		glBegin(GL_QUADS);
		for( i=0; i<4; ++i )
		{
			g_glMultiTexCoord2fARB( GL_TEXTURE0_ARB, uv[i].x, uv[i].y );
			g_glMultiTexCoord2fARB( GL_TEXTURE1_ARB, uvtoo[i].x, uvtoo[i].y );
			glVertex2f( xy[i].x, xy[i].y );
		}
		glEnd();

		g_glActiveTextureARB( GL_TEXTURE1_ARB );
		glDisable( GL_TEXTURE_2D );

		g_glActiveTextureARB( GL_TEXTURE0_ARB );
		glDisable( GL_TEXTURE_2D );
	}
	else
	{
		glBindTexture( GL_TEXTURE_2D,m_Texture->ID() );
	
		glDisable( GL_BLEND );
		glEnable( GL_TEXTURE_2D );
		glBegin(GL_QUADS);
		for( i=0; i<4; ++i )
		{
			glTexCoord2f( uv[i].x, uv[i].y );
			glVertex2f( xy[i].x, xy[i].y );
		}
		glEnd();
	}
}




RetroBackground::RetroBackground() :
	m_ForeTexture(0),
	m_BackTexture(0)
{
	m_ForeTexture = new RetroGrid( 128, 128, 4, Colour( 0.2f, 0.5f, 0.2f ) );
	m_BackTexture = new RetroGrid( 128, 128, 12, Colour( 0.0f, 0.1f, 0.0f ) );
	g_Display->AddTexture( m_ForeTexture );
	g_Display->AddTexture( m_BackTexture );
}


RetroBackground::~RetroBackground()
{
	g_Display->RemoveTexture( m_ForeTexture );
	g_Display->RemoveTexture( m_BackTexture );
	delete m_ForeTexture;
	delete m_BackTexture;
}


void RetroBackground::Draw( vec2 const& pos, float orient )
{
	const vec2* xy = g_Display->m_Extent;

	vec2 uv[4];
	vec2 uvtoo[4];
	int i;
	for( i=0; i<4; ++i )
	{
		uv[i] = Rotate( xy[i], orient );
		uvtoo[i] = uv[i];

		uv[i] += pos / 4.0f;	//2.0f;	// Parallax
		uv[i] /= 256.0f;				// texture scale

		uvtoo[i] += pos / 1.0f;	// Parallax
		uvtoo[i] /= 256.0f;				// texture scale
	}

	glDisable( GL_BLEND );
	glEnable( GL_TEXTURE_2D );

	if( g_GLMultitexture && g_NumTextureUnits>1 && g_ARB_texture_env_add )
	{
		g_glActiveTextureARB( GL_TEXTURE0_ARB );
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D,m_BackTexture->ID() );
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		g_glActiveTextureARB( GL_TEXTURE1_ARB );
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D,m_ForeTexture->ID() );
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD );

		glBegin(GL_QUADS);
		for( i=0; i<4; ++i )
		{
			g_glMultiTexCoord2fARB( GL_TEXTURE0_ARB, uv[i].x, uv[i].y );
			g_glMultiTexCoord2fARB( GL_TEXTURE1_ARB, uvtoo[i].x, uvtoo[i].y );
			glVertex2f( xy[i].x, xy[i].y );
		}
		glEnd();

		g_glActiveTextureARB( GL_TEXTURE1_ARB );
		glDisable( GL_TEXTURE_2D );

		g_glActiveTextureARB( GL_TEXTURE0_ARB );
		glDisable( GL_TEXTURE_2D );
	}
	else
	{
		glBindTexture( GL_TEXTURE_2D,m_ForeTexture->ID() );

		glBegin(GL_QUADS);
		for( i=0; i<4; ++i )
		{
			glTexCoord2f( uv[i].x, uv[i].y );
			glVertex2f( xy[i].x, xy[i].y );
		}
		glEnd();
	}
}

