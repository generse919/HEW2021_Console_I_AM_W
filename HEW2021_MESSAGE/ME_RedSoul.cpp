#include"ME_RedSoul.h"


static  PS_Mes_Red scn = NULL;

static GAMESTATE Start(PS_Mes_Red  p_this);
static GAMESTATE Update(PS_Mes_Red p_this);
static void Stop(PS_Mes_Red p_this);
void RedSoulAppear(PBOOL isActive); 
BOOL isRedSoulAbsorb();
static int script_row;
static BOOL isActive;
static BOOL isAbsorb;
static FILE *fs;
static FILE *fm;


//RedSoulAppear用変数
static STARTUPINFO si;
static PROCESS_INFORMATION pi;


void ME_RedSoul(GAMESTATE * gstate)
{
	if (*gstate == RED_SOUL_MES_INIT)
	{

		//Init(s_this.scn_title);
		scn = (PS_Mes_Red)calloc(1, sizeof(S_Mes_Red));
		LoadScript(&scn->msg_s, "../Release/scripts/test4.txt", 24);



		if (!(fs = fopen("../Release/scripts/soullog.txt", "w")))
		{
			*gstate = APP_EXIT;
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
	else if (*gstate == RED_SOUL_MES)
	{
		*gstate = Update(scn);
	}
}
GAMESTATE Start(PS_Mes_Red  p_this)
{
	p_this->msg_s.Message.pos = { 300,0 };//表示端座標
	script_row = 0;
	isActive = TRUE;
	
	isAbsorb = FALSE;
	return RED_SOUL_MES;
}
GAMESTATE Update(PS_Mes_Red p_this)
{
	if (isActive)
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

	if (CheckFirstString(&p_this->msg_s, script_row) == L'$' && !isAbsorb)
	{
		RedSoulAppear(&isActive);
		DrawStringLog(&p_this->msg_s, 5, 10, script_row - 3, { 36,28,237,0 }, { 0,0,0,0 }, 0.1);
		if (isRedSoulAbsorb())
		{
			freopen("../Release/scripts/soullog.txt", "w", fs);
			fputs("AbsorbedRedSoul", fs);
			fflush(fs);
			isActive = TRUE;
			isAbsorb = TRUE;

		}
	} 
	if (isAbsorb)
	{
		if (!DrawStringLog(&p_this->msg_s, 5, 10, script_row, { 255,255,255,0 }, { 0,0,0,0 }, 0.1))
		{
			//fputs("AbsorbedRedSoul", fs);
			Stop(p_this);
			return APP_EXIT;
		}

	}
	

	//ウィンドウ位置固定
	MoveWin(p_this->msg_s.Message.pos.x, p_this->msg_s.Message.pos.y);
	return RED_SOUL_MES;
}
void Stop(PS_Mes_Red p_this)
{
	SAFE_CLOSE(fs);
	SAFE_CLOSE(fm);
	ResetStringLog();
	SAFE_FREE(scn);
}


void RedSoulAppear(PBOOL isActive)
{
	if (*isActive)
	{

		//ファイルログの更新
		freopen("../Release/scripts/soullog.txt", "w", fs);
		fputs("AbsorbedBlueSoul", fs);
		fflush(fs);
		fclose(fs);
		//Sleep(500);
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow =SW_SHOW;
		si.lpTitle = (LPSTR)"RedSoul";
		CreateProcess(NULL, (LPTSTR)("HEW2021_Soul.exe"), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

		*isActive = FALSE;
		// コールバック関数の呼び出し。
		EnumWindows(EnumWindowsProc, (LPARAM)&pi);


		//ここで処理を少し止めないとうまくいかないことがある
		//Sleep(2000);
	}

}

BOOL isRedSoulAbsorb()
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
