#pragma once



#include"YSConsoleGameEngine.h"

	//	BYTE：符号なし８ビット整数型（０～２５５）（0x00～0xFF）
	//	PBYTE：BYTE型へのポインタ
	typedef unsigned char	BYTE, *PBYTE;

	//フレームバッファ操作用構造体の定義
	//	FRAMEBUFFER構造体
	//	PFRAMEBUFFER構造体へのポインタ
	typedef struct {
		BITMAPFILEHEADER	bfh;	//ビットマップファイルヘッダ
		BITMAPINFOHEADER	bih;	//ビットマップ画像情報
		//パレット部分
		int	pal_count;	//パレットの色数（１６色）
		RGBQUAD*	ppal;	//パレット配列（RGBQUAD型配列のポインタ）
		//画像ピクセル部分
		DWORD	ppx_size;	//フレームバッファのサイズ（バイト数）
		PBYTE	ppx;	//フレームバッファ本体（１バイト／ピクセル）
		//PBYTE	pixel_data;	//フレームバッファ本体（１バイト／ピクセル）
		//画像ピクセルを使いやすい形式に変換したデータ部分
		LONG	ppx_stride;	//３２ビットアライメント換算の横方向バイト数
		int bypp;			//１ピクセル当たりのバイト数(bpp(Byte Per Pixel))
		int	width;	//画像幅（ピクセル）
		int	height;	//画像高（ピクセル）
		FILE*	fp;
		char*	file_path;	//読み込んだファイルパス
	} FRAMEBUFFER, *PFRAMEBUFFER;

	//BITMAPデータの読み込み
	PFRAMEBUFFER bmp_load(const char* file_path);
	//ピクセルのRGB並びの並べ替えREDとBLUEを交換
	PBYTE bmp_swapRB(PFRAMEBUFFER pfb);
	//２４ビット画像を３２ビット画像に変換
	PBYTE bmp_conv2432(PFRAMEBUFFER pfb);
	//画像データ（FRAMEBUFFER）の使用終了
	void bmp_end(PFRAMEBUFFER pfb);
	//画像データをファイルに保存
	PFRAMEBUFFER bmp_save(const char* file_patgh, int width, int height, const char* ppx, RGBQUAD* palette_table);
	//==== HEX-DUMP ====
	//BITMAPファイル情報のダンプ表示
	void  bmp_info(PFRAMEBUFFER pfb);
	//パレットデータをダンプ表示
	void bmp_pal_dump(PFRAMEBUFFER pfb);



