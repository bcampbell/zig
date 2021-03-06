
#include "soundmgr.h"
#if !defined(DISABLE_SOUND)
#include "retromat/retromat.h"
#include "retromat/gen.h"
#endif
#include "wobbly.h"
#include "zigconfig.h"
#include "zig.h"
#include "log.h"

#include <math.h>

//-----------------------------------------------------------------
//


// the singleton instance
SoundMgr* SoundMgr::s_Instance = 0;


// static
void SoundMgr::Destroy()
{
	delete s_Instance;
	s_Instance = 0;
}


//-----------------------------------------------------------------

// static
void NullSoundMgr::Create()
{
	s_Instance = new NullSoundMgr;
}


NullSoundMgr::NullSoundMgr()
{
	if( g_Config.sounddebug )
		log_infof("running silent\n" );
}

//-----------------------------------------------------------------


#if !defined(DISABLE_SOUND)

// helper
const char* GetSampleFormatName( int fmt )
{
	switch( fmt )
	{
		case AUDIO_U8:		return "U8";
		case AUDIO_S8:		return "S8";
		case AUDIO_U16LSB:	return "U16LSB";
		case AUDIO_S16LSB:	return "S16LSB";
		case AUDIO_U16MSB:	return "U16MSB";
		case AUDIO_S16MSB:	return "S16MSB";
	};
	return "unknown";
}


// static
void RealSoundMgr::Create()
{
	s_Instance = new RealSoundMgr;
}



RealSoundMgr::RealSoundMgr() : m_Alloced(0)
{	
//	if( Mix_OpenAudio( 44100, AUDIO_S16, 1, 1024 ) < 0 )
//
//
//

	int req_freq = g_Config.soundfreq;
	int req_fmt = MIX_DEFAULT_FORMAT;
	int req_channels = 1;

	if( g_Config.sounddebug )
	{
		log_infof("sound: requesting  frequency=%dHz  format=%s  channels=%d\n",
			req_freq, GetSampleFormatName( req_fmt ), req_channels );
	}

	if( Mix_OpenAudio( req_freq, req_fmt, req_channels, g_Config.soundbuffer ) < 0 )
	{
		Wobbly w("Mix_OpenAudio() failed: %s", Mix_GetError() );
		throw w;
	}

	Mix_QuerySpec( &m_DeviceFreq, &m_DeviceFmt, &m_DeviceChannels );

	if( g_Config.sounddebug )
	{
		log_infof("sound: got  frequency=%dHz  format=%s  channels=%d\n",
			m_DeviceFreq, GetSampleFormatName( m_DeviceFmt ), m_DeviceChannels );
	}

    m_NumChans = Mix_AllocateChannels( 16 );

    m_Alloced = new bool[m_NumChans];
    int i;
    for(i=0; i<m_NumChans; ++i)
    {
        m_Alloced[i]=false;
    }

	GenerateSounds();
}


RealSoundMgr::~RealSoundMgr()
{
	Mix_CloseAudio();

    delete [] m_Alloced;

	while( !m_Sounds.empty() )
	{
		if( m_Sounds.back() != 0 )
			Mix_FreeChunk( m_Sounds.back() );
		m_Sounds.pop_back();
	}
}


int RealSoundMgr::findFreeChan()
{
    int i;
    for (i=0; i<m_NumChans; ++i)
    {
        if (m_Alloced[i])
            continue;
        if (!Mix_Playing(i))
            return i;
    }
    return -1;
}



int RealSoundMgr::AllocChan()
{
    int chan = findFreeChan();
    if (chan>=0)
        m_Alloced[chan] = true;

    return chan;
}

void RealSoundMgr::FreeChan(int chan)
{
    assert(m_Alloced[chan]==true);
    m_Alloced[chan] = false;
}



#if 0
void RealSoundMgr::LoadSound( sfxid_t id, std::string const& filename )
{
	if( id >= m_Sounds.size() )
		m_Sounds.resize( id+1, 0 );

	assert( m_Sounds[id] == 0 );	// can only load once.

	m_Sounds[id] = Mix_LoadWAV( filename.c_str() );
	if( !m_Sounds[id] )
	{
		throw Wobbly( "Failed to load '%s': %s", filename.c_str(),
			Mix_GetError() );
	}
}
#endif

void RealSoundMgr::Play( sfxid_t id )
{
	assert( id<m_Sounds.size() );
	assert( m_Sounds[id] != 0 );

    int chan = findFreeChan();
    if (chan>=0)
    {
        Mix_Volume(chan,128);
    	Mix_PlayChannel( chan, m_Sounds[id], 0 );
    }
}


