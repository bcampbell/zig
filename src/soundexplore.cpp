#include <SDL.h>
#include <SDL_opengl.h>

#include "soundexplore.h"
#include "controller.h"
#include "drawing.h"
#include "soundmgr.h"
#include "menusupport.h"
#include "zig.h"




class SoundMenu : public Menu
{
public:
    SoundMenu();
    virtual ~SoundMenu();
    bool IsDone() const { return m_Done; }
private:
	virtual void OnSelect( int id );
	virtual void OnFocus( int id );
    void Shush();
    bool m_Done;
    ScopedSnd m_Snd;
};

SoundMenu::SoundMenu() :
    m_Done(false)
{
    vec2 pos(0.0f,100.0f);
    int i;
    for (i=0; i<SFX_NUM_EFFECTS; ++i)
    {
        char txt[64];
        sprintf(txt,"SOUND %d",i);
        pos.y -= 32.0f;
        MenuItem* item = new MenuItem(i,pos,txt);
        AddItem(item);
    }

    pos.y -= 40.0f;
    AddItem( new MenuItem(-1,pos,"EXIT", true, CTRL_BTN_ESC )); 

}


void SoundMenu::Shush()
{
    m_Snd.Stop();
}


SoundMenu::~SoundMenu()
{
    Shush();
}

void SoundMenu::OnSelect( int id )
{
    Shush();
    if (id<0)
    {    // exit?
        m_Done = true;
        return;
    }
    m_Snd.Start((sfxid_t)id);
}


void SoundMenu::OnFocus( int id )
{
    Shush();
}



SoundExplore::SoundExplore() :
    m_Menu(0)
{
    m_Menu = new SoundMenu();
}


SoundExplore::~SoundExplore()
{
    delete m_Menu;
}

void SoundExplore::Render()
{
	glClear( GL_COLOR_BUFFER_BIT );
	glLoadIdentity();

	glPushMatrix();
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		glTranslatef( 0.0f, 150.0f, 0.0f );
		PlonkText( *g_Font, "SOUNDS", true );
	glPopMatrix();
    m_Menu->Draw();
}

void SoundExplore::Tick()
{
    m_Menu->Tick();
}

SceneResult SoundExplore::Result()
{
	return m_Menu->IsDone() ? DONE:NONE;
}


