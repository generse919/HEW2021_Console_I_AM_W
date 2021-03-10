#pragma once
#include "BaseScene.h"

typedef struct
{
	//Scene scn;
	BaseSprite Absorb_Y;
	BaseSprite Clock[15];

}S_GreenAbs, *PS_GreenAbs;

void Green_Abs(GAMESTATE *gstate);