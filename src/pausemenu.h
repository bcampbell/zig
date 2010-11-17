#ifndef PAUSEMENU_H
#define PAUSEMENU_H

#include "menusupport.h"

class PauseMenu : public Menu
{
public:
	PauseMenu();
	virtual ~PauseMenu();
	virtual void Draw();

	enum Result
	{
		ResumeGame,
		AbortGame,
	};

	bool IsDone() const;
	Result GetResult() const;
private:
	virtual void OnSelect( int id );
	bool m_Done;
	Result m_Result;
};




inline bool PauseMenu::IsDone() const
	{ return m_Done; }

inline PauseMenu::Result PauseMenu::GetResult() const
{
	assert( IsDone() );
	return m_Result;
}


#endif // PAUSEMENU_H

