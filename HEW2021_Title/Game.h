#pragma once
#include "../YSEngine/YSConsoleGameEngine.h"
#include "Title.h";

#define TITLE_SIZE 256;

/*Gameクラスの定義
 *
 *
 *
 *publicメンバ関数は.hで記述
 *
 *
 */
typedef struct
{


	ConsoleManager m_consoleManager;

	Input m_input;
	//メンバ変数は以下に定義する
	GameTimer m_gameTimer;
	int		m_gameState;//ゲームシーンの状態


	BOOL m_bEnableSound;

	


	

	

}GamePlay, *PGamePlay;





//以下はpublic変数
BOOL CallInitialize(PGamePlay const p_this, int width, int height, int fontw, int fonth);
void Start(PGamePlay const p_this);
void StopGame();



//private変数はcppに記述