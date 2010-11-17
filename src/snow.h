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

#ifndef SNOW_FAKE_INCLUDED
#define SNOW_FAKE_INCLUDED

class UniqueSnowFlake
{
	public:
	typedef unsigned char byte;
	
	//ctor returns in non-deterministic time
	UniqueSnowFlake();

	~UniqueSnowFlake();

	//returns pointer to 8bit rgba buffer containing snowflake image
	inline const byte* buffer() const
	{ return myImage; }
	
	//returns width and depth of buffer
	const int width() const;
	const int depth() const;

	private:
	
	//sets point with 8x reflection
	void point(int x, int y);

	//set a single pixel
	void pixel(int x, int y, byte r, byte g, byte b, byte a);

	byte* myImage;
	bool *myBMP;
};

#endif//SNOW_FAKE_INCLUDED

