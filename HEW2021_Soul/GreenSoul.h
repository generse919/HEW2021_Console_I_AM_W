#pragma once
#include "BaseScene.h"
#include "message.h"

typedef struct
{
	//Scene scn;
	BaseSprite SP_GreenSoul;
	Message num;

}S_GreenSoul, *PS_GreenSoul;
//GAMESTATE Blue_Init(GAMESTATE gstate);
void Green(GAMESTATE *gstate);