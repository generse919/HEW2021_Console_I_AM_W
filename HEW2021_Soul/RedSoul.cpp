#include "RedSoul.h"
static PS_RedSoul p_bs = NULL;
static GAMESTATE Start(PS_RedSoul p_this);
static GAMESTATE Update(PS_RedSoul p_this);
static void Stop(PS_RedSoul p_this);

static int rect[2] = {0,0};
void Red(GAMESTATE * gstate)
{
	if (*gstate == RED_INIT)
	{
		p_bs = (PS_RedSoul)calloc(1, sizeof(S_RedSoul));
		Create(&p_bs->SP_RedSoul, REDSOUL_PIC, REDSOUL);
		/*if (!(fp = fopen("../Release/scripts/playerlog.txt", "r")))
		{
			*gstate = APP_EXIT;
			return;
		}
		if (!(fs = fopen("../Release/scripts/soullog.txt", "w")))
		{
			*gstate = APP_EXIT;
			return;
		}*/
		*gstate = Start(p_bs);

	}
	else if (*gstate == RED)
	{
		*gstate = Update(p_bs);
	}
}

GAMESTATE Start(PS_RedSoul p_this)
{
	rect[0] = 0;
	rect[1] = DesktopHeight() - WindowHeight();



	return RED;
}
GAMESTATE Update(PS_RedSoul p_this)
{
	
	MoveWin(rect[0], rect[1]);

	for (int y = 0; y < ScreenHeight(); y++)
		for (int x = 0; x < ScreenWidth(); x++)
		{
			Draw(x, y, GetColor(&p_this->SP_RedSoul, x, y));
		}
	return RED;
}
void Stop(PS_RedSoul p_this)
{

}
