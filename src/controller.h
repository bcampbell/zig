#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL_joystick.h>

class Controller
{
public:
	Controller() 				{}
	virtual ~Controller()		{}
	virtual float	XAxis() = 0;		// left: -ve right +ve
	virtual float	YAxis() = 0;		// up: +ve down: -ve
	virtual bool	Button() = 0;
};




// standard controller for user input
class StandardController : public Controller
{
public:
	StandardController();
	~StandardController();
	virtual float	XAxis();
	virtual float	YAxis();
	virtual bool	Button();
private:
	SDL_Joystick*	m_Joystick;
	enum { DEADZONE=8000 };
};


// autopilot!
class Autopilot : public Controller
{
public:
	Autopilot();
	virtual float	XAxis();
	virtual float	YAxis();
	virtual bool	Button();
private:
	float m_X;
	float m_Y;
};


// A meta-controller which sits on top of another controller.
// Only returns values when they have changed since the last check.
// Used for menus.
class LatchedController : public Controller
{
public:
	LatchedController( Controller& source );
	~LatchedController();
	virtual float	XAxis();
	virtual float	YAxis();
	virtual bool	Button();
private:
	Controller& m_Source;

	bool m_Button;

	enum { AUTOREPEAT=10 };
	int m_LeftCount;
	int m_RightCount;
	int m_UpCount;
	int m_DownCount;
};


#endif	// CONTROLLER_H


