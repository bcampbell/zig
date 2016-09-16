

#ifndef SCENE_H
#define SCENE_H

#include <string>

#include <SDL_keyboard.h>
#include <SDL_events.h>

// Scene encapsulates the main timing and event loop.
// Derived classes handle the main game action (the Level class), titlescreen
// and any other sequence that needs to update and display.
//

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
	virtual bool IsFinished()=0;

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

