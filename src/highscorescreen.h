

#ifndef HIGHSCORESCREEN_H
#define HIGHSCORESCREEN_H

#include "zig.h"
#include "scene.h"

class HighScores;

class HighScoreScreen : public Scene
{
public:
	HighScoreScreen( HighScores& scores );
	virtual ~HighScoreScreen();
	
	void EntryMode( int scoreidx );

	virtual void Render();
	virtual void Tick();
	virtual bool IsFinished();
	virtual void HandleKeyDown( SDL_Keysym& keysym );
public:
	HighScoreScreen();

	HighScores& m_Scores;
	int m_TimeOut;

	// index of score being entered (-1=none)
	int m_EntryTarget;
};


#endif // HIGHSCORESCREEN_H


