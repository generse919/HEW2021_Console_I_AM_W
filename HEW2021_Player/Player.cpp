#include "Player.h"

//シーン保存用private
PS_Player scn = NULL;
static GAMESTATE Start(PS_Player const p_this);
static GAMESTATE Update(PS_Player const p_this);
static void Stop(PS_Player p_this);



void printError(const char *message);
void MessageMode(PBOOL isActive, PBOOL isTextClosed);



static int sx = 0;
static int sy = 0;
static float speed;
static Vector2_F playermidpos;
static BOOL SoulAbsorbed;
static BOOL TextClosed;
static float ftimer;

//色の明るさ用θ角
static float ftheta;

// メッセージ用exe起動
static STARTUPINFO si;
static PROCESS_INFORMATION pi;


static FILE *f;
static FILE *_fp;
static FILE *fm;





void Player(GAMESTATE *gstate)
{
	if (*gstate == PLAYER_SCN_INIT)
	{
		scn = (PS_Player)calloc(1, sizeof(S_Player));
		Create(&scn->Player, PIC_WHITESOUL, PLAYER_DEFAULT);
		Create(&scn->Absorb_Y, PIC_YELLOWSOUL, PLAYER_MODE_ONE);
		if (!(f = fopen("../Release/scripts/soullog.txt", "r")))
		{
			*gstate = APP_EXIT;
			return;
		}
		if (!(_fp = fopen("../Release/scripts/playerlog.txt", "w")))
		{
			*gstate = APP_EXIT;
			return;
		}
		if ((!(fm = fopen("../Release/scripts/messagelog.txt", "r"))))
		{
			*gstate = APP_EXIT;
			return;
		}


		//スタート関数の実行
		*gstate = Start(scn);

	}
	else if (*gstate == PLAYER_SCN)
	{
		*gstate = Update(scn);
	}

	return;




}

//GameState Init(GameState gstate)//シーン環境準備が終わった場合
GAMESTATE Start(PS_Player const p_this)
{
	p_this->Player.isActive = TRUE;
	SoulAbsorbed = FALSE;
	TextClosed = FALSE;
	scn->Player.pos = { 0.0f,0.0f };
	scn->Absorb_Y.pos = { 0.0f,0.0f };
	playermidpos = { (float)sx + (WindowWidth() / 2), (float)sy + (WindowHeight() / 2) };
	ftimer = 0.0f;
	ftheta = 0.0f;
	speed = 5.0f;
	//scn->BackGround.pos = { 0.0f,0.0f };

	return PLAYER_SCN;
}
GAMESTATE Update(PS_Player p_this)
{
	float deltaTime = GetDeltaTime();



	if (p_this->Player.isActive)
	{
		/*if (ReleasedKey(PK_P))
		{
			return APP_EXIT;
		}*/

		if (HeldKey(PK_A))
		{

			scn->Player.pos.x -= 1.0f;
			if (scn->Player.pos.x <= 0.0f)
			{
				scn->Player.pos.x += 1.0f;
			}

			sx -= 2;
		}

		if (HeldKey(PK_D))
		{
			scn->Player.pos.x += 1.0f;
			if (scn->Player.pos.x >= (float)(ScreenWidth() - scn->Player.w))
			{
				scn->Player.pos.x -= 1.0f;
			}

			sx += 2;
		}

		if (HeldKey(PK_W))
		{
			scn->Player.pos.y -= 1.0f;
			if (scn->Player.pos.y <= 0.0f)
			{
				scn->Player.pos.y += 1.0f;
			}

			sy -= 2;
		}

		if (HeldKey(PK_S))
		{
			scn->Player.pos.y += 1.0f;
			if (scn->Player.pos.y >= (float)(ScreenHeight() - scn->Player.h))
			{
				scn->Player.pos.y -= 1.0f;
			}
			sy += 2;
		}

		/*if (ReleasedKey(PM_LEFT))
		{
			sy++;
		}*/
		playermidpos = { (float)sx + (WindowWidth() / 2), (float)sy + (WindowHeight() / 2) };

		float dx = (float)GetDesktopMousePosX() - playermidpos.x;
		float dy = (float)GetDesktopMousePosY() - playermidpos.y;
		float distance = sqrtf(pow(dx, 2.0) + pow(dy, 2.0));


		float fdest_theta = atan2(dy, dx);

		p_this->Player.vec.x = distance * speed * cos(fdest_theta)*GetDeltaTime();
		p_this->Player.vec.y = distance * speed * sin(fdest_theta)*GetDeltaTime();

		sx += p_this->Player.vec.x;
		sy += p_this->Player.vec.y;


	}

	


	ftheta += deltaTime;
	if (ftheta >= 2 * PI) ftheta = 0.0f;

	//scn->Player.pos.x += scn->Player->vec.x;


	/*for (int y = 0; y < scn->BackGround->h; y++)
		for (int x = 0; x < scn->BackGround->w; x++)
		{
			Draw(x + (int)scn->BackGround.pos.x, y + (int)scn->BackGround.pos.y, GetColor(scn->BackGround, x, y));
		}*/


	ftimer += deltaTime;

	if (ftimer >= 6.0f && !TextClosed)
	{
		if (p_this->Player.isActive)
		{
			char tmp[100];
#if 0
			
			snprintf(tmp, 100, "x:%f,y:%f", (float)playermidpos.x, (float)playermidpos.y);
			fputs(tmp, _fp);
#endif

			DWORD processid = GetWindowThreadProcessId(Get_HWND(), NULL);
			snprintf(tmp, sizeof(tmp), "%lu", processid);
			fputs(tmp,_fp);
			fflush(_fp);
			fclose(_fp);
		}
		MessageMode(&p_this->Player.isActive, &TextClosed);

	}

	/*MoveWin(sx, sy);*/
	SetWindowPos(Get_HWND(), HWND_TOPMOST, sx, sy, 0, 0, (SWP_NOSIZE));

	if (TextClosed)
	{
		Stop(p_this);
		return GREEN_SOUL_INIT;
	}

	if (SoulAbsorbed)
	{
		p_this->Absorb_Y.pos = p_this->Player.pos;

		for (int y = 0; y < ScreenHeight(); y++)
			for (int x = 0; x < ScreenWidth(); x++)
			{

				RGBQUAD Blight = { 255,245,223,0 };
				RGBQUAD Dark = GetColor(&p_this->Absorb_Y, x, y);

				/*if (Dark.rgbBlue != 0 || Dark.rgbGreen != 0 || Dark.rgbRed != 0 )
				{
					SetColor(&scn->Player, x, y, { (BYTE)((float)Dark.rgbBlue + ((float)(Blight.rgbBlue - Dark.rgbBlue) *0.2* absf(sinf(ftheta)))),
					(BYTE)((float)Dark.rgbGreen + ((float)(Blight.rgbGreen - Dark.rgbGreen) * 0.2*absf(sinf(ftheta)))),
					(BYTE)((float)Dark.rgbRed + ((float)(Blight.rgbRed - Dark.rgbRed) * 0.2*absf(sinf(ftheta)))) });
				}*/
				if (Dark.rgbBlue != 0 || Dark.rgbGreen != 0 || Dark.rgbRed != 0)
				{

					Draw(x + (int)scn->Absorb_Y.pos.x, y + (int)scn->Absorb_Y.pos.y,
						{ (BYTE)((float)Dark.rgbBlue + ((float)(Blight.rgbBlue - Dark.rgbBlue) * absf(sinf(ftheta)))),
						(BYTE)((float)Dark.rgbGreen + ((float)(Blight.rgbGreen - Dark.rgbGreen) *absf(sinf(ftheta)))),
					(BYTE)((float)Dark.rgbRed + ((float)(Blight.rgbRed - Dark.rgbRed)*absf(sinf(ftheta)))) });

				}
				else
				{
					Draw(x + (int)scn->Absorb_Y.pos.x, y + (int)scn->Absorb_Y.pos.y, GetColor(&scn->Absorb_Y, x, y));
				}



			}
	}
	else
	{
		for (int y = 0; y < ScreenHeight(); y++)
			for (int x = 0; x < ScreenWidth(); x++)
			{

				Draw(x + (int)scn->Player.pos.x, y + (int)scn->Player.pos.y, GetColor(&scn->Player, x, y));

			}
	}



	return PLAYER_SCN;
}
void Stop(PS_Player p_this)
{
	Destroy(&p_this->Player);
	Destroy(&p_this->Absorb_Y);
	if (f != NULL)fclose(f); f = NULL;
	//Destroy(p_this->BackGround);
	/*p_this->m_inp = NULL;
	p_this->m_cm = NULL;
	p_this->m_gt = NULL;*/
	SAFE_FREE(p_this);

}



