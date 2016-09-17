#ifndef OPTIONSSCREEN_H
#define OPTIONSSCREEN_H

#include "scene.h"
#include "optionsmenu.h"



class OptionsScreen : public Scene
{
public:
	virtual void Render();
	virtual void Tick();
    virtual SceneResult Result();
private:
	OptionsMenu m_Menu;
};


#endif // OPTIONSSCREEN_H


