#pragma once
#include "BaseScene.h"

typedef struct
{
	//Scene scn;
	BaseSprite Player;
	BaseSprite Absorb_Y;

}S_Player, *PS_Player;

void Player(GAMESTATE *gstate);