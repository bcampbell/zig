#include <algorithm>
#include <assert.h>
#include <SDL_keyboard.h>
#include <SDL.h>

#include "controller.h"
#include "mathutil.h"
#include "wobbly.h"






// Controller interface for SDL gamecontroller
class SDLController : public Controller
{
public:
	SDLController(int j_idx);
	~SDLController();
	virtual float	XAxis();
	virtual float	YAxis();
	virtual int 	Buttons();

    SDL_JoystickID InstanceID();
private:
	SDL_GameController*	m_Ctrl;
	enum { DEADZONE=8000 };
};

SDL_JoystickID SDLController::InstanceID()
{
    SDL_Joystick* joy = SDL_GameControllerGetJoystick(m_Ctrl);
    return SDL_JoystickInstanceID(joy);
}

SDLController::SDLController(int j_idx) : m_Ctrl(0)
{
    m_Ctrl = SDL_GameControllerOpen(j_idx);
    if (!m_Ctrl)
    {
        throw Wobbly("Couldn't open controller: %s", SDL_GetError());
    }
    const char *name = SDL_GameControllerNameForIndex(j_idx);
}

SDLController::~SDLController()
{
    if (m_Ctrl)
    {
        SDL_GameControllerClose(m_Ctrl);
    }
}

float SDLController::XAxis()
{
    Sint16 v = SDL_GameControllerGetAxis(m_Ctrl, SDL_CONTROLLER_AXIS_LEFTX);
    if (v>-DEADZONE && v<DEADZONE) {
        return 0.0f;
    } else {
        return ((float)v) / 32768.0f;
    }
}

float SDLController::YAxis()
{
    Sint16 v = SDL_GameControllerGetAxis(m_Ctrl, SDL_CONTROLLER_AXIS_LEFTY);
    if (v>-DEADZONE && v<DEADZONE) {
        return 0.0f;
    } else {
        return ((float)v) / 32768.0f;
    }
}

int SDLController::Buttons()
{
    int buttons =  0;
    if (SDL_GameControllerGetButton(m_Ctrl,SDL_CONTROLLER_BUTTON_A)==1)
        buttons |= CTRL_BTN_FIRE;
    if (SDL_GameControllerGetButton(m_Ctrl,SDL_CONTROLLER_BUTTON_START)==1)
        buttons |= CTRL_BTN_START;
    return buttons;
}



KeyboardController::KeyboardController()
{
}


KeyboardController::~KeyboardController()
{
}


float KeyboardController::XAxis()
{
	int numkeys;
    const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	if( keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A] )
		return -1.0f;
	if( keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D] )
		return 1.0f;
	return 0.0f;
}

float KeyboardController::YAxis()
{
	int numkeys;
    const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	if( keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W] )
		return -1.0f;
	if( keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S] )
		return 1.0f;
	return 0.0f;
}



int KeyboardController::Buttons()
{
    int buttons=0;
	int numkeys;
    const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_RCTRL] ||
		keys[SDL_SCANCODE_LCTRL] ||
		keys[SDL_SCANCODE_RSHIFT] ||
		keys[SDL_SCANCODE_LSHIFT] ||
		keys[SDL_SCANCODE_SPACE] ||
		keys[SDL_SCANCODE_RETURN])
	{
		buttons |= CTRL_BTN_FIRE;
	}
	if (keys[SDL_SCANCODE_ESCAPE])
        buttons |= CTRL_BTN_ESC;

	return buttons;
}



// could probably improve on this ai :-)
Autopilot::Autopilot() : Controller(), m_X(0.0f), m_Y(0.0f)
{
}

float Autopilot::XAxis()
{
	if( Rnd(0.0f,1.0f) < 0.01 )
		m_X = Rnd( -1.0f, 1.0f );
	return m_X;
}

float Autopilot::YAxis()
{
	if( Rnd(0.0f,1.0f) < 0.01 )
		m_Y = Rnd( -1.0f, 0.0f );	// don't go backwards
	return m_Y;
}

int Autopilot::Buttons()
{
	return CTRL_BTN_FIRE;
}


LatchedController::LatchedController( Controller& source ) : m_Source(source)
{
	float x = m_Source.XAxis();
	float y = m_Source.YAxis();
	
	m_Buttons = m_Source.Buttons();

	m_UpCount = y > 0.0f ? AUTOREPEAT:0;
	m_DownCount = y < 0.0f ? AUTOREPEAT:0;
	m_LeftCount = x < 0.0f ? AUTOREPEAT:0;
	m_RightCount = x > 0.0f ? AUTOREPEAT:0;
}

