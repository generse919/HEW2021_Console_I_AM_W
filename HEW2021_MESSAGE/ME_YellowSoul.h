#pragma once
#include "BaseScene.h"
#include "message.h"


typedef struct
{
	Scene scn_title;
	Message msg_s;

}S_Mes_Yellow, *PS_Mes_Yellow;

void ME_YellowSoul(GAMESTATE *gstate);

