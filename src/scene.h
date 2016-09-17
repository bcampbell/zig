

#ifndef SCENE_H
#define SCENE_H

#include <string>

#include <SDL_keyboard.h>
#include <SDL_events.h>

// Scene encapsulates the main timing and event loop.
// Derived classes handle the main game action (the Level class), titlescreen
// and any other sequence that needs to update and display.
//


enum SceneResult {
    NONE=0, // scene is still running
    HARDEXIT,
    DONE,   // scene complete (eg level finished)
    CANCEL, // cancelled, eg escape key pressed
    STARTGAME,  // request to start a new game
    GAMEOVER,   // game has finished
    TIMEOUT,    // scene timed out (eg demo starts if title idle)
    CONFIG      // request for config screen
};

class Scene
{
public:


	// Run the scene.
	// 
	// Returns when the scene is over (ie when the derived IsFinished()
	// returns true).
	// If app window is closed, Run() throws a QuitNotification exception.
	void Run();

	virtual ~Scene()	{};

	class QuitNotification
	{
	};

	// draw the scene
	virtual void Render()=0;

	// update the state of the scene
	virtual void Tick()=0;

	// derived classes must provide an IsFinished() which returns true when
	// the scene is over.
	//virtual bool IsFinished()=0;

    virtual SceneResult Result()=0;

	// if the derived scene wants to handle key events it can override this.
	virtual void HandleKeyDown( SDL_Keysym& keysym )
	{}

    // incoming text, cooked into utf-8
    virtual void HandleTextInput( SDL_TextInputEvent& ev)
    {}

    // app has lost focus
    virtual void HandleFocusLost()
    {}

private:
};


#endif // SCENE_H

