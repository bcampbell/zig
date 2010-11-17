#ifndef CRIPPLECLOCK_H
#define CRIPPLECLOCK_H

#ifdef CRIPPLED

#include "scene.h"
#include "menusupport.h"



#define DEFAULT_REMAINING 30*60	// 30min

class CrippleClock
{
public:
	static void Init();

	static void Render();
	static void ReduceRemainingTime( float dt );
	static void WriteRemainingTime();
	static bool Expired();

	static void LaunchWebSite();
private:
	static float s_Remaining;
	static int s_Secs;
	static float s_Punch;
};



class NagMenu : public Menu
{
public:
	NagMenu();
	enum ResultID { ID_BUY, ID_NOBUY };
	ResultID GetResult() const;
	bool IsDone() const
		{ return m_Done; }
private:
	virtual void OnSelect( int id );
	ResultID m_Result;
	bool m_Done;
};



class NagScreen : public Scene
{
public:
	NagScreen();
	bool LaunchWeb();
protected:
	virtual void Render();
	virtual void Tick();
	virtual bool IsFinished();
public:
	float m_Timer;
	NagMenu m_Menu;
};

#endif // CRIPPLED
#endif // CRIPPLECLOCK_H

