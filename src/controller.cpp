
#include "controller.h"
#include "mathutil.h"

#include <assert.h>
#include <SDL_keyboard.h>


StandardController::StandardController() :
	m_Joystick(0)
{
	if( SDL_NumJoysticks() > 0 )
	{
		m_Joystick = SDL_JoystickOpen(0);

		// need at least two axes and one button :-)
		if( SDL_JoystickNumAxes(m_Joystick) < 2 ||
			SDL_JoystickNumButtons(m_Joystick) < 1 )
		{
			SDL_JoystickClose( m_Joystick );
			m_Joystick = 0;
		}
	}
}


StandardController::~StandardController()
{
	if( m_Joystick )
		SDL_JoystickClose( m_Joystick );
}


float StandardController::XAxis()
{
	int numkeys;
	Uint8* keys;
	keys = SDL_GetKeyState(&numkeys);
	
	if( keys[SDLK_LEFT] || keys[SDLK_a] )
		return -1.0f;
	if( keys[SDLK_RIGHT] || keys[SDLK_d] )
		return 1.0f;

	if( m_Joystick )
	{
		Sint16 rawx;
		rawx = SDL_JoystickGetAxis( m_Joystick, 0 );
		if( rawx < -DEADZONE || rawx > DEADZONE )
			return (float)rawx / 32767.0f;
	}
	return 0.0f;
}

float StandardController::YAxis()
{
	int numkeys;
	Uint8* keys;
	keys = SDL_GetKeyState(&numkeys);
	
	if( keys[SDLK_UP] || keys[SDLK_w] )
		return 1.0f;
	if( keys[SDLK_DOWN] || keys[SDLK_s] )
		return -1.0f;

	if( m_Joystick )
	{
		Sint16 rawy;
		rawy = SDL_JoystickGetAxis( m_Joystick, 1 );
		if( rawy < -DEADZONE || rawy > DEADZONE )
			return (float)rawy / 32767.0f;
	}
	return 0.0f;
}



bool StandardController::Button()
{
	int numkeys;
	Uint8* keys;
	keys = SDL_GetKeyState(&numkeys);

	if( keys[SDLK_RCTRL] ||
		keys[SDLK_LCTRL] ||
		keys[SDLK_RSHIFT] ||
		keys[SDLK_LSHIFT] ||
		keys[SDLK_SPACE] ||
		keys[SDLK_RETURN] )
	{
		return true;
	}

	if( m_Joystick && SDL_JoystickGetButton( m_Joystick, 0 ) )
		return true;

	return false;
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

bool Autopilot::Button()
{
	return true;
}


LatchedController::LatchedController( Controller& source ) : m_Source(source)
{
	float x = m_Source.XAxis();
	float y = m_Source.YAxis();
	
	m_Button = m_Source.Button();

	m_UpCount = y > 0.0f ? AUTOREPEAT:0;
	m_DownCount = y < 0.0f ? AUTOREPEAT:0;
	m_LeftCount = x < 0.0f ? AUTOREPEAT:0;
	m_RightCount = x > 0.0f ? AUTOREPEAT:0;
}

LatchedController::~LatchedController()
{
}

bool LatchedController::Button()
{
	bool prev = m_Button;
	m_Button = m_Source.Button();
	return( m_Button && !prev );
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

