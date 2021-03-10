#include "RedSoulAbs.h"

static PS_RedAbs scn = NULL;
static GAMESTATE Start(PS_RedAbs p_this);
static GAMESTATE Update(PS_RedAbs p_this);
static void Stop(PS_RedAbs p_this);
void TitleAppear();

static FILE *fm;
static FILE *fs;
//
static Vector2_F playerMidPos;
static float speed;
//描画用θ角
static float ftheta;
static int colornum;

static BOOL isAbsorb;
static BOOL Finish;
static float ftime;

//GreenSoulAppear用変数
static STARTUPINFO si;
static PROCESS_INFORMATION pi;




void Red_Abs(GAMESTATE * gstate)
{
	if (*gstate == RED_SOUL_INIT)
	{
		scn = (PS_RedAbs)calloc(1, sizeof(S_RedAbs));
		Create(&scn->Absorb_B, PIC_BLUESOUL, PLAYER_MODE_THREE);
		Create(&scn->Absorb_Y, PIC_YELLOWSOUL, PLAYER_MODE_ONE);
		Create(&scn->Absorb_G, PIC_GREENSOUL, PLAYER_MODE_TWO);
		Create(&scn->Absorb_R, PIC_REDSOUL, PLAYER_MODE_FOUR);

		if (!(fs = fopen("../Release/scripts/soullog.txt", "r")))
		{
			*gstate = APP_EXIT;
			return;
		}
		//最初は読み込み
		/*if (!(_fp = fopen("../Release/scripts/playerlog.txt", "r")))
		{
			*gstate = APP_EXIT;
			return;
		}*/
		/*if (!(fs = fopen("../Release/scripts/soullog.txt", "r")))
		{
			*gstate = APP_EXIT;
			return;
		}*/


		//スタート関数の実行
		*gstate = Start(scn);

	}
	else if (*gstate == RED_SOUL)
	{
		*gstate = Update(scn);
	}

	return;
}

