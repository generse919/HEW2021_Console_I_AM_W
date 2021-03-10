#include "Game.h"



int main(void)
{
	GamePlay game;
	
	if (CallInitialize(&game, 20,20,2,2))
	{
		Start(&game);
	}
	StopGame();

	return 0;
}