


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
class Controller;



#define ZIGVERSION "1.1"


extern ZigConfig g_Config;

extern Player* g_Player;
extern Level* g_CurrentLevel;

extern bool g_KeepYourSectorTidy;

extern bool g_BigHeadMode;
extern bool g_NoExtraLives;

extern AgentManager* g_Agents;
extern Display* g_Display;
extern Controller* g_Controller;
extern Controller* g_MenuController;

extern Texture* g_Font;
extern Texture* g_InvaderTexture;
extern Texture* g_BlueGlow;

#define VW (640)
#define VH (480)
#define TARGET_FPS 50
#define TICK_INTERVAL (1000/TARGET_FPS)

#define ARENA_RADIUS_MIN 150

std::string ZigUserDir();


#endif // ZIG_H

