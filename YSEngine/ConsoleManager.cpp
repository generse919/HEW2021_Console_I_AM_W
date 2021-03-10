#include "ConsoleManager.h"
// ワークエリア領域の取得
#define GetMonitorRect(rc)  SystemParametersInfo(SPI_GETWORKAREA,0,rc,0)


CONSOLE_SCREEN_BUFFER_INFOEX csbi = { sizeof(CONSOLE_SCREEN_BUFFER_INFOEX) };
CONSOLE_FONT_INFOEX cfi = { sizeof(CONSOLE_FONT_INFOEX) };

PConsoleManager pcm = NULL;
//コンソールマネージャー変数(private)
//static ConsoleManager cm;
static int	m_nScreenWidth;//スクリーンサイズ横
static int	m_nScreenHeight;//スクリーンサイズ縦
static int  m_nWindowWidth;
static int  m_nWindowHeight;
static int  m_nDesktopWidth;//デスクトップサイズ横
static int  m_nDesktopHeight;//デスクトップサイズ横
//エラー用ハンドル
HANDLE m_hOriginalConsole;

//WindowsAPI用ハンドル
static HWND hCon;

//RGB値（数字３文字）"０００"～"２５５"のテーブル()
static char CharRGBconvTBL[3][256] = {
	{'0','0','0'},
};
//フルカラー用(エスケープシーケンスに関かんしては次の資料を参考) https://en.wikipedia.org/wiki/ANSI_escape_code
static char* screen_buffer_full = NULL;

/*ユーザーが指定することのできるパレット配列*/


//private関数
static BOOL Error(const char *msg);

//static BOOL GetConsoleManager(PConsoleManager const p_this);
//static BOOL SetConsoleManager(void);


