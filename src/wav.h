#ifndef WAV_H_INCLUDED
#define WAV_H_INCLUDED

#include <cstdint>

// write out a PCM wave file
bool wav_dump( const char* filename, int bytespersample, int channels, int freq, void const* data, int bytecnt);

#endif // WAV_H_INCLUDED

