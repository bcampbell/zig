

#include <SDL.h>
#include "drawing.h"
#include "texture.h"
#include "zig.h"
#include "vec2.h"
#include "mathutil.h"


// some global vars

bool g_GLMultitexture = false;
int g_NumTextureUnits = 0;
bool g_ARB_texture_env_add = false;

#ifdef _WIN32
PFNGLACTIVETEXTUREARBPROC		g_glActiveTextureARB = 0;
PFNGLMULTITEXCOORD2FARBPROC		g_glMultiTexCoord2fARB = 0;
#else
GL_ActiveTextureARB_Func g_glActiveTextureARB = 0;
GL_MultiTexCoord2fARB_Func g_glMultiTexCoord2fARB = 0;
#endif



// origin is at bottom-left corner of text bounding rect.
void PlonkText( Texture& font,
	std::string const& text,
	bool centred,
	float charw,
	float charh )
{

	glBindTexture( GL_TEXTURE_2D,font.ID() );
	glEnable( GL_TEXTURE_2D );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	float x=0.0f;
	float y=0.0f;

	if( centred )
	{
		x -= ( charw * text.size() )/2;
		y -= charh/2;
	}
	
	float du=1.0f/16.0f;	// font is 16x16 array of chars
	float dv=1.0f/16.0f;
	std::string::const_iterator it;
	glBegin( GL_QUADS );
	for(it=text.begin(); it!=text.end(); ++it)
	{
		int c = (int)(*it);
		float u,v;
		v = ((float)(c/16))/16.0f;
		u = ((float)(c%16))/16.0f;
		glTexCoord2f(u,v);
		glVertex2f(x,y+charh);
		glTexCoord2f(u+du,v);
		glVertex2f(x+charw,y+charh);
		glTexCoord2f(u+du,v+dv);
		glVertex2f(x+charw,y);
		glTexCoord2f(u,v+dv);
		glVertex2f(x,y);
		x+=charw;
	}
	glEnd();
	glDisable( GL_TEXTURE_2D );
}



void InitGLExtensions()
{
#ifdef __EMSCRIPTEN__
    return;
#endif
	const char* p = (const char*)glGetString( GL_EXTENSIONS );
	std::string extname;
	while( *p )
	{
		char c = *p++;
		if( c != ' ' )
			extname += c;
		else
		{
	//		printf("'%s'\n", extname.c_str() );
			if( !g_Config.nomultitexture &&
				(extname == "GL_ARB_multitexture" ||
				 extname == "GL_EXT_multitexture" ) )
			{
#ifdef WIN32
				g_glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)
					SDL_GL_GetProcAddress( "glActiveTextureARB" );

				g_glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)
					SDL_GL_GetProcAddress( "glMultiTexCoord2fARB" );
#else
				g_glActiveTextureARB = (GL_ActiveTextureARB_Func)
					SDL_GL_GetProcAddress( "glActiveTextureARB" );

				g_glMultiTexCoord2fARB = (GL_MultiTexCoord2fARB_Func)
					SDL_GL_GetProcAddress( "glMultiTexCoord2fARB" );
#endif
				if( g_glActiveTextureARB && g_glMultiTexCoord2fARB )
				{

					GLint numunits;
					glGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, &numunits );
					g_NumTextureUnits = numunits;

					g_GLMultitexture = true;

//					printf( "got %s (%d textureunits)\n", extname.c_str(), g_NumTextureUnits );
				}
			}
			else if( extname == "GL_ARB_texture_env_add" )
			{
				g_ARB_texture_env_add = true;
//				printf( "got texture_env_add\n" );
			}

			extname.erase();
		}
	}
}



void DrawCircle( vec2 const& pos, float r )
{
	float theta;
	const float step = twopi / 64.0f;
	glBegin( GL_LINE_LOOP );
	for( theta=0.0f; theta<twopi; theta += step )
	{
		const float x = pos.x + r*sin(theta);
		const float y = pos.y + r*cos(theta);
		glVertex2f( x,y );
	}
	glEnd();
}