BOOL Init(PConsoleManager const p_this, int width, int height, int fontw, int fonth)
{
	pcm = p_this;
	char* out_buf;
	pcm->m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	pcm->m_hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);


	if (pcm->m_hConsole == INVALID_HANDLE_VALUE)
		return Error("Bad Handle");

	m_nScreenWidth = width;
	m_nScreenHeight = height;

	//コンソールのサイズを最小に初期化
	pcm->m_rectWindow = { 0,0,1,1 };
	SetConsoleWindowInfo(pcm->m_hConsole, TRUE, &pcm->m_rectWindow);

	// スクリーンバッファをセットする
	COORD coord = { (short)m_nScreenWidth, (short)m_nScreenHeight };
	if (!SetConsoleScreenBufferSize(pcm->m_hConsole, coord))
		Error("SetConsoleScreenBufferSize");

	// スクリーンバッファがコンソールに割り当てられたため、フォントサイズをセット

	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = fontw;
	cfi.dwFontSize.Y = fonth;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	wcscpy_s(cfi.FaceName, L"Consolas");
	if (!SetCurrentConsoleFontEx(pcm->m_hConsole, FALSE, &cfi))
		return Error("SetCurrentConsoleFontEx");


	//screen buffer infoを取得し、可能な最大ウィンドウサイズをチェック。
	//超えていた場合、可能最大サイズに変更する。

	if (!GetConsoleScreenBufferInfoEx(pcm->m_hConsole, &csbi))
		return Error("GetConsoleScreenBufferInfo");
	if (m_nScreenHeight > csbi.dwMaximumWindowSize.Y)
		m_nScreenHeight = csbi.dwMaximumWindowSize.Y;

	if (m_nScreenWidth > csbi.dwMaximumWindowSize.X)
		m_nScreenWidth = csbi.dwMaximumWindowSize.X;


	// コンソールのサイズをセット
	pcm->m_rectWindow = { 0, 0, (short)m_nScreenWidth - 1, (short)m_nScreenHeight - 1 };
	if (!SetConsoleWindowInfo(pcm->m_hConsole, TRUE, &pcm->m_rectWindow))
		return Error("SetConsoleWindowInfo");

	//出力ウインドウのモードセット(これがないとexeでエスケープシーケンスが適用されない)
	if (!SetConsoleMode(pcm->m_hConsole, ENABLE_PROCESSED_OUTPUT | ENABLE_LVB_GRID_WORLDWIDE | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
		return Error("SetConsoleMode");

	// マウスのインプットフラグをセット
	if (!SetConsoleMode(pcm->m_hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
		return Error("SetConsoleMode");

	//コンソールウィンドウ情報をハンドルに渡す
	hCon = GetConsoleWindow();

	//デスクトップのサイズを初期化
	INT nCorner = 7;
	RECT    rc1;        // デスクトップ領域
	RECT    rc2;        // ウインドウ領域

	// サイズの取得
	GetMonitorRect(&rc1);                            // デスクトップのサイズ
	GetWindowRect(hCon, &rc2);                       // ウインドウのサイズ


	m_nWindowWidth = rc2.right - rc2.left;		//ウィンドウの横サイズ
	m_nWindowHeight = rc2.bottom - rc2.top;		//ウィンドウの縦サイズ


	m_nDesktopWidth = rc1.right - rc1.left;        // 終点位置の横方向
	m_nDesktopHeight = rc1.bottom - rc1.top;      // 終点位置の縦方向

	// スクリーンバッファ(input)用のメモリを割り当てる
	pcm->m_bufScreen = (PBYTE)calloc(3, m_nScreenWidth*m_nScreenHeight);
	/*memset(pcm->m_bufScreen, 0, sizeof(unsigned char) * m_nScreenWidth * m_nScreenHeight * 3);*/


	// スクリーンバッファ(フルカラーoutput用)を生成
	screen_buffer_full = (char *)malloc(sizeof(char) * (((m_nScreenWidth * 20) * m_nScreenHeight) + m_nScreenHeight));

	// フルカラー用にエスケープシーケンス文字列を設定
	out_buf = screen_buffer_full;
	for (int y = 0; y < m_nScreenHeight; y++)
	{
		for (int x = 0; x < m_nScreenWidth; x++)
		{
			sprintf_s(out_buf, 21, "\033[48;2;000;000;000m ");
			out_buf += 20;
		}
		*out_buf = '\n';
		out_buf++;
	}
	out_buf--;//終端は改行ではなく戻ってゼロ埋めする(ここのせいでエラー地獄に落とされた(怒))
	*out_buf = '\0';


	//テーブル配列を000~255の文字列に初期化する
	//UnicodeまたはJISの場合、数字の文字列は48番目以降になるため、0x30を足す
	for (int i = 0; i < 256; i++) {
		CharRGBconvTBL[0][i] = 0x30 + (i / 100);//百の位 0~2
		CharRGBconvTBL[1][i] = 0x30 + ((i % 100) / 10);//十の位 0~9(200を超えると5まで)
		CharRGBconvTBL[2][i] = 0x30 + (i % 10);//一の位 0~9(250を超えると5まで)
	}
	

	//ウィンドウを固定する
	FixWin();

	ShowWindow(Get_HWND(), SW_SHOW);
	MoveWin(0, 0);
	//でバッグ
	//_RPTN(_CRT_WARN, "%d%d", m_nScreenWidth, m_nScreenHeight);

	return TRUE;
}
/*
*@brief		初期化したスクリーンサイズ(横)を返す
*/
int ScreenWidth()
{
	return m_nScreenWidth;
}
/*
*@brief		初期化したスクリーンサイズ(縦)を返す
*/
int ScreenHeight()
{
	return m_nScreenHeight;
}

int WindowWidth()
{
	return m_nWindowWidth;
}

int WindowHeight()
{
	return m_nWindowHeight;
}

int DesktopWidth()
{
	return m_nDesktopWidth;
}
int DesktopHeight()
{
	return m_nDesktopHeight;
}
//コンソールマネージャー情報を取得
static BOOL GetConsoleManager(PConsoleManager const p_this)
{
	/*cm = *p_this;
	if (&cm == NULL)
	{
		Error(p_this, "Could not Get Console Manager\n");
		return FALSE;
	}

	return TRUE;*/
}
//コンソールマネージャー情報をセット
static BOOL SetConsoleManager(PConsoleManager const p_this)
{
	//*p_this = cm;
}

//描画用バッファをゼロクリアする
void ClearFrameBuffer(PConsoleManager const p_this)
{
	ZeroMemory(p_this->m_bufScreen, sizeof(unsigned char) * m_nScreenWidth * m_nScreenHeight * 3);
}

//void PrintFrameBuffer(PConsoleManager const p_this)
//{
//	WriteConsoleOutput(p_this->m_hConsole, p_this->m_bufScreen, { (short)m_nScreenWidth, (short)m_nScreenHeight }, { 0,0 }, &p_this->m_rectWindow);
//}
/**
	 * @brief	画像の出力
	 *
	 * @param	buf [入力] RGB画像データ配列のポインタ
	 * @note
	 *	RGB画像データ配列はスクリーンの横幅×縦幅のバイト数以上の配列とし、
	 *	配列の中身はRGB各1バイト(合計3バイト)を1画素としたデータにする。
	 *	全ての画素は連続している必要あり。
	 *	例)横80文字×縦25行の場合、80x25=200バイト以上の配列を渡す
	 */
void PrintImage(PConsoleManager const p_this)
{

	DWORD write_num;
	unsigned char* in_buf = p_this->m_bufScreen;
	//memcpy(in_buf, (PBYTE)p_this->m_bufScreen, sizeof(BYTE) * ScreenWidth() * ScreenHeight());
	char *out_buf = screen_buffer_full;
#ifdef MT



	for (int y = 0; y < m_nScreenHeight; y++)
	{
		int x;
#pragma omp parallel for schedule(static) ordered
		for (x = 0; x < m_nScreenWidth; x++)
		{
#pragma omp ordered
			{
				out_buf += 7;
				// R設定
				*(out_buf + 0) = CharRGBconvTBL[0][*in_buf];	//0x30 + (*in_buf / 100);
				*(out_buf + 1) = CharRGBconvTBL[1][*in_buf];	//0x30 + (*in_buf % 100 / 10);
				*(out_buf + 2) = CharRGBconvTBL[2][*in_buf];	//0x30 + (*in_buf % 10);
				in_buf++;
				// G設定
				*(out_buf + 4) = CharRGBconvTBL[0][*in_buf];	//0x30 + (*in_buf / 100);
				*(out_buf + 5) = CharRGBconvTBL[1][*in_buf];	//0x30 + (*in_buf % 100 / 10);
				*(out_buf + 6) = CharRGBconvTBL[2][*in_buf];	//0x30 + (*in_buf % 10);
				in_buf++;
				// B設定
				*(out_buf + 8) = CharRGBconvTBL[0][*in_buf];	//0x30 + (*in_buf / 100);
				*(out_buf + 9) = CharRGBconvTBL[1][*in_buf];	//0x30 + (*in_buf % 100 / 10);
				*(out_buf + 10) = CharRGBconvTBL[2][*in_buf];	//0x30 + (*in_buf % 10);
				in_buf++;
				out_buf += 13;
			}

		}
		out_buf++;

	}






#else
	for (int y = 0; y < m_nScreenHeight; y++)
	{

		for (int x = 0; x < m_nScreenWidth; x++)
		{
			out_buf += 7;
			// R設定
			*(out_buf + 0) = CharRGBconvTBL[0][*in_buf];	//0x30 + (*in_buf / 100);
			*(out_buf + 1) = CharRGBconvTBL[1][*in_buf];	//0x30 + (*in_buf % 100 / 10);
			*(out_buf + 2) = CharRGBconvTBL[2][*in_buf];	//0x30 + (*in_buf % 10);
			in_buf++;
			// G設定
			*(out_buf + 4) = CharRGBconvTBL[0][*in_buf];	//0x30 + (*in_buf / 100);
			*(out_buf + 5) = CharRGBconvTBL[1][*in_buf];	//0x30 + (*in_buf % 100 / 10);
			*(out_buf + 6) = CharRGBconvTBL[2][*in_buf];	//0x30 + (*in_buf % 10);
			in_buf++;
			// B設定
			*(out_buf + 8) = CharRGBconvTBL[0][*in_buf];	//0x30 + (*in_buf / 100);
			*(out_buf + 9) = CharRGBconvTBL[1][*in_buf];	//0x30 + (*in_buf % 100 / 10);
			*(out_buf + 10) = CharRGBconvTBL[2][*in_buf];	//0x30 + (*in_buf % 10);
			in_buf++;
			out_buf += 13;
	}
		out_buf++;

}

#endif





	DWORD t0 = timeGetTime();

#ifdef MT

#pragma omp parallel
	{

		WriteConsole(p_this->m_hConsole, screen_buffer_full, m_nScreenHeight * (m_nScreenWidth * 20) + (m_nScreenHeight - 1), &write_num, NULL);
	}

#else
	WriteConsole(p_this->m_hConsole, screen_buffer_full, m_nScreenHeight * (m_nScreenWidth * 20) + (m_nScreenHeight - 1), &write_num, NULL);
#endif
	//WriteConsole(p_this->m_hConsole, screen_buffer_full, m_nScreenHeight * (m_nScreenWidth * 20) + (m_nScreenHeight - 1), &write_num, NULL);

	DWORD t1 = timeGetTime() - t0;
	t0 = t1;



	DWORD t3 = t0 - t1;

	int i = 0;



}

/*
*@brief		初期化したHWNDを取得する
*
*@return	初期化したHWND
*/
HWND Get_HWND()
{
	return hCon;
}




//ウィンドウサイズを固定する
void FixWin(void)
{
	//ウィンドウサイズ変更禁止
	HWND hCon = GetConsoleWindow();
	LONG lStyle = GetWindowLong(hCon, GWL_STYLE);
	lStyle &= ~(WS_THICKFRAME | WS_HSCROLL | WS_VSCROLL);	//'~'ビットごとの反転(１の補数)
	lStyle = SetWindowLong(hCon, GWL_STYLE, lStyle);
	//SetWindowPos(hCon, NULL, 0, 0, frmb.width + 20, frmb.height, SWP_NOSIZE | SWP_NOZORDER);
}
void MoveWin(int x, int y)
{
	SetWindowPos(hCon, NULL, x, y, 0, 0, (SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER));
}
/**
	 * @brief	ウィンドウメニューを削除する(消されたくないウィンドウに使用する)
	 *
	 * @param	hcon	このウィンドウのハンドル
	 */
void DeleteWinMenu(HWND hcon)
{
	DWORD lWndStyle = GetWindowLong(hcon, GWL_STYLE);
	lWndStyle &= ~WS_SYSMENU;
	SetWindowLong(hcon, GWL_STYLE, lWndStyle);
}


static BOOL Error(const char *msg)
{
	char buf[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
	SetConsoleActiveScreenBuffer(m_hOriginalConsole);
	wprintf(L"ERROR: %s\n\t%s\n", msg, buf);
	return FALSE;
}

void StopConsoleManager()
{
	SetConsoleActiveScreenBuffer(m_hOriginalConsole);
	//SAFE_FREE(screen_buffer_full);
	/*if (screen_buffer_full != NULL)
	{
		free(screen_buffer_full);
		screen_buffer_full = NULL;
	}*/

	SAFE_FREE(pcm->m_bufScreen);
	pcm = NULL;
}

int fff = 0;
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

//プロセスidからHWNDを取得する
/*
*@brief		初期化したHWNDを取得する
*
*@param		TargetID 目的のプロセスID
*
*@param		hwnd 格納したいハンドルポインタ(NULL指定可能)
*
*@return	成功 望みのHWND  / 失敗 NULL
*/
HWND GetWindowHandle(const DWORD TargetID,HWND hwnd)
{
	HWND hWnd = GetTopWindow(NULL);
	do {
		if (GetWindowLong(hWnd, GWL_HWNDPARENT) != 0)
			continue;
		DWORD ProcessID;
		GetWindowThreadProcessId(hWnd, &ProcessID);
		if (TargetID == ProcessID)
		{
			if(hwnd != NULL)hwnd = hWnd;
			
			return hWnd;
		}
			
	} while ((hWnd = GetNextWindow(hWnd, GW_HWNDNEXT)) != NULL);

	return NULL;
}

