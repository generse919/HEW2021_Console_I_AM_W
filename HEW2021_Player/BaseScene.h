#pragma once
#include "BaseSprite.h"

enum GAMESTATE
{
	//SYSTEM_INIT = 0,
	PLAYER_SCN_INIT,
	GREEN_SOUL_INIT,
	BLUE_SOUL_INIT,
	RED_SOUL_INIT,
	WINDOW_SOUL_INIT,
	PLAYER_SCN,
	//YELLOW_SOUL_INIT,
	GREEN_SOUL,
	BLUE_SOUL,
	RED_SOUL,
	WINDOW_SOUL,
	//YELLOW_SOUL,
	APP_EXIT,

};

typedef struct
{
	BaseSprite Player;

}Scene, *PScene;


//
//GAMESTATE Title(GAMESTATE gstate);
//GAMESTATE Start(PScene &p_this);
//GAMESTATE Update(PScene const p_this);
//void Stop(PScene &p_this);