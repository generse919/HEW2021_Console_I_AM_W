#include "message.h"



//������i�[�p�ӂ�����ۂ���
static FILE *fp = NULL;

static float msgTime;

static int seekSetPos = 0;//���b�Z�[�W���O�p�V�[�N��̈ʒu
static BOOL seekSet = FALSE;//���b�Z�[�W���O�p�V�[�N�Z�b�g�t���O
static BOOL LogGoing = TRUE;
static int charCount = 0;//�V�[�N���̕����J�E���g�p


void ConvBpp1ToB(BmpChar* _pbc, GLYPHMETRICS* _pgm);
void DrawBmpChar(BmpChar* _pbc, GLYPHMETRICS* _pgm, TEXTMETRIC* _ptxm);

//================================================================
//	1bpp�̉摜��8bpp�̉摜�ɕϊ�����B
//arg:
//	BmpChar* _pbc : �r�b�g�}�b�v�����f�[�^�ւ̃|�C���^
//	GLYPHMETRICS* _pgm : �ϊ��������̃O���t���
//return:
//	�Ȃ�
//================================================================
void ConvBpp1ToB(BmpChar* _pbc, GLYPHMETRICS* _pgm)
{
	int w_pix = _pgm->gmBlackBoxX;
	int h_pix = _pgm->gmBlackBoxY;
	int stride = (_pbc->buf_size / _pgm->gmBlackBoxY);
	int stride4 = (w_pix + 0b0011)& (~0b0011);			//�Wbpp�摜�̂S�o�C�g���E�̃o�C�g��
	int bits_size = stride4 * h_pix;
	char* pFontBitmap = (char*)malloc(bits_size);
	ZeroMemory(pFontBitmap, bits_size);
	for (int y = 0; y < h_pix; y++)
	{
		for (int x = 0; x < stride; x++)
		{
			int idxSrc = (y * stride + x);
			UINT bit8 = _pbc->pPix[idxSrc];
			int idxDest = (y * stride4) + (x * 8);
			for (int bitN = 0; bitN < 8; bitN++)
			{
				if ((idxDest + bitN) < bits_size)
				{
					//pFontBitmap[idxDest + bitN] = (bit8 & (0b10000000 >> bitN)) ? 1 : 0;	//0xFF : 0x00;
					if ((bit8 & (0b10000000 >> bitN)) != 0) {
						pFontBitmap[idxDest + bitN] = 1;
					}
				}
			}
		}
	}
	//�Â�1bpp�̃o�b�t�@�͍폜���ĐV���������8bpp�o�b�t�@�ɓ���ւ���B
	free(_pbc->pPix);
	_pbc->pPix = pFontBitmap;
	_pbc->buf_size = bits_size;
}	//ConvBpp1ToB

//================================================================
//	�r�b�g�}�b�v�����P�����̕\���ʒu�𒲐����ăr�b�g�}�b�v����蒼���B
//arg:
//	BitmapChar* _pbc : �r�b�g�}�b�v�����̃|�C���^�B���̃|�C���^���w���r�b�g�}�b�v�����f�[�^�̕\���ʒu�𒲐����ăo�b�t�@����蒼�����B
//	GLYPHMETRICS* _pgm : �ϊ��������̃O���t���
//	TEXTMETRIC* _ptxm : �ϊ����t�H���g�̌v���i�����̐��@�j���
//return:
//	����
//================================================================
void DrawBmpChar(BmpChar* _pbc, GLYPHMETRICS* _pgm, TEXTMETRIC* _ptxm)
{
	int	dest_width = _pgm->gmCellIncX;
	int dest_height = _ptxm->tmHeight;
	int dest_buf_size = dest_width * dest_height;
	char* pDest = (char*)malloc(dest_buf_size);
	ZeroMemory(pDest, dest_buf_size);
	int width = _pgm->gmBlackBoxX;
	int widthBytes = (width + 0b0011)& (~0b0011);	//�����̃o�C�g���͂S�̔{���ɍ��킹��
	int height = _pgm->gmBlackBoxY;
	//
	int pn = 0;
	int stride = _pbc->buf_size / _pbc->size.Y;
	//
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int xp = _pgm->gmptGlyphOrigin.x + x;
			int yp = (_ptxm->tmAscent - _pgm->gmptGlyphOrigin.y) + y;
			if (yp < 0)
			{
				continue;
			}
			if ((xp >= 0) && (xp < dest_width) && (yp >= 0) && (yp < dest_height))
			{
				int read_idx = (y * widthBytes + x);
				if ((read_idx >= 0) && ((int)_pbc->buf_size > read_idx))
				{
					unsigned char dot = _pbc->pPix[y * stride + x];
					if (dot != 0x00)
					{
						if (_pbc->aaLv == 2)
						{
							dot = 0x0F;
						}
						else
						{
							dot = (unsigned char)((double)(16.0 / (double)(_pbc->aaLv - 1)) *  (double)dot);
						}
						pDest[yp * dest_width + xp] = dot;
					}
				}
			}
		}
	}
	free(_pbc->pPix);
	_pbc->pPix = pDest;
	_pbc->buf_size = dest_buf_size;
	_pbc->size.X = dest_width;
	_pbc->size.Y = dest_height;
	return;
}	//DrawBmpChar

