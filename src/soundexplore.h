#ifndef SOUNDEXPLORE_H
#define SOUNDEXPLORE_H

#include "scene.h"

class SoundMenu;

// dev screen to play sound effects from menu



class SoundExplore : public Scene
{
public:
	SoundExplore();
	~SoundExplore();
	virtual void Render();
	virtual void Tick();
	virtual Scene* NextScene();
private:
    SoundMenu* m_Menu;
};

#endif // SOUNDEXPLORE_H
