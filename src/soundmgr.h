

#ifndef SOUNDMGR_H
#define SOUNDMGR_H

#include <assert.h>
#include <vector>
#include <string>
#include <SDL_mixer.h>

#include "retromat/retromat.h"

// sound effect IDs
enum
{
	SFX_PLAYERFIRE=0,
	SFX_DULLBLAST,
	SFX_BAITERALERT,
	SFX_PLAYERTOAST,
	SFX_LEVELINTRO,
	SFX_BIGEXPLOSION,
	SFX_GAMEOVER,
//	SFX_FATZAPPERWARN,
//	SFX_FATZAPPERFIRE,
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


	// Load a wavefile into the SoundMgr
	// Sounds stay resident until manager is destroyed.
//	virtual void LoadSound( unsigned int id, std::string const& filename )=0;
	
	// play a one-shot sound effect
	virtual void Play( unsigned int id )=0;


	// play a sound on a continuous loop.
	// returns the channel used to play it, or -1 if it couldn't play
	virtual int PlayLooped( unsigned int id )=0;

	// stop a sound started with PlayLooped().
	virtual void StopLooped( int channel )=0;

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

//	virtual void LoadSound( unsigned int id, std::string const& filename ) {}
	virtual void Play( unsigned int id ) {}
	virtual int PlayLooped( unsigned int id ) { return -1; }
	virtual void StopLooped( int channel  ) {}
private:
	NullSoundMgr();

	NullSoundMgr( NullSoundMgr const& );	// not allowed
};



//--------------------------------------------------------------
// RealSoundMgr uses SDL_mixer lib to do it's stuff.

class RealSoundMgr : public SoundMgr
{
public:
	// Create the singleton SoundMgr (as a RealSoundMgr)
	static void Create();

	virtual void Play( unsigned int id );
	virtual int PlayLooped( unsigned int id );
	virtual void StopLooped( int channel );

private:
	RealSoundMgr();
	virtual ~RealSoundMgr();

	void LoadSound( unsigned int id, std::string const& filename );
	void GenerateSounds();
	Mix_Chunk* ConvertToMixChunk( std::vector<float> const& src );

	Mix_Chunk* Gen( RetromatFn fn );

	RealSoundMgr( RealSoundMgr const& );	// not allowed
	std::vector< Mix_Chunk* > m_Sounds;		// our loaded sounds

	int m_DeviceFreq;
	unsigned short int m_DeviceFmt;
	int m_DeviceChannels;
};




#endif // SOUNDMGR_H

