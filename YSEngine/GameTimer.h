#pragma once
#include "YSConsoleGameEngine.h"
//#define MULTI_THREAD //マルチスレッド処理の有効化

typedef struct
{
#ifdef MULTI_THREAD
	double	t1; //マルチスレッド用フレーム開始時間
	double	t2; //マルチスレッド用フレーム終了時間
#else
	DWORD	t1;//フレーム開始時間
	DWORD	t2;//フレーム終了時間
#endif
	float	m_fElapsedTime;	//フレームの経過時間
	//DWORD	m_fElapsedTime;	//フレームの経過時間
	int	m_frameCount;//0～2



}GameTimer, *PGameTimer;

BOOL Init(PGameTimer const p_this);
void StartTimer(PGameTimer const p_this);
void FrameCalculation(PGameTimer const p_this);
float GetDeltaTime();
float GetFPS(PGameTimer const p_this);