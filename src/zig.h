


#ifndef ZIG_H
#define ZIG_H

#include <SDL.h>
#include <vector>
#include <string>

#include "zigconfig.h"

class AgentManager;
class Display;
class Texture;
class Level;
class Player;
class ControllerMgr;



#define ZIGVERSION "1.1"


// stuff that needs to persist over mulitple levels
struct GameState {
    GameState() : KeepYourSectorTidy(false), BigHeadMode(false), NoExtraLives(false) {};
    ~GameState() {};

    // set if all blastable dudes blasted at level end
    bool KeepYourSectorTidy;

    // double-size player
    bool BigHeadMode;

    // no more extra-life bonuses
    bool NoExtraLives;
};


extern GameState* g_GameState;

extern ZigConfig g_Config;

extern Player* g_Player;
extern Level* g_CurrentLevel;


extern AgentManager* g_Agents;
extern Display* g_Display;
extern ControllerMgr* g_ControllerMgr;

extern Texture* g_Font;

enum { TX_FONT=0,
    TX_INVADER,
    TX_BLUEGLOW,
    TX_NARROWBEAMGRADIENT,
    TX_WIDEBEAMGRADIENT,
    TX_NUMTEXTURES
};

extern Texture* g_Textures[TX_NUMTEXTURES];


#define VW (640)
#define VH (480)
#define TARGET_FPS 50
#define TICK_INTERVAL (1000/TARGET_FPS)

#define ARENA_RADIUS_MIN 150

std::string ZigUserDir();


#endif // ZIG_H

