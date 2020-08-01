#include <algorithm>
#include <assert.h>
#include <SDL_keyboard.h>
#include <SDL.h>

#include "controller.h"
#include "log.h"
#include "mathutil.h"
#include "paths.h"
#include "wobbly.h"
#include "zig.h"

Controller::Controller() :
    m_BtnState(0),
    m_PrevBtnState(0),
    m_X(0.0f),
    m_Y(0.0f)
{
}



// Controller interface for SDL gamecontroller
class SDLController : public Controller
{
public:
    SDLController(int j_idx);
    ~SDLController();
    void Tick();
    SDL_JoystickID InstanceID();
private:
	SDL_GameController*	m_Ctrl;
	enum { DEADZONE=8000 };

    float ApplyDeadZone(Sint16 v)
    {
        if (v>-DEADZONE && v<DEADZONE) {
            return 0.0f;
        } else {
            return ((float)v) / 32768.0f;
        }
    }
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
    //const char *name = SDL_GameControllerNameForIndex(j_idx);
}

SDLController::~SDLController()
{
    if (m_Ctrl)
    {
        SDL_GameControllerClose(m_Ctrl);
    }
}


// return the average of the non-zero values
static inline float AvgNonZero(float (&v)[3])
{
    float out = 0.0f;
    int i,cnt;
    cnt=0;
    for(i=0;i<3;++i)
    {
        if (v[i]!=0.0f)
        {
            out += v[i];
            ++cnt;
        }
    }
    if (cnt > 0)
        out /= (float)cnt;

    return out;
}

void SDLController::Tick()
{
    float x[3];
    float y[3];

    // we'll accept input from left stick, right stick or dpad...

    // X axis
    x[0] = ApplyDeadZone(SDL_GameControllerGetAxis(m_Ctrl, SDL_CONTROLLER_AXIS_LEFTX));
    x[1] = ApplyDeadZone(SDL_GameControllerGetAxis(m_Ctrl, SDL_CONTROLLER_AXIS_RIGHTX));
    if (SDL_GameControllerGetButton(m_Ctrl, SDL_CONTROLLER_BUTTON_DPAD_LEFT))
        x[2] = -1.0f;
    else if (SDL_GameControllerGetButton(m_Ctrl, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
        x[2] = 1.0f;
    else
        x[2] = 0.0f;

    m_X = AvgNonZero(x);

    // Y axis
    y[0] = ApplyDeadZone(SDL_GameControllerGetAxis(m_Ctrl, SDL_CONTROLLER_AXIS_LEFTY));
    y[1] = ApplyDeadZone(SDL_GameControllerGetAxis(m_Ctrl, SDL_CONTROLLER_AXIS_RIGHTY));
    if (SDL_GameControllerGetButton(m_Ctrl, SDL_CONTROLLER_BUTTON_DPAD_UP))
        y[2] = -1.0f;
    else if (SDL_GameControllerGetButton(m_Ctrl, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
        y[2] = 1.0f;
    else
        y[2] = 0.0f;
    m_Y = AvgNonZero(y);



    // buttons
    int buttons =  0;
    if (SDL_GameControllerGetButton(m_Ctrl,SDL_CONTROLLER_BUTTON_A)==1)
        buttons |= CTRL_BTN_FIRE;
    if (SDL_GameControllerGetButton(m_Ctrl,SDL_CONTROLLER_BUTTON_START)==1)
        buttons |= CTRL_BTN_START;

    m_PrevBtnState = m_BtnState;
    m_BtnState = buttons;
}



KeyboardController::KeyboardController()
{
}


KeyboardController::~KeyboardController()
{
}


void KeyboardController::Tick()
{
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    m_X = 0.0f;    
    if( keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A] )
        m_X -= 1.0f;
    if( keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D] )
        m_X += 1.0f;
    
    m_Y = 0.0f;
    if( keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W] )
        m_Y -= 1.0f;
    if( keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S] )
        m_Y += 1.0f;

    int buttons=0;

    // note: on osx, CTRL+arrows changes virtual desktop, so don't use CTRL for fire
    if (keys[SDL_SCANCODE_RSHIFT] ||
        keys[SDL_SCANCODE_LSHIFT] ||
#if !(defined( __APPLE__ ) && defined( __MACH__ ))
        keys[SDL_SCANCODE_RCTRL] ||
        keys[SDL_SCANCODE_LCTRL] ||
#endif
        keys[SDL_SCANCODE_SPACE] ||
        keys[SDL_SCANCODE_KP_ENTER] ||
        keys[SDL_SCANCODE_RETURN])
    {
        buttons |= CTRL_BTN_FIRE;
    }
    if (keys[SDL_SCANCODE_ESCAPE])
        buttons |= CTRL_BTN_ESC;

    m_PrevBtnState = m_BtnState;
    m_BtnState = buttons;
}