GAMESTATE Start(PS_RedAbs p_this)
{

	/*if (!fscanf(_fp, "x:%f,y:%f", &p_this->Absorb_B.pos.x, &p_this->Absorb_B.pos.y))
		return APP_EXIT;
	fclose(_fp);*/
	//今度は書き込み用
	/*if (!(_fp = fopen("../Release/scripts/playerlog.txt", "w")))
	{
		return APP_EXIT;
	}*/
	//p_this->Absorb_B.isActive = TRUE;

	p_this->Absorb_B.pos.x = DesktopWidth() / 2;
	p_this->Absorb_B.pos.y = DesktopHeight() / 2;
	speed = 50.0f;
	ftheta = 0.0f;
	ftime = 0.0f;
	isAbsorb = FALSE;
	Finish = FALSE;
	//soul.exeを実行
	//GreenSoulAppear();


	return RED_SOUL;
}
GAMESTATE Update(PS_RedAbs p_this)
{
	if (p_this->Absorb_B.isActive)
	{
		if (HeldKey(PK_A))
		{
			p_this->Absorb_B.pos.x -= 60.0f* GetDeltaTime();
		}

		if (HeldKey(PK_D))
		{
			p_this->Absorb_B.pos.x += 60.0f* GetDeltaTime();
		}

		if (HeldKey(PK_W))
		{
			p_this->Absorb_B.pos.y -= 60.0f* GetDeltaTime();
		}
		if (HeldKey(PK_S))
		{
			p_this->Absorb_B.pos.y += 60.0f* GetDeltaTime();
		}

		if (ReleasedKey(PM_LEFT))
		{

			Finish = TRUE;
		}

	}
	if (Finish)
	{
		ftime += GetDeltaTime();
		if (ftime >= 6.0f)
		{
			/*FILE *fc;
			if ((fc = fopen("../Release/scripts/titleflg.txt", "r")))
			{
				char buf[512];
				fclose(fc);
			}*/

			TitleAppear();
			Stop(p_this);
			return APP_EXIT;

		}
	}


	//レッドソウルを吸収したか判定する(タイトルシーンへ)
	//ZeroMemory(&buf, sizeof(buf));
	char buf_s[100];
	freopen("../Release/scripts/soullog.txt", "r", fs);
	while (fscanf(fs, "%s", &buf_s) != EOF)
	{
		if (!strcmp(buf_s, "AbsorbedRedSoul"))
		{
			isAbsorb = TRUE;
			p_this->Absorb_B.isActive = TRUE;

		}
	}




	SetWindowPos(Get_HWND(), HWND_TOP, p_this->Absorb_B.pos.x, p_this->Absorb_B.pos.y, 0, 0, (SWP_NOSIZE | SWP_NOOWNERZORDER));

	if (ftheta < PI / 2 && ftheta + GetDeltaTime() >= PI / 2)//2/πを境に色を変える
	{
		colornum++;
	}
	ftheta += GetDeltaTime();

	if (ftheta >= PI)
	{
		//colornum++;
		ftheta = 0.0f;

	}

	if (!isAbsorb)
	{
		//ソウル描画
		for (int y = 0; y < ScreenHeight(); y++)
			for (int x = 0; x < ScreenWidth(); x++)
			{

				RGBQUAD Blight = { 255,245,223,0 };
				RGBQUAD Dark;
				switch (colornum % 3)//3色を入れ替える
				{
				case 0:Dark = GetColor(&p_this->Absorb_Y, x, y); break;
				case 1:Dark = GetColor(&p_this->Absorb_G, x, y); break;
				case 2:Dark = GetColor(&p_this->Absorb_B, x, y); break;
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

	}
	else
	{
		//完全体描画
		//ソウル描画
		for (int y = 0; y < ScreenHeight(); y++)
			for (int x = 0; x < ScreenWidth(); x++)
			{

				RGBQUAD Blight = { 255,245,223,0 };
				RGBQUAD Dark = GetColor(&p_this->Absorb_R, x, y);

				if (Dark.rgbBlue != 0 || Dark.rgbGreen != 0 || Dark.rgbRed != 0)
				{

					Draw(x, y,
						{ (BYTE)((float)Dark.rgbBlue + ((float)(Blight.rgbBlue - Dark.rgbBlue) * absf(sinf(ftheta)))),
						(BYTE)((float)Dark.rgbGreen + ((float)(Blight.rgbGreen - Dark.rgbGreen) *absf(sinf(ftheta)))),
					(BYTE)((float)Dark.rgbRed + ((float)(Blight.rgbRed - Dark.rgbRed)*absf(sinf(ftheta)))) });

				}
				else
				{
					Draw(x, y, GetColor(&scn->Absorb_R, x, y));
				}

			}
	}





	return RED_SOUL;
}
void Stop(PS_RedAbs p_this)
{
	Destroy(&p_this->Absorb_Y);
	Destroy(&p_this->Absorb_G);
	Destroy(&p_this->Absorb_B);

	//SAFE_CLOSE(fp);
	SAFE_CLOSE(fs);
	SAFE_FREE(scn);
}

void TitleAppear()
{

	FILE *fc;
	char buf[512];
	if ((fc = fopen("../Release/scripts/titleflg.txt", "r")))
	{
		fscanf(fc, "%s", &buf);
		/*freopen("../Release/scripts/titleflg.txt", "w", fc);
		fputs("clear", fc);*/
		fclose(fc);
	}


	STARTUPINFO ti;
	PROCESS_INFORMATION tpi;
	//ファイルログの更新
	ZeroMemory(&ti, sizeof(STARTUPINFO));
	ti.cb = sizeof(STARTUPINFO);
	ti.dwFlags = STARTF_USESHOWWINDOW;
	ti.wShowWindow = SW_SHOW;
	CreateProcess(NULL, (LPTSTR)buf, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &ti, &tpi);

}