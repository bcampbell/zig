#include "sidstyle.h"


#include <cassert>
#include <cmath>

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

	m_Filter = new OnePole();
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
    const float timestep = g_RStep;
	float t = m_Elapsed / m_Conf.Duration;
	float modfreq = m_Conf.ModFreq0 + t*(m_Conf.ModFreq1 - m_Conf.ModFreq0);
	m_ModOsc->setFrequency( modfreq );

	float srcbasefreq = m_Conf.SrcFreq0 + t*(m_Conf.SrcFreq1 - m_Conf.SrcFreq0);
	float srcfreq = srcbasefreq + m_ModOsc->tick()*m_Conf.ModAmpl;
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
	samp = m_SrcOsc->tick();
	samp *= m_ADSR->tick();
	samp = m_Filter->tick( samp );

	samp *= m_Conf.Gain;

	m_Elapsed += timestep;

	return samp;
}

bool SIDStyle::IsDone()
{
	return (m_Elapsed >= m_Conf.Duration);
}


