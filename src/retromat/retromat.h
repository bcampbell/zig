#ifndef RETROMAT_H
#define RETROMAT_H


#include <vector>
#include <string>
#include <cassert>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib>

void SetRetromatFreq( int samplerate );

typedef void (*RetromatFn)( std::vector<float>& out );

extern float g_RFreq;   // sample rate for retromat
extern float g_RStep;   // 1/g_RFreq

class ADSR;

const float RETRO_PI = 3.14159265358979323846f;
const float RETRO_TWOPI = 2.0f * RETRO_PI;

// Base oscillator class
class Osc
{
public:
    Osc(float freq) : m_t(0.0f),m_Freq(freq) {}
    virtual ~Osc()  {}
    // return value in range [-1.0f,1.0f]
    virtual float tick() = 0;
    void setFrequency(float freq) { assert(freq>=0.0f); m_Freq=freq; }
protected:
    float m_t;
    float m_Freq;
    inline void Advance()
        { m_t = fmod(m_t + (g_RStep*m_Freq), 1.0f); }
};


class SineOsc : public Osc
{
public:
    SineOsc(float freq=8000.0f) : Osc(freq) {}
    virtual float tick()
    {
        Advance();
        float samp = sinf(m_t*RETRO_TWOPI);
        assert( samp>=-1.0f);
        assert( samp<=1.0f);
        //printf("%f %f\n", m_t,samp);
        return samp;
    }
};

class SquareOsc : public Osc
{
public:
    SquareOsc(float freq=8000.0f) : Osc(freq) {}
    virtual float tick()
    {
        Advance();
        float samp = m_t >=0.5f ? -1.0f : 1.0f;
        assert( samp>=-1.0f);
        assert( samp<=1.0f);
        return samp;
    }
};

class TriangleOsc : public Osc
{
public:
    TriangleOsc(float freq=8000.0f) : Osc(freq) {}
    virtual float tick()
    {
        Advance();
        float samp;
        if (m_t<=0.5f)
            samp = (m_t*4.0f)-1.0f;
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
    SawtoothOsc(float freq=8000.0f) : Osc(freq) {}
    virtual float tick()
    {
        Advance();
        float samp = (m_t*2.0f)-1.0f;
        assert( samp>=-1.0f);
        assert( samp<=1.0f);
        return samp;
    }
};

class NoiseOsc : public Osc
{
public:
    NoiseOsc(float freq=8000.0f) : Osc(freq) {}
    virtual float tick()
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
    float tick();
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
        float pole = expf(-RETRO_TWOPI*cutoffFreq/sampleFreq);
        setPole(pole);
    }

public:
    float m_gain;
    float m_a1;
    float m_b0;
    float m_prev;
};


#endif // RETROMAT_H