void RealSoundMgr::PlayLooped( int chan, sfxid_t id, int fadeinms )
{
    assert( m_Alloced[chan]==true );
	assert( id<m_Sounds.size() );
	assert( m_Sounds[id] != 0 );
    assert( chan>=0 && chan<m_NumChans );
 
    Mix_HaltChannel(chan);
    Mix_Volume(chan,128);

    if (fadeinms>0)
        Mix_FadeInChannel( chan, m_Sounds[id], -1,fadeinms );
    else
    {
        Mix_PlayChannel( chan, m_Sounds[id], -1 );
    }
}

void RealSoundMgr::StopLooped( int chan, int fadems )
{
    assert( m_Alloced[chan]==true );
    if(fadems>0)
        Mix_FadeOutChannel(chan, fadems);
    else
        Mix_HaltChannel( chan );
}


//
Mix_Chunk* RealSoundMgr::ConvertToMixChunk( std::vector<float> const& src )
{
	SDL_AudioCVT cvt;
	if( SDL_BuildAudioCVT( &cvt,
		AUDIO_S16SYS, 1, m_DeviceFreq,
		m_DeviceFmt, m_DeviceChannels, m_DeviceFreq ) <0 )
	{
		throw Wobbly( "Audio conversion failure!" );
	}

	cvt.len = src.size() * sizeof(Sint16);
	cvt.buf = (Uint8*)malloc( cvt.len*cvt.len_mult );

	// plonk our data into the front of the buffer
	// (converting to s16 while we're at it)
    // TODO: use SDL native float->s16 conversion
	unsigned int i;
	Sint16* shortp = (Sint16*)cvt.buf;
	for( i=0; i<src.size(); ++i )
	{
		shortp[i] = (Sint16)(src[i]*32767.0f);
	}

	// do it.
	if( SDL_ConvertAudio( &cvt ) < 0 )
		throw Wobbly( "Audio conversion failure!" );

    int n = (int)((double)cvt.len * cvt.len_ratio);

	Mix_Chunk* chunk = Mix_QuickLoad_RAW( cvt.buf, n );
	if( !chunk )
		throw Wobbly( "Out of memory" );

	// pass ownership of buffer to sdl_mixer
	chunk->allocated = 1;

	// TODO: should use cvt.len_ratio to trim buffer down to proper size if needed, but it's not a big deal.

	return chunk;
}



Mix_Chunk* RealSoundMgr::Gen( RetromatFn fn )
{
	std::vector<float> fbuf;
	fbuf.reserve( 32768 );
	fn( fbuf );
	return ConvertToMixChunk( fbuf );
}



void RealSoundMgr::GenerateSounds()
{
	m_Sounds.resize( SFX_NUM_EFFECTS, 0 );


	SetRetromatFreq( m_DeviceFreq );

	m_Sounds[ SFX_PLAYERFIRE ] = Gen( GenerateSmallThud );
	m_Sounds[ SFX_SMALLTHUD ] = Gen( GenerateLaser );
//	m_Sounds[ SFX_PLAYERFIRE ] = Gen( GenerateLaser );
//	m_Sounds[ SFX_SMALLTHUD ] = Gen( GenerateSmallThud );
	m_Sounds[ SFX_ELECTRIC ] = Gen( GenerateElectric );
	m_Sounds[ SFX_DULLBLAST ] = Gen( GenerateDullBlast );
	m_Sounds[ SFX_BAITERALERT ] = Gen( GenerateBaiterAlert );
	m_Sounds[ SFX_PLAYERTOAST ] = Gen( GeneratePlayerToast );
	m_Sounds[ SFX_LEVELINTRO ] = Gen( GenerateLevelIntro );
	m_Sounds[ SFX_BIGEXPLOSION ] = Gen( GenerateBigExplosion );
	m_Sounds[ SFX_GAMEOVER ] = Gen( GenerateGameOver );
	m_Sounds[ SFX_WIBBLEPOP ] = Gen( GenerateWibblePop );
	m_Sounds[ SFX_CHARGEUP ] = Gen( GenerateChargeUp );

	m_Sounds[ SFX_THRUST ] = Gen( GenerateThrust );
}

const char* RealSoundMgr::DebugString()
{
    static char out[128+1];
    char* p = out;
    int i;
    for(i=0;i<m_NumChans; ++i)
    {
        if(m_Alloced[i]) {
            *p++ = 'A';
        } else {
            *p++ = ' ';
        }


        if(Mix_Playing(i)) {
            *p++ = '*';
        } else {
            *p++ = '.';
        }
        switch (Mix_FadingChannel(i)) {
            case MIX_FADING_IN: *p++ = 'I'; break;
            case MIX_FADING_OUT: *p++ = 'O'; break;
            case MIX_NO_FADING:
            default:
               *p++ = '-'; break;
        }
    }
    *p++ = '\0';

    return out;
}

#endif

