#include "Title.h"

static PS_Title scn = NULL;

static GAMESTATE Start(PS_Title  p_this);
static GAMESTATE Update(PS_Title p_this);
static void Stop(PS_Title p_this);
void PlayerAppear();

static BOOL cleared;
static BOOL b_timer;
static float ftime;

static FILE *fp;
static FILE *fs;
static FILE *fm;
static FILE *fc;
static int rect[2];


void Title(GAMESTATE *gstate)
{
	if (*gstate == TITLE_INIT)
	{

		STARTUPINFO s;
		GetStartupInfo(&s);

		//Init(s_this.scn_title);
		scn = (PS_Title)calloc(1, sizeof(S_Title));

		//#if 0
		if (s.wShowWindow == (SW_SHOW))//exeから実行したかどうか
		{
			Create(&scn->Titlepic, TITLE_PIC_CLEAR, ID_TITLE_CLEAR);
		}
		else
		{
			Create(&scn->Titlepic, TITLE_PIC, ID_TITLE);
		}

		//#endif
#if 0
		FILE *fc;
		if (!(fc = fopen("../Release/scripts/titleflg.txt", "r")))
		{
			*gstate = APP_EXIT;
			return;
		}
		char buf[512];
		cleared = FALSE;
		while ((fscanf(fc, "%s", &buf)) != EOF)
		{
			if (!strcmp(buf, "clear"))
			{
				cleared = TRUE;
			}
		}
		if (cleared)
		{
			Create(&scn->Titlepic, TITLE_PIC_CLEAR, ID_TITLE_CLEAR);
		}
		else
		{
			Create(&scn->Titlepic, TITLE_PIC, ID_TITLE);
		}
		SAFE_CLOSE(fc);

#endif

		//スタート関数の実行
		*gstate = Start(scn);

	}
	else if (*gstate == TITLE)
	{
		*gstate = Update(scn);
	}
}

GAMESTATE Start(PS_Title  p_this)
{
	//ファイルを空にする
	if ((fp = fopen("../Release/scripts/playerlog.txt", "w")) &&
		(fs = fopen("../Release/scripts/soullog.txt", "w")) &&
		(fm = fopen("../Release/scripts/messagelog.txt", "w")) &&
		(fc = fopen("../Release/scripts/titleflg.txt", "w"))
		)
	{
		SAFE_CLOSE(fp);
		SAFE_CLOSE(fs);
		SAFE_CLOSE(fm);
		SAFE_CLOSE(fc);
	}
	else return APP_EXIT;

	rect[0] = DesktopWidth() / 2 - (WindowWidth() / 2);
	rect[1] = DesktopHeight() / 2 - (WindowHeight() / 2);

	b_timer = FALSE;
	return TITLE;
}
GAMESTATE Update(PS_Title p_this)
{
	if (ReleasedKey(PM_LEFT))
	{
		b_timer = TRUE;
		/*DWORD processid = 
		HWND hn = GetWindowHandle(processid, NULL);*/
		PROCESS_INFORMATION pp;
		pp.dwProcessId = GetWindowThreadProcessId(Get_HWND(), NULL);
		EnumWindows(EnumWindowsProc, (LPARAM)&pp);
		//if (hn = NULL)
			return APP_EXIT;
	}
	if (b_timer)ftime += GetDeltaTime();

	if (ftime >= 2.0f)
	{
		PlayerAppear();
		Stop(p_this);
		return APP_EXIT;
	}
	//MoveWin(rect[0], rect[1]);
	SetWindowPos(Get_HWND(), HWND_TOP, rect[0], rect[1], 0, 0, (SWP_NOSIZE));
	for (int y = 0; y < ScreenHeight(); y++)
		for (int x = 0; x < ScreenWidth(); x++)
		{
			Draw(x, y, GetColor(&p_this->Titlepic, x, y));
		}

	return TITLE;
}
void Stop(PS_Title p_this)
{

	Destroy(&p_this->Titlepic);
	SAFE_FREE(p_this);


}

void PlayerAppear()
{




	FILE *fc;
	if (!(fc = fopen("../Release/scripts/titleflg.txt", "w")))
	{
		return;
	}
	fputs(GetCommandLineA(), fc);
	SAFE_CLOSE(fc);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	//ファイルログの更新
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	//si.lpTitle = (LPSTR)"Player";
	CreateProcess(NULL, (LPTSTR)("exe/HEW2021_Player.exe"), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

	
}