// could probably improve on this ai :-)
Autopilot::Autopilot()
{
}

void Autopilot::Tick()
{
    if( Rnd(0.0f,1.0f) < 0.01 )
        m_X = Rnd( -1.0f, 1.0f );
    if( Rnd(0.0f,1.0f) < 0.01 )
        m_Y = Rnd( -1.0f, 0.0f );   // don't go backwards
    m_PrevBtnState = m_BtnState;
    m_BtnState = CTRL_BTN_FIRE;
}


LatchedController::LatchedController( Controller& source ) :
    m_Source(source),
    m_XCnt(0),
    m_YCnt(0),
    m_PrevX(0.0f),
    m_PrevY(0.0f)
{
}

LatchedController::~LatchedController()
{
}

float LatchedController::quant(float f)
{
    const float threshold = 0.001f;
    if (f>threshold)
        return 1.0f;
    if (f<-threshold)
        return -1.0f;
    return 0.0f;
}

void LatchedController::Tick()
{
    float x = m_Source.XAxis();
    float y = m_Source.YAxis();

    x= quant(x);
    y= quant(y);
    m_X = 0.0f;
    m_Y = 0.0f;
    if (x!=m_PrevX || m_XCnt>=AUTOREPEAT)
    {
        m_XCnt = 0;
        m_X = x;
    } else {
        m_X = 0.0f;
    }

    if (y!=m_PrevY || m_YCnt>=AUTOREPEAT)
    {
        m_YCnt = 0;
        m_Y = y;
    } else {
        m_Y = 0.0f;
    }
    ++m_XCnt;
    ++m_YCnt;


    m_PrevX = x;
    m_PrevY = y;

    m_PrevBtnState = m_BtnState;
    m_BtnState = m_Source.Buttons();
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

void AggregateController::Tick()
{
    int buttons = 0;
    std::list<Controller*>::iterator it;
    for (it=m_Sources.begin(); it!=m_Sources.end(); ++it)
        buttons |= (*it)->Buttons();
    m_BtnState = buttons;
    m_PrevBtnState = m_BtnState;

    const float threshold = 0.001f;
    float xtot=0.0f;
    float ytot=0.0f;
    int xcnt=0, ycnt=0;
    for (it=m_Sources.begin(); it!=m_Sources.end(); ++it)
    {
        float x = (*it)->XAxis();
        if (x<-threshold || x>threshold) {
            xtot += x;
            xcnt++;
        }
        float y = (*it)->YAxis();
        if (y<-threshold || y>threshold) {
            ytot += y;
            ycnt++;
        }
    }
    if (xcnt>0)
        m_X = xtot/xcnt;
    else
        m_X = 0.0f;
    if (ycnt>0)
        m_Y = ytot/ycnt;
    else
        m_Y = 0.0f;


    //printf("%f %f\n", m_X, m_Y);
}


ControllerMgr::ControllerMgr() :
    m_KBCtrl(),
    m_GameCtrl(),
    m_MenuCtrl(m_GameCtrl)
{
    m_GameCtrl.Add(&m_KBCtrl);

    std::string mappings = g_DataPath->ResolveForRead("gamecontrollerdb.txt");
    if(SDL_GameControllerAddMappingsFromFile(mappings.c_str())<0 ) {
        log_errorf("Couldn't load gamecontrollerdb.txt. Some controllers might not work.\n");
    }

    // scan for already-attached controllers
    int i;
    for(i=0; i<SDL_NumJoysticks(); ++i)
    {
        if (!SDL_IsGameController(i)) {
//            printf("joy %d is not controller\n",i);
            continue;
        }
        SDLController* c = new SDLController(i);
        m_Attached.push_back(c);
        m_GameCtrl.Add(c);
//        printf("Found controller: %d\n",c->InstanceID());
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


void ControllerMgr::Tick()
{
    std::list<SDLController*>::iterator it;
    for(it=m_Attached.begin(); it!=m_Attached.end(); ++it)
    {
        (*it)->Tick();
    }
    m_KBCtrl.Tick();
    m_GameCtrl.Tick();
    m_MenuCtrl.Tick();
}

void ControllerMgr::HandleControllerAdded(SDL_ControllerDeviceEvent* ev)
{
    // TODO: cope with multiple adds 
    int idx = ev->which;
    SDLController* c = new SDLController(idx);
    m_Attached.push_back(c);
    m_GameCtrl.Add(c);
//    printf("Attached: %d\n",c->InstanceID());
}

void ControllerMgr::HandleControllerRemoved(SDL_ControllerDeviceEvent* ev)
{
    SDLController* ctrl = FindAttached(ev->which);
    if (ctrl) {
//        printf("Removed: %d\n",ctrl->InstanceID());
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


