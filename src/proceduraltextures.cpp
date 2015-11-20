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


NarrowBeamGradient::NarrowBeamGradient( int pixw, int pixh ) :
	CalculatedTexture( pixw, pixh )
{
}

void NarrowBeamGradient::Calc( float x, float y, Colour& out )
{
    float d = fabs((x-0.5f)*2);   // d in [0..1)
    float v = 1.0f-d;
    v=v*v;  //*v*v;

	out.r = v;
	out.g = v;
	out.b = v;
	out.a = 1.0f;
}


WideBeamGradient::WideBeamGradient( int pixw, int pixh ) :
	CalculatedTexture( pixw, pixh )
{
}

void WideBeamGradient::Calc( float x, float y, Colour& out )
{
    float d = fabs((x-0.5f)*2);   // d in [0..1)
    float v = 1.0f-d;
    v=v*v*v*v*v*v*v*v;

	out.r = v;
	out.g = v;
	out.b = v;
	out.a = 1.0f;
}

