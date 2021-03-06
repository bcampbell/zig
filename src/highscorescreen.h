

#ifndef HIGHSCORESCREEN_H
#define HIGHSCORESCREEN_H

#include "zig.h"
#include "scene.h"

class HighScores;

class HighScoreScreen : public Scene
{
public:
	HighScoreScreen();
	virtual ~HighScoreScreen();
	
	void EntryMode( int scoreidx );

	virtual void Render();
	virtual void Tick();
	virtual Scene* NextScene();
	virtual void HandleKeyDown( SDL_Keysym& keysym );
private:

	float m_Time;

	// index of score being entered (-1=none)
	int m_EntryTarget;

    // current index of char for gamepad-based entry
    int m_StickIdx;
};


#endif // HIGHSCORESCREEN_H


