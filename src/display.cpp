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


Display::~Display()
{
    if(m_Window) {
        SDL_DestroyWindow(m_Window);
    }
	assert( m_Textures.empty() );
}

void Display::Present()
{
    glFlush();
    SDL_GL_SwapWindow(m_Window);
}

void Display::Extent(vec2 (&out)[4])
{
    int i;
    for (i=0; i<4; ++i)
    {
        out[i] = m_Extent[i];
    }
}

void Display::ChangeSettings(bool fullscreen )
{
    if(fullscreen)
    {
        SDL_SetWindowFullscreen(m_Window,SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    else
    {
        SDL_SetWindowFullscreen(m_Window,0);
    }

#if 0
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
#endif
}




Display::Display( bool fullscreen ) : m_Window(0)
{
    Uint32  flags = SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE;
    if(fullscreen) {
//        flags |= SDL_WINDOW_FULLSCREEN;
//        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    m_Window = SDL_CreateWindow("Zig",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640, 480,
        flags);
	if( !m_Window )
	{
		Wobbly e("SDL_CreateWindow() failed");	//: %s\n", SDL_GetError() );
		throw e;
	}

    m_GLContext = SDL_GL_CreateContext(m_Window);

    // TODO: call SDL_GL_SetSwapInterval()
/*
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
*/
//	SDL_ShowCursor(SDL_DISABLE);	// hide the mouse


	//----------------------------------------------

	InitGLExtensions();

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( -VW/2.0f, VW/2.0f, -VH/2.0f, VH/2.0f, -1.0, 1.0 );
	glMatrixMode( GL_MODELVIEW );

	//glViewport( 0,0, res.w, res.h );
	//glViewport( 0,0, 640,480 );
    int w,h;	
    SDL_GetWindowSize(m_Window,&w,&h);
    HandleResize(w,h);
}

void Display::HandleResize( int winw, int winh )
{
    const float target_aspect = (float)VW/(float)VH;
    float r = ((float)winw/(float)winh) / target_aspect;
    float vw = VW;
    float vh = VH;
    if(r >= 1.0f) {
        // widescreen
        vw *= r;
    } else {
        // tallscreen
        vh *= (1.0f/r);
    }
    float xmag = vw/2.0;
    float ymag = vh/2.0;
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

    glOrtho( -xmag, xmag, -ymag,ymag, -1.0, 1.0 );
	glMatrixMode( GL_MODELVIEW );
	glViewport( 0,0, winw, winh );

    m_Extent[0] = vec2(-xmag,ymag);
    m_Extent[1] = vec2(xmag,ymag);
    m_Extent[2] = vec2(xmag,-ymag);
    m_Extent[3] = vec2(-xmag,-ymag);

   // printf("resize %dx%d (%f) -> %fx%f\n",winw,winh,r,vw,vh);
}


void Display::TakeScreenshot()
{
#if 0
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
#endif
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





