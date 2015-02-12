

#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <stdlib.h>
#include <math.h>
#ifndef M_PI
#define M_PI (3.141592653589793)
#endif

const float pi = (float)M_PI;
const float twopi = 2.0f*pi;
const float halfpi = pi/2.0f;

inline float r2d( float radians )
	{ return radians * (360.0f/twopi); }

// get random number in the range [low, high)
inline float Rnd( float low=0.0f, float high=1.0f )
{
	float r =(float)((float)rand() / ((float)RAND_MAX + 1) * (high-low));
	r += low;

	return r;
}


inline int RndI( int low, int high )
{
	return (rand() % (1+high-low)) + low;
}


inline float zfmin(float a, float b)
{
    return a < b ? a : b;
}
inline float zfmax(float a, float b)
{
    return a > b ? a : b;
}

inline int zimin(int a, int b)
{
    return a < b ? a : b;
}

inline int zimax(int a, int b)
{
    return a > b ? a : b;
}

#endif