LatchedController::~LatchedController()
{
}

int LatchedController::Buttons()
{
	int prev = m_Buttons;
	m_Buttons = m_Source.Buttons();
	return( m_Buttons && !prev );
}

float LatchedController::XAxis()
{
	float x = m_Source.XAxis();
	if( x < 0.0f )
	{
		if( m_LeftCount <= 0 )
		{
			m_LeftCount = AUTOREPEAT;
			return x;
		}
		--m_LeftCount;
	}
	else
		m_LeftCount = 0;

	if( x > 0.0f )
	{
		if( m_RightCount <= 0 )
		{
			m_RightCount = AUTOREPEAT;
			return x;
		}
		--m_RightCount;
	}
	else
		m_RightCount = 0;

	return 0.0f;
}


float LatchedController::YAxis()
{
	float y = m_Source.YAxis();
	if( y > 0.0f )
	{
		if( m_UpCount <= 0 )
		{
			m_UpCount = AUTOREPEAT;
			return y;
		}
		--m_UpCount;
	}
	else
		m_UpCount = 0;

	if( y < 0.0f )
	{
		if( m_DownCount <= 0 )
		{
			m_DownCount = AUTOREPEAT;
			return y;
		}
		--m_DownCount;
	}
	else
		m_DownCount = 0;

	return 0.0f;
}


AggregateController::AggregateController()
{
}

AggregateController::~AggregateController()
{
}

void AggregateController::Remove( Controller* src)
{
    m_Sources.remove(src);
}

float AggregateController::XAxis()
{
    const float threshold = 0.001f;
    std::list<Controller*>::iterator it;
    for (it=m_Sources.begin(); it!=m_Sources.end(); ++it)
    {
        float v=(*it)->XAxis();
        if (v<-threshold || v>threshold)
            return v;
    }
    return 0.0f;
}

float AggregateController::YAxis()
{
    const float threshold = 0.001f;
    std::list<Controller*>::iterator it;
    for (it=m_Sources.begin(); it!=m_Sources.end(); ++it)
    {
        float v=(*it)->YAxis();
        if (v<-threshold || v>threshold)
            return v;
    }
    return 0.0f;
}

int AggregateController::Buttons()
{
    int buttons = 0;
    std::list<Controller*>::iterator it;
    for (it=m_Sources.begin(); it!=m_Sources.end(); ++it)
        buttons |= (*it)->Buttons();
    return buttons;
}


ControllerMgr::ControllerMgr() :
    m_KBCtrl(),
    m_GameCtrl(),
    m_MenuCtrl(m_GameCtrl)
{
    m_GameCtrl.Add(&m_KBCtrl);

    if(SDL_GameControllerAddMappingsFromFile("/home/ben/proj/zig/gamecontrollerdb.txt")<0 ) {
        printf("Add mapping failed: %s\n",SDL_GetError());
    }

    // scan for already-attached controllers
    int i;
    for(i=0; i<SDL_NumJoysticks(); ++i)
    {
        if (!SDL_IsGameController(i)) {
            printf("joy %d is not controller\n",i);
            continue;
        }
        SDLController* c = new SDLController(i);
        m_Attached.push_back(c);
        m_GameCtrl.Add(c);
        printf("Found controller: %d\n",c->InstanceID());
        break;
    }
}

ControllerMgr::~ControllerMgr()
{
    while(!m_Attached.empty())
    {
        delete m_Attached.back();
        m_Attached.pop_back();
    }
}

void ControllerMgr::HandleControllerAdded(SDL_ControllerDeviceEvent* ev)
{
    // TODO: cope with multiple adds 
    int idx = ev->which;
    SDLController* c = new SDLController(idx);
    m_Attached.push_back(c);
    m_GameCtrl.Add(c);
    printf("Attached: %d\n",c->InstanceID());
}

void ControllerMgr::HandleControllerRemoved(SDL_ControllerDeviceEvent* ev)
{
    SDLController* ctrl = FindAttached(ev->which);
    if (ctrl) {
        printf("Removed: %d\n",ctrl->InstanceID());
        m_GameCtrl.Remove(ctrl);
        m_Attached.remove(ctrl);
        delete ctrl;
    }
}

SDLController* ControllerMgr::FindAttached(SDL_JoystickID instanceID)
{
    std::list<SDLController*>::iterator it;
    for(it=m_Attached.begin(); it!=m_Attached.end(); ++it) {
        SDLController* ctrl = *it;
        if (ctrl->InstanceID() == instanceID)
            return ctrl;
    }
    return 0;
}


