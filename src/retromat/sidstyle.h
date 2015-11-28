#ifndef SIDSTYLE_H
#define SIDSTYLE_H


#include <cstdlib>
#include <cstdio>

#include "retromat.h"

enum OscType
{
	SINE, SQUARE, TRIANGLE, SAWTOOTH, NOISE
};


// a sound effect generator, based _very_ loosely on the SID chip
// base waveform with frequency modulated by another oscillator
// output passed through a lowpass filter
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
    OnePole*	m_Filter;

	float m_Elapsed;
};

#endif // SIDSTYLE_H

