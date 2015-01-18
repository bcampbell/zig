#ifndef SIDSTYLE_H
#define SIDSTYLE_H

#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <cmath>

enum OscType
{
	SINE, SQUARE, TRIANGLE, SAWTOOTH, NOISE
};

class ADSR;

namespace stk {
    class OnePole;
};


// Base oscillator class
class Osc
{
public:
    Osc() : m_t(0.0f),m_Freq(8000.0f) {}
    // return value in range [-1.0f,1.0f]
    virtual float tick(float timestep) = 0;
    void setFrequency(float freq) { assert(freq>=0.0f); m_Freq=freq; }
protected:
    float m_t;
    float m_Freq;
    inline void Advance(float timestep)
        { m_t = fmod(m_t + (timestep*m_Freq), 1.0f); }
};



class SIDStyle
{
public:
	struct Config
	{
		float Duration; // total duration of effect
		OscType	SrcType;
		float	SrcFreq0;
		float	SrcFreq1;
		OscType ModType;
		float ModFreq0;
		float ModFreq1;
		float ModAmpl;
		float Attack;   // attack time (s)
		float Decay;    // decay time (s)
		float Sustain;  // sustain level (0..1)
		float Release;  // release time
		float FilterPole0;
		float FilterPole1;
		float Gain;
	};

	SIDStyle( Config const& conf );
	~SIDStyle();
	float Tick();
	bool IsDone();
private:
	Config m_Conf;

    Osc*        	m_SrcOsc;
    Osc*        	m_ModOsc;
    ADSR*		m_ADSR;
    stk::OnePole*	m_Filter;

	float m_Elapsed;
};

#endif // SIDSTYLE_H

