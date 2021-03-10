#pragma once
#include "YSConsoleGameEngine.h"


typedef struct
{

	//メンバ変数は以下に定義する



	//出力用ハンドル
	HANDLE m_hConsole;
	//入力用ハンドル
	HANDLE m_hConsoleIn;


	//ウィンドウサイズ、位置
	SMALL_RECT m_rectWindow;

	//スクリーンバッファ(エスケープシーケンスに関かんしては次の資料を参考 https://en.wikipedia.org/wiki/ANSI_escape_code)
	unsigned char* m_bufScreen;





}ConsoleManager, *PConsoleManager;


//メンバ関数
BOOL Init(PConsoleManager const p_this, int width, int height, int fontw, int fonth);
int ScreenWidth();
int ScreenHeight();
int WindowWidth();
int WindowHeight();
int DesktopWidth();
int DesktopHeight();
void FixWin(void);	//ウィンドウサイズを固定する
void MoveWin(int x, int y);
void DeleteWinMenu(HWND hcon);
void ClearFrameBuffer(PConsoleManager const p_this);
//void PrintFrameBuffer(PConsoleManager const p_this);
HWND Get_HWND();
void PrintImage(PConsoleManager const p_this);
void StopConsoleManager();
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
HWND GetWindowHandle(const DWORD TargetID,HWND hwnd);


