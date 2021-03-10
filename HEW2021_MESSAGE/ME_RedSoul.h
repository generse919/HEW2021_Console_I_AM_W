#pragma once
#include "BaseScene.h"
#include "message.h"


typedef struct
{
	Scene scn_title;
	Message msg_s;

}S_Mes_Red, *PS_Mes_Red;

void ME_RedSoul(GAMESTATE *gstate);

