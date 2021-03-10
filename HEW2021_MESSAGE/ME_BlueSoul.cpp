#include"ME_BlueSoul.h"

static  PS_Mes_Blue scn = NULL;

static GAMESTATE Start(PS_Mes_Blue  p_this);
static GAMESTATE Update(PS_Mes_Blue p_this);
static void Stop(PS_Mes_Blue p_this);

static int script_row;
static BOOL isActive;
static BOOL movedBlueSoul;
static BOOL isAbsorbed;
static FILE *fs;
static FILE *fm;

static float ftime;

//GreenSoulAppear用変数
static STARTUPINFO si;
static PROCESS_INFORMATION pi;
/*
*
*@brief		グリーンソウルプロセスを実行する
*/
void BlueSoulAppear(PBOOL isActive);
BOOL isBlueSoulAbsorb();
void ME_BlueSoul(GAMESTATE * gstate)
{
	if (*gstate == BLUE_SOUL_MES_INIT)
	{

		//Init(s_this.scn_title);
		scn = (PS_Mes_Blue)calloc(1, sizeof(S_Mes_Blue));
		LoadScript(&scn->msg_s, "../Release/scripts/test3.txt", 16);



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
	else if (*gstate == BLUE_SOUL_MES)
	{
		*gstate = Update(scn);
	}
}


GAMESTATE Start(PS_Mes_Blue  p_this)
{
	p_this->msg_s.Message.pos = { 300,0 };//表示端座標
	script_row = 0;
	ftime = 0.0f;
	isActive = TRUE;
	movedBlueSoul = FALSE;
	isAbsorbed = FALSE;
	return BLUE_SOUL_MES;
}
GAMESTATE Update(PS_Mes_Blue p_this)
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

	//if (isGreenSoulAbsorb())
	//{
	//	isActive = TRUE;
	//	ShowWindow(Get_HWND(), SW_SHOW);
	//	soulCount++;

	//	if (soulCount >= 3)
	//	{
	//		freopen("../Release/scripts/soullog.txt", "w", fs);
	//		fputs("AbsorbedGreenSoul", fs);//グリーンソウル取得メッセージ
	//		fflush(fs);
	//	}

	//}


	//ブルーソウルが現れる
	if (CheckFirstString(&p_this->msg_s, script_row) == L'$' && !movedBlueSoul)
	{
		ftime += GetDeltaTime();

		BlueSoulAppear(&isActive);
		DrawStringLog(&p_this->msg_s, 5, 10, script_row - 3, { 255,255,255,0 }, { 0,0,0,0 }, 0.1);

		if (ftime >= 3.0f)
		{
			isActive = TRUE;
			movedBlueSoul = TRUE;
			//script_row += 3;
		}



	}
	else if (CheckFirstString(&p_this->msg_s, script_row) == L'#' && !isAbsorbed)
	{
		if (isActive)
		{
			freopen("../Release/scripts/messagelog.txt", "w", fm);
			fputs("AbsorbedGreenSoul", fm);
			fflush(fm);
		}
		DrawStringLog(&p_this->msg_s, 5, 10, script_row - 3, { 36,28,237,0 }, { 0,0,0,0 }, 0.1);
		isActive = FALSE;
		if (isBlueSoulAbsorb())
		{

			isActive = TRUE;
			isAbsorbed = TRUE;
			//script_row += 3;
		}

	}
	else if (isActive && script_row != 16)
	{
		
		if (!DrawStringLog(&p_this->msg_s, 5, 10, script_row, { 255,255,255,0 }, { 0,0,0,0 }, 0.1))
		{
			fputs("AbsorbedBlueSoul", fm);
			Stop(p_this);
			return RED_SOUL_MES_INIT;
		}

	}



	//ウィンドウ位置固定
	MoveWin(p_this->msg_s.Message.pos.x, p_this->msg_s.Message.pos.y);
	return BLUE_SOUL_MES;
}
void Stop(PS_Mes_Blue p_this)
{
	SAFE_CLOSE(fm);
	SAFE_CLOSE(fs);
	ResetStringLog();
	SAFE_FREE(scn);
}

void BlueSoulAppear(PBOOL isActive)
{
	if (*isActive)
	{

		//Sleep(500);
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOW;
		si.lpTitle = (LPSTR)"BlueSoul";
		CreateProcess(NULL, (LPTSTR)("HEW2021_Soul.exe"), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

		*isActive = FALSE;
		// コールバック関数の呼び出し。
		EnumWindows(EnumWindowsProc, (LPARAM)&pi);


		//ここで処理を少し止めないとうまくいかないことがある
		//Sleep(2000);
	}

}

BOOL isBlueSoulAbsorb()
{
	if (WaitForSingleObject(pi.hProcess, 1) == WAIT_OBJECT_0 || ReleasedKey(PK_D))
	{
		TerminateProcess(pi.hProcess, 0);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return TRUE;

	}
	return FALSE;
}
