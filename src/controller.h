#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <list>
//#include <SDL_joystick.h>
//#include <SDL_gamecontroller.h>
#include <SDL_events.h>



enum {
    CTRL_BTN_FIRE = 1,
    CTRL_BTN_ESC=2,
    CTRL_BTN_START=4,
};

class Controller
{
public:
	Controller();
	virtual ~Controller()		{}
    virtual void Tick()         {}
	float XAxis() { return m_X; }		// left: -ve right +ve
	float YAxis() { return m_Y; }		// up: -ve down: +ve
    int Buttons()   { return m_BtnState; } // CTRL_BTN_x  bitflags
    int Pressed()   { return ~m_PrevBtnState & m_BtnState; }
    int Released()  { return m_PrevBtnState & ~m_BtnState; }
protected:
    int m_BtnState;
    int m_PrevBtnState;
    float m_X;
    float m_Y;
};



class SDLController;

class KeyboardController : public Controller
{
public:
	KeyboardController();
	~KeyboardController();
    void Tick();
private:
};




// autopilot!
class Autopilot : public Controller
{
public:
	Autopilot();
    void Tick();
private:
};


// A meta-controller which sits on top of another controller.
// Only returns values when they have changed since the last check.
// Used for menus.
class LatchedController : public Controller
{
public:
	LatchedController( Controller& source );
	~LatchedController();
    void Tick();
private:
	Controller& m_Source;

    float quant(float f);
	enum { AUTOREPEAT=10 };
    int m_XCnt;
    int m_YCnt;
    float m_PrevX;
    float m_PrevY;
};


// merges multiple controllers into one
class AggregateController : public Controller
{
public:
	AggregateController();
    virtual	~AggregateController();
    void Tick();
    void Add(Controller* src)
        { m_Sources.push_back(src); }
    void Remove(Controller* src);
private:
    std::list<Controller*> m_Sources;
};


// Manager class for wrangling controllers
// handles attach/detach, and presents virtualised controlllers
// for gameplay and menu navigation
class ControllerMgr
{
public:
    ControllerMgr();
    ~ControllerMgr();

    void Tick();
    // HandleJoyDevice/ControllerDevice events

    Controller& MenuController() { return m_MenuCtrl; }
    Controller& GameController() { return m_GameCtrl; }


    void HandleControllerAdded(SDL_ControllerDeviceEvent* ev);
    void HandleControllerRemoved(SDL_ControllerDeviceEvent* ev);
private:
    SDLController* FindAttached(SDL_JoystickID instanceID);
    std::list<SDLController*> m_Attached;
    KeyboardController m_KBCtrl;
    AggregateController m_GameCtrl;
    LatchedController m_MenuCtrl;
};





#endif	// CONTROLLER_H


