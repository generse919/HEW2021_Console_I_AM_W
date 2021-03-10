#pragma once
#include "YSConsoleGameEngine.h"


typedef struct
{

	//�����o�ϐ��͈ȉ��ɒ�`����



	//�o�͗p�n���h��
	HANDLE m_hConsole;
	//���͗p�n���h��
	HANDLE m_hConsoleIn;


	//�E�B���h�E�T�C�Y�A�ʒu
	SMALL_RECT m_rectWindow;

	//�X�N���[���o�b�t�@(�G�X�P�[�v�V�[�P���X�Ɋւ��񂵂Ă͎��̎������Q�l https://en.wikipedia.org/wiki/ANSI_escape_code)
	unsigned char* m_bufScreen;





}ConsoleManager, *PConsoleManager;


//�����o�֐�
BOOL Init(PConsoleManager const p_this, int width, int height, int fontw, int fonth);
int ScreenWidth();
int ScreenHeight();
int WindowWidth();
int WindowHeight();
int DesktopWidth();
int DesktopHeight();
void FixWin(void);	//�E�B���h�E�T�C�Y���Œ肷��
void MoveWin(int x, int y);
void DeleteWinMenu(HWND hcon);
void ClearFrameBuffer(PConsoleManager const p_this);
//void PrintFrameBuffer(PConsoleManager const p_this);
HWND Get_HWND();
void PrintImage(PConsoleManager const p_this);
void StopConsoleManager();
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
HWND GetWindowHandle(const DWORD TargetID,HWND hwnd);


