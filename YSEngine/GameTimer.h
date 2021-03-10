#pragma once
#include "YSConsoleGameEngine.h"
//#define MULTI_THREAD //�}���`�X���b�h�����̗L����

typedef struct
{
#ifdef MULTI_THREAD
	double	t1; //�}���`�X���b�h�p�t���[���J�n����
	double	t2; //�}���`�X���b�h�p�t���[���I������
#else
	DWORD	t1;//�t���[���J�n����
	DWORD	t2;//�t���[���I������
#endif
	float	m_fElapsedTime;	//�t���[���̌o�ߎ���
	//DWORD	m_fElapsedTime;	//�t���[���̌o�ߎ���
	int	m_frameCount;//0�`2



}GameTimer, *PGameTimer;

BOOL Init(PGameTimer const p_this);
void StartTimer(PGameTimer const p_this);
void FrameCalculation(PGameTimer const p_this);
float GetDeltaTime();
float GetFPS(PGameTimer const p_this);