#include "message.h"



//文字列格納用ふぁいるぽいんた
static FILE *fp = NULL;

static float msgTime;

static int seekSetPos = 0;//メッセージログ用シーク後の位置
static BOOL seekSet = FALSE;//メッセージログ用シークセットフラグ
static BOOL LogGoing = TRUE;
static int charCount = 0;//シーク時の文字カウント用


void ConvBpp1ToB(BmpChar* _pbc, GLYPHMETRICS* _pgm);
void DrawBmpChar(BmpChar* _pbc, GLYPHMETRICS* _pgm, TEXTMETRIC* _ptxm);

//================================================================
//	1bppの画像を8bppの画像に変換する。
//arg:
//	BmpChar* _pbc : ビットマップ文字データへのポインタ
//	GLYPHMETRICS* _pgm : 変換元文字のグリフ情報
//return:
//	なし
//================================================================
void ConvBpp1ToB(BmpChar* _pbc, GLYPHMETRICS* _pgm)
{
	int w_pix = _pgm->gmBlackBoxX;
	int h_pix = _pgm->gmBlackBoxY;
	int stride = (_pbc->buf_size / _pgm->gmBlackBoxY);
	int stride4 = (w_pix + 0b0011)& (~0b0011);			//８bpp画像の４バイト境界のバイト数
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
	//古い1bppのバッファは削除して新しく作った8bppバッファに入れ替える。
	free(_pbc->pPix);
	_pbc->pPix = pFontBitmap;
	_pbc->buf_size = bits_size;
}	//ConvBpp1ToB

