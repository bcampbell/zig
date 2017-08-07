#include <SDL.h>
#include <SDL_opengl.h>

#include "controller.h"
#include "drawing.h"
#include "menusupport.h"
#include "soundexplore.h"
#include "soundmgr.h"
#include "titlescreen.h"
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
	virtual void OnLeft( int id );
	virtual void OnRight( int id );
    void Shush();
    bool m_Done;
    ScopedSnd m_Snd;
public:
    int m_Current;
};

SoundMenu::SoundMenu() :
    m_Done(false),
    m_Current(0)
{
    vec2 pos(0.0f,100.0f);
    pos.y -= 32.0f;
    MenuItem* item = new MenuItem(0,pos,"PLAY");
    AddItem(item);

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
    //m_Snd.Start((sfxid_t)m_Current);
    
    SoundMgr::Inst().Play((sfxid_t)m_Current);

}


void SoundMenu::OnLeft( int id )
{
    if(id==0)
    {
        --m_Current;
        if(m_Current<0)
        {
            m_Current=0;
        }
    }
}

void SoundMenu::OnRight( int id )
{
    if(id==0)
    {
        ++m_Current;
        if(m_Current>(SFX_NUM_EFFECTS-1))
        {
            m_Current = SFX_NUM_EFFECTS-1;
        }
    }
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
	glPushMatrix();
        char buf[16];
        sprintf(buf, "%d", m_Menu->m_Current);
		glTranslatef( 80.0f, 100.0f-32.0f, 0.0f );
        PlonkText( *g_Font, buf, true, 16.0f, 16.0f );
	glPopMatrix();
    m_Menu->Draw();
}

void SoundExplore::Tick()
{
    m_Menu->Tick();
}

Scene* SoundExplore::NextScene()
{
	if (m_Menu->IsDone())
    {
        delete this;
        return new TitleScreen();
    }
    else
    {
        return this;
    }
}


