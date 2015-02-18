#ifndef SIDSTYLE_H
#define SIDSTYLE_H


// a sound effect generator, based _very_ loosely on the SID chip
// base waveform with frequency modulated by another oscillator
// output passed through a lowpass filter
#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <cmath>

#ifndef M_PI
#define M_PI (3.141592653589793)
#endif


enum OscType
{
	SINE, SQUARE, TRIANGLE, SAWTOOTH, NOISE
};

class ADSR;


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



class OnePole
{
public:
    OnePole() : m_gain(1.0f), m_prev(0.0f) { setPole(0.0f); }
    float tick(float inp)
    {
        inp *= m_gain;
        float out = m_b0*inp - m_a1*m_prev;
        m_prev = out;
        return out;
    }
    void setPole( float pole)
    {
        assert(fabsf(pole)<1.0f);
        m_b0 = (pole<0.0f) ? 1.0f+pole:1.0f-pole;
        m_a1 = -pole;
    }

    void setCutoff( float cutoffFreq, float sampleFreq)
    {
        float pole = expf(-2.0f*M_PI*cutoffFreq/sampleFreq);
        setPole(pole);
    }

public:
    float m_gain;
    float m_a1;
    float m_b0;
    float m_prev;
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
	float Tick(float timestep);
	bool IsDone();
private:
	Config m_Conf;

    Osc*        	m_SrcOsc;
    Osc*        	m_ModOsc;
    ADSR*		m_ADSR;
    OnePole*	m_Filter;

	float m_Elapsed;
};

#endif // SIDSTYLE_H

