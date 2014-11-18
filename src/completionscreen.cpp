#include "completionscreen.h"
#include "drawing.h"
//#include "texture.h"
//#include "colours.h"
#include "zig.h"

#include <SDL_opengl.h>
#include <assert.h>
//#include <string>

CompletionScreen::CompletionScreen( int wrapnum ) :
	m_Done(false),
	m_WrapNum(wrapnum),
	m_Elapsed(0.0f)
{
	assert( wrapnum > 0 );	// must have wrapped at least once!
}

CompletionScreen::~CompletionScreen()
{
}

void CompletionScreen::Render()
{
	glClear( GL_COLOR_BUFFER_BIT );
	glLoadIdentity();

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glPushMatrix();
		glTranslatef( 0.0f, 150.0f, 0.0f );
		PlonkText( *g_Font, "COMPLETED!", true, 32, 32 );

	glTranslatef( 0.0f, -100.0f, 0.0f );
	switch( m_WrapNum )
	{
	case 0:	// should never happen
		break;
	case 1:
		{
			PlonkText( *g_Font, "NOW TRY IT AGAIN...", true, 16, 16 );
			glTranslatef( 0.0f, -50.0f, 0.0f );
			PlonkText( *g_Font, "...IN HEADBIG MODE!", true, 16, 16 );
		}
		break;
	case 2:
		PlonkText( *g_Font, "OH? YOU WANT MORE?", true, 16, 16 );
		glTranslatef( 0.0f, -50.0f, 0.0f );
		PlonkText( *g_Font, "OK... BUT NO MORE EXTRA LIVES!", true, 16, 16 );
		break;
	default:
		{
			PlonkText( *g_Font, "SHEESH! ARE YOU *STILL* HERE?", true, 16, 16 );
			glTranslatef( 0.0f, -50.0f, 0.0f );
			PlonkText( *g_Font, "KEEP GOING! KEEP GOING!", true, 16, 16 );
		}
		break;
	}		

	glPopMatrix();
	
}


void CompletionScreen::Tick()
{
	m_Elapsed += ((float)TICK_INTERVAL)/1000.0f;
}
	

bool CompletionScreen::IsFinished()
{
	return m_Done;
}


void CompletionScreen::HandleKeyDown( SDL_Keysym& keysym )
{
	if( m_Elapsed > 2.0f )
		m_Done = true;
}


