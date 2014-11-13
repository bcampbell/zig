#include "sidstyle.h"

#include <ADSR.h>
#include <OnePole.h>
#include <FileLoop.h>
#include <Stk.h>

#include <assert.h>

using namespace stk;

FileLoop* CreateOsc( OscType t )
{
	std::string filename = Stk::rawwavePath();
	
	switch( t )
	{
	case SINE:		filename += "sine.raw";		break;
	case SQUARE:	filename += "square.raw";		break;
	case TRIANGLE:	filename += "triangle.raw";	break;
	case SAWTOOTH:	filename += "sawtooth.raw";	break;
	case NOISE:
	default:
		assert( false );
		return 0;
	};

	return new FileLoop( filename.c_str(), true );
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

	float modfreq = m_Conf.ModFreq0 + t*(m_Conf.ModFreq1 - m_Conf.ModFreq0);
	m_ModOsc->setFrequency( modfreq );

	float srcbasefreq = m_Conf.SrcFreq0 + t*(m_Conf.SrcFreq1 - m_Conf.SrcFreq0);
	float srcfreq = srcbasefreq + m_ModOsc->tick()*m_Conf.ModAmpl;

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

	m_Elapsed += 1.0f / Stk::sampleRate();

	return samp;
}

bool SIDStyle::IsDone()
{
	return (m_Elapsed >= m_Conf.Duration);
}


