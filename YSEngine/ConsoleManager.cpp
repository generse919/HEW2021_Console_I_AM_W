#include "ConsoleManager.h"
// ���[�N�G���A�̈�̎擾
#define GetMonitorRect(rc)  SystemParametersInfo(SPI_GETWORKAREA,0,rc,0)


CONSOLE_SCREEN_BUFFER_INFOEX csbi = { sizeof(CONSOLE_SCREEN_BUFFER_INFOEX) };
CONSOLE_FONT_INFOEX cfi = { sizeof(CONSOLE_FONT_INFOEX) };

PConsoleManager pcm = NULL;
//�R���\�[���}�l�[�W���[�ϐ�(private)
//static ConsoleManager cm;
static int	m_nScreenWidth;//�X�N���[���T�C�Y��
static int	m_nScreenHeight;//�X�N���[���T�C�Y�c
static int  m_nWindowWidth;
static int  m_nWindowHeight;
static int  m_nDesktopWidth;//�f�X�N�g�b�v�T�C�Y��
static int  m_nDesktopHeight;//�f�X�N�g�b�v�T�C�Y��
//�G���[�p�n���h��
HANDLE m_hOriginalConsole;

//WindowsAPI�p�n���h��
static HWND hCon;

//RGB�l�i�����R�����j"�O�O�O"�`"�Q�T�T"�̃e�[�u��()
static char CharRGBconvTBL[3][256] = {
	{'0','0','0'},
};
//�t���J���[�p(�G�X�P�[�v�V�[�P���X�Ɋւ��񂵂Ă͎��̎������Q�l) https://en.wikipedia.org/wiki/ANSI_escape_code
static char* screen_buffer_full = NULL;

/*���[�U�[���w�肷�邱�Ƃ̂ł���p���b�g�z��*/


