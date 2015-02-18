#include "wav.h"

#include <cstdio>

inline static uint8_t* put_4CC( uint8_t* out, const char* id)
{
    *out++=*id++; *out++=*id++; *out++=*id++; *out++=*id++;
    return out;
} 

inline static uint8_t* put_u32( uint8_t* out, uint32_t v)
{
    *out++ = v & 0xff;
    *out++ = (v>>8) & 0xff;
    *out++ = (v>>16) & 0xff;
    *out++ = (v>>24) & 0xff;
    return out;
}

inline static uint8_t* put_u16( uint8_t* out, uint16_t v)
{
    *out++ = v & 0xff;
    *out++ = (v>>8) & 0xff;
    return out;
}

bool wav_dump( const char* filename, int bytespersample, int channels, int freq, void const* data, int bytecnt)
{
    // room for file header, fmt chunk and data chunk header
    uint8_t buf[(8+4)+(8+18) + 8];

    uint8_t* p=buf;
    p=put_4CC(p,"RIFF");
    p=put_u32(p,4 + (8+18) + (8+bytecnt));
    p=put_4CC(p,"WAVE");
    // fmt chunk
    p=put_4CC(p,"fmt ");
    p=put_u32(p,18);        // chunksize
    p=put_u16(p,0x0001);    // wFormatTag (WAVE_FORMAT_PCM)
    p=put_u16(p,channels);  // nChannels
    p=put_u32(p,freq);      // nSamplesPerSec
    p=put_u32(p,freq*channels*bytespersample);  // nAvgBytesPerSec
    p=put_u16(p,bytespersample*channels);       // nBlockAlign
    p=put_u16(p,bytespersample*8);              // wBitsPerSample
    p=put_u16(p,0);         // cbSize

    // data chunk (header only)
    p=put_4CC(p,"data");
    p=put_u32(p,bytecnt);

    FILE* fp = fopen(filename,"wb");
    if (!fp)
        return false;

    if (fwrite(buf,sizeof(buf),1,fp) != 1)
    {
        fclose(fp);
        return false;
    }

    if (fwrite(data,bytecnt,1,fp) != 1)
    {
        fclose(fp);
        return false;
    }
    fclose(fp);
    return true;
}


