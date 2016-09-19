

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
	virtual ~Scene()	{};

	// draw the scene
	virtual void Render()=0;

	// update the state of the scene
	virtual void Tick()=0;

    // Returns the scene to be used next time.
    // Can be "this" (if still going), a new scene, or NULL to exit.
    // Scenes are responsible for deleting themselves, eg:
    //
    //     if (done) {
    //         delete this;
    //         return new State_TITLE();
    //     }
    // Rationale: this allows scenes to call specific ctors for the next scene.
    virtual Scene* NextScene()=0;

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

