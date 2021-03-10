#include "BlueSoulAbs.h"

static PS_BlueAbs scn = NULL;
static GAMESTATE Start(PS_BlueAbs p_this);
static GAMESTATE Update(PS_BlueAbs p_this);
static void Stop(PS_BlueAbs p_this);

static FILE *fp;
static FILE *fs;
static FILE *fm;
static int sx = 400;
static int sy = 300;
static Vector2_F playerMidPos;
static float speed;
//描画用θ角
static float ftheta;
static int colornum;

void Blue_Abs(GAMESTATE * gstate)
{
	if (*gstate == BLUE_SOUL_INIT)
	{
		scn = (PS_BlueAbs)calloc(1, sizeof(S_BlueAbs));
		Create(&scn->Absorb_Y, PIC_YELLOWSOUL, PLAYER_MODE_ONE);
		Create(&scn->Absorb_G, PIC_GREENSOUL, PLAYER_MODE_TWO);



		if (!(fp = fopen("../Release/scripts/playerlog.txt", "r")))
		{
			*gstate = APP_EXIT;
			return;
		}
		if (!(fs = fopen("../Release/scripts/soullog.txt", "r")))
		{
			*gstate = APP_EXIT;
			return;
		}
		if (!(fm = fopen("../Release/scripts/messagelog.txt", "r")))
		{

			*gstate = APP_EXIT;
			return;

		}

		//スタート関数の実行
		*gstate = Start(scn);

	}
	else if (*gstate == BLUE_SOUL)
	{
		*gstate = Update(scn);
	}

	return;
}



GAMESTATE Start(PS_BlueAbs p_this)
{
	/*if (!fscanf(fp, "x:%f,y:%f", &p_this->Absorb_G.pos.x, &p_this->Absorb_G.pos.y))
		return APP_EXIT;*/
	colornum = 0;
	playerMidPos = {p_this->Absorb_G.pos.x + (WindowWidth() / 2), p_this->Absorb_Y.pos.y + (WindowHeight() / 2) };
	speed = 0.005f;
	ftheta = 0.0f;

	//p_this->Absorb_G.isActive = TRUE;
	return BLUE_SOUL;
}
GAMESTATE Update(PS_BlueAbs p_this)
{
	//入力
	if (p_this->Absorb_G.isActive)
	{
		
		playerMidPos = { (float)sx + (WindowWidth() / 2), (float)sy + (WindowHeight() / 2) };

		float dx =(float)GetDesktopMousePosX() - playerMidPos.x;
		float dy =((float)GetDesktopMousePosY() - playerMidPos.y);
		float distance = sqrtf(pow(dx, 2.0) + pow(dy, 2.0));


		float fdest_theta = atan2(dy, dx);

		p_this->Absorb_G.vec.x += distance*speed * cos(fdest_theta)*GetDeltaTime();
		p_this->Absorb_G.vec.y += distance*speed * sin(fdest_theta)*GetDeltaTime();

		sx += p_this->Absorb_G.vec.x;
		sy += p_this->Absorb_G.vec.y;

		if (sx <= -300 || sx >= DesktopWidth() + 300)
		{
			p_this->Absorb_G.vec.x = -p_this->Absorb_G.vec.x;
			//sx = GetDesktopMousePosX();

		}

		if (sy <= -200 || sy >= DesktopHeight() + 200)
		{
			p_this->Absorb_G.vec.y = -p_this->Absorb_G.vec.y;
			//sy = GetDesktopMousePosY();

		}


		char buf_p[100];
		freopen("../Release/scripts/playerlog.txt", "w", fp);
		snprintf(buf_p, sizeof(buf_p), "x:%f,y:%f", playerMidPos.x, playerMidPos.y);
		fputs(buf_p, fp);
		fflush(fp);
		
		
	}


	//WaitMessage
	if (!p_this->Absorb_G.isActive)
	{
		char buf_m[100];
		freopen("../Release/scripts/messagelog.txt", "r", fm);
		while ((fscanf(fm, "%s", &buf_m)) != EOF)
		{
			if (!strcmp(buf_m, "AbsorbedGreenSoul"))
			{
				p_this->Absorb_G.isActive = TRUE;
			}
		}
	}
	
	//ブルーソウルを吸収したら終了
	if (p_this->Absorb_G.isActive)
	{
		char buf_s[100];
		freopen("../Release/scripts/soullog.txt", "r", fs);
		while ((fscanf(fs, "%s", &buf_s)) != EOF)
		{
			if (!strcmp(buf_s, "AbsorbedBlueSoul"))
			{
				Stop(p_this);
				return RED_SOUL_INIT;
			}
		}
	}


	MoveWin(sx, sy);



	if (ftheta < PI / 2 && ftheta + GetDeltaTime() >= PI / 2)//2/πを境に色を変える
	{
		colornum++;
	}
	ftheta += GetDeltaTime();

	if (ftheta >= PI)
	{
		ftheta = 0.0f;

	}
	

	//ソウル描画
	for (int y = 0; y < ScreenHeight(); y++)
		for (int x = 0; x < ScreenWidth(); x++)
		{

			RGBQUAD Blight = { 255,245,223,0 };
			RGBQUAD Dark;
			switch (colornum % 2)//2色を入れ替える
			{
			case 0:Dark = GetColor(&p_this->Absorb_Y, x, y); break;
			case 1:Dark = GetColor(&p_this->Absorb_G, x, y); break;
			default:Dark = { 255,255,255,0 }; break;
			}
			if (Dark.rgbBlue != 0 || Dark.rgbGreen != 0 || Dark.rgbRed != 0)
			{

				Draw(x, y,
					{ (BYTE)((float)Dark.rgbBlue + ((float)(Blight.rgbBlue - Dark.rgbBlue) * absf(sinf(ftheta)))),
					(BYTE)((float)Dark.rgbGreen + ((float)(Blight.rgbGreen - Dark.rgbGreen) *absf(sinf(ftheta)))),
				(BYTE)((float)Dark.rgbRed + ((float)(Blight.rgbRed - Dark.rgbRed)*absf(sinf(ftheta)))) });

			}
			else
			{
				Draw(x, y, GetColor(&scn->Absorb_G, x, y));
			}



		}
	return BLUE_SOUL;
}
void Stop(PS_BlueAbs p_this)
{

}
