#include "BaseScene.h"
#include <crtdbg.h>



#if 0
//シーン保存用private
PScene scn = NULL;
static int sx = 0;
static int sy = 0;
static BOOL ActiveMode;
static BOOL TextOpen;
static BOOL TextClosed;

//色の明るさ用θ角
static float ftheta;

// メッセージ用exe起動
static STARTUPINFO si;
static PROCESS_INFORMATION pi;

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


static HANDLE childProcess = NULL;

//int execute(LPTSTR commandLine)
//{
//	if (!TextOpen)
//	{
//		STARTUPINFO si = { sizeof(STARTUPINFO) };
//		PROCESS_INFORMATION pi = {};
//		if (!CreateProcess(
//			NULL,
//			commandLine,
//			NULL,	//プロセスのセキュリティー記述子
//			NULL,	//スレッドのセキュリティー記述子
//			FALSE,	//ハンドルを継承しない
//			0,	//作成フラグ
//			NULL,	//環境変数は引き継ぐ
//			NULL,	//カレントディレクトリーは同じ
//			&si,
//			&pi)
//			) {
//			//printError("CreateProcess");
//			return -1;
//		}
//		// 子プロセス起動成功
//		childProcess = pi.hProcess;
//
//		// 不要なスレッドハンドルをクローズする
//		if (!CloseHandle(pi.hThread)) {
//			//printError("CloseHandle(hThread)");
//			return -1;
//		}
//		TextOpen = TRUE;
//	}
//	
//
//	//printf("child processId=%d\n", pi.dwProcessId);
//
//	// 子プロセスの終了待ち
//	Sleep(3000);
//	DWORD r = WaitForSingleObject(childProcess, INFINITE);
//	
//	switch (r) {
//	case WAIT_FAILED:
//		//printError("wait result=WAIT_FAILED");
//		return -1;
//	case WAIT_ABANDONED:
//		//printf("wait result=WAIT_ABANDONED\n");
//		return -1;
//	case WAIT_OBJECT_0: //正常終了
//		//printf("wait result=WAIT_OBJECT_0\n");
//		ActiveMode = TRUE;
//		TextClosed = TRUE;
//		//TextClosed = TRUE;
//		break;
//	case WAIT_TIMEOUT:ActiveMode = FALSE; TextClosed = TRUE; break;
//		//printf("wait result=WAIT_TIMEOUT\n");
//		//return -1;
//	default:
//		//printf("wait result=%d\n", r);
//		return -1;
//	}
//
//	
//
//	//return exitCode;
//}

// ウィンドウハンドルを取得しアプリケーションを終了させる。
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	// CreateProcess()で取得したPROCESS_INFORMATION構造体のポインタを取得
	PROCESS_INFORMATION* pi = (PROCESS_INFORMATION*)lParam;

	// ウインドウを作成したプロセスIDを取得。
	DWORD lpdwProcessId = 0;
	GetWindowThreadProcessId(hWnd, &lpdwProcessId);

	// CreateProcessで起動したアプリのプロセスIDとメインウィンドウを
	// 作成したプロセスIDが同じ場合、起動したアプリを終了させる。
	if (pi->dwProcessId == lpdwProcessId)
	{
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		return FALSE;
	}
	return TRUE;
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

		//AfxMessageBox(_T("メモ帳を終了させます。"));

		//Sleep(3000);
		// コールバック関数の呼び出し。
		EnumWindows(EnumWindowsProc, (LPARAM)&pi);

		*isActive = FALSE;
		*isTextClosed = FALSE;
		Sleep(2000);
		

	}
	SetWindowPos(Get_HWND(), HWND_NOTOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));
	
	if (WaitForSingleObject(pi.hProcess, 1) == WAIT_OBJECT_0)
	/*DWORD exitcode;
	if (!GetExitCodeProcess(pi.hProcess, &exitcode))*/
	{
		//if (AfxMessageBox(_T("強制終了させますか？"), MB_YESNO) == IDYES)
		TerminateProcess(pi.hProcess, 0);
		*isActive		= TRUE;
		*isTextClosed	= TRUE;
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
		




	
}

