#pragma once
#include "BaseSprite.h"


enum GAMESTATE
{
	TITLE_INIT,
	TITLE,
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