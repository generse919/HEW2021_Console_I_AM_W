#include "ME_YellowSoul.h"

//シーン内プライベート変数
static  PS_Mes_Yellow s_this;

static int script_row;
static GAMESTATE Start(PS_Mes_Yellow const p_this);
static GAMESTATE Update(PS_Mes_Yellow p_this);
static void Stop(PS_Mes_Yellow p_this);

void MessageMode(PBOOL isActive, PBOOL isTextClosed);
static BOOL isActive;
static BOOL isTextClosed;



static FILE *rr;
static FILE *fw;


// exe起動用変数
static STARTUPINFO si;
static PROCESS_INFORMATION pi;


void MessageMode(PBOOL isActive, PBOOL isTextClosed)
{

	if (*isActive)
	{

		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOW;
		si.lpTitle = (LPSTR)"YellowSoul";
		CreateProcess(NULL, (LPTSTR)("HEW2021_Soul.exe"), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);


		// コールバック関数の呼び出し。
		EnumWindows(EnumWindowsProc, (LPARAM)&pi);

		*isActive = FALSE;
		*isTextClosed = FALSE;
		//ここで処理を少し止めないとうまくいかないことがある
		Sleep(2000);


	}
	SetWindowPos(Get_HWND(), HWND_NOTOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));

	//メッセージコンソール終了まで1フレ単位で待機モード(終了した場合下を実行)
	if (WaitForSingleObject(pi.hProcess, 1) == WAIT_OBJECT_0)
	{
		TerminateProcess(pi.hProcess, 0);
		*isActive = TRUE;
		*isTextClosed = TRUE;
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);

		
		fputs("AbsorbedYellowSoul",rr);
		fflush(rr);
		fclose(rr);
		//EnumWindows(PostWinMessage, (LPARAM)&pi);



	}

}



void ME_YellowSoul(GAMESTATE *gstate)
{


	if (*gstate == YELLOW_SOUL_MES_INIT)
	{
		//Init(s_this.scn_title);
		s_this = (PS_Mes_Yellow)calloc(1, sizeof(S_Mes_Yellow));
		LoadScript(&s_this->msg_s, "../Release/scripts/test.txt", 16);
		//LoadGameLog();
		//Create(&scn->Player, LETTERS, LETTERS);
		//Create(scn->BackGround, TITLE_PIC);
		

		if (!(rr = fopen("../Release/scripts/soullog.txt","w")))
		{
			*gstate = APP_EXIT;
			return;
		}
		if (!(fw = fopen("../Release/scripts/messagelog.txt", "w")))
		{
			*gstate = APP_EXIT;
			return;
		}

		//スタート関数の実行
		*gstate = Start(s_this);

	}
	else if (*gstate == YELLOW_SOUL_MES)
	{
		*gstate = Update(s_this);
	}




}

GAMESTATE Start(PS_Mes_Yellow const p_this)
{
	
	p_this->msg_s.Message.pos = { 300,0 };//表示端座標
	script_row = 0;
	isActive = TRUE;
	isTextClosed = FALSE;
	MoveWin(p_this->msg_s.Message.pos.x, p_this->msg_s.Message.pos.y);
	
	return YELLOW_SOUL_MES;
}

GAMESTATE Update(PS_Mes_Yellow p_this)
{
	float deltaTime = GetDeltaTime();


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
	
	//ウィンドウ位置固定
	MoveWin(p_this->msg_s.Message.pos.x, p_this->msg_s.Message.pos.y);

	if (CheckFirstString(&p_this->msg_s, script_row) == L'$' && !isTextClosed)
	{
		MessageMode(&isActive,&isTextClosed);
		DrawStringLog(&p_this->msg_s, 5, 10, script_row-3, { 255,255,255,0 }, { 0,0,0,0 },0.01);

	}

	if (isActive) 
	{
		if (!DrawStringLog(&p_this->msg_s, 5, 10, script_row, { 255,255,255,0 }, { 0,0,0,0 }, 0.01))
		{
			fputs("AbsorbedYellowSoul",fw);
			Stop(p_this);
			return GREEN_SOUL_MES_INIT;
		}
	}


	
	
	//Stop(p_this);
	

	//while (s_this->msg_s.script_buffer[n].pPix != NULL) {
	//	//１文字分のビットマップをフレームバッファに書き込む
	//	DrawChar(&s_this->msg_s.script_buffer[n],xpos + 10, 30);
	//	xpos += s_this->msg_s.script_buffer[n].size.X;	//次の文字の横方向位置をセット
	//	n++;	//次の文字
	//}



	return YELLOW_SOUL_MES;

}
void Stop(PS_Mes_Yellow p_this)
{

	if (rr != NULL)fclose(rr); rr = NULL;
	
	if (fw != NULL)fclose(fw); fw = NULL;
	
	ResetStringLog();//こいつがないと次のシーンでもスクリプトが引き継がれてしまう。
	script_row = 0;
	SAFE_FREE(p_this);


}