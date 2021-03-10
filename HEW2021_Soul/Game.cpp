#include "Game.h"


//private関数のプロトタイプ宣言
static void gameThread(PGamePlay const p_this);
static void setCaption(PGamePlay const p_this);



static PGamePlay pgame = NULL;

//RGBQUAD userColorPaletts[NUM_PALETTE] =
//{
//	//{BLUE,GREEN,RED,RESERVE}
//	  {0  ,0  ,0  ,0  },	//BLACK
//	  {218,55 ,0  ,0  },	//DARK BLUE
//	  {14	,161,19	,0	},	//DARK GREEN
//	  {221,150,58	,0	},	//DARK CYAN
//	  {31	,15	,197,0	},	//DARK RED
//	  {152,23	,136,0	},	//DARK MAGENTA	
//	  {0	,156,193,0	},	//DARK YELLOW
//	  {204,204,204,0	},	//GREY
//	  {118,118,118,0	},	//DARK GREY
//	  {225,120,59	,0	},	//BLUE
//	  {22	,198,22	,0	},	//GREEN
//	  {214,214,97	,0	},	//CYAN
//	  {86	,72	,231,0	},	//RED
//	  {158,0	,180,0	},	//MAGENTA
//	  {165,241,249,0	},	//YELLOW
//	  {242,242,242,0	},	//WHITE
//
//};

/*
*@brief Gameで使用する変数、及び関数の初期化を行う。
*
*@param p_this  mainで生成したGamePlayインスタンスのアドレス
*/
BOOL CallInitialize(PGamePlay const p_this, int width, int height, int fontw, int fonth)
{
	pgame = p_this;

	Init(&pgame->m_consoleManager, width, height, fontw, fonth);//コンソールマネージャーの初期化
	Init(&pgame->m_input);//インプットの初期化
	Init(&pgame->m_gameTimer);//gameTimerの初期化
	//変数初期化


	
	return TRUE;
}

//
void Start(PGamePlay const p_this)
{
//#ifdef MULTI_THREAD
//	#pragma omp parallel//並行処理を有効にする(未完成)
//	{
//		gameThread(p_this);
//	}
//#else
	
	

	//ゲームループ
		gameThread(p_this);
//#endif // MULTI_THREAD

	return;
	
}

void gameThread(PGamePlay const p_this)
{
	pgame = p_this;
	

	GAMESTATE m_gstate;
	m_gstate = SYSTEM_INIT;
	//m_gstate = GREEN_INIT;
	//m_gstate = BLUE_INIT;
	//m_gstate = RED_INIT;

	void(*SoulScene[])(GAMESTATE*)
	{
		System_Init,
		Blue,
		Red,
		Green,
		Yellow,
		//App_Exit
	};
	
	//CMのスクリーンバッファを取得
	GetScreenBuffer(&pgame->m_consoleManager);

	while (m_gstate != APP_EXIT)//m_bAtomActive
	{
		//時間計測
		StartTimer(&pgame->m_gameTimer);
		//入力
		UpdateInputStates(&pgame->m_input);

		//入力用フレームバッファクリア
		ClearFrameBuffer(&pgame->m_consoleManager);

		

		//ゲームシーンのアップデート
		switch (m_gstate)
		{
			case SYSTEM_INIT:SoulScene[SYSTEM_INIT](&m_gstate); break;
			case BLUE_INIT:
			case BLUE:		SoulScene[BLUE_INIT](&m_gstate); break;
			case RED_INIT:
			case RED:		SoulScene[RED_INIT](&m_gstate); break;
			case GREEN_INIT:
			case GREEN:		SoulScene[GREEN_INIT](&m_gstate); break;
			case YELLOW_INIT:
			case YELLOW:	SoulScene[YELLOW_INIT](&m_gstate); break;

			default: m_gstate = APP_EXIT; break;
		}
		//描画

		
		
		//処理にかかった時間を計測&固定フレームの時間調節
		

		PrintImage(&pgame->m_consoleManager);

		FrameCalculation(&pgame->m_gameTimer);

		setCaption(pgame);

	}
	

	
}
void StopGame()
{
	StopConsoleManager();
	ReleaseBuffer();
	StopInput();

}

void setCaption(PGamePlay const p_this)
{
	PGamePlay pgame = p_this;
	char Title[256];
	snprintf(Title, 256, "%3.2f", GetFPS(&pgame->m_gameTimer));
	SetConsoleTitle(Title);
}
