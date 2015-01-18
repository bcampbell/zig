#include "sidstyle.h"

#include <OnePole.h>
#include <Stk.h>

#include <cassert>
#include <cmath>

using namespace stk;


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


class ADSR
{
public:
    enum State { ATTACK, DECAY, SUSTAIN, RELEASE, IDLE };
    ADSR();
    float tick(float timestep);
    void keyOn();
    void keyOff();
    void setAllTimes (float aTime, float dTime, float sLevel, float rTime);
    State getState() { return m_State; }
private:
    State m_State;
    float m_StateTime;
    float m_Attack;
    float m_Decay;
    float m_SustainLevel;
    float m_Release;
};

ADSR::ADSR() :
    m_State(IDLE),
    m_StateTime(0.0f),
    m_Attack(0.0f),
    m_Decay(0.0f),
    m_SustainLevel(0.0f),
    m_Release(0.0f)
{
}

float ADSR::tick(float timestep)
{
    m_StateTime += timestep;
    switch( m_State )
    {
    case ATTACK:
        if (m_StateTime<m_Attack)
            return m_StateTime/m_Attack;
        m_StateTime -= m_Attack;
        m_State = DECAY;
        // fall through
    case DECAY:
        if (m_StateTime<m_Decay)
        {
            float t = m_StateTime/m_Decay;
            return 1.0f*(1.0f-t) + m_SustainLevel*t;
        }
        m_State = SUSTAIN;
        m_StateTime -= m_Decay;
        // fall through
        break;
    case SUSTAIN:
        return m_SustainLevel;
    case RELEASE:
        if (m_StateTime<m_Release)
        {
            float t = m_StateTime/m_Release;
            return m_SustainLevel*(1.0f-t);
        }
        m_State = IDLE;
        m_StateTime -= m_Release;
        // fall through
    case IDLE:
        return 0.0f;
    }
}

void ADSR::keyOn()
{
    m_State = ATTACK;
    m_StateTime = 0.0f;
}

void ADSR::keyOff()
{
    if (m_State!=IDLE && m_State!=RELEASE)
    {
        m_State = RELEASE;
        m_StateTime = 0.0f;
    }
}

void ADSR::setAllTimes(float aTime, float dTime, float sLevel, float rTime)
{
    m_Attack = aTime;
    m_Decay = dTime;
    m_SustainLevel = sLevel;
    m_Release = rTime;
}

Osc* CreateOsc( OscType t )
{
	switch( t )
	{
	case SINE:
        return new SineOsc();
	case SQUARE:
        return new SquareOsc();
	case TRIANGLE:
        return new TriangleOsc();
	case SAWTOOTH:
        return new SawtoothOsc();
	case NOISE:
        return new NoiseOsc();
	};

	assert( false );
	return 0;
}


SIDStyle::SIDStyle( Config const& conf ) :
	m_Conf(conf),
	m_Elapsed(0.0f)
{
	m_SrcOsc = CreateOsc( m_Conf.SrcType );
	m_ModOsc = CreateOsc( m_Conf.ModType );
	m_ADSR = new ADSR();

	m_ADSR->setAllTimes(
		m_Conf.Attack,
		m_Conf.Decay,
		m_Conf.Sustain,
		m_Conf.Release );
	m_ADSR->keyOn();

	m_Filter = new OnePole( 0.0f );
}


SIDStyle::~SIDStyle()
{
	delete m_SrcOsc;
	delete m_ModOsc;
	delete m_ADSR;
	delete m_Filter;
}


float SIDStyle::Tick()
{
	float t = m_Elapsed / m_Conf.Duration;
    float timestep = (1.0f / Stk::sampleRate());
	float modfreq = m_Conf.ModFreq0 + t*(m_Conf.ModFreq1 - m_Conf.ModFreq0);
	m_ModOsc->setFrequency( modfreq );

	float srcbasefreq = m_Conf.SrcFreq0 + t*(m_Conf.SrcFreq1 - m_Conf.SrcFreq0);
	float srcfreq = srcbasefreq + m_ModOsc->tick(timestep)*m_Conf.ModAmpl;
    if (srcfreq<0.0f) {
        srcfreq = 0.0f;
    }
	m_SrcOsc->setFrequency( srcfreq );

	float releasetime = m_Conf.Duration - m_Conf.Release;
	if( m_Elapsed>=releasetime &&
		m_ADSR->getState() != ADSR::RELEASE &&
		m_ADSR->getState() != ADSR::IDLE )
	{
		m_ADSR->keyOff();
	}

	float pole = m_Conf.FilterPole0 + t*(m_Conf.FilterPole1-m_Conf.FilterPole0);
	m_Filter->setPole( pole );
	
	float samp;
	samp = m_SrcOsc->tick(timestep);
	samp *= m_ADSR->tick(timestep);
	samp = m_Filter->tick( samp );

	samp *= m_Conf.Gain;

	m_Elapsed += timestep;

	return samp;
}

bool SIDStyle::IsDone()
{
	return (m_Elapsed >= m_Conf.Duration);
}


