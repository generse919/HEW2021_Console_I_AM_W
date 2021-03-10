#include "GreenSoul.h"
static PS_GreenSoul scn = NULL;
static GAMESTATE Start(PS_GreenSoul p_this);
static GAMESTATE Update(PS_GreenSoul p_this);
static void Stop(PS_GreenSoul p_this);

//playerlog用
static FILE* fp;
//soullog用
//static FILE* fs;

static Vector2_F thisMidPos;
static Vector2_I playerMidPos;
static float speed;
//ソウルに表示する数字
static int timeNum;
static BOOL justTime;

static HWND playerHwnd;

void Green(GAMESTATE * gstate)
{
	if (*gstate == GREEN_INIT)
	{
		scn = (PS_GreenSoul)calloc(1, sizeof(S_GreenSoul));
		Create(&scn->SP_GreenSoul, GREENSOUL_PIC, GREENSOUL);
		LoadScript(&scn->num, "../Release/scripts/number.txt", 16);


		if (!(fp = fopen("../Release/scripts/playerlog.txt", "r")))
		{
			*gstate = APP_EXIT;
			return;
		}

		DWORD playerProcessId;
		if (!fscanf(fp, "%lu", &playerProcessId))//プレイヤーのプロセスid取得
		{
			*gstate = APP_EXIT;
			return;
		}

		if (GetWindowHandle(playerProcessId,playerHwnd) == NULL)
		{
			*gstate = APP_EXIT;
			return;
		}
		SAFE_CLOSE(fp);
	/*	if (!(fs = fopen("../Release/scripts/soullog.txt", "w")))
		{
			*gstate = APP_EXIT;
		}*/
		*gstate = Start(scn);

	}
	else if (*gstate == GREEN)
	{
		*gstate = Update(scn);
	}

}


