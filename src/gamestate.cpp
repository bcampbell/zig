#include "gamestate.h"
#include "player.h"
#include "zig.h"


GameState::GameState() :
    KeepYourSectorTidy(false),
    BigHeadMode(false),
    NoExtraLives(false),
    m_Level(0),
    m_WrapCnt(0),
    m_Demo(false),
    m_Player(0)
{
}
GameState::~GameState()
{
    if( m_Player )
        delete m_Player;
}

int GameState::PerceivedLevel() const
{
    return g_LevelDefs.size()*m_WrapCnt + m_Level + 1;
}

void GameState::StartNewGame()
{
    KeepYourSectorTidy = false;
    BigHeadMode = false;
    NoExtraLives = false;
    m_Level = 0;
    m_WrapCnt = 0;
    m_Demo = false;
    if( m_Player )
        delete m_Player;
    m_Player = new Player();
}

void GameState::StartNewDemo()
{
    KeepYourSectorTidy = false;
    BigHeadMode = false;
    NoExtraLives = false;
    m_Level = 0;
    m_WrapCnt = 0;
    m_Demo = true;
    if( m_Player )
        delete m_Player;
    m_Player = new Player(true);
}

bool GameState::LevelCompleted()
{
    ++m_Level;
    if( m_Level < (int)g_LevelDefs.size() )
    {
        return false;
    }
    else
    {
        // wrapped...
        m_Level = 0;
        ++m_WrapCnt;
        switch( m_WrapCnt )
        {
        case 0:
            g_GameState->BigHeadMode = false;
            g_GameState->NoExtraLives = false;
            break;
        case 1:
            g_GameState->BigHeadMode = true;
            g_GameState->NoExtraLives = false;
            break;
        case 2:
            g_GameState->BigHeadMode = false;
            g_GameState->NoExtraLives = true;
            break;
        }
        return true;    // wrapped
    }
}