void printError(const char *message)
{
	LPVOID lpvMessageBuffer;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpvMessageBuffer,
		0,
		NULL);
	fprintf(stderr, "%s: %s\n", message, lpvMessageBuffer);
	LocalFree(lpvMessageBuffer);
}



void MessageMode(PBOOL isActive, PBOOL isTextClosed)
{

	if (*isActive)
	{

		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOW;
		CreateProcess(NULL, (LPTSTR)("HEW2021_MESSAGE.exe"), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);


		// コールバック関数の呼び出し。
		EnumWindows(EnumWindowsProc, (LPARAM)&pi);

		*isActive = FALSE;
		*isTextClosed = FALSE;
		//ここで処理を少し止めないとうまくいかないことがある
		Sleep(2000);
	
		
	}
	//SetWindowPos(Get_HWND(), HWND_NOTOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));

	freopen("../Release/scripts/messagelog.txt", "r", f);
	char buf[216];

	while ((fscanf(f, "%s", &buf)) != EOF)
	{
		if (!strcmp(buf, "AbsorbedYellowSoul"))
		{
			TextClosed = TRUE;
		}
	}
	//メッセージコンソール終了まで1フレ単位で待機モード(終了した場合下を実行)
	/*if (WaitForSingleObject(pi.hProcess, 1) == WAIT_OBJECT_0 || TextClosed)
	{
		
		TerminateProcess(pi.hProcess, 0);
		*isActive = TRUE;
		*isTextClosed = TRUE;
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);

	}*/

	if (!SoulAbsorbed)
	{
		freopen("../Release/scripts/soullog.txt", "r", f);
		char buf[216];
		//char [216];
		//while()
		while ((fscanf(f, "%s", &buf)) != EOF)
		{
			if (!strcmp(buf, "AbsorbedYellowSoul"))
			{
				SoulAbsorbed = TRUE;
			}
		}

	}




	/*PROCESS_INFORMATION f;

	EnumWindows(GetColorMes, (LPARAM)&f);*/





}