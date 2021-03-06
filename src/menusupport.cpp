#include "colours.h"
#include "controller.h"
#include "drawing.h"
#include "mathutil.h"
#include "menusupport.h"
#include "soundmgr.h"
#include "zig.h"
#include <SDL_opengl.h>
#include <SDL_keyboard.h>



static const Colour focuscolours[] =
{
//	Colour( 0.4f, 0.2f, 0.2f),
//	Colour( 0.9f, 0.9f, 0.9f),
	Colour( 1.0f, 1.0f, 0.0f),
	Colour( 0.7f, 0.2f, 0.0f),
};

static const ColourRange focusrange( focuscolours, 2 );



MenuItem::MenuItem( int id, vec2 const& pos, std::string const& text, bool centre, int shortcut ) :
	m_ID(id),
	m_Pos( pos ),
	m_Text( text ),
    m_Shortcut(shortcut),
	m_State( Normal ),
	m_Wibble( 0.0f ),
	m_Cyc( 0.0f ),
	m_Centre( centre )
{
}


void MenuItem::Tick()
{
	m_Wibble *= 0.9f;
    m_Cyc = fmodf(g_Time, 1.0f);
}


void MenuItem::Draw()
{
	glPushMatrix();

	float xsize;
	float ysize;
	if( m_State == Focused )
	{
//		glColor4f( 1.0f, 1.0f, 0.0f, 1.0f );
		Colour c = focusrange.Get( m_Cyc, false );
		glColor4f( c.r, c.g, c.b, c.a );

		xsize = 24.0f + sinf( m_Cyc*twopi)*1.0f;
		ysize = 24.0f + cosf( m_Cyc*twopi)*1.0f;;
	}
	else
	{
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		xsize = 16.0f;
		ysize = 16.0f;
	}

	xsize += m_Wibble*16.0f;
	ysize += m_Wibble*16.0f;

	glTranslatef( m_Pos.x, m_Pos.y, 0.0f );
	PlonkText( *g_Font, m_Text, m_Centre, xsize, ysize );
	glPopMatrix();
}

void MenuItem::SetFocus( bool focus )
{
	if( focus )
	{
		m_State = Focused;
		m_Wibble = 1.0f;
	}
	else
	{
		m_State = Normal;
		m_Wibble = 0.0f;
	}

}


Menu::Menu() : m_InactivityTime(0.0f)
{
	m_Current = m_Items.end();
}


Menu::~Menu()
{
	while( !m_Items.empty() )
	{
		delete m_Items.back();
		m_Items.pop_back();
	}
}

void Menu::AddItem( MenuItem* item )
{
	m_Items.push_back( item );
	if( m_Items.size() == 1 )
	{
		// select first item.
		m_Current = m_Items.begin();
		item->SetFocus( true );
	}
}


void Menu::Tick()
{
	assert( !m_Items.empty() );

    Controller& ctrl = g_ControllerMgr->MenuController();
	float x = ctrl.XAxis();
	float y = ctrl.YAxis();
	int b = ctrl.Pressed();

	itemlist::const_iterator end = m_Items.end();
	itemlist::iterator it;
	for( it=m_Items.begin(); it!=end; ++it )
    {
        
        if (b & (*it)->Shortcut())
        {
            SoundMgr::Inst().Play( SFX_DULLBLAST );
            OnSelect((*it)->GetID());
            return;
        }
        //
		(*it)->Tick();
    }

	if( x == 0.0f && y == 0.0f && !b )
		m_InactivityTime += 1.0f/(float)TARGET_FPS;
	else
		m_InactivityTime = 0.0f;

	if( y < 0.0f )		// up?
	{
		(*m_Current)->SetFocus( false );
		if( m_Current != m_Items.begin() )
			--m_Current;
		(*m_Current)->SetFocus( true );
        OnFocus((*m_Current)->GetID());
	}

	if( y > 0.0f )	// down?
	{
		(*m_Current)->SetFocus( false );
		++m_Current;
		if( m_Current == m_Items.end() )
			--m_Current;
		(*m_Current)->SetFocus( true );

        OnFocus((*m_Current)->GetID());
	}

	if( x<0.0f )		// left?
		OnLeft( (*m_Current)->GetID() );

	if( x>0.0f )		// right?
		OnRight( (*m_Current)->GetID() );

	if( b & CTRL_BTN_FIRE )
		OnSelect( (*m_Current)->GetID() );
}

void Menu::Draw()
{
	itemlist::const_iterator end = m_Items.end();
	itemlist::iterator it;
	for( it=m_Items.begin(); it!=end; ++it )
		(*it)->Draw();
}


void Menu::OnFocus(int id)
{
		SoundMgr::Inst().Play( SFX_DULLBLAST );
}


