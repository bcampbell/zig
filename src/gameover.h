

#ifndef GAMEOVER_H
#define GAMEOVER_H

#include "scene.h"

// displays "Game Over" on a blank screen for a couple of seconds.

class GameOver : public Scene
{
public:
	GameOver( int score, int level );
	virtual void Render();
	virtual void Tick();
	virtual bool IsFinished();
public:
	float m_Timer;
	int m_Score;
	int m_Level;
	bool m_Done;
};

#endif // GAMEOVER_H
