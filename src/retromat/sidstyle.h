#ifndef SIDSTYLE_H
#define SIDSTYLE_H

#include <cmath>
#include <cstdlib>
#include <cassert>
#include <cstdio>

enum OscType
{
	SINE, SQUARE, TRIANGLE, SAWTOOTH, NOISE
};

namespace stk {
    class ADSR;
    class OnePole;
};

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


class SineOsc : public Osc
{
public:
    virtual float tick(float timestep)
    {
        Advance(timestep);
        float samp = sinf(m_t*2.0f*M_PI);
        assert( samp>=-1.0f);
        assert( samp<=1.0f);
        //printf("%f %f\n", m_t,samp);
        return samp;
    }
};

class SquareOsc : public Osc
{
public:
    virtual float tick(float timestep)
    {
        Advance(timestep);
        float samp = m_t >=0.5f ? -1.0f : 1.0f;
        assert( samp>=-1.0f);
        assert( samp<=1.0f);
        return samp;
    }
};

class TriangleOsc : public Osc
{
public:
    virtual float tick(float timestep)
    {
        Advance(timestep);
        float samp;
        if (m_t<=0.5f)
            samp = (m_t*4.0)-1.0f;
        else
            samp = 3.0f-(m_t*4.0f);
//        printf("%8f %f\n",m_t,samp);
        assert( samp>=-1.0f);
        assert( samp<=1.0f);
        return samp;
    }
};

class SawtoothOsc : public Osc
{
public:
    virtual float tick(float timestep)
    {
        Advance(timestep);
        float samp = (m_t*2.0f)-1.0f;
        assert( samp>=-1.0f);
        assert( samp<=1.0f);
        return samp;
    }
};

class NoiseOsc : public Osc
{
public:
    virtual float tick(float timestep)
    {
        float samp = (2.0f * ((float)rand() / (float)RAND_MAX)) - 1.0f;
        assert( samp>=-1.0f);
        assert( samp<=1.0f);
        return samp;
    }
};


class SIDStyle
{
public:
	struct Config
	{
		float Duration;
		OscType	SrcType;
		float	SrcFreq0;
		float	SrcFreq1;
		OscType ModType;
		float ModFreq0;
		float ModFreq1;
		float ModAmpl;
		float Attack;
		float Decay;
		float Sustain;
		float Release;
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
    stk::ADSR*		m_ADSR;
    stk::OnePole*	m_Filter;

	float m_Elapsed;
};

#endif // SIDSTYLE_H

