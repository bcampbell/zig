#ifndef OPTIONSSCREEN_H
#define OPTIONSSCREEN_H

#include "scene.h"
#include "optionsmenu.h"



class OptionsScreen : public Scene
{
public:
protected:
	virtual void Render();
	virtual void Tick();
	virtual bool IsFinished();
	virtual void HandleKeyDown( SDL_Keysym& keysym );
private:
	OptionsMenu m_Menu;
};


#endif // OPTIONSSCREEN_H


