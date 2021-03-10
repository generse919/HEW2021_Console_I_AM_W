#include "SystemInit.h"


void System_Init(GAMESTATE * gstate)
{
	FILE *fp = NULL;

	if (!(fp = fopen("../Release/scripts/soullog.txt", "r")))
	{
		*gstate = APP_EXIT;
	}

	//*gstate = YELLOW_INIT;
#if 0
	while (fscanf(fp, "%s", &tmp) != EOF)
	{
		if (!strcmp(tmp, "AbsorbedYellowSoul"))
		{
			*gstate = GREEN_INIT;
			break;
		}
		if (!strcmp(tmp, "AbsorbedGreenSoul"))
		{

			*gstate = BLUE_INIT;
			break;
		}
		if (!strcmp(tmp, "AbsorbedBlueSoul"))
		{
			*gstate = RED_INIT;
			break;
		}
		if (!strcmp(tmp, "AbsorbedRedSoul"))
		{
			//*gstate = RED_INIT;
		}
	}
#endif
	STARTUPINFO s;
	GetStartupInfo(&s);
	//switch (s.wShowWindow)//表示状態でシーン遷移を管理
	//{
	//case FG_GREEN:*gstate = GREEN_INIT; break;
	//case FG_BLUE:*gstate = BLUE_INIT; break;
	//case FG_RED:*gstate = RED_INIT; break;
	//case FG_YELLOW:
	//default:	*gstate = YELLOW_INIT; break;
	//}
	
	if (!strcmp(s.lpTitle, "YellowSoul"))
	{
		*gstate = YELLOW_INIT;
		
	}
	else if (!strcmp(s.lpTitle, "GreenSoul"))
	{
		*gstate = GREEN_INIT;
		
	}
	else if (!strcmp(s.lpTitle, "BlueSoul"))
	{
		*gstate = BLUE_INIT;
		
	}
	else if (!strcmp(s.lpTitle, "RedSoul"))
	{
		*gstate = RED_INIT;
		
	}
	else
	{
		*gstate = APP_EXIT;
	}
	//ShowWindow(Get_HWND(),SW_SHOW);//表示状態は戻しておく
	if(*gstate != RED_INIT)DeleteWinMenu(Get_HWND());//ユーザーウィンドウ初期設定(メニューを消す)

	fclose(fp);
	


}
