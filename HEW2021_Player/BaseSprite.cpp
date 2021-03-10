#include "BaseSprite.h"

//�`��p�o�b�t�@�|�C���^
static unsigned char* m_getBuffer = NULL;
//�t���J���[�e�[�u��
//extern char CharRGBconvTBL[3][256];


//�F�̃����o�|�C���^
static unsigned char *m_Colors = NULL;
static short *m_Glyphs = NULL;

static BmpInfo bmp_lists[] = {
	{"../Release/bmp/WhiteSoul60x60.bmp", PIC_WHITESOUL, NULL},
	{"../Release/bmp/YellowSoul60x60.bmp", PIC_YELLOWSOUL, NULL},
	{"../Release/bmp/GreenSoul60x60.bmp", PIC_GREENSOUL, NULL},
	{"../Release/bmp/BlueSoul60x60.bmp", PIC_BLUESOUL, NULL},
	{"../Release/bmp/RedSoul60x60.bmp", PIC_REDSOUL, NULL},
	{"../Release/bmp/ClockSample_0.bmp", PIC_CLOCK_ZERO, NULL},
	{"../Release/bmp/ClockSample_1.bmp", PIC_CLOCK_ONE, NULL},
	{"../Release/bmp/ClockSample_2.bmp", PIC_CLOCK_TWO, NULL},
	{"../Release/bmp/ClockSample_3.bmp", PIC_CLOCK_THREE, NULL},
	{"../Release/bmp/ClockSample_4.bmp", PIC_CLOCK_FOUR, NULL},
	{"../Release/bmp/ClockSample_5.bmp", PIC_CLOCK_FIVE, NULL},
	{"../Release/bmp/ClockSample_6.bmp", PIC_CLOCK_SIX, NULL},
	{"../Release/bmp/ClockSample_7.bmp", PIC_CLOCK_SEVEN, NULL},
	{"../Release/bmp/ClockSample_8.bmp", PIC_CLOCK_EIGHT, NULL},
	{"../Release/bmp/ClockSample_9.bmp", PIC_CLOCK_NINE, NULL},
	{"../Release/bmp/ClockSample_10.bmp", PIC_CLOCK_TEN, NULL},
	{"../Release/bmp/ClockSample_11.bmp", PIC_CLOCK_ELEVEN, NULL},
	{"../Release/bmp/ClockSample_12.bmp", PIC_CLOCK_TWELVE, NULL},
	{"../Release/bmp/ClockSample_13.bmp", PIC_CLOCK_THIRTEEN, NULL},
	{"../Release/bmp/ClockSample_14.bmp", PIC_CLOCK_FOURTEEN, NULL},

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
*@brief		sprite�̏�����
*
*
*@param		p_this	sprite�̃|�C���^
*
*@param		spritefile	�ǂݍ��݂���bmp�̃t�@�C���p�X
*/
void Create(PBaseSprite const p_this, int picid, int characterID)
{
	//�X�v���C�g�ۑ��p�|�C���^
	//p_this = (PBaseSprite)calloc(1, sizeof(BaseSprite));
	if (!(p_this->pfb = bmp_load(bmp_lists[picid].fileName)))
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

	p_this->w = 0;
	p_this->h = 0;


	p_this->pos = { 0.0f,0.0f };
	p_this->vec = { 0.0f,0.0f };

	p_this->m_ID = 0;

	p_this->isActive = FALSE;
	
}

/*
*@brief		�w�肵���ʒu��1�s�N�Z���ɓ_��ł�
*
*@param		buf_x	�o�b�t�@��x���W
*@param		buf_y	�o�b�t�@��y���W
*@param		c		�s�N�Z���̕����^�C�v(1,0.75,0.5,0.25��4���)
*@param		col		�s�N�Z���̐F(1,0.75,0.5,0.25��4���)
*/
void Draw(int buf_x, int buf_y,RGBQUAD rgb)
{
	//RGB���Ƃɓ����
	/*memcpy(&m_getBuffer[buf_y * ScreenWidth() + buf_x] 3);*/
	m_getBuffer[(buf_y * ScreenWidth() * 3 ) + (buf_x* 3) + 0] = rgb.rgbRed   & 0xFF;
	m_getBuffer[(buf_y * ScreenWidth() * 3 ) + (buf_x* 3) + 1] = rgb.rgbGreen  & 0xFF;
	m_getBuffer[(buf_y * ScreenWidth() * 3 ) + (buf_x* 3) + 2] = rgb.rgbBlue  & 0xFF;
	//m_getBuffer[buf_y * ScreenWidth() + buf_x]
}
/*
*@brief		�w�肵���X�v���C�g�̃��[�J�����W�ɂ���1�s�N�Z�����擾����
*
*@param		p_this	�X�v���C�g�̃A�h���X
*@param		buf_x	�X�v���C�g��x���W
*@param		buf_y	�X�v���C�g��y���W
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
*@brief		�w�肵���X�v���C�g�̃��[�J�����W�ɂ���1�s�N�Z����C�ӂ̐F�ɐݒ肷��
*
*@param		p_this	�X�v���C�g�̃A�h���X
*@param		buf_x	�X�v���C�g��x���W
*@param		buf_y	�X�v���C�g��y���W
*@param		rgb 	rgbq(0~255)�̔z��
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
*@brief		�X�v���C�g�̃��[�J�����W(����)��1�s�N�Z���𒊏o����
*
*@param		p_this	�X�v���C�g�̃A�h���X
*@param		x	�X�v���C�g��x���W
*@param		y	�X�v���C�g��y���W
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