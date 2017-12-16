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
    Texture* m_SnowTexture;
    void DrawDude(int n);
    bool m_Done;
    float m_Time;

    enum {NUM_KINDS=15, NUM_TO_SHOW=8};

    int m_Picks[NUM_TO_SHOW];
};


#endif // DUDEGALLERY_H

