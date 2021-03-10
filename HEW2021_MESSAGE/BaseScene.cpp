#include "BaseScene.h"
#include <crtdbg.h>
//シーン保存用private
PScene scn = NULL;
static int sx = 0;
static int sy = 0;

GAMESTATE Init(PScene p_this)
{
	/*if (gstate == GAMESTATE::TITLE_INIT)
	{*/
		scn = (PScene)calloc(1,sizeof(Scene));
		//LoadScript("scripts/test.txt");
		//Create(&scn->Player, LETTERS, LETTERS);
		//Create(scn->BackGround, TITLE_PIC);


		//スタート関数の実行
		/*return Start(scn);
		
	}
	else if(gstate == GAMESTATE::TITLE)
	{
		return Update(scn);
	}
	
	return GAMESTATE::TITLE;
	*/
	
	return APP_EXIT;
	
}
//
////GameState Init(GameState gstate)//シーン環境準備が終わった場合
//GAMESTATE Start(PScene &p_this)
//{
//	//scn->Player.pos = { 200.0f,0.0f };
//	//scn->BackGround.pos = { 0.0f,0.0f };
//	//MoveWin(sx, sy);
//	
//
//	return TITLE;
//}
//GAMESTATE Update(PScene const p_this)
//{
//	//float deltaTime = GetDeltaTime();
//	
//	/*char message[] = "!!!";
//	int s = sizeof(message);
//
//	for (int i = 0; i < s; i++)
//	{
//		UINT code = (UINT)message[i];
//
//		for (int y = 0; y < 12; y++)
//			for (int x = 0; x < 8; x++)
//			{
//				Draw(x + (i*8), y, GetColor(&scn->Player,(code%94) * 8 + x, (code % 94) * 12 + y));
//			}
//
//		
//	}*/
//	
//	
//
//
//
//
//	return GAMESTATE::TITLE;
//}
//void Stop(PScene &p_this)
//{
//	//Destroy(&p_this->Player);
//	//Destroy(p_this->BackGround);
//	/*p_this->m_inp = NULL;
//	p_this->m_cm = NULL;
//	p_this->m_gt = NULL;*/
//	//SAFE_FREE(p_this);
//
//}