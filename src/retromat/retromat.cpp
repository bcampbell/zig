#include "retromat.h"

#include <cmath>


float g_RFreq = 8000.0f;
float g_RStep = 0.0;


void SetRetromatFreq( int samplerate ) 
{
    g_RFreq = samplerate;
    g_RStep = 1.0f / (float)samplerate;
}




ADSR::ADSR() :
    m_State(IDLE),
    m_StateTime(0.0f),
    m_Attack(0.0f),
    m_Decay(0.0f),
    m_SustainLevel(0.0f),
    m_Release(0.0f)
{
}

float ADSR::tick()
{
    m_StateTime += g_RStep;
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
    assert(false);  // should _never_ get here!
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

