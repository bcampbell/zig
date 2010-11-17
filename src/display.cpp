#include "display.h"
#include "drawing.h"
#include "wobbly.h"
#include "texture.h"
#include "image.h"
#include "zig.h"

#include <SDL_opengl.h>
#include <SDL.h>
#include <algorithm>
#include <sys/stat.h>
#include <errno.h>

//------------------------------------------------
// Display class

Display::Display( Res res, int depth, bool fullscreen )
{
	CalcAvailableResolutions();
	OpenMode( res, depth, fullscreen );
}


Display::~Display()
{
	assert( m_Textures.empty() );
}


void Display::ChangeSettings( Res res, int depth, bool fullscreen )
{
	// quick sanity check:
	int sdlflags = SDL_OPENGL;
	if( fullscreen )
		sdlflags |= SDL_FULLSCREEN;
	int bpp = SDL_VideoModeOK( res.w, res.h, depth, sdlflags );
	if( !bpp )
		return;	// can't do it. leave mode as is.

//	if( depth != bpp )
//		printf("Can't do depth %d, using %d instead\n", depth, bpp );


	// unload all textures
	std::list< Texture* >::iterator it;
	for( it=m_Textures.begin(); it!=m_Textures.end(); ++it )
	{
		GLuint id = (*it)->ID();
		glDeleteTextures(1, &id );
	}

	// switch to new mode (may recreate the GL context)
	OpenMode( res, bpp, fullscreen );

	// rebuild textures
	for( it=m_Textures.begin(); it!=m_Textures.end(); ++it )
	{
		GLuint id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		(*it)->SetID( id );
		(*it)->UploadToGL();
	}
}




// helper fn - set up display, make sure textures are OK
void Display::OpenMode(Res res, int depth, bool fullscreen )
{
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	int sdlflags = SDL_OPENGL;
	if( fullscreen )
		sdlflags |= SDL_FULLSCREEN;
	m_Screen =	SDL_SetVideoMode( res.w, res.h, depth, sdlflags );
	if( !m_Screen )
	{
		Wobbly e("SDL_SetVideoMode() failed");	//: %s\n", SDL_GetError() );
		throw e;
	}
	SDL_ShowCursor(SDL_DISABLE);	// hide the mouse


	//----------------------------------------------

	InitGLExtensions();

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( -VW/2.0f, VW/2.0f, -VH/2.0f, VH/2.0f, -1.0, 1.0 );
	glMatrixMode( GL_MODELVIEW );

	glViewport( 0,0, res.w, res.h );
	
}




// build up internal list of resolutions we allow.
void Display::CalcAvailableResolutions()
{
	std::vector<Res>& reslist = m_Resolutions;
	assert( reslist.empty() );

	SDL_Rect **modes;
	modes = SDL_ListModes( 0, SDL_FULLSCREEN|SDL_OPENGL );

	if( modes==(SDL_Rect **)0 )
		throw Wobbly( "No video modes available!" );
	if( modes!=(SDL_Rect **)-1 )
	{
		int i=0;
		while( modes[i] )
		{
			Res r( (int)modes[i]->w,(int)modes[i]->h );
			++i;

			if( find( reslist.begin(), reslist.end(), r ) != reslist.end() )
				continue;	// aldready got this mode!

			reslist.push_back( r );
		}
	}

	if( reslist.empty() )
	{
		// uh-oh. Lets add some safe defaults...
		reslist.push_back( Res( 640,480 ) );
		reslist.push_back( Res( 800,600 ) );
		reslist.push_back( Res( 1024,768 ) );
	}

	// sort it.
	std::sort( reslist.begin(), reslist.end() );
}





Res Display::CurrentRes() const
{
	return Res( m_Screen->w, m_Screen->h );
}

int Display::CurrentDepth() const
{
	int depth = m_Screen->format->BitsPerPixel;
	if( depth == 32 || depth == 24 )
		return 32;
	else
		return 16;
}


bool Display::IsFullscreen() const
{
	return (m_Screen->flags & SDL_FULLSCREEN) ? true:false;
}


// find index of res r (-1 if no match)
int Display::MatchRes( Res const& r ) const
{
	unsigned int i;
	for( i=0; i<m_Resolutions.size(); ++i )
	{
		if( r == m_Resolutions[i] )
			return i;
	}
	return -1;
}






void Display::TakeScreenshot()
{
	char filename[256];
	int id=0;
	while( true )
	{
		struct stat inf;
		sprintf( filename, "zig_screenshot%d.bmp", id );
		if( stat( filename, &inf ) < 0 && errno == ENOENT )
			break;	// got one!
		++id;
		if( id > 255 )
			return;
	}

	SDL_Surface* tmp;
	int w = m_Screen->w;
	int h = m_Screen->h;

	tmp = SDL_CreateRGBSurface( SDL_SWSURFACE, w, h,
		24, 0x0000FF, 0x00FF00, 0xFF0000, 0x000000 );

	int y;
	for( y=0; y<h; ++y )
	{
		// gotta flip the screen
		int srcy = (h-1)-y;
		uint8_t* dest = (uint8_t*)tmp->pixels;
		dest += y*tmp->pitch;
		glReadPixels( 0, srcy, w, 1, GL_RGB, GL_UNSIGNED_BYTE, dest );
	}

	SDL_SaveBMP( tmp, filename );
	SDL_FreeSurface( tmp );

	printf("Screenshot '%s'\n", filename );
	fflush( stdout );
}



void Display::AddTexture( Texture* t )
{
	m_Textures.push_back(t);

	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	t->SetID( id );
	t->UploadToGL();
}


void Display::RemoveTexture( Texture* t)
{
	if( t == 0 )
		return;
	GLuint id = t->ID();
	glDeleteTextures(1, &id );
	m_Textures.remove(t);
}