//private�֐�
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

	//�R���\�[���̃T�C�Y���ŏ��ɏ�����
	pcm->m_rectWindow = { 0,0,1,1 };
	SetConsoleWindowInfo(pcm->m_hConsole, TRUE, &pcm->m_rectWindow);

	// �X�N���[���o�b�t�@���Z�b�g����
	COORD coord = { (short)m_nScreenWidth, (short)m_nScreenHeight };
	if (!SetConsoleScreenBufferSize(pcm->m_hConsole, coord))
		Error("SetConsoleScreenBufferSize");

	// �X�N���[���o�b�t�@���R���\�[���Ɋ��蓖�Ă�ꂽ���߁A�t�H���g�T�C�Y���Z�b�g

	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = fontw;
	cfi.dwFontSize.Y = fonth;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	wcscpy_s(cfi.FaceName, L"Consolas");
	if (!SetCurrentConsoleFontEx(pcm->m_hConsole, FALSE, &cfi))
		return Error("SetCurrentConsoleFontEx");


	//screen buffer info���擾���A�\�ȍő�E�B���h�E�T�C�Y���`�F�b�N�B
	//�����Ă����ꍇ�A�\�ő�T�C�Y�ɕύX����B

	if (!GetConsoleScreenBufferInfoEx(pcm->m_hConsole, &csbi))
		return Error("GetConsoleScreenBufferInfo");
	if (m_nScreenHeight > csbi.dwMaximumWindowSize.Y)
		m_nScreenHeight = csbi.dwMaximumWindowSize.Y;

	if (m_nScreenWidth > csbi.dwMaximumWindowSize.X)
		m_nScreenWidth = csbi.dwMaximumWindowSize.X;


	// �R���\�[���̃T�C�Y���Z�b�g
	pcm->m_rectWindow = { 0, 0, (short)m_nScreenWidth - 1, (short)m_nScreenHeight - 1 };
	if (!SetConsoleWindowInfo(pcm->m_hConsole, TRUE, &pcm->m_rectWindow))
		return Error("SetConsoleWindowInfo");

	//�o�̓E�C���h�E�̃��[�h�Z�b�g(���ꂪ�Ȃ���exe�ŃG�X�P�[�v�V�[�P���X���K�p����Ȃ�)
	if (!SetConsoleMode(pcm->m_hConsole, ENABLE_PROCESSED_OUTPUT | ENABLE_LVB_GRID_WORLDWIDE | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
		return Error("SetConsoleMode");

	// �}�E�X�̃C���v�b�g�t���O���Z�b�g
	if (!SetConsoleMode(pcm->m_hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
		return Error("SetConsoleMode");

	//�R���\�[���E�B���h�E�����n���h���ɓn��
	hCon = GetConsoleWindow();

	//�f�X�N�g�b�v�̃T�C�Y��������
	INT nCorner = 7;
	RECT    rc1;        // �f�X�N�g�b�v�̈�
	RECT    rc2;        // �E�C���h�E�̈�

	// �T�C�Y�̎擾
	GetMonitorRect(&rc1);                            // �f�X�N�g�b�v�̃T�C�Y
	GetWindowRect(hCon, &rc2);                       // �E�C���h�E�̃T�C�Y


	m_nWindowWidth = rc2.right - rc2.left;		//�E�B���h�E�̉��T�C�Y
	m_nWindowHeight = rc2.bottom - rc2.top;		//�E�B���h�E�̏c�T�C�Y


	m_nDesktopWidth = rc1.right - rc1.left;        // �I�_�ʒu�̉�����
	m_nDesktopHeight = rc1.bottom - rc1.top;      // �I�_�ʒu�̏c����

	// �X�N���[���o�b�t�@(input)�p�̃����������蓖�Ă�
	pcm->m_bufScreen = (PBYTE)calloc(3, m_nScreenWidth*m_nScreenHeight);
	/*memset(pcm->m_bufScreen, 0, sizeof(unsigned char) * m_nScreenWidth * m_nScreenHeight * 3);*/


	// �X�N���[���o�b�t�@(�t���J���[output�p)�𐶐�
	screen_buffer_full = (char *)malloc(sizeof(char) * (((m_nScreenWidth * 20) * m_nScreenHeight) + m_nScreenHeight));

	// �t���J���[�p�ɃG�X�P�[�v�V�[�P���X�������ݒ�
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
	out_buf--;//�I�[�͉��s�ł͂Ȃ��߂��ă[�����߂���(�����̂����ŃG���[�n���ɗ��Ƃ��ꂽ(�{))
	*out_buf = '\0';


	//�e�[�u���z���000~255�̕�����ɏ���������
	//Unicode�܂���JIS�̏ꍇ�A�����̕������48�Ԗڈȍ~�ɂȂ邽�߁A0x30�𑫂�
	for (int i = 0; i < 256; i++) {
		CharRGBconvTBL[0][i] = 0x30 + (i / 100);//�S�̈� 0~2
		CharRGBconvTBL[1][i] = 0x30 + ((i % 100) / 10);//�\�̈� 0~9(200�𒴂����5�܂�)
		CharRGBconvTBL[2][i] = 0x30 + (i % 10);//��̈� 0~9(250�𒴂����5�܂�)
	}
	

	//�E�B���h�E���Œ肷��
	FixWin();

	ShowWindow(Get_HWND(), SW_SHOW);
	MoveWin(0, 0);
	//�Ńo�b�O
	//_RPTN(_CRT_WARN, "%d%d", m_nScreenWidth, m_nScreenHeight);

	return TRUE;
}
/*
*@brief		�����������X�N���[���T�C�Y(��)��Ԃ�
*/
int ScreenWidth()
{
	return m_nScreenWidth;
}
/*
*@brief		�����������X�N���[���T�C�Y(�c)��Ԃ�
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
//�R���\�[���}�l�[�W���[�����擾
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
//�R���\�[���}�l�[�W���[�����Z�b�g
static BOOL SetConsoleManager(PConsoleManager const p_this)
{
	//*p_this = cm;
}

//�`��p�o�b�t�@���[���N���A����
void ClearFrameBuffer(PConsoleManager const p_this)
{
	ZeroMemory(p_this->m_bufScreen, sizeof(unsigned char) * m_nScreenWidth * m_nScreenHeight * 3);
}

//void PrintFrameBuffer(PConsoleManager const p_this)
//{
//	WriteConsoleOutput(p_this->m_hConsole, p_this->m_bufScreen, { (short)m_nScreenWidth, (short)m_nScreenHeight }, { 0,0 }, &p_this->m_rectWindow);
//}
/**
	 * @brief	�摜�̏o��
	 *
	 * @param	buf [����] RGB�摜�f�[�^�z��̃|�C���^
	 * @note
	 *	RGB�摜�f�[�^�z��̓X�N���[���̉����~�c���̃o�C�g���ȏ�̔z��Ƃ��A
	 *	�z��̒��g��RGB�e1�o�C�g(���v3�o�C�g)��1��f�Ƃ����f�[�^�ɂ���B
	 *	�S�Ẳ�f�͘A�����Ă���K�v����B
	 *	��)��80�����~�c25�s�̏ꍇ�A80x25=200�o�C�g�ȏ�̔z���n��
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
				// R�ݒ�
				*(out_buf + 0) = CharRGBconvTBL[0][*in_buf];	//0x30 + (*in_buf / 100);
				*(out_buf + 1) = CharRGBconvTBL[1][*in_buf];	//0x30 + (*in_buf % 100 / 10);
				*(out_buf + 2) = CharRGBconvTBL[2][*in_buf];	//0x30 + (*in_buf % 10);
				in_buf++;
				// G�ݒ�
				*(out_buf + 4) = CharRGBconvTBL[0][*in_buf];	//0x30 + (*in_buf / 100);
				*(out_buf + 5) = CharRGBconvTBL[1][*in_buf];	//0x30 + (*in_buf % 100 / 10);
				*(out_buf + 6) = CharRGBconvTBL[2][*in_buf];	//0x30 + (*in_buf % 10);
				in_buf++;
				// B�ݒ�
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
			// R�ݒ�
			*(out_buf + 0) = CharRGBconvTBL[0][*in_buf];	//0x30 + (*in_buf / 100);
			*(out_buf + 1) = CharRGBconvTBL[1][*in_buf];	//0x30 + (*in_buf % 100 / 10);
			*(out_buf + 2) = CharRGBconvTBL[2][*in_buf];	//0x30 + (*in_buf % 10);
			in_buf++;
			// G�ݒ�
			*(out_buf + 4) = CharRGBconvTBL[0][*in_buf];	//0x30 + (*in_buf / 100);
			*(out_buf + 5) = CharRGBconvTBL[1][*in_buf];	//0x30 + (*in_buf % 100 / 10);
			*(out_buf + 6) = CharRGBconvTBL[2][*in_buf];	//0x30 + (*in_buf % 10);
			in_buf++;
			// B�ݒ�
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
*@brief		����������HWND���擾����
*
*@return	����������HWND
*/
HWND Get_HWND()
{
	return hCon;
}




//�E�B���h�E�T�C�Y���Œ肷��
void FixWin(void)
{
	//�E�B���h�E�T�C�Y�ύX�֎~
	HWND hCon = GetConsoleWindow();
	LONG lStyle = GetWindowLong(hCon, GWL_STYLE);
	lStyle &= ~(WS_THICKFRAME | WS_HSCROLL | WS_VSCROLL);	//'~'�r�b�g���Ƃ̔��](�P�̕␔)
	lStyle = SetWindowLong(hCon, GWL_STYLE, lStyle);
	//SetWindowPos(hCon, NULL, 0, 0, frmb.width + 20, frmb.height, SWP_NOSIZE | SWP_NOZORDER);
}
void MoveWin(int x, int y)
{
	SetWindowPos(hCon, NULL, x, y, 0, 0, (SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER));
}
/**
	 * @brief	�E�B���h�E���j���[���폜����(�����ꂽ���Ȃ��E�B���h�E�Ɏg�p����)
	 *
	 * @param	hcon	���̃E�B���h�E�̃n���h��
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
// �E�B���h�E�n���h�����擾���A�v���P�[�V�������I��������B
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	// CreateProcess()�Ŏ擾����PROCESS_INFORMATION�\���̂̃|�C���^���擾
	PROCESS_INFORMATION* pi = (PROCESS_INFORMATION*)lParam;

	// �E�C���h�E���쐬�����v���Z�XID���擾�B
	DWORD lpdwProcessId = 0;
	GetWindowThreadProcessId(hWnd, &lpdwProcessId);

	// CreateProcess�ŋN�������A�v���̃v���Z�XID�ƃ��C���E�B���h�E��
	// �쐬�����v���Z�XID�������ꍇ�A�N�������A�v�����I��������B
	if (pi->dwProcessId == lpdwProcessId)
	{
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		return FALSE;
	}
	return TRUE;
}

//�v���Z�Xid����HWND���擾����
/*
*@brief		����������HWND���擾����
*
*@param		TargetID �ړI�̃v���Z�XID
*
*@param		hwnd �i�[�������n���h���|�C���^(NULL�w��\)
*
*@return	���� �]�݂�HWND  / ���s NULL
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

