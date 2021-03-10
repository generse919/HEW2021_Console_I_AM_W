#pragma once
#include "BaseScene.h"

typedef struct
{
	//Scene scn;

	BaseSprite Absorb_Y;
	BaseSprite Absorb_G;
	BaseSprite Absorb_B;
	BaseSprite Absorb_R;

}S_RedAbs, *PS_RedAbs;

void Red_Abs(GAMESTATE *gstate);