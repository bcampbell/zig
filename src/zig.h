


#ifndef ZIG_H
#define ZIG_H

#include <SDL.h>
#include <vector>
#include <string>

#include "leveldef.h"
#include "zigconfig.h"

class AgentManager;
class Display;
class Texture;
class Level;
class Player;
struct GameState;
class ControllerMgr;
class HighScores;
class PathResolver;


#define ZIGVERSION "1.1"



extern PathResolver* g_ConfigPath;      // for config files
extern PathResolver* g_DataPath;        // for generated data (highscores)
extern PathResolver* g_ResourcePath;    // for read-only data (textures etc)

extern GameState* g_GameState;

extern ZigConfig g_Config;
extern std::vector<LevelDef> g_LevelDefs;

extern Player* g_Player;
extern float g_Time;
extern Level* g_CurrentLevel;


extern AgentManager* g_Agents;
extern Display* g_Display;
extern ControllerMgr* g_ControllerMgr;
extern HighScores* g_HighScores;

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



#endif // ZIG_H

