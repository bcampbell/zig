#include "proceduraltextures.h"
#include "colours.h"

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

