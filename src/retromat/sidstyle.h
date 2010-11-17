#ifndef SIDSTYLE_H
#define SIDSTYLE_H


enum OscType
{
	SINE, SQUARE, TRIANGLE, SAWTOOTH, NOISE
};

namespace stk {
    class FileLoop;
    class ADSR;
    class OnePole;
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

    stk::FileLoop*	m_SrcOsc;
    stk::FileLoop*	m_ModOsc;
    stk::ADSR*		m_ADSR;
    stk::OnePole*	m_Filter;

	float m_Elapsed;
};

#endif // SIDSTYLE_H

