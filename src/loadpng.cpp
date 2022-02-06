#include <SDL.h>
#include <SDL_rwops.h>
/*
#include <assert.h>
#include <errno.h>
#include <png.h>
#include <string.h>
#include <cstdio>
*/
#include "wobbly.h"
#include "lodepng.h"

SDL_Surface* LoadPNG(const char* filename) {

  size_t rawsize;
  uint8_t* raw = (uint8_t*)SDL_LoadFile(filename, &rawsize);
  if (!raw) {
    throw Wobbly("Error loading \"%s\"", filename); 
  }

  SDL_Surface* surface = nullptr;

  unsigned char* out;
  unsigned w,h;
  unsigned rv = lodepng_decode32(&out, &w, &h, raw, rawsize);
  if (rv != 0) {
    SDL_free(raw);
    throw Wobbly("decode failed \"%s\" (code %d)", filename, rv);
  }

  Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  rmask = 0xff000000;
  gmask = 0x00ff0000;
  bmask = 0x0000ff00;
  amask = 0x000000ff;
#else
  rmask = 0x000000ff;
  gmask = 0x0000ff00;
  bmask = 0x00ff0000;
  amask = 0xff000000;
#endif
  surface = SDL_CreateRGBSurface(0, w, h, 32,
    rmask, gmask, bmask, amask);
  if (!surface) {
    free(out);
    SDL_free(raw);
    throw Wobbly("out of memory (SDL_CreateRGBSurface() failed)");
  }
  uint8_t const* src = out;
  for (unsigned y = 0; y < h; ++y) {
    uint8_t *dest = (Uint8*)surface->pixels + (y * surface->pitch);
    memcpy(dest, src, w*4);
    src += w*4;
  }

  // TODO - only if alpha channel!!!!!!!!
  //SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);

  free(out);
  SDL_free(raw);
  return surface;
}
