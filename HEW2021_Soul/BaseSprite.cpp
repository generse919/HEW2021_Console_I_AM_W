#include "BaseSprite.h"

//描画用バッファポインタ
static unsigned char* m_getBuffer = NULL;
//フルカラーテーブル
//extern char CharRGBconvTBL[3][256];


//色のメンバポインタ
static unsigned char *m_Colors = NULL;
static short *m_Glyphs = NULL;

static BmpInfo bmp_lists[] = {
	{"../Release/bmp/WhiteSoul20x20.bmp", DEFAULT_PIC, NULL},
	{"../Release/bmp/BlueSoul20x20.bmp",BLUESOUL_PIC, NULL},
	{"../Release/bmp/RedSoul20x20.bmp",REDSOUL_PIC, NULL},
	{"../Release/bmp/GreenSoul20x20.bmp",GREENSOUL_PIC, NULL},
	{"../Release/bmp/YellowSoul20x20.bmp",YELLOWSOUL_PIC, NULL},
};

/*
*
*
*/
void GetScreenBuffer(const PConsoleManager const p_cm)
{
	/*if (m_getBuffer == NULL)
	{
		m_getBuffer = (unsigned char*)malloc(sizeof(unsigned char) * ScreenWidth() * ScreenHeight() * 3);
	}*/
	
	m_getBuffer = p_cm->m_bufScreen;
	//memcpy(m_getBuffer,p_cm->m_bufScreen,sizeof(unsigned char) * ScreenWidth() * ScreenHeight() * 3);

}
/*
*
*@brief		spriteの初期化
*
*
*@param		p_this	spriteのポインタ
*
*@param		spritefile	読み込みたいbmpのファイルパス
*/
void Create(PBaseSprite const p_this, PICID id, int characterID)
{
	//スプライト保存用ポインタ
	//p_this = (PBaseSprite)calloc(1, sizeof(BaseSprite));
	if (!(p_this->pfb = bmp_load(bmp_lists[id].fileName)))
	{
		return;
	}
	/*memset(&p_this->w, p_this->pfb->width, sizeof(int));
	memset(&p_this->h, p_this->pfb->height, sizeof(int));*/


	p_this->w = p_this->pfb->width;
	p_this->h = p_this->pfb->height;
	/*if (p_this->pfb->bih.biBitCount == 24)
	{
		bmp_swapRB(p_this->pfb);
	}*/
	
	/*m_Colors = (unsigned char*)malloc(p_this->w * p_this->h * 3);
	
	memcpy(m_Colors, p_this->pfb->ppx, p_this->pfb->ppx_size);*/
	

	p_this->pos = { 0.0f,0.0f };
	p_this->vec = { 0.0f,0.0f };

	p_this->m_ID = characterID;

	p_this->isActive = FALSE;


}

void Destroy(PBaseSprite const p_this)
{
	
	//SAFE_FREE(m_Colors);

	bmp_end(p_this->pfb);
	//p_this->pfb = NULL;

	//SAFE_FREE(p_this);
}

/*
*@brief		指定した位置の1ピクセルに点を打つ
*
*@param		buf_x	バッファのx座標
*@param		buf_y	バッファのy座標
*@param		c		ピクセルの文字タイプ(1,0.75,0.5,0.25の4種類)
*@param		col		ピクセルの色(1,0.75,0.5,0.25の4種類)
*/
void Draw(int buf_x, int buf_y,RGBQUAD rgb)
{
	//RGBごとに入れる
	/*memcpy(&m_getBuffer[buf_y * ScreenWidth() + buf_x] 3);*/
	m_getBuffer[(buf_y * ScreenWidth() * 3 ) + (buf_x* 3) + 0] = rgb.rgbRed   & 0xFF;
	m_getBuffer[(buf_y * ScreenWidth() * 3 ) + (buf_x* 3) + 1] = rgb.rgbGreen  & 0xFF;
	m_getBuffer[(buf_y * ScreenWidth() * 3 ) + (buf_x* 3) + 2] = rgb.rgbBlue  & 0xFF;
	//m_getBuffer[buf_y * ScreenWidth() + buf_x]
}
/*
*@brief		指定したスプライトのローカル座標にある1ピクセルを取得する
*
*@param		p_this	スプライトのアドレス
*@param		buf_x	スプライトのx座標
*@param		buf_y	スプライトのy座標
*
*/
RGBQUAD GetColor(const PBaseSprite const p_this, int x, int y)
{
	if (x < 0 || x >= p_this->w || y < 0 || y >= p_this->h)
		return {0,0,0,0};
	else
	{
		return { p_this->pfb->ppx[(y * p_this->w * 3 ) +  (x * 3) + 0],//Blue
				p_this->pfb->ppx[(y * p_this->w * 3 ) +  (x * 3) + 1],//Green
				p_this->pfb->ppx[(y * p_this->w * 3 ) +  (x * 3) + 2],//Red
				0};
	}
		
}
/*
*@brief		指定したスプライトのローカル座標にある1ピクセルを任意の色に設定する
*
*@param		p_this	スプライトのアドレス
*@param		buf_x	スプライトのx座標
*@param		buf_y	スプライトのy座標
*@param		rgb 	rgbq(0~255)の配列
*
*/

void SetColor(PBaseSprite const p_this, int x, int y, RGBQUAD rgb)
{
	if (x < 0 || x >= p_this->w || y < 0 || y >= p_this->h)
	{
		p_this->pfb->ppx[(y * p_this->w * 3) + (x * 3) + 0] = 0;
		p_this->pfb->ppx[(y * p_this->w * 3) + (x * 3) + 1] = 0;
		p_this->pfb->ppx[(y * p_this->w * 3) + (x * 3) + 2] = 0;
	}
	else
	{
		p_this->pfb->ppx[(y * p_this->w * 3) + (x * 3) + 0] = rgb.rgbRed;
		p_this->pfb->ppx[(y * p_this->w * 3) + (x * 3) + 1] = rgb.rgbGreen;
		p_this->pfb->ppx[(y * p_this->w * 3) + (x * 3) + 2] = rgb.rgbBlue;
	}
}



/*
*@brief		スプライトのローカル座標(実数)の1ピクセルを抽出する
*
*@param		p_this	スプライトのアドレス
*@param		x	スプライトのx座標
*@param		y	スプライトのy座標
*
*/
RGBQUAD SampleColour(PBaseSprite const p_this,float x, float y)
{
	int sx = (int)(x * (float)p_this->w);
	int sy = (int)(y * (float)p_this->h - 1.0f);
	if (sx < 0 || sx >= p_this->w || sy < 0 || sy >= p_this->h)
		return { 0,0,0,0 };
	else
		return {p_this->pfb->ppx[(sy * p_this->w * 3) + (sx * 3) + 0],
				p_this->pfb->ppx[(sy * p_this->w * 3) + (sx * 3) + 1],
				p_this->pfb->ppx[(sy * p_this->w * 3) + (sx * 3) + 2],
				0};
}


void SetScreenBuffer(PConsoleManager p_cm)
{
	memcpy(p_cm->m_bufScreen, m_getBuffer, sizeof(unsigned char) * ScreenWidth() * ScreenHeight() * 3);
}

void ReleaseBuffer()
{
	m_getBuffer = NULL;
}