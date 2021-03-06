

#ifndef LEVEL_H
#define LEVEL_H

#include "zig.h"
#include "scene.h"

class PauseMenu;

//
// The Level class is the basic unit which encapsulates the main game action.
//


class Level;
class Agent;
class Background;
class LevelDef;
class Arena;


class Level : public Scene
{
public:
	Level();
	virtual ~Level();

	// get the size of the bordering circle
	float ArenaRadius() const;
	void ShrinkArenaBy( float d );

	// Overrides to implement Scene class
	virtual void Render();
	virtual void Tick();
	virtual Scene* NextScene();
	virtual void HandleKeyDown( SDL_Keysym& keysym );
    virtual void HandleFocusLost();
private:

	void DrawHUD();
	void Restart();
	void Populate();
	void EvalState();
	void DoBaiters();
	void DoUFOs();
	void DoArenaShrinkage();
	void GiveKeepYourSectorTidyBonus();

	enum
	{
		intro,
		inplay,
		dying,
		victorydance,
		completed,
		thatsitmangameovermangameover,
		quit,
		demofinished
	} m_State;

	float m_StateTimer;
	float m_ArenaShrinkage;
	Arena* m_Arena;

	LevelDef const* m_LevelDef;
	Background* m_Background;
	bool m_AttractMode;
	PauseMenu*	m_PauseMenu;

	int		m_Attempt;		// how many times player has died

	float	m_BaiterDelay;	// time remaining until next baiter
	int		m_BaiterCount;	// num of baiters due to spawn next time

	float	m_VictoryDuration;
};



#endif // LEVEL_H