GAMESTATE Title(GAMESTATE gstate)
{
	if (gstate == GAMESTATE::TITLE_INIT)
	{
		scn = (PScene)calloc(1,sizeof(Scene));
		Create(&scn->Player, PLAYER_PIC, PLAYER);

		//Create(scn->BackGround, TITLE_PIC);
		

		/*if (pin != NULL)
		{
			scn->m_inp = pin;
		}
		if (pcm != NULL)
		{
			scn->m_cm = pcm;
		}
		if (pgt != NULL)
		{
			scn->m_gt = pgt;
		}*/


		//スタート関数の実行
		return Start(scn);
		
	}
	else if(gstate == GAMESTATE::TITLE)
	{
		return Update(scn);
	}
	
	return GAMESTATE::TITLE;
	
	

	
}

//GameState Init(GameState gstate)//シーン環境準備が終わった場合
GAMESTATE Start(PScene &p_this)
{
	ActiveMode = TRUE;
	TextOpen = FALSE;
	TextClosed = FALSE;
	scn->Player.pos = { 0.0f,0.0f };
	ftheta = 0.0f;
	//scn->BackGround.pos = { 0.0f,0.0f };
	
	return TITLE;
}
GAMESTATE Update(PScene const p_this)
{
	float deltaTime = GetDeltaTime();
	
	if (ActiveMode)
	{
		if (ReleasedKey(PK_P))
		{
			Stop(scn);
			return GAMESTATE::APP_EXIT;
		}

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
	}

	
	ftheta += deltaTime;
	if (ftheta >= 2 * PI) ftheta = 0.0f;

	//scn->Player.pos.x += scn->Player->vec.x;


	/*for (int y = 0; y < scn->BackGround->h; y++)
		for (int x = 0; x < scn->BackGround->w; x++)
		{
			Draw(x + (int)scn->BackGround.pos.x, y + (int)scn->BackGround.pos.y, GetColor(scn->BackGround, x, y));
		}*/

	


	if (sx >= 40 && !TextClosed)
	{
		MessageMode(&ActiveMode, &TextClosed);

	}
	else
	{
		MoveWin(sx, sy);
	}

	

	for(int y = 0; y < ScreenHeight(); y++)
		for (int x = 0; x < ScreenWidth(); x++)
		{

			RGBQUAD Blight = { 255,245,223,0};
			RGBQUAD Dark = GetColor(&p_this->Player,x,y);

			/*if (Dark.rgbBlue != 0 || Dark.rgbGreen != 0 || Dark.rgbRed != 0 )
			{
				SetColor(&scn->Player, x, y, { (BYTE)((float)Dark.rgbBlue + ((float)(Blight.rgbBlue - Dark.rgbBlue) *0.2* absf(sinf(ftheta)))),
				(BYTE)((float)Dark.rgbGreen + ((float)(Blight.rgbGreen - Dark.rgbGreen) * 0.2*absf(sinf(ftheta)))),
				(BYTE)((float)Dark.rgbRed + ((float)(Blight.rgbRed - Dark.rgbRed) * 0.2*absf(sinf(ftheta)))) });
			}*/
			if (Dark.rgbBlue != 0 || Dark.rgbGreen != 0 || Dark.rgbRed != 0 )
			{

				Draw(x + (int)scn->Player.pos.x, y + (int)scn->Player.pos.y,
					{ (BYTE)((float)Dark.rgbBlue + ((float)(Blight.rgbBlue - Dark.rgbBlue) * absf(sinf(ftheta)))),
					(BYTE)((float)Dark.rgbGreen + ((float)(Blight.rgbGreen - Dark.rgbGreen) *absf(sinf(ftheta)))),
				(BYTE)((float)Dark.rgbRed + ((float)(Blight.rgbRed - Dark.rgbRed)*absf(sinf(ftheta)))) });
				
			}
			else
			{
				Draw(x + (int)scn->Player.pos.x, y + (int)scn->Player.pos.y, GetColor(&scn->Player, x, y));
			}
			

			
		}



	
	




	return GAMESTATE::TITLE;
}
void Stop(PScene &p_this)
{
	Destroy(&p_this->Player);
	//Destroy(p_this->BackGround);
	/*p_this->m_inp = NULL;
	p_this->m_cm = NULL;
	p_this->m_gt = NULL;*/
	SAFE_FREE(p_this);

}
#endif