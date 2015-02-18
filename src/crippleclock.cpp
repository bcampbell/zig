
#ifdef CRIPPLED

#include "crippleclock.h"
#include "drawing.h"
#include "texture.h"
#include "soundmgr.h"
#include "util.h"

#include "zig.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <assert.h>
#include <string>


#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winreg.h>
#include <shellapi.h>

#define SUBKEY "Software\\zig"
#define VALUENAME ZIGVERSION


static int GetTimeFromRegistry()
{
	LONG r;
	HKEY key;
	r = RegOpenKeyEx( HKEY_CURRENT_USER, SUBKEY, 0, KEY_READ, &key );
	if( r != ERROR_SUCCESS )
		return DEFAULT_REMAINING;

	DWORD type;
	DWORD data;
	DWORD datasize=4;
	r = RegQueryValueEx( key, VALUENAME, 0, &type, (BYTE*)&data, &datasize );
	if( r != ERROR_SUCCESS )
		return DEFAULT_REMAINING;
	if( type != REG_DWORD )
		return DEFAULT_REMAINING;

	if( (int)data > DEFAULT_REMAINING )
		return DEFAULT_REMAINING;

	return (int)data;
}

static void SaveTimeToRegistry( int t )
{
	HKEY key;
	DWORD disposition;
	LONG r;
	r = RegCreateKeyEx( HKEY_CURRENT_USER, SUBKEY, 0, 0,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &key, &disposition );

	if( r != ERROR_SUCCESS )
		return;

	r = RegSetValueEx(key,VALUENAME,0,REG_DWORD,(const BYTE*)&t,4);
	if( r != ERROR_SUCCESS )
		return;
	return;
}


void CrippleClock::Init()
{
	s_Remaining = (float)GetTimeFromRegistry();
}

void CrippleClock::WriteRemainingTime()
{
	SaveTimeToRegistry( (int)s_Remaining );
}

#else
// mac, linux

#define CRIPPLEFILE ".crip"

void CrippleClock::Init()
{
	s_Remaining = DEFAULT_REMAINING;

	std::string filename = JoinPath( ZigUserDir(), CRIPPLEFILE );
	FILE* fp = fopen( filename.c_str(), "rb" );
	if( fp )
	{
		float foo;
		if( fread( &foo, sizeof( foo ), 1, fp ) == 1 )
		{
			s_Remaining = foo;

		}
			
		fclose( fp );
	}
	//printf("read '%s': %f\n", filename.c_str(), s_Remaining );
}

void CrippleClock::WriteRemainingTime()
{
	std::string filename = JoinPath( ZigUserDir(), CRIPPLEFILE );
	FILE* fp = fopen( filename.c_str(), "wb" );
	if( fp )
	{
		fwrite( &s_Remaining, sizeof(s_Remaining), 1,fp);
		fclose( fp );
//		printf("wrote '%s': %f\n", filename.c_str(), s_Remaining );
	}
}





#endif







float CrippleClock::s_Remaining = DEFAULT_REMAINING;
int CrippleClock::s_Secs = (int)DEFAULT_REMAINING;
float CrippleClock::s_Punch = 0.0f;


bool CrippleClock::Expired()
{
	if( s_Remaining<=0.0f )
		return true;
	else
		return false;
}



void CrippleClock::Render()
{
	int newsecs = (int)s_Remaining;
	if( newsecs != s_Secs )
	{
		s_Punch = 0.1f;
		s_Secs = newsecs;
	}

	char clktxt[64];
	sprintf( clktxt, "%d:%02d", s_Secs/60, s_Secs%60 );

	float f = 1.0f + s_Punch;

	glColor4f( 1.0f, 1.0f, 1.0f, 0.5f );
	glPushMatrix();
		glTranslatef( -40.0f, -220.0f, 0.0f );
		PlonkText( *g_Font, "DEMO TIME REMAINING", true, 12,12 );

		glTranslatef( 160.0f, 0.0f, 0.0f );
		PlonkText( *g_Font, clktxt, true, 12*f, 12*f );
	glPopMatrix();

	s_Punch *= 0.9f;
}



void CrippleClock::ReduceRemainingTime( float dt )
{
	s_Remaining -= dt;
	if( s_Remaining < 0.0f )
		s_Remaining = 0.0f;
}


void CrippleClock::LaunchWebSite()
{
	const char* url = "http://www.itsagamewhereyoushootstuff.com/buybuybuy";
#ifdef _WIN32
	::ShellExecute( NULL, "open", url, NULL, "C:\\", SW_SHOWNORMAL );
#endif
	//mac?
	//system("open <url>");
}



//------------------------------------------

NagMenu::NagMenu() :
	m_Done(false)
{
	AddItem( new MenuItem( ID_BUY, vec2(0.0f,-60.0f), "YES! CONSUMER FRENZY!" ) );
	AddItem( new MenuItem( ID_NOBUY, vec2(0.0f,-100.0f), "UH... MAYBE LATER..." ) );
}

NagMenu::ResultID NagMenu::GetResult() const
{
	assert( m_Done );
	return m_Result;
}

void NagMenu::OnSelect( int id )
{
	m_Done=true;
	m_Result=(ResultID)id;
}



//------------------------------------------

NagScreen::NagScreen() :
	m_Timer( 0.0f )
{
	SoundMgr::Inst().Play( SFX_GAMEOVER );
}

void NagScreen::Render()
{
	glClear( GL_COLOR_BUFFER_BIT );
	glLoadIdentity();

	glPushMatrix();
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		glTranslatef( 0.0f, 140.0f, 0.0f );
		if( CrippleClock::Expired() )
		{
			PlonkText( *g_Font, "-- DEMO EXPIRED --", true );
		}
		glTranslatef( 0.0f, -40.0f, 0.0f );
		PlonkText( *g_Font, "WHY NOT BUY THE FULL VERSION?", true );
	glPopMatrix();

	m_Menu.Draw();
}

void NagScreen::Tick()
{
//	const float timeout = 10.0f;
//	m_Timer += 1.0f/TARGET_FPS;
//	if( m_Timer > timeout )
//		m_Done = true;

	m_Menu.Tick();
}

bool NagScreen::IsFinished()
{
	return m_Menu.IsDone();
}


bool NagScreen::LaunchWeb()
{
	return m_Menu.GetResult() == NagMenu::ID_BUY;
}

#endif // CRIPPLED

