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

#include <cmath>
#include <cstdlib>
#include "sinusoidal_texture.h"

class SinTab
{	
public:
	SinTab();	

	inline unsigned char operator[](unsigned char i) const {
		return table[i];
	}

	unsigned char table[255];
};

static const SinTab sine;
SinTab::SinTab()
{
	const float pi = 3.141592654f;
	float t=0,dt=(2*pi)/256;
	for(int i=0;i<255;++i){
		//table[i] = (unsigned char)((sin(t)+1.0f)*128.0f);
		table[i] = (unsigned char)((sin(t)+1.0f)*64.0f);
		t += dt;
	}
}


SinusoidalTexture::SinusoidalTexture()
: myImage(new byte[256*256*4])
{
	byte* p=myImage;

	byte vr=rand();
	byte vg=rand();
	byte vb=rand();
	byte hr=rand();
	byte hg=rand();
	byte hb=rand();

	for (int v=0;v<256;++v){
		for (int h=0;h<256;++h){
			*p++ = sine[++hr] + sine[vr];
			*p++ = sine[++hg] + sine[vg];
			*p++ = sine[++hb] + sine[vb];
			*p++ = 255;
		}
		++vr;++vg;++vb;
	}
}

SinusoidalTexture::~SinusoidalTexture()
{
	delete[] myImage;
}

//returns width and depth of buffer
const int SinusoidalTexture::width() const
{
	return 256;
}

const int SinusoidalTexture::depth() const
{
	return 256;
}