//================================================================
//	ビットマップ文字１文字の表示位置を調整してビットマップを作り直す。
//arg:
//	BitmapChar* _pbc : ビットマップ文字のポインタ。このポインタが指すビットマップ文字データの表示位置を調整してバッファが作り直される。
//	GLYPHMETRICS* _pgm : 変換元文字のグリフ情報
//	TEXTMETRIC* _ptxm : 変換元フォントの計測（文字の寸法）情報
//return:
//	無し
//================================================================
void DrawBmpChar(BmpChar* _pbc, GLYPHMETRICS* _pgm, TEXTMETRIC* _ptxm)
{
	int	dest_width = _pgm->gmCellIncX;
	int dest_height = _ptxm->tmHeight;
	int dest_buf_size = dest_width * dest_height;
	char* pDest = (char*)malloc(dest_buf_size);
	ZeroMemory(pDest, dest_buf_size);
	int width = _pgm->gmBlackBoxX;
	int widthBytes = (width + 0b0011)& (~0b0011);	//横幅のバイト数は４の倍数に合わせる
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
*@brief		指定した位置の1ピクセルに点を打つ
*
*@param		p_this	メッセージ
*@param		filepath	読み込みたいファイルのパス
*@param		fontsize	フォントサイズ

*/
void LoadScript(PMessage p_this, const char *filepath, int fontsize)
{

	//BaseSpriteはゼロ埋め
	//p_this->Message = (PBaseSprite)calloc(1, sizeof(BaseSprite));
	//ZeroMemory(&p_this->Message, sizeof(BaseSprite));

	if (!(fp = fopen(filepath, "rb")))
	{
		return;
	}

	int fileByte = 0;//リードファイルの文字数をカウント
	int row = 0;//行数のカウント
	long offset[MAX_COLUMN] = { 0 };//1行当たりのカーソルを保存する
	char buff[1000] = { 0 };
	char *p = buff;
	//txtファイルの内容をバッファに保存
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

	//回転行列
	MAT2	mat2{ {0,1},{0,0},{0,0},{0,1} };
	//フォントの設定～作成
	LOGFONT	lf;
	lf.lfHeight = fontsize;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;	//文字単位の回転角度左回り
	lf.lfOrientation = 0;

	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = FALSE;	//斜体
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = ANSI_CHARSET;
	lf.lfOutPrecision = OUT_TT_PRECIS;	//OUT_DEFAULT_PRECIS
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = (DEFAULT_PITCH | FF_MODERN);
	lf.lfFaceName[0] = '\0';
	//書体名をコピー（文字数制限あり）
	CopyMemory(lf.lfFaceName, "Comic Sans MS", LF_FACESIZE * sizeof(CHAR));
	//フォント生成
	HFONT hFont = CreateFontIndirect(&lf);
	if (hFont == NULL)
	{
		return;
	}
	// デバイスにフォントを選択する
	HDC hdc = GetDC(Get_HWND());
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
	//生成したフォントの計測データを取得する
	TEXTMETRIC	txm;		//変換したフォントの情報を入れる構造体
	GetTextMetrics(hdc, &txm);	//計測データを取得
	int aa_level = 2;	//フォント階調は2で固定

	//指定のフォントで出来たビットマップ文字で文字列を作成する。
	GLYPHMETRICS	gm;	//グリフ設定データ
	UINT code;
	BmpChar*	pBmpChr = (BmpChar*)malloc(sizeof(BmpChar) * (fileByte + 1));
	//wchar_t *_wtext;
	//mbstowcs(_wtext, buff, sizeof(buff));//charからwchar_tへ変換

	ZeroMemory(pBmpChr, sizeof(BmpChar) * (fileByte + 1));

	for (int txn = 0; txn < fileByte; txn++) {
		code = (UINT)buff[txn];
		//これから生成する文字ビットマップデータのバイト数を取得する。
		int buff_size = GetGlyphOutlineW(hdc, code, GGO_BITMAP, &gm, 0, NULL, &mat2);
		if (buff_size > 0)
		{
			//取得したサイズ分のバッファを確保する。
			pBmpChr[txn].pPix = (char*)malloc(buff_size);
			GetGlyphOutlineW(hdc, code, GGO_BITMAP, &gm, buff_size, pBmpChr[txn].pPix, &mat2);

			//1bppのビットマップは表示しにくいので８bppに変換する。
			pBmpChr[txn].buf_size = buff_size;		//バッファサイズ
			ConvBpp1ToB(&pBmpChr[txn], &gm);
			buff_size = pBmpChr[txn].buf_size;

			pBmpChr[txn].size.X = gm.gmBlackBoxX;	//横ピクセル数
			pBmpChr[txn].size.Y = gm.gmBlackBoxY;	//縦ピクセル数
			pBmpChr[txn].buf_size = buff_size;		//バッファサイズ
			pBmpChr[txn].aaLv = aa_level;			//アンチエイリアスの諧調レベル
			pBmpChr[txn].wch = code;				//変換元の文字コード
			//文字位置を調整してバッファを作り直す。
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
*@brief			行の頭の文字を確認する
*
*@param			row		確認したい行
*
*@return		確認行の先頭文字
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
*@brief		ロードしたファイルから指定した行の文字列を表示する
*
*@param		sx			文字の左端の座標
*@param		sy			文字の右端の座標
*
*/

void DrawChar(PBmpChar p_this, int sx, int sy, RGBQUAD letter_color, RGBQUAD background_color)
{
	int pn = 0;

	if (p_this->wch == '\r' || p_this->wch == '\n' || p_this->wch == 65535)//エスケープ文字はスルー
	{
		for (int y = 0; y < p_this->size.Y; y++)
		{
			for (int x = 0; x < p_this->size.X; x++)
			{
				int xp = (sx + x);	//次の横方向位置
				int yp = (sy + y);
				if ((xp >= 0) && (xp < ScreenWidth()) && (yp >= 0) && (yp < ScreenHeight()))
				{

					Draw(xp, yp, { 0,0,0,0 });


				}
				pn++;	//次のピクセル読み出し位置
			}
		}
	}
	else if (p_this->wch == '\0')
	{
		for (int y = 0; y < 15; y++)
		{
			for (int x = 0; x < 6; x++)
			{
				int xp = (sx + x);	//次の横方向位置
				int yp = (sy + y);
				if ((xp >= 0) && (xp < ScreenWidth()) && (yp >= 0) && (yp < ScreenHeight()))
				{

					Draw(xp, yp, { 0,0,0,0 });


				}
				pn++;	//次のピクセル読み出し位置
			}
		}
	}
	else
	{
		for (int y = 0; y < p_this->size.Y; y++)
		{
			for (int x = 0; x < p_this->size.X; x++)
			{
				int xp = (sx + x);	//次の横方向位置
				int yp = (sy + y);
				if ((xp >= 0) && (xp < ScreenWidth()) && (yp >= 0) && (yp < ScreenHeight()))
				{
					switch ((UINT)p_this->pPix[pn])//１ピクセル書き込む
					{
					case 0:Draw(xp, yp, background_color); break;//黒
					case 15:Draw(xp, yp, letter_color); break;//白(文字)
					default:break;
					}

				}
				pn++;	//次のピクセル読み出し位置
			}
		}
	}

}
/*
*@brief		ロードしたファイルから指定した行の文字列をRPGログのように表示する
*
*@param		p_this		メッセージのポインタ
*@param		sx			文字の左端の座標
*@param		sy			文字の右端の座標
*@param		row			行数(0はじまり)
*@param		sec			一文字当たりの表示秒数
*
*/
BOOL DrawStringLog(PMessage p_this, int const sx, int const sy, int row, RGBQUAD letter_color, RGBQUAD background_color, float sec)
{
	int xpos = 0;
	int ypos = 0;

	//秒数カウント
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

		////シーク後空欄行ならストップ
		//if (p_this->script_buffer[charCount].wch == '\r' && p_this->script_buffer[charCount + 1].wch == '\n')
		//{
		//	charCount = 0;
		//	seekSet = FALSE;
		//	return;
		//}

		seekSetPos = charCount;//シーク直後の文字数番目を保存
		seekSet = TRUE;


	}





	if (msgTime >= sec)
	{

		if (LogGoing)charCount++;

		//文字チェック

		msgTime = 0.0f;
	}

	for (int i = seekSetPos; i <= charCount; i++)//行数シーク後から現在の文字まで表示する
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
			//ログのエンドドットの描画
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
*@brief		DrawStringLogに必要な変数の初期化を行う
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