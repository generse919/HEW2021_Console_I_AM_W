#pragma once
#include "BaseSprite.h"

#define MAX_COLUMN 1024
#define MAX_ROW 100

typedef struct {
	char*	pPix;	//�����̃r�b�g�}�b�v�f�[�^�ւ̃|�C���^
	COORD	size;	//�����̕��ƍ���
	int buf_size;	//�r�b�g�}�b�v�f�[�^�̃T�C�Y�i�o�C�g���j
	int aaLv;		//�~�����i�A���`�G�C���A�X���x���j
	wchar_t	wch;	//�ϊ����̕���
} BmpChar, *PBmpChar;


typedef struct
{
	BaseSprite Message;

	PBmpChar script_buffer;

}Message, *PMessage;

void LoadScript(PMessage p_this, const char *filepath, int fontsize);
wchar_t CheckFirstString(const PMessage p_this, int col);
void DrawChar(PBmpChar p_this, int sx, int sy, RGBQUAD letter_color, RGBQUAD background_color);
BOOL DrawStringLog(PMessage p_this, int const  sx, int const sy, int row, RGBQUAD letter_color, RGBQUAD background_color,float sec);
void LoadGameLog();
void ResetStringLog();