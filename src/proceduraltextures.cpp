#include "proceduraltextures.h"
#include "colours.h"
#include "mathutil.h"

#include <math.h>

BlueGlow::BlueGlow( int pixw, int pixh ) :
	CalculatedTexture( pixw, pixh )
{
}

void BlueGlow::Calc( float x, float y, Colour& out )
{
	float r = 2.0f*sqrtf( ((x-0.5f)*(x-0.5f) + (y-0.5f)*(y-0.5f) ) );
	float ir = 1.0f-r;

	out.r = ir*ir;
	out.g = ir*ir;
	out.b = ir;
	out.a = 1.0f;
}


BeamGradient::BeamGradient( int pixw, int pixh ) :
	CalculatedTexture( pixw, pixh )
{
}

void BeamGradient::Calc( float x, float y, Colour& out )
{
    float a = (0.5f-x)*2;
    float v = 1.0f - (a*a*a*a);

	out.r = v;
	out.g = v*v*v*v*v*v;
	out.b = v*v*v*v*v*v;
	out.a = 1.0f;
}

