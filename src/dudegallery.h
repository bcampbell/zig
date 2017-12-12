#ifndef DUDEGALLERY_H
#define DUDEGALLERY_H

#include "scene.h"


class DudeGallery : public Scene
{
public:

	DudeGallery();
	~DudeGallery();

	virtual void Render();
	virtual void Tick();
    virtual Scene* NextScene();
    virtual void HandleKeyDown( SDL_Keysym& keysym );
private:
    void DrawDude(int n);
    bool m_Done;
    float m_Time;
};


#endif // DUDEGALLERY_H