/*
*@brief		�w�肵���ʒu��1�s�N�Z���ɓ_��ł�
*
*@param		p_this	���b�Z�[�W
*@param		filepath	�ǂݍ��݂����t�@�C���̃p�X
*@param		fontsize	�t�H���g�T�C�Y

*/
void LoadScript(PMessage p_this, const char *filepath, int fontsize)
{

	//BaseSprite�̓[������
	//p_this->Message = (PBaseSprite)calloc(1, sizeof(BaseSprite));
	//ZeroMemory(&p_this->Message, sizeof(BaseSprite));

	if (!(fp = fopen(filepath, "rb")))
	{
		return;
	}

	int fileByte = 0;//���[�h�t�@�C���̕��������J�E���g
	int row = 0;//�s���̃J�E���g
	long offset[MAX_COLUMN] = { 0 };//1�s������̃J�[�\����ۑ�����
	char buff[1000] = { 0 };
	char *p = buff;
	//txt�t�@�C���̓��e���o�b�t�@�ɕۑ�
	for (int i = 0; !feof(fp); i++)
	{
		if (i >= MAX_COLUMN) break;
		*p = fgetc(fp);
		if (*p == '\n')
		{
			offset[row] = ftell(fp);
			row++;

		}
		fileByte++;
		p++;


	}

	//��]�s��
	MAT2	mat2{ {0,1},{0,0},{0,0},{0,1} };
	//�t�H���g�̐ݒ�`�쐬
	LOGFONT	lf;
	lf.lfHeight = fontsize;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;	//�����P�ʂ̉�]�p�x�����
	lf.lfOrientation = 0;

	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = FALSE;	//�Α�
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = ANSI_CHARSET;
	lf.lfOutPrecision = OUT_TT_PRECIS;	//OUT_DEFAULT_PRECIS
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = (DEFAULT_PITCH | FF_MODERN);
	lf.lfFaceName[0] = '\0';
	//���̖����R�s�[�i��������������j
	CopyMemory(lf.lfFaceName, "Comic Sans MS", LF_FACESIZE * sizeof(CHAR));
	//�t�H���g����
	HFONT hFont = CreateFontIndirect(&lf);
	if (hFont == NULL)
	{
		return;
	}
	// �f�o�C�X�Ƀt�H���g��I������
	HDC hdc = GetDC(Get_HWND());
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
	//���������t�H���g�̌v���f�[�^���擾����
	TEXTMETRIC	txm;		//�ϊ������t�H���g�̏�������\����
	GetTextMetrics(hdc, &txm);	//�v���f�[�^���擾
	int aa_level = 2;	//�t�H���g�K����2�ŌŒ�

	//�w��̃t�H���g�ŏo�����r�b�g�}�b�v�����ŕ�������쐬����B
	GLYPHMETRICS	gm;	//�O���t�ݒ�f�[�^
	UINT code;
	BmpChar*	pBmpChr = (BmpChar*)malloc(sizeof(BmpChar) * (fileByte + 1));
	//wchar_t *_wtext;
	//mbstowcs(_wtext, buff, sizeof(buff));//char����wchar_t�֕ϊ�

	ZeroMemory(pBmpChr, sizeof(BmpChar) * (fileByte + 1));

	for (int txn = 0; txn < fileByte; txn++) {
		code = (UINT)buff[txn];
		//���ꂩ�琶�����镶���r�b�g�}�b�v�f�[�^�̃o�C�g�����擾����B
		int buff_size = GetGlyphOutlineW(hdc, code, GGO_BITMAP, &gm, 0, NULL, &mat2);
		if (buff_size > 0)
		{
			//�擾�����T�C�Y���̃o�b�t�@���m�ۂ���B
			pBmpChr[txn].pPix = (char*)malloc(buff_size);
			GetGlyphOutlineW(hdc, code, GGO_BITMAP, &gm, buff_size, pBmpChr[txn].pPix, &mat2);

			//1bpp�̃r�b�g�}�b�v�͕\�����ɂ����̂łWbpp�ɕϊ�����B
			pBmpChr[txn].buf_size = buff_size;		//�o�b�t�@�T�C�Y
			ConvBpp1ToB(&pBmpChr[txn], &gm);
			buff_size = pBmpChr[txn].buf_size;

			pBmpChr[txn].size.X = gm.gmBlackBoxX;	//���s�N�Z����
			pBmpChr[txn].size.Y = gm.gmBlackBoxY;	//�c�s�N�Z����
			pBmpChr[txn].buf_size = buff_size;		//�o�b�t�@�T�C�Y
			pBmpChr[txn].aaLv = aa_level;			//�A���`�G�C���A�X���~�����x��
			pBmpChr[txn].wch = code;				//�ϊ����̕����R�[�h
			//�����ʒu�𒲐����ăo�b�t�@����蒼���B
			DrawBmpChar(&pBmpChr[txn], &gm, &txm);
		}
	}
	p_this->script_buffer = pBmpChr;
	/*p_this->script_buffer = (PBmpChar)malloc(sizeof(BmpChar)*fileByte);
	memcpy(p_this->script_buffer, pBmpChr, sizeof(pBmpChr));*/
	//return	pBmpChr;
	fclose(fp);

	//while (fgets())
}
/*
*@brief			�s�̓��̕������m�F����
*
*@param			row		�m�F�������s
*
*@return		�m�F�s�̐擪����
*
*/
wchar_t CheckFirstString(const PMessage p_this, int row)
{
	int count = 0;
	for (int rowCount = 0; rowCount < row; rowCount++)
	{
		while (p_this->script_buffer[count].wch != '\n')
		{
			count++;
			//ypos += p_this->script_buffer[charCount].size.Y;
		}
		count++;
	}


	return p_this->script_buffer[count].wch;
}


