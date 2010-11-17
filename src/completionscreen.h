#ifndef COMPLETIONSCREEN_H
#define COMPLETIONSCREEN_H

#include "scene.h"

class CompletionScreen : public Scene
{
public:
	CompletionScreen( int wrapnum );
	virtual ~CompletionScreen();
protected:
	virtual void Render();
	virtual void Tick();
	virtual bool IsFinished();
	virtual void KeyDown( SDL_keysym& keysym );
private:
	CompletionScreen();

	bool m_Done;
	int m_WrapNum;
	float m_Elapsed;
};


#endif // COMPLETIONSCREEN_H


