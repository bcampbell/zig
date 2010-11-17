

#ifndef IMAGE_H
#define IMAGE_H

// class Image
// for manipulating pixel data.

#include <assert.h>
#include <string>
#include "zigtypes.h"

enum PixelFormat
{
	RGBx=0,	// x=pad byte
	RGBA=1,
	GREY=2,
	GREYA=3,
};

class Image
{
public:
	// load from file
	Image( std::string const& filename );

	// construct empty image (initial pixels are random)
	Image( int w, int h, PixelFormat fmt );

	~Image();

	PixelFormat Fmt() const;

	// get width (in pixels)
	int Width() const;

	// get height (in pixels)
	int Height() const;

	// number of bytes to add to ptr to get to next line
	int BytesPerLine() const;
	int BytesPerPixel() const;

	uint8_t*	LineAddr( int y );
	uint8_t*	PixelAddr( int x, int y );

	uint8_t const*	LineAddrConst( int y ) const;
	uint8_t const*	PixelAddrConst( int x, int y ) const;
private:
	PixelFormat 	m_Format;
	int			m_Width;
	int			m_Height;
	int			m_BytesPerLine;
	int			m_BytesPerPixel;
	uint8_t*	m_Data;	// for procedural textures, we keep our own copy here
};







inline PixelFormat Image::Fmt() const
	{ return m_Format; }

inline int Image::Width() const
	{ return m_Width; }

inline int Image::Height() const
	{ return m_Height; }

inline int Image::BytesPerLine() const
	{ return m_BytesPerLine; }

inline int Image::BytesPerPixel() const
	{ return m_BytesPerPixel; }
	
inline uint8_t* Image::LineAddr( int y )
{
	assert( m_Data );
	assert( y>=0 && y<m_Height );
	return m_Data + (y*m_BytesPerLine);
}

inline uint8_t* Image::PixelAddr( int x, int y )
{
	assert( m_Data );
	assert( x>=0 && x<m_Width );
	assert( y>=0 && y<m_Height );
	
	return m_Data + (y*m_BytesPerLine) + (x*m_BytesPerPixel);
}

	
inline uint8_t const* Image::LineAddrConst( int y ) const
{
	assert( m_Data );
	assert( y>=0 && y<m_Height );
	return m_Data + (y*m_BytesPerLine);
}

inline uint8_t const* Image::PixelAddrConst( int x, int y ) const
{
	assert( m_Data );
	assert( x>=0 && x<m_Width );
	assert( y>=0 && y<m_Height );
	
	return m_Data + (y*m_BytesPerLine) + (x*m_BytesPerPixel);
}


#endif // IMAGE_H



