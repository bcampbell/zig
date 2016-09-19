

#ifndef GAMEOVER_H
#define GAMEOVER_H

#include "scene.h"

// displays "Game Over" on a blank screen for a couple of seconds.

class GameOver : public Scene
{
public:
	GameOver();
	virtual void Render();
	virtual void Tick();
	virtual Scene* NextScene();
public:
	float m_Timer;
	bool m_Done;
};

#endif // GAMEOVER_H