GAMESTATE Start(PS_GreenSoul p_this)
{
	p_this->SP_GreenSoul.isActive = TRUE;
	speed = 0.0f;
	//if (!fscanf(fp, "x:%f,y:%f", &p_this->SP_GreenSoul.pos.x, &p_this->SP_GreenSoul.pos.y))
	//	return APP_EXIT;

	justTime = FALSE;
	p_this->SP_GreenSoul.pos = { (float)(DesktopWidth() - WindowWidth()), (float)(rand() % DesktopHeight()) };
	p_this->SP_GreenSoul.vec = { -3.0f,3.0f };
	srand(time(NULL));
	timeNum = rand() % 10;
	p_this->SP_GreenSoul.vec = { (float)-timeNum,(float)timeNum };
	return GREEN;

}
GAMESTATE Update(PS_GreenSoul p_this)
{
	//入力
	if (p_this->SP_GreenSoul.isActive)
	{

		thisMidPos = { (float)p_this->SP_GreenSoul.pos.x + (WindowWidth() / 2), (float)(float)p_this->SP_GreenSoul.pos.y + (WindowHeight() / 2) };
		/*float dx = (float)GetDesktopMousePosX() - thisMidPos.x;
		float dy = (float)GetDesktopMousePosY() - thisMidPos.y;
		float distance = sqrtf(pow(dx, 2.0) + pow(dy, 2.0));


		float fdest_theta = atan2(dy, dx);*/
		//speed = 5 - rand() % 10;//5~-5

		p_this->SP_GreenSoul.vec.x += (5 - rand() % 10)*0.8f *GetDeltaTime();
		p_this->SP_GreenSoul.vec.y += (5 - rand() % 10)*0.8f *GetDeltaTime();

		p_this->SP_GreenSoul.pos.x += p_this->SP_GreenSoul.vec.x;
		p_this->SP_GreenSoul.pos.y += p_this->SP_GreenSoul.vec.y;

		//反射処理
		if (p_this->SP_GreenSoul.pos.x <= 0.0f || p_this->SP_GreenSoul.pos.x + WindowWidth() >= DesktopWidth())
		{
			p_this->SP_GreenSoul.pos.x -= p_this->SP_GreenSoul.vec.x;
			p_this->SP_GreenSoul.vec.x = -(p_this->SP_GreenSoul.vec.x);
		}
		if (p_this->SP_GreenSoul.pos.y <= 0.0f || p_this->SP_GreenSoul.pos.y + WindowHeight() >= DesktopHeight())
		{
			p_this->SP_GreenSoul.pos.y -= p_this->SP_GreenSoul.vec.y;
			p_this->SP_GreenSoul.vec.y = -(p_this->SP_GreenSoul.vec.y);
		}

	}
#if 0
	freopen("../Release/scripts/playerlog.txt", "r", fp);
	if (!fscanf(fp, "x:%f,y:%f", &playerMidPos.x, &playerMidPos.y))
		return APP_EXIT;
#endif
	RECT player_rect = {0,0,0,0};
	GetWindowRect(playerHwnd,&player_rect);

	playerMidPos = { player_rect.right - ((player_rect.right - player_rect.left) / 2),player_rect.bottom - ((player_rect.bottom - player_rect.top) / 2) };

	float dx = thisMidPos.x - (float)playerMidPos.x;
	float dy = thisMidPos.y - (float)playerMidPos.y;
	float distance = sqrtf(pow(dx, 2.0) + pow(dy, 2.0));


	//時間取得
	unsigned char timeData[3];

	time_t t = time(NULL);

	timeData[0] = (t / 3600 + 9) % 12;//時間(0~11)
	timeData[1] = t / 60 % 60;//分(0~59)
	timeData[2] = t % 60;//秒(0~59)

	//各位の数字を取る。
	unsigned char countNum[6];
	countNum[0] = timeData[0] / 10;
	countNum[1] = timeData[0] % 10;
	countNum[2] = timeData[1] / 10;
	countNum[3] = timeData[1] % 10;
	countNum[4] = timeData[2] / 10;
	countNum[5] = timeData[2] % 10;


	for (int i = 0; i < 6; i++)
	{
		if (countNum[i] == timeNum)
		{
			justTime = TRUE;
			break;
		}
		justTime = FALSE;


	}
	if ((distance <= 50.0f && justTime) || ReleasedKey(PK_D))
	{
		/*freopen("../Release/scripts/playerlog.txt", "w", fp);
		char buf[32];
		snprintf(buf,sizeof(buf), "x:%f,y:%f", playerMidPos.x, playerMidPos.y);
		fputs(buf,fp);
		fflush(fp);*/
		Stop(p_this);
		return APP_EXIT;
	}


	//プレイヤ位置は常に最前面にしておく
	SetWindowPos(Get_HWND(), HWND_TOPMOST, p_this->SP_GreenSoul.pos.x, p_this->SP_GreenSoul.pos.y, 0, 0, (SWP_NOSIZE | SWP_NOOWNERZORDER));

	if (justTime)
	{
		for (int y = 0; y < ScreenHeight(); y++)
			for (int x = 0; x < ScreenWidth(); x++)
			{
				Draw(x, y, GetColor(&p_this->SP_GreenSoul, x, y));
			}
	}
	
	//文字を描画

	int sx = 7;
	int sy = 3;
	int pn = 0;
	for (int y = 0; y < p_this->num.script_buffer[timeNum].size.Y; y++)
	{
		for (int x = 0; x < p_this->num.script_buffer[timeNum].size.X; x++)
		{
			int xp = (sx + x);	//次の横方向位置
			int yp = (sy + y);
			if ((xp >= 0) && (xp < ScreenWidth()) && (yp >= 0) && (yp < ScreenHeight()))
			{
				switch ((UINT)p_this->num.script_buffer[timeNum].pPix[pn])//１ピクセル書き込む
				{
				case 0:(justTime ? Draw(xp, yp, { 44,202,132,0 }): Draw(xp, yp, { 0,0,0,0 })); break;//緑
				case 15:Draw(xp, yp, { 255,255,255,0 }); break;//白(文字)
				default:break;
				}

			}
			pn++;	//次のピクセル読み出し位置
		}
	}





	return GREEN;
}
void Stop(PS_GreenSoul p_this)
{
	Destroy(&p_this->SP_GreenSoul);
	ResetStringLog();
	SAFE_CLOSE(fp);
	SAFE_FREE(p_this);
}

