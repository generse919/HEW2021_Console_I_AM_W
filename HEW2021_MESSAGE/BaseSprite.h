#pragma once
#include "../YSEngine/YSConsoleGameEngine.h"

enum PICID
{
	LETTERS,

};
enum CHAR_ID
{
	MESSAGE,
};
typedef struct
{
	//キャラのサイズ
	int w;
	int h;


	//座標
	Vector2_F pos;
	//方向ベクトル
	Vector2_F vec;
	//スプライトのID
	int m_ID;

	//スプライトのスポーン状況
	BOOL isActive;

	//bmp情報を保存するポインタ
	PFRAMEBUFFER pfb;


}BaseSprite, *PBaseSprite;
typedef struct
{
	const char* fileName;
	PICID	id;
	FRAMEBUFFER*	pBmp;
}BmpInfo;



//enum COLOR
//{
//
//	FG_BLACK = 0x0000,
//	FG_DARK_BLUE = 0x0001,
//	FG_DARK_GREEN = 0x0002,
//	FG_DARK_CYAN = 0x0003,
//	FG_DARK_RED = 0x0004,
//	FG_DARK_MAGENTA = 0x0005,
//	FG_DARK_YELLOW = 0x0006,
//	FG_GREY = 0x0007, // Thanks MS :-/
//	FG_DARK_GREY = 0x0008,
//	FG_BLUE = 0x0009,
//	FG_GREEN = 0x000A,
//	FG_CYAN = 0x000B,
//	FG_RED = 0x000C,
//	FG_MAGENTA = 0x000D,
//	FG_YELLOW = 0x000E,
//	FG_WHITE = 0x000F,
//	BG_BLACK = 0x0000,
//	BG_DARK_BLUE = 0x0010,
//	BG_DARK_GREEN = 0x0020,
//	BG_DARK_CYAN = 0x0030,
//	BG_DARK_RED = 0x0040,
//	BG_DARK_MAGENTA = 0x0050,
//	BG_DARK_YELLOW = 0x0060,
//	BG_GREY = 0x0070,
//	BG_DARK_GREY = 0x0080,
//	BG_BLUE = 0x0090,
//	BG_GREEN = 0x00A0,
//	BG_CYAN = 0x00B0,
//	BG_RED = 0x00C0,
//	BG_MAGENTA = 0x00D0,
//	BG_YELLOW = 0x00E0,
//	BG_WHITE = 0x00F0,
//};
//
//enum PIXEL_TYPE
//{
//	PIXEL_SOLID = 0x2588,
//	PIXEL_THREEQUARTERS = 0x2593,
//	PIXEL_HALF = 0x2592,
//	PIXEL_QUARTER = 0x2591,
//};




void GetScreenBuffer(const PConsoleManager const p_cm);
void Create(PBaseSprite const p_this, PICID id, int characterID);
void Destroy(PBaseSprite  const p_this);
void Draw(int buf_x, int buf_y, RGBQUAD rgb);
RGBQUAD GetColor(PBaseSprite const p_this, int x, int y);
//void Clip(int &x, int &y);
//void Fill(int x1, int y1, int x2, int y2, short c, short col);
//void SetGlyph(PBaseSprite const p_this, int x, int y, short c);
//void SetColour(PBaseSprite const p_this, int x, int y, short c);
//short GetGlyph(PBaseSprite const p_this, int x, int y);
//short GetColour(PBaseSprite const p_this, int x, int y);
//short SampleGlyph(PBaseSprite const p_this, float x, float y);
//short SampleColour(PBaseSprite const p_this, float x, float y);
void SetScreenBuffer(PConsoleManager p_cm);
void ReleaseBuffer();