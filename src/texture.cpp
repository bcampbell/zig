

#include "texture.h"
#include "colours.h"
#include "loadpng.h"
#include "wobbly.h"
#include "zigtypes.h"

#include <string.h>	// for memset
#include <assert.h>
#include <SDL.h>



FileTexture::FileTexture( const char* filename )
{
	m_Image = LoadPNG( filename );
	if( !m_Image )
		throw Wobbly("LoadPNG() failed");
}

FileTexture::~FileTexture()
{
	SDL_FreeSurface( m_Image );
}


void FileTexture::UploadToGL()
{
	GLenum fmt;
	GLenum internalfmt;

	assert( m_Image->pitch == m_Image->w*m_Image->format->BytesPerPixel );

	int bpp = m_Image->format->BitsPerPixel;
	switch( bpp )
	{
		case 24:
			fmt = GL_RGB;
			internalfmt = GL_RGB;
			break;
		case 32:
			fmt = GL_RGBA;
			internalfmt = GL_RGBA;
			break;
		default:
			throw Wobbly("Image not 24 or 32 bit");
			break;
	}


	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		internalfmt,
		m_Image->w,
		m_Image->h,
		0,
		fmt,
		GL_UNSIGNED_BYTE,
		m_Image->pixels );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}



CalculatedTexture::CalculatedTexture( int pixw, int pixh ) :
	m_Width(pixw),
	m_Height(pixh)
{
}

void CalculatedTexture::UploadToGL()
{
	uint8_t* buf = new uint8_t[ m_Width*m_Height*4 ];

	uint8_t* p = buf;
	int y;
	for( y=0; y<m_Height; ++y )
	{
		float fy = (float)y/(float)m_Height;
		int x;
		for( x=0; x<m_Width; ++x )
		{
			float fx = (float)x/(float)m_Width;
			Colour c;
			Calc( fx, fy, c );
			c.Clamp();
			*p++ = (uint8_t)(c.r * 255.0f);
			*p++ = (uint8_t)(c.g * 255.0f);
			*p++ = (uint8_t)(c.b * 255.0f);
			*p++ = (uint8_t)(c.a * 255.0f);
		}
	}

	//memset( buf, 0, m_Width*m_Height*4);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, buf );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	delete [] buf;
}


