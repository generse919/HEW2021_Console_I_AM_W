#pragma once
#include "../YSEngine/YSConsoleGameEngine.h"
#include "Title.h";

#define TITLE_SIZE 256;

/*Game�N���X�̒�`
 *
 *
 *
 *public�����o�֐���.h�ŋL�q
 *
 *
 */
typedef struct
{


	ConsoleManager m_consoleManager;

	Input m_input;
	//�����o�ϐ��͈ȉ��ɒ�`����
	GameTimer m_gameTimer;
	int		m_gameState;//�Q�[���V�[���̏��


	BOOL m_bEnableSound;

	


	

	

}GamePlay, *PGamePlay;





//�ȉ���public�ϐ�
BOOL CallInitialize(PGamePlay const p_this, int width, int height, int fontw, int fonth);
void Start(PGamePlay const p_this);
void StopGame();



//private�ϐ���cpp�ɋL�q