#pragma once
#include "../YSEngine/YSConsoleGameEngine.h"
//StartupInfo‚ÅŽg‚¤ƒtƒ‰ƒOŒQ
#define FG_YELLOW	0 * 14 + SW_SHOW
#define FG_GREEN	1 * 14 + SW_SHOW
#define FG_BLUE		2 * 14 + SW_SHOW
#define FG_RED		3 * 14 + SW_SHOW

enum GAMESTATE
{
	//SYSTEM_INIT = 0,
	YELLOW_SOUL_MES_INIT = 0,
	GREEN_SOUL_MES_INIT,
	BLUE_SOUL_MES_INIT,
	RED_SOUL_MES_INIT,
	WINDOW_MES_INIT,
	YELLOW_SOUL_MES,
	GREEN_SOUL_MES,
	BLUE_SOUL_MES,
	RED_SOUL_MES,
	WINDOW_MES,
	APP_EXIT,

};
typedef struct
{
	//BaseSprite Player;
	//Message msg;
	//PBaseSprite BackGround;

	/*PInput m_inp;
	PConsoleManager m_cm;
	PGameTimer m_gt;*/

	

}Scene, *PScene;



GAMESTATE Init(PScene p_this);
//GAMESTATE Start(PScene const p_this);
//GAMESTATE Update(PScene const p_this);
void Stop(PScene &p_this);