#ifndef PROCEDURALTEXTURES_H
#define PROCEDURALTEXTURES_H


#include "texture.h"

class Colour;

class BlueGlow : public CalculatedTexture
{
public:
	BlueGlow( int pixw, int pixh );
	virtual void Calc( float x, float y, Colour& out );
private:
};


class NarrowBeamGradient : public CalculatedTexture
{
public:
	NarrowBeamGradient( int pixw, int pixh );
	virtual void Calc( float x, float y, Colour& out );
private:
};

class WideBeamGradient : public CalculatedTexture
{
public:
	WideBeamGradient( int pixw, int pixh );
	virtual void Calc( float x, float y, Colour& out );
private:
};


#endif	// PROCEDURALTEXTURES_H

