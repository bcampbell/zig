#ifndef COMPLETIONSCREEN_H
#define COMPLETIONSCREEN_H

#include "scene.h"

class CompletionScreen : public Scene
{
public:
	CompletionScreen( int wrapnum );
	virtual ~CompletionScreen();
	virtual void Render();
	virtual void Tick();
	virtual SceneResult Result();
	virtual void HandleKeyDown( SDL_Keysym& keysym );
private:
	CompletionScreen();

	bool m_Done;
	int m_WrapNum;
	float m_Elapsed;
};


#endif // COMPLETIONSCREEN_H


