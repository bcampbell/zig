#ifndef ICONGENSCREEN_H
#define ICONGENSCREEN_H

// Hackery for generating images to use for building an icon.
// runs onscreen, then when you press escape, dumps some .tga files
// out to /tmp and exits.
//

//#define ENABLE_ICONGEN_HACKERY


#ifdef ENABLE_ICONGEN_HACKERY

#define GL_GLEXT_PROTOTYPES
#include <SDL_opengl.h>
#include "scene.h"


class Arena;
class Background;


class IconGenScreen : public Scene
{
public:

	IconGenScreen();
	~IconGenScreen();

	virtual void Render();
	virtual void Tick();
    virtual Scene* NextScene();
    void HandleKeyDown( SDL_Keysym& keysym );
public:
    void RenderBG();
    void RenderPlayer();
    void RenderTrail();

    void initOffscreen(int w,int h);
    void DumpEm();
    void ditchOffscreen();

    void DumpTGA( const char* filename);
    Arena* m_Arena;
	Background* m_Background;
	bool m_Done;

    int m_W;
    int m_H;
    GLuint m_FrameBuf;
    GLuint m_Texture;
    GLuint m_Depth;
};

#endif // ENABLE_ICONGEN_HACKERY

#endif // ICONGENSCREEN_H

