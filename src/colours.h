

#ifndef COLOURS_H
#define COLOURS_H

class Colour
{
public:
	float r;
	float g;
	float b;
	float a;

	static const Colour WHITE;
	Colour( float red, float green, float blue, float alpha=1.0f);
	Colour() {};
	Colour& operator*=( float s )
		{ r*=s; g*=s; b*=s; a*=s; return *this; }
	Colour& operator+=( float v )
		{ r+=v; g+=v; b+=v; a+=v; return *this; }
	Colour& operator+( Colour const& c )
		{ r+=c.r; g+=c.g; b+=c.b; a+=c.a; return *this; }
	Colour& operator*=( Colour const& c )
		{ r*=c.r; g*=c.g; b*=c.b; a*=c.a; return *this; }
	void Clamp();
};

inline Colour operator*( Colour const& c, float f )
{
	Colour r(c);
	r*=f;
	return r;
}

inline Colour ColourLerp( Colour const& a, Colour const& b, float factor )
{
	return( a*(1-factor) + b*(factor) );
}


inline Colour::Colour( float red, float green, float blue, float alpha ) :
	r(red),g(green),b(blue),a(alpha)
{
}

inline void Colour::Clamp()
{
	if( r>1.0f ) r=1.0f;
	if( g>1.0f ) g=1.0f;
	if( b>1.0f ) b=1.0f;
	if( a>1.0f ) a=1.0f;

	if( r<0.0f ) r=0.0f;
	if( g<0.0f ) g=0.0f;
	if( b<0.0f ) b=0.0f;
	if( a<0.0f ) a=0.0f;
}


class ColourCycle
{
public:
	ColourCycle( Colour const* rawcolours, int numcolours );
	bool Tick( float step, bool loop=false );
	Colour const& Current() const;
private:
	Colour const*	m_Colours;
	int				m_NumColours;
	Colour			m_Current;
	float m_Pos;
};


class ColourRange
{
public:
	ColourRange( Colour const* rawcolours, int numcolours );
	Colour Get( float pos, bool clamp ) const;
private:
	Colour const*	m_Colours;
	int				m_NumColours;
};


inline ColourRange::ColourRange( Colour const* rawcolours, int numcolours ) :
	m_Colours(rawcolours), m_NumColours(numcolours)
{
}


#endif // COLOURS_H

