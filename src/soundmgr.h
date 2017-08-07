

#ifndef SOUNDMGR_H
#define SOUNDMGR_H

#include <assert.h>
#include <vector>
#include <string>

//#define DISABLE_SOUND

#if !defined(DISABLE_SOUND)
#include <SDL_mixer.h>
#endif

#include "retromat/retromat.h"

// sound effect IDs
enum sfxid_t
{
	SFX_PLAYERFIRE=0,
	SFX_SMALLTHUD,
	SFX_THRUST,
    SFX_CHARGEUP,
	SFX_ELECTRIC,
	SFX_DULLBLAST,
	SFX_BAITERALERT,
	SFX_PLAYERTOAST,
	SFX_LEVELINTRO,
	SFX_BIGEXPLOSION,
	SFX_GAMEOVER,
	SFX_WIBBLEPOP,
	SFX_NUM_EFFECTS
};


//--------------------------------------------------------------
//
class SoundMgr
{
public:
	// singleton stuff
	// Note: SoundMgr creation is handled by the derived classes

	// Close down the soundmgr
	static void Destroy();

	// access the single SoundMgr
	static SoundMgr& Inst();

	// returns true if SoundMgr is running
	static bool Running();


	// play a one-shot sound effect
	virtual void Play( sfxid_t id )=0;

	// play a sound on a continuous loop.
	virtual void PlayLooped( int chan, sfxid_t id, int fadeinms=0 )=0;

	// stop a sound started with PlayLooped().
	virtual void StopLooped( int chan, int fadems=0 )=0;

    virtual bool IsPlaying( int chan )=0;
    virtual bool IsFadingIn( int chan )=0;
    virtual bool IsFadingOut( int chan )=0;

    virtual int AllocChan() = 0;
    virtual void FreeChan(int chan) = 0;

    virtual const char* DebugString() {return "";}

protected:
	// the single allowed soundmgr.
	static SoundMgr*	s_Instance;

	// private ctors and dtors - creation is handled by derived classes,
	// destruction is handled by Destroy().
	SoundMgr()
		{}
	virtual ~SoundMgr()
		{};

	SoundMgr( SoundMgr const& );	// not allowed
};




inline SoundMgr& SoundMgr::Inst()
{
	assert( s_Instance != 0 );
	return *s_Instance;
}

inline bool SoundMgr::Running()
{
	return ( s_Instance != 0 );
}



//--------------------------------------------------------------
// A dummy SoundMgr which does nothing.

class NullSoundMgr : public SoundMgr
{
public:
	// Create the singleton SoundMgr (as a NullSoundMgr)
	static void Create();

	virtual void Play( sfxid_t id ) {}
	virtual void PlayLooped( int chan, sfxid_t id, int fadeinms=0 ) {}
	virtual void StopLooped( int chan, int fadems=0 ) {}
    virtual bool IsPlaying( int chan ) { return false; }
    virtual bool IsFadingIn( int chan ) { return false; }
    virtual bool IsFadingOut( int chan ) { return false; }
    virtual int AllocChan()             {return -1; }
    virtual void FreeChan(int chan)  {}
private:
	NullSoundMgr();

	NullSoundMgr( NullSoundMgr const& );	// not allowed
};



#if !defined(DISABLE_SOUND)
//--------------------------------------------------------------
// RealSoundMgr uses SDL_mixer lib to do it's stuff.

class RealSoundMgr : public SoundMgr
{
public:
	// Create the singleton SoundMgr (as a RealSoundMgr)
	static void Create();

	virtual void Play( sfxid_t id );
	virtual void PlayLooped( int chan, sfxid_t id, int fadeinms=0 );
	virtual void StopLooped( int channel, int fadems=0 );
    virtual bool IsPlaying( int chan ) { return Mix_Playing(chan) ? true:false; }
    virtual bool IsFadingIn( int chan ) { return Mix_FadingChannel(chan)==MIX_FADING_IN ? true:false; }
    virtual bool IsFadingOut( int chan ) { return Mix_FadingChannel(chan)==MIX_FADING_OUT ? true:false; }
    virtual int AllocChan();
    virtual void FreeChan(int chan);
    virtual const char* DebugString();
private:
	RealSoundMgr();
	virtual ~RealSoundMgr();

//	void LoadSound( sfxid_t id, std::string const& filename );
	void GenerateSounds();
	Mix_Chunk* ConvertToMixChunk( std::vector<float> const& src );

	Mix_Chunk* Gen( RetromatFn fn );


    int findFreeChan();

	RealSoundMgr( RealSoundMgr const& );	// not allowed
	std::vector< Mix_Chunk* > m_Sounds;		// our loaded sounds

	int m_DeviceFreq;
	unsigned short int m_DeviceFmt;
	int m_DeviceChannels;

    int m_NumChans;
    // which channels have been allocated
    bool *m_Alloced;

};

#endif //



// a looped sound. Stops when it goes out of scope (if playing)
class ScopedSnd
{
public:
    ScopedSnd() : m_Chan(-1)
    {
        m_Chan = SoundMgr::Inst().AllocChan();
        //printf("ScopedSnd() [chan %d]\n",m_Chan);
    }
    ~ScopedSnd()
    {
        //printf("~ScopedSnd() [chan %d]\n", m_Chan);
        if( m_Chan>=0 )
        {
            Stop();
            SoundMgr::Inst().FreeChan(m_Chan);
        }
    }
    void Start(sfxid_t sfxid, int fadeinms=0)
    {
        if (m_Chan<0)
            return; // TODO: try allocating again
        SoundMgr::Inst().PlayLooped(m_Chan, sfxid,fadeinms);
        //printf("playing %d on chan %d\n",sfxid, m_Chan);
    }

    void Stop(int fadeoutms=0)
    {
        if (m_Chan<0)
            return;
        //printf("stop chan %d\n", m_Chan);
        SoundMgr::Inst().StopLooped(m_Chan,fadeoutms);
    }


    bool IsPlaying() { return SoundMgr::Inst().IsPlaying(m_Chan); }
    bool IsFadingOut() { return SoundMgr::Inst().IsFadingOut(m_Chan); }
private:
    int m_Chan;
};


#endif // SOUNDMGR_H

