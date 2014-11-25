#include <assert.h>
#include <cstdio>
#include <SDL.h>
#include <png.h>

#include "wobbly.h"


static SDL_Surface* Read( FILE* fp );


SDL_Surface* LoadPNG( const char* filename )
{
	SDL_Surface* s = 0;
	FILE* fp = fopen( filename, "rb" );
	if (!fp)
		throw Wobbly( "couldn't open '%s'", filename );

	try
	{
		s=Read( fp );
		assert( s );
	}
	catch( Wobbly& e )
	{
		fclose(fp);
		throw Wobbly( "Error loading \"%s\": %s", filename, e.what() );
	}


	fclose( fp );
	return s;
}




static SDL_Surface* Read( FILE* fp )
{
	SDL_Surface *surface = 0;
	png_structp png_ptr = 0;
	png_infop info_ptr = 0;
	png_bytep* row_pointers = 0;

	try
	{
		png_byte header[8];
		fread( header, 1, sizeof(header), fp );
		bool ispng = !png_sig_cmp( header, 0, sizeof(header) );
		if( !ispng )
			throw Wobbly( "not a png file");

		png_ptr = png_create_read_struct(
			PNG_LIBPNG_VER_STRING,
			0,	//(png_voidp)user_error_ptr,
			0,	//user_error_fn,
			0 );	//user_warning_fn );
		if( !png_ptr )
			throw Wobbly( "out of memory (png_create_read_struct() failed)" );

		info_ptr = png_create_info_struct(png_ptr);
		if( !info_ptr )
			throw Wobbly( "out of memory (png_create_info_struct() failed)" );

		if( setjmp( png_jmpbuf( png_ptr ) ) )
			throw( "read failed" );

		png_init_io( png_ptr, fp );
		png_set_sig_bytes( png_ptr, sizeof(header) );


		png_read_info( png_ptr, info_ptr );
		png_uint_32 width;
		png_uint_32 height;
		int bitdepth;
		int colourtype;
		png_get_IHDR( png_ptr, info_ptr, &width, &height, &bitdepth,
			&colourtype, 0,0,0 );

		// set up some transforms

		if( colourtype == PNG_COLOR_TYPE_PALETTE )
			png_set_palette_to_rgb( png_ptr );

		if( colourtype == PNG_COLOR_TYPE_GRAY && bitdepth<8 )
			png_set_expand_gray_1_2_4_to_8( png_ptr );

	//    if( png_get_valid(png_ptr, info_ptr,
	//        PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
		if( bitdepth == 16 )
			png_set_strip_16(png_ptr);


		// update our info to reflect the transforms we specified
		png_read_update_info(png_ptr, info_ptr);
		png_get_IHDR( png_ptr, info_ptr, &width, &height, &bitdepth,
			&colourtype, 0,0,0 );



//		int channels = png_get_channels(png_ptr, info_ptr);

//		printf("channels=%d\n",channels);
//		printf( "bitdepth=%d\n", bitdepth );
//		printf( "colourtype=%d\n", colourtype );

		// set up the SDL surface we want to read into:
		int depth;
		Uint32 rmask, gmask, bmask, amask;
		if( colourtype == PNG_COLOR_TYPE_RGB )
		{
			assert( png_get_channels(png_ptr, info_ptr) == 3 );
			depth=24;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			rmask = 0x00ff0000;
			gmask = 0x0000ff00;
			bmask = 0x000000ff;
			amask = 0x00000000;
#else
			rmask = 0x000000ff;
			gmask = 0x0000ff00;
			bmask = 0x00ff0000;
			amask = 0x00000000;
#endif
		}
		else if( colourtype == PNG_COLOR_TYPE_RGB_ALPHA )
		{
			assert( png_get_channels(png_ptr, info_ptr) == 4 );
			depth=32;
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
		}
		else
		{
			throw Wobbly( "unsupported colour type" );
		}

		if( bitdepth != 8 )
			throw Wobbly( "Unsupported bitdepth (%d)", bitdepth );

		surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, depth,
									   rmask, gmask, bmask, amask);
		if( !surface )
			throw Wobbly( "out of memory (SDL_CreateRGBSurface() failed)" );

		assert( png_get_rowbytes( png_ptr, info_ptr ) <= surface->pitch );


		// point png at our surface rows
		row_pointers = new png_bytep[height];
		unsigned int y;
		for( y=0; y<height; ++y )
		{
			row_pointers[y] =  (Uint8*)surface->pixels + (y * surface->pitch);
		}

		png_read_image(png_ptr, row_pointers);
		png_read_end( png_ptr, 0 );
	}
	catch( Wobbly& e )
	{
		if( png_ptr && info_ptr )
			png_destroy_read_struct(&png_ptr, &info_ptr, 0 );
		else if( png_ptr )
			png_destroy_read_struct(&png_ptr, 0, 0 );
		if( surface )
			SDL_FreeSurface( surface );

		throw;
	}


	delete [] row_pointers;

	png_destroy_read_struct(&png_ptr, &info_ptr, 0 );
	return surface;
}




