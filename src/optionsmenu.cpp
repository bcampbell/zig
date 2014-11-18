#include <SDL_opengl.h>
#include "optionsmenu.h"
#include "colours.h"
#include "display.h"
#include "drawing.h"
#include "soundmgr.h"
#include "zig.h"


OptionsMenu::OptionsMenu() :
	m_Done( false )
{
//	AddItem( new MenuItem( ID_RES, vec2(-100.0f,45.0f), "RESOLUTION" ) );
//	AddItem( new MenuItem( ID_DEPTH, vec2(-100.0f,15.0f), "BIT DEPTH" ) );
	AddItem( new MenuItem( ID_FULLSCREEN, vec2(-100.0f,-15.0f), "FULLSCREEN" ) );
	AddItem( new MenuItem( ID_ACCEPT, vec2(0.0f,-80.0f), "ACCEPT" ) );
	AddItem( new MenuItem( ID_CANCEL, vec2(0.0f,-110.0f), "CANCEL", true, SDLK_ESCAPE ) );

//	int i = g_Display->MatchRes( g_Display->CurrentRes() );
//	if( i==-1 )
//		i=0;
	m_SelectedRes = 0;
	m_BitDepth = 0;
	m_Fullscreen = g_Display->IsFullscreen();
}


void OptionsMenu::LeftArrow( vec2 const& offset )
{
	glPushMatrix();
	glTranslatef( offset.x, offset.y, 0.0f );
	glBegin( GL_TRIANGLES );
		glColor3f( 0.5f, 0.5f, 0.5f );
		glVertex2f( -4.0f, 0.0f );
		glVertex2f( 4.0f, 10.0f );
		glVertex2f( 4.0f, -10.0f );
	glEnd();
	glPopMatrix();
}

void OptionsMenu::RightArrow( vec2 const& offset )
{
	glPushMatrix();
	glTranslatef( offset.x, offset.y, 0.0f );
	glBegin( GL_TRIANGLES );
		glColor3f( 0.5f, 0.5f, 0.5f );
		glVertex2f( 4.0f, 0.0f );
		glVertex2f( -4.0f, 10.0f );
		glVertex2f( -4.0f, -10.0f );
	glEnd();
	glPopMatrix();
}



void OptionsMenu::Draw()
{
#if 0
	std::vector<Res> const& rlist = g_Display->Resolutions();

	// display current res
	{
		glPushMatrix();
		glTranslatef( 8.0f*16.0f, 45.0f, 0.0f );
		Res const& r = rlist[m_SelectedRes];
		char txt[32];
		sprintf( txt, "%dX%d", r.w, r.h );
		Colour c = Colour::WHITE;
		glColor3f( c.r, c.g, c.b );
		PlonkText( *g_Font, txt, true, 16.0f, 16.0f );
		float txtw = 16.0f + strlen(txt)*16.0f;

		if( Current() == ID_RES )
		{
			glDisable( GL_BLEND );
			glDisable( GL_TEXTURE_2D );
			glShadeModel( GL_FLAT );
			if( m_SelectedRes > 0 )
				LeftArrow( vec2( -txtw/2, 0.0f ) );
			if( m_SelectedRes < (int)g_Display->Resolutions().size()-1 )
				RightArrow( vec2( txtw/2, 0.0f ) );
		}
		glPopMatrix();
	}


	// display bitdepth
	{
		glPushMatrix();
		glTranslatef( 8.0f*16.0f, 15.0f, 0.0f );
		Colour c = Colour::WHITE;
		glColor3f( c.r, c.g, c.b );
		const char* txt="16";
		if( m_BitDepth == 32 )
			txt = "32";
		PlonkText( *g_Font, txt, true, 16.0f, 16.0f );
		float txtw = 16.0f + strlen(txt)*16.0f;

		if( Current() == ID_DEPTH )
		{
			LeftArrow( vec2( -txtw/2, 0.0f ) );
			RightArrow( vec2( txtw/2, 0.0f ) );
		}
		glPopMatrix();
	}
#endif
	// display fullscreen
	{
		glPushMatrix();
		glTranslatef( 8.0f*16.0f, -15.0f, 0.0f );
		Colour c = Colour::WHITE;
		glColor3f( c.r, c.g, c.b );
		const char* txt = "NO";
		if( m_Fullscreen )
			txt = "YES";
		float txtw = 16.0f + strlen(txt)*16.0f;
		PlonkText( *g_Font, txt, true, 16.0f, 16.0f );
		if( Current() == ID_FULLSCREEN )
		{
			LeftArrow( vec2( -txtw/2, 0.0f ) );
			RightArrow( vec2( txtw/2, 0.0f ) );
		}

		glPopMatrix();
	}



	Menu::Draw();
}

void OptionsMenu::Tick()
{
	Menu::Tick();
}




void OptionsMenu::OnSelect( int id )
{
	switch( (ItemID)id )
	{
	case ID_ACCEPT:
		{
			m_Done = true;
			g_Display->ChangeSettings(m_Fullscreen);
#if 0
			Res const& r = g_Display->Resolutions()[m_SelectedRes];
			g_Config.fullscreen = m_Fullscreen;
			g_Config.width = r.w;
			g_Config.height = r.h;
			g_Config.depth = m_BitDepth;

			g_Display->ChangeSettings(
				g_Config.fullscreen );

			g_Config.Save();
#endif
		}
		break;
	case ID_CANCEL:
		m_Done = true;
		break;
	case ID_FULLSCREEN:
		m_Fullscreen = m_Fullscreen ? false : true;
		SoundMgr::Inst().Play( SFX_PLAYERFIRE );
		break;
	case ID_DEPTH:
		m_BitDepth = m_BitDepth==32 ? 16 : 32;
		SoundMgr::Inst().Play( SFX_PLAYERFIRE );
		break;
	default:
		break;
	};
}


void OptionsMenu::OnLeft( int id )
{
	switch( (ItemID)id )
	{
	case ID_RES:
		if( m_SelectedRes > 0 )
			--m_SelectedRes;
		SoundMgr::Inst().Play( SFX_PLAYERFIRE );
		break;
	case ID_FULLSCREEN:
		m_Fullscreen = m_Fullscreen ? false : true;
		SoundMgr::Inst().Play( SFX_PLAYERFIRE );
		break;
	case ID_DEPTH:
		m_BitDepth = m_BitDepth==32 ? 16 : 32;
		SoundMgr::Inst().Play( SFX_PLAYERFIRE );
		break;
	default:
		break;
	};
}


void OptionsMenu::OnRight( int id )
{
	switch( (ItemID)id )
	{
	case ID_RES:
#if 0
		if( m_SelectedRes < (int)g_Display->Resolutions().size()-1 )
			++m_SelectedRes;
		SoundMgr::Inst().Play( SFX_PLAYERFIRE );
#endif
		break;
	case ID_FULLSCREEN:
		m_Fullscreen = m_Fullscreen ? false : true;
		SoundMgr::Inst().Play( SFX_PLAYERFIRE );
		break;
	case ID_DEPTH:
		m_BitDepth = m_BitDepth==32 ? 16 : 32;
		SoundMgr::Inst().Play( SFX_PLAYERFIRE );
		break;
	default:
		break;
	};
}

