

#include "colours.h"
#include <assert.h>
#include <cstdio>
#include <math.h>

//static
const Colour Colour::WHITE = Colour(1.0f,1.0f,1.0f,1.0f);

ColourCycle::ColourCycle(
	Colour const* rawcolours,
	int numcolours ):
	m_Colours(rawcolours),
	m_NumColours(numcolours),
	m_Pos(0.0f)
{
	m_Current = m_Colours[0];
}

bool ColourCycle::Tick( float step, bool loop )
{
	bool done = false;

	m_Pos += step;
	if( m_Pos >= 1.0f )
	{
		if(loop)
			m_Pos -= 1.0f;	// loop
		else
		{
			done=true;
			m_Pos = 1.0f;
		}
	}

	// calc colour

	int i1 = (int)(m_Pos * (float)(m_NumColours));
	int i2 = (i1+1);

	float t = fmodf( m_Pos * m_NumColours, 1.0 );
	float s = 1.0f-t;

	if( !loop && i1>=m_NumColours-1 )
		i1 = i2 = m_NumColours-1;
	else
	{
		i1 %= m_NumColours;
		i2 %= m_NumColours;
	}

	Colour const* p1 = &m_Colours[i1];
	Colour const* p2 = &m_Colours[i2];
	m_Current.r = (p1->r)*s + (p2->r)*t;
	m_Current.g = (p1->g)*s + (p2->g)*t;
	m_Current.b = (p1->b)*s + (p2->b)*t;
	m_Current.a = (p1->a)*s + (p2->a)*t;

	return done;
}


Colour const& ColourCycle::Current() const
{
	return m_Current;
}





Colour ColourRange::Get( float pos, bool clamp ) const
{
	// calc colour

	int i1 = (int)(pos * (float)(m_NumColours));
	int i2 = (i1+1);

	float t = fmodf( pos * m_NumColours, 1.0 );
	float s = 1.0f-t;

	if( clamp && i1>=m_NumColours-1 )
		i1 = i2 = m_NumColours-1;
	else
	{
		i1 %= m_NumColours;
		i2 %= m_NumColours;
	}

	Colour const* p1 = &m_Colours[i1];
	Colour const* p2 = &m_Colours[i2];
	return Colour( (p1->r)*s + (p2->r)*t,
		(p1->g)*s + (p2->g)*t,
		(p1->b)*s + (p2->b)*t,
		(p1->a)*s + (p2->a)*t );
}

