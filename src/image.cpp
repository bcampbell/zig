

#include "image.h"
#include "loadpng.h"
#include "wobbly.h"


Image::Image( int w, int h, PixelFormat fmt ) :
	m_Format(fmt),
	m_Width(w),
	m_Height(h)
{
	switch( m_Format )
	{
		case RGBx:
		case RGBA:
			m_BytesPerPixel = 4;
			break;
		case GREY:
			m_BytesPerPixel = 1;
			break;
		case GREYA:
			m_BytesPerPixel = 2;
			break;
		default:
			assert( false );	// invalid pixelformat!
			break;
	}

	m_BytesPerLine = m_BytesPerPixel * m_Width;

	m_Data = new uint8_t[ m_Height * m_BytesPerLine ];
}



Image::~Image()
{
	delete [] m_Data;
}




// load texture from file
Image::Image( std::string const& filename )
{
	SDL_Surface* image = LoadPNG( filename.c_str() );
	if( !image )
		throw Wobbly("LoadPNG() failed");

	int bpp = image->format->BitsPerPixel;
	switch( bpp )
	{
		case 24:
			m_Format = RGBx;
			break;
		case 32:
			m_Format = RGBA;
			break;
		default:
			SDL_FreeSurface(image);
			throw Wobbly("Image not 24 or 32 bit");
			break;
	}

	m_BytesPerPixel = 4;
	m_Width = image->w;
	m_Height = image->h;
	m_BytesPerLine  = m_BytesPerPixel * m_Width;

	m_Data = new uint8_t[ m_BytesPerLine * m_Height ];
	int x,y;
	for(y = 0; y < image->h; y++)
	{
		uint8_t const* src = (uint8_t*)image->pixels + y*image->pitch;
		uint8_t* dest = m_Data + y*m_BytesPerLine;
		for(x = 0; x < image->w; x++)
		{
			*dest++ = *(src + image->format->Rshift / 8);
			*dest++ = *(src + image->format->Gshift / 8);
			*dest++ = *(src + image->format->Bshift / 8);
			if( bpp == 32 )
			{
				*dest++ = *(src + image->format->Ashift / 8);
				src += 4;
			}
			else
			{
				*dest++ = 0;
				src += 3;
			}
		}
	}
	SDL_FreeSurface(image);
}