/*
*@brief		���[�h�����t�@�C������w�肵���s�̕������\������
*
*@param		sx			�����̍��[�̍��W
*@param		sy			�����̉E�[�̍��W
*
*/

void DrawChar(PBmpChar p_this, int sx, int sy, RGBQUAD letter_color, RGBQUAD background_color)
{
	int pn = 0;

	if (p_this->wch == '\r' || p_this->wch == '\n' || p_this->wch == 65535)//�G�X�P�[�v�����̓X���[
	{
		for (int y = 0; y < p_this->size.Y; y++)
		{
			for (int x = 0; x < p_this->size.X; x++)
			{
				int xp = (sx + x);	//���̉������ʒu
				int yp = (sy + y);
				if ((xp >= 0) && (xp < ScreenWidth()) && (yp >= 0) && (yp < ScreenHeight()))
				{

					Draw(xp, yp, { 0,0,0,0 });


				}
				pn++;	//���̃s�N�Z���ǂݏo���ʒu
			}
		}
	}
	else if (p_this->wch == '\0')
	{
		for (int y = 0; y < 15; y++)
		{
			for (int x = 0; x < 6; x++)
			{
				int xp = (sx + x);	//���̉������ʒu
				int yp = (sy + y);
				if ((xp >= 0) && (xp < ScreenWidth()) && (yp >= 0) && (yp < ScreenHeight()))
				{

					Draw(xp, yp, { 0,0,0,0 });


				}
				pn++;	//���̃s�N�Z���ǂݏo���ʒu
			}
		}
	}
	else
	{
		for (int y = 0; y < p_this->size.Y; y++)
		{
			for (int x = 0; x < p_this->size.X; x++)
			{
				int xp = (sx + x);	//���̉������ʒu
				int yp = (sy + y);
				if ((xp >= 0) && (xp < ScreenWidth()) && (yp >= 0) && (yp < ScreenHeight()))
				{
					switch ((UINT)p_this->pPix[pn])//�P�s�N�Z����������
					{
					case 0:Draw(xp, yp, background_color); break;//��
					case 15:Draw(xp, yp, letter_color); break;//��(����)
					default:break;
					}

				}
				pn++;	//���̃s�N�Z���ǂݏo���ʒu
			}
		}
	}

}
/*
*@brief		���[�h�����t�@�C������w�肵���s�̕������RPG���O�̂悤�ɕ\������
*
*@param		p_this		���b�Z�[�W�̃|�C���^
*@param		sx			�����̍��[�̍��W
*@param		sy			�����̉E�[�̍��W
*@param		row			�s��(0�͂��܂�)
*@param		sec			�ꕶ��������̕\���b��
*
*/
BOOL DrawStringLog(PMessage p_this, int const sx, int const sy, int row, RGBQUAD letter_color, RGBQUAD background_color, float sec)
{
	int xpos = 0;
	int ypos = 0;

	//�b���J�E���g
	msgTime += GetDeltaTime();
	if (!seekSet)
	{
		for (int rowCount = 0; rowCount < row; rowCount++)
		{
			while (p_this->script_buffer[charCount].wch != '\n')
			{
				charCount++;
				//ypos += p_this->script_buffer[charCount].size.Y;
			}
			charCount++;
		}

		////�V�[�N��󗓍s�Ȃ�X�g�b�v
		//if (p_this->script_buffer[charCount].wch == '\r' && p_this->script_buffer[charCount + 1].wch == '\n')
		//{
		//	charCount = 0;
		//	seekSet = FALSE;
		//	return;
		//}

		seekSetPos = charCount;//�V�[�N����̕������Ԗڂ�ۑ�
		seekSet = TRUE;


	}





	if (msgTime >= sec)
	{

		if (LogGoing)charCount++;

		//�����`�F�b�N

		msgTime = 0.0f;
	}

	for (int i = seekSetPos; i <= charCount; i++)//�s���V�[�N�ォ�猻�݂̕����܂ŕ\������
	{

		if (p_this->script_buffer[i].wch == '\n')
		{
			if (p_this->script_buffer[i + 1].wch == '\r' && p_this->script_buffer[i + 2].wch == '\n')
			{
				LogGoing = FALSE;
				//seekSet = FALSE;
			}
			else
			{
				//charCount++;
				xpos = 0;
				ypos += p_this->script_buffer[i].size.Y;
			}

		}

		if (p_this->script_buffer[i].wch == EOF || p_this->script_buffer[i].wch == 65535)
		{
			return FALSE;
		}

		DrawChar(&p_this->script_buffer[i], sx + xpos, sy + ypos,letter_color,background_color);

		if (p_this->script_buffer[i].wch == '\0') xpos += 6;
		else if (p_this->script_buffer[i].wch != '\n') xpos += p_this->script_buffer[i].size.X;


		if (i == charCount && (!LogGoing))
		{
			//���O�̃G���h�h�b�g�̕`��
			for (int y = 0; y < 2; y++)
				for (int x = 0; x < 2; x++)
				{
					Draw(x + sx + xpos, y + sy + ypos + 7 + (3 * absf((float)sin(sqrtf(clock() / 20)))), { 255,255,255,0 });
				}

		}

	}

	return TRUE;


}



/*
*@brief		DrawStringLog�ɕK�v�ȕϐ��̏��������s��
*
*/
void ResetStringLog()
{
	msgTime = 0;
	seekSetPos = 0;
	LogGoing = TRUE;
	seekSet = FALSE;
	charCount = 0;


}