#pragma once
#include "BaseSprite.h"
//StartupInfo‚ÅŽg‚¤ƒtƒ‰ƒOŒQ
#define FG_YELLOW	0 * 14 + SW_SHOW
#define FG_GREEN	1 * 14 + SW_SHOW
#define FG_BLUE		2 * 14 + SW_SHOW
#define FG_RED		3 * 14 + SW_SHOW
enum GAMESTATE
{
	SYSTEM_INIT = 0,
	BLUE_INIT,
	RED_INIT,
	GREEN_INIT,
	YELLOW_INIT,
	BLUE,
	RED,
	GREEN,
	YELLOW,
	APP_EXIT,

};

typedef struct
{
	//BaseSprite Player;

	//PBaseSprite BackGround;

	/*PInput m_inp;
	PConsoleManager m_cm;
	PGameTimer m_gt;*/



}Scene, *PScene;



//GAMESTATE Title(GAMESTATE gstate);
//GAMESTATE Start(PScene &p_this);
//GAMESTATE Update(PScene const p_this);
//void Stop(PScene &p_this);