

#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include "scene.h"
#include "menusupport.h"


// Displays the titlescreen

class TitleMenu : public Menu
{
public:
	TitleMenu();
	enum ResultID { ID_PLAY, ID_OPTIONS, ID_EXIT };
	ResultID GetResult() const;
	bool IsDone() const
		{ return m_Done; }
private:
	virtual void OnSelect( int id );

	ResultID m_Result;
	bool m_Done;
};


class TitleScreen : public Scene
{
public:

	TitleScreen();
	~TitleScreen();

	virtual void Render();
	virtual void Tick();
    virtual Scene* NextScene();
public:
	bool m_Escaped;
	bool m_Play;
	bool m_Config;
	int m_CommentNum;
	TitleMenu m_Menu;
};


#endif // TITLESCREEN_H


