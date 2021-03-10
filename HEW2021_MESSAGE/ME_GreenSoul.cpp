#include "ME_GreenSoul.h"

static  PS_Mes_Green scn = NULL;

static GAMESTATE Start(PS_Mes_Green  p_this);
static GAMESTATE Update(PS_Mes_Green p_this);
static void Stop(PS_Mes_Green p_this);

static int script_row;
//繰り返し行う回数
static int soulCount;
static FILE *fs;
static FILE *fm;

static BOOL isActive;
//static BOOL isTextClosed;
static float ftime;

//GreenSoulAppear用変数
static STARTUPINFO si;
static PROCESS_INFORMATION pi;

void GreenSoulAppear(PBOOL isActive);
BOOL isGreenSoulAbsorb();

void ME_GreenSoul(GAMESTATE * gstate)
{
	if (*gstate == GREEN_SOUL_MES_INIT)
	{
		
		//Init(s_this.scn_title);
		scn = (PS_Mes_Green)calloc(1, sizeof(S_Mes_Green));
		LoadScript(&scn->msg_s, "../Release/scripts/test2.txt", 16);

		

		if (!(fs = fopen("../Release/scripts/soullog.txt", "w")))
		{
			*gstate =  APP_EXIT;
			return;
		}
		if (!(fm = fopen("../Release/scripts/messagelog.txt", "w")))
		{
			*gstate = APP_EXIT;
			return;
		}

		//スタート関数の実行
		*gstate = Start(scn);

	}
	else if (*gstate == GREEN_SOUL_MES)
	{
		*gstate = Update(scn);
	}

}

GAMESTATE Start(PS_Mes_Green  p_this)
{
	p_this->msg_s.Message.pos = { 300,0 };//表示端座標
	script_row = 0;
	soulCount = 0;
	isActive = TRUE;
	//isTextClosed = FALSE;
	return GREEN_SOUL_MES;
}
GAMESTATE Update(PS_Mes_Green p_this)
{
	ftime += GetDeltaTime();
	if (isActive && ftime >= 2.0f)
	{
		if (ReleasedKey(PK_F) || ReleasedKey(PM_LEFT))
		{
			ResetStringLog();
			script_row += 3;
		}
		if (HeldKey(PK_1) && HeldKey(PK_2))
		{
			Stop(p_this);
			return APP_EXIT;

		}
	}


	if (isGreenSoulAbsorb())
	{
		isActive = TRUE;
		ShowWindow(Get_HWND(), SW_SHOW);
		soulCount++;

		if (soulCount >= 3)
		{
			freopen("../Release/scripts/soullog.txt", "w", fs);
			fputs("AbsorbedGreenSoul",fs);//グリーンソウル取得メッセージ
			fflush(fs);
			ResetStringLog();
			script_row += 3;
		}
		
	}

	

	if (CheckFirstString(&p_this->msg_s, script_row) == L'$' && soulCount < 3)
	{
		DrawStringLog(&p_this->msg_s, 5, 10, script_row - 3, { 36,28,237,0 }, {0,0,0,0}, 0.1);
		GreenSoulAppear(&isActive);
		ShowWindow(Get_HWND(), SW_HIDE);
		

	}

	

	if (isActive)
	{
		if (script_row == 6)
		{
			DrawStringLog(&p_this->msg_s, 5, 10, script_row, { 36,28,237,0 }, { 0,0,0,0 }, 0.1);
		}
		else if (!DrawStringLog(&p_this->msg_s, 5, 10, script_row, { 255,255,255,0 }, {0,0,0,0},0.1))
		{
			//fputs("AbsorbedGreenSoul",fm);
			Stop(p_this);
			return BLUE_SOUL_MES_INIT;
		}
	}
	//ウィンドウ位置固定
	MoveWin(p_this->msg_s.Message.pos.x, p_this->msg_s.Message.pos.y);
	return GREEN_SOUL_MES;
}
void Stop(PS_Mes_Green p_this)
{
	SAFE_CLOSE(fs);
	SAFE_CLOSE(fm);
	ResetStringLog();
	SAFE_FREE(p_this);
}


/*
*
*@brief		グリーンソウルプロセスを実行する
*/
void GreenSoulAppear(PBOOL isActive)
{
	if (*isActive)
	{
		
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOW;
		si.lpTitle = (LPSTR)"GreenSoul";
		CreateProcess(NULL, (LPTSTR)("HEW2021_Soul.exe"), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

		*isActive = FALSE;
		// コールバック関数の呼び出し。
		EnumWindows(EnumWindowsProc, (LPARAM)&pi);


		//ここで処理を少し止めないとうまくいかないことがある
		//Sleep(2000);
	}
	





}
/*
*
*@brief		グリーンソウルを吸収したかどうか(プロセスが終了したかどうか)を確認する
*/
BOOL isGreenSoulAbsorb()
{
	if (WaitForSingleObject(pi.hProcess, 1) == WAIT_OBJECT_0)
	{
		TerminateProcess(pi.hProcess, 0);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return TRUE;

	}
	return FALSE;
}