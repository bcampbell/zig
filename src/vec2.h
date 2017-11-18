

#ifndef VEC2_H
#define VEC2_H

#include <cmath>
#include <cassert>

class vec2
{
public:
	static vec2 ZERO;

	float x;
	float y;

	vec2()
		{}

	vec2( float xval, float yval ) : x(xval), y(yval)
		{}

//	void Rotate( float theta );

	float Len() const
		{ return (float)sqrt(LenSq()); }
	float LenSq() const
		{ return x*x + y*y; }

	void Normalise()
		{ float l=Len(); x/=l; y/=l; }

	void CapMagnitude( float maxmag )
	{
		if( LenSq() < maxmag*maxmag )
			return;
		Normalise();
		x*=maxmag;
		y*=maxmag;
	}

	vec2& operator+=( vec2 const& other )
		{ x += other.x; y += other.y; return *this; }
	vec2& operator-=( vec2 const& other )
		{ x -= other.x; y -= other.y; return *this; }
	vec2& operator*=( float scale )
		{ x *= scale; y *= scale; return *this; }
	vec2& operator/=( float d )
	{
        assert(std::isnormal(d));
        x /= d;
        y /= d;
        return *this;
    }
	vec2 const& operator+() const
		{ return *this; }
	vec2 operator-() const
		{ return vec2( -x, -y ); }

	friend vec2 operator+( vec2 const& v1, vec2 const& v2 );
	friend vec2 operator-( vec2 const& v1, vec2 const& v2 );
	friend vec2 operator*( vec2 const& v, float s );
	friend vec2 operator*( float s, vec2 const& v );
	friend vec2 operator/( vec2 const& v, float d );
//	friend vec2 operator/( float d, vec2 const& v );
	friend vec2 Rotate( vec2 const& v, float theta );
	friend float Dot( vec2 const& v1, vec2 const& v2 );

};

inline vec2 operator+( vec2 const& v1, vec2 const& v2 )
	{ return vec2( v1.x + v2.x, v1.y + v2.y ); }

inline vec2 operator-( vec2 const& v1, vec2 const& v2 )
	{ return vec2( v1.x - v2.x, v1.y - v2.y ); }

inline vec2 operator*( vec2 const& v, float s )
	{ return vec2( v.x*s, v.y*s ); }

inline vec2 operator*( float s, vec2 const& v )
	{ return vec2( v.x*s, v.y*s ); }

inline vec2 operator/( vec2 const& v, float d )
	{
        assert(std::isnormal(d));
        return vec2( v.x/d, v.y/d );
    }

//inline vec2 operator/( float d, vec2 const& v )
//	{ return vec2( v.x/d, v.y/d ); }


inline vec2 Rotate( vec2 const& v, float theta )
{
	float s = (float)(sin(-theta));
	float c = (float)(cos(-theta));
	return( vec2( v.x*c - v.y*s, v.x*s + v.y*c ) );
}


inline float Dot( vec2 const& v1, vec2 const& v2 )
{
	return v1.x*v2.x + v1.y*v2.y;
}
#endif // VEC2_H

