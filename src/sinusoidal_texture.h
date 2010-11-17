/*
 * Copyright (c) 2001
 * Thaddaeus Frogley
 * codemonkey_uk@hotmail.com
 *
 * Permission to use, copy, modify, distribute and sell this
 * software and its documentation for any purpose is hereby
 * granted without fee, provided that the above copyright notice
 * appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation.
 * Thaddaeus Frogley makes no representations about the
 * suitability of this software for any purpose. It is provided
 * "as is" without express or implied warranty.
*/

#ifndef SINUSOIDAL_TEXTURE_INCLUDED
#define SINUSOIDAL_TEXTURE_INCLUDED

class SinusoidalTexture
{
	public:
	typedef unsigned char byte;

	SinusoidalTexture();
	~SinusoidalTexture();

	//returns width and depth of buffer
	const int width() const;
	const int depth() const;

	//returns pointer to 8bit rgba buffer containing plasma image
	inline const byte* buffer() const
	{ return myImage; }

	private:

		byte* myImage;
};

#endif//SINUSOIDAL_TEXTURE_INCLUDED