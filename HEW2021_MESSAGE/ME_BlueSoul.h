#pragma once
#include "BaseScene.h"
#include "message.h"


typedef struct
{
	Scene scn_title;
	Message msg_s;

}S_Mes_Blue, *PS_Mes_Blue;

void ME_BlueSoul(GAMESTATE *gstate);

