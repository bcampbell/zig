#ifndef MENUCONTROLLER_H
#define MENUCONTROLLER_H

#include <SDL_keyboard.h>
#include <deque>
#include <string>
#include <list>

#include <assert.h>

#include "vec2.h"




class MenuItem
{
public:
	MenuItem( int id, vec2 const& pos, std::string const& text, bool centre=true, SDL_Keycode shortcut=SDLK_UNKNOWN );
	void Tick();
	void Draw();

	void SetFocus( bool focus );

	int GetID() const		{ return m_ID; }
	SDL_Keycode Shortcut() const		{ return m_Shortcut; }
private:
	int				m_ID;
	vec2			m_Pos;
	std::string		m_Text;
    SDL_Keycode     m_Shortcut;

	enum { Normal, Focused } m_State;
	float m_Wibble;
	float m_Cyc;
	float m_Centre;
};



class Menu
{
public:
	virtual void Tick();
	virtual void Draw();
	virtual ~Menu();

	float InactivityTime()	{ return m_InactivityTime; }

    void HandleKeyDown(SDL_Keysym& sym);
protected:
	Menu();
	void AddItem( MenuItem* item );
	virtual void OnSelect( int id )		{}
	virtual void OnLeft( int id )		{}
	virtual void OnRight( int id )		{}
	int Current() const					{ return (*m_Current)->GetID(); }
private:
	typedef std::list<MenuItem*> itemlist;
	itemlist m_Items;
	itemlist::iterator m_Current;

	float m_InactivityTime;
};

#endif // MENUCONTROLLER_H


