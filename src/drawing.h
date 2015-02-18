

#ifndef DRAWING_H
#define DRAWING_H

#include <string>
#include <SDL_opengl.h>

class Texture;
class vec2;

extern bool g_GLMultitexture;
extern int g_NumTextureUnits;
extern bool g_ARB_texture_env_add;

#ifdef _WIN32
extern PFNGLACTIVETEXTUREARBPROC		g_glActiveTextureARB;
extern PFNGLMULTITEXCOORD2FARBPROC		g_glMultiTexCoord2fARB;

#else

typedef void (*GL_ActiveTextureARB_Func)( GLenum );
typedef void (*GL_MultiTexCoord2fARB_Func)( GLenum, GLfloat, GLfloat );

extern GL_ActiveTextureARB_Func g_glActiveTextureARB;
extern GL_MultiTexCoord2fARB_Func g_glMultiTexCoord2fARB;

#endif


void InitGLExtensions();


void PlonkText(
	Texture& font,
	std::string const& text,
	bool centred=false,
	float charw=16.0f,
	float charh=16.0f );


void DrawCircle( vec2 const& pos, float r );

#endif // DRAWING_H

