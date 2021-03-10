#pragma once
#include "BaseSprite.h"

#define MAX_COLUMN 1024
#define MAX_ROW 100

typedef struct {
	char*	pPix;	//文字のビットマップデータへのポインタ
	COORD	size;	//文字の幅と高さ
	int buf_size;	//ビットマップデータのサイズ（バイト数）
	int aaLv;		//諧調数（アンチエイリアスレベル）
	wchar_t	wch;	//変換元の文字
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