#include "FileLoader.h"




	typedef struct tagBMP_ERR {
		char* p_msg = NULL;
		struct tagBMP_ERR* p_next;
	} BMP_ERR;
	BMP_ERR* g_pBmpErr = NULL;
	BMP_ERR* g_pBmpErrRoot = NULL;
	/*
		エラー情報：初期化
	*/
	void bmp_err_init(void)
	{
		g_pBmpErr = (BMP_ERR*)calloc(1, sizeof(BMP_ERR));
		g_pBmpErrRoot = g_pBmpErr;
	}	//bmp_err_init
	/*
		エラー情報：追加
	*/
	void bmp_err_add(const char* msg_1, const char* msg_2)
	{
		if (g_pBmpErr == NULL) {
			return;
		}
		g_pBmpErr->p_next = (BMP_ERR*)calloc(1, sizeof(BMP_ERR));
		g_pBmpErr = g_pBmpErr->p_next;
		//message.
		int buff_len = strlen(msg_1) + (strlen(msg_2) + 1);
		g_pBmpErr->p_msg = (char*)malloc(buff_len);
		strcpy_s(g_pBmpErr->p_msg, buff_len, msg_1);
		strcat_s(g_pBmpErr->p_msg, buff_len, msg_2);
	}	//bmp_err_add
	/*
		エラー情報：終了
	*/
	void bmp_err_end(void)
	{
		BMP_ERR* p = g_pBmpErr;
		while (p != NULL) {
			BMP_ERR* next = p->p_next;
			SAFE_FREE(p->p_msg);
			p->p_next = NULL;
			free(p);
			p = next;
		}
		g_pBmpErr = NULL;
	}	//bmp_err_end


	//================================================================
	//	ビットマップファイル操作ユーティリティー
	//================================================================
	//--------------------------------
	//画像のビット解像度別変換関数。
	//----------------------------------------------------------------
	/*
		1bpp
		1bit / 1pixel
	*/
	PBYTE print_line_1bpp(int w_pixels, PBYTE dst_pxbuf, int stride, PBYTE src_ppx)
	{
		for (int x = 0; x < stride; x++) {
			for (int mask8 = 0b10000000; mask8 != 0; mask8 >>= 1) {
				*dst_pxbuf = (src_ppx[x] & mask8) ? 0x01 : 0x00;
				dst_pxbuf++;
				w_pixels--;
				if (w_pixels <= 0) {
					return dst_pxbuf;
				}
			}
		}
		return dst_pxbuf;
	}	//print_line_1bpp
	//----------------------------------------------------------------
	/*
		4bpp
		4bit / 1pixel
	*/
	PBYTE print_line_4bpp(int w_pixels, PBYTE dst_pxbuf, int stride, PBYTE src_ppx)
	{
		for (int x = 0; x < stride; x++) {
			*dst_pxbuf = (src_ppx[x] >> 4) & 0x0F;
			dst_pxbuf++;
			w_pixels--;
			if (w_pixels <= 0) {
				return dst_pxbuf;
			}
			*dst_pxbuf = src_ppx[x] & 0x0F;
			dst_pxbuf++;
			w_pixels--;
			if (w_pixels <= 0) {
				return dst_pxbuf;
			}
		}
		return dst_pxbuf;
	}	//print_line_4bpp
	//----------------------------------------------------------------
	/*
		8bpp
		8bit / 1pixel
	*/
	PBYTE print_line_8bpp(int w_pixels, PBYTE dst_pxbuf, int stride, PBYTE src_ppx)
	{
		for (int x = 0; x < w_pixels; x++) {
			*dst_pxbuf = src_ppx[x];
			dst_pxbuf++;
		}
		return dst_pxbuf;
	}	//print_line_8bpp
	//----------------------------------------------------------------
	/*
		16bpp
		16bit / 1pixel
		WORD{0b0rrrrrgggggbbbbb}
		0x07,0x29 -> 0x2907 -> 0b0010_1001_0000_0111 -> 0b0_01010_01000_00111 -> 0A,08,07 -> (0A*FF)/1F,(08*FF)/1F,(07*FF)/1F -> 52,41,39
	*/
	PBYTE print_line_16bpp(int w_pixels, PBYTE dst_pxbuf, int stride, PBYTE src_ppx)
	{
		WORD* dwpx = (WORD*)dst_pxbuf;
		WORD* swp = (WORD*)src_ppx;
		for (int x = 0; x < w_pixels; x++) {
			*dwpx = swp[x];
			dwpx++;
		}
		return (PBYTE)dwpx;
	}	//print_line_16bpp
	//----------------------------------------------------------------
	/*
		24bpp
		24bit / 1pixel
		[BB][GG][RR][??] -> DWORD{0x??RRGGBB & 0x00FFFFFF} -> (DWORD)0x00RRGGBB -> 0xRRGGBB
	*/
	PBYTE print_line_24bpp(int w_pixels, PBYTE dst_pxbuf, int stride, PBYTE src_ppx)
	{
		for (int x = 0; x < w_pixels; x++, src_ppx += 3) {
			dst_pxbuf[0] = src_ppx[0];	//BB
			dst_pxbuf[1] = src_ppx[1];	//GG
			dst_pxbuf[2] = src_ppx[2];	//RR
			dst_pxbuf += 3;
		}
		return dst_pxbuf;
	}	//print_line_24bpp
	//----------------------------------------------------------------
	/*
		32bpp
		32bit / 1pixel
		DWORD{0x00RRGGBB}
	*/
	PBYTE print_line_32bpp(int w_pixels, PBYTE dst_pxbuf, int stride, PBYTE src_ppx)
	{
		DWORD* ddwpx = (DWORD*)dst_pxbuf;
		DWORD* sdwp = (DWORD*)src_ppx;
		for (int x = 0; x < w_pixels; x++) {
			*ddwpx = sdwp[x];
			ddwpx++;
		}
		return (PBYTE)ddwpx;
	}	//print_line_32bpp
	//----------------------------------------------------------------
	/*
		解像度別表示関数のポインタテーブル（配列）
		void (*fp)();	//関数ポインタ
		fp();	//呼出
		void (*fpp[])();	//関数ポインタ配列
		fpp[n]();	//呼出
	ｇ*/
	PBYTE(*print_line[])(int, PBYTE, int, PBYTE) = {
		print_line_1bpp,
		print_line_4bpp,
		print_line_8bpp,
		print_line_16bpp,
		print_line_24bpp,
		print_line_32bpp
	};
	//----------------------------------------------------------------
	/*
		関数配列インデックス用：列挙子
	*/
	enum FUNC_NUM {
		BPP_1 = 0,
		BPP_4,
		BPP_8,
		BPP_16,
		BPP_24,
		BPP_32
	};

	//----------------------------------------------------------------
	/*
		PFRAMEBUFFER bmp_load(const char* file_path)
		ビットマップ操作：読み込み：各種ビット幅の画像データに対応している（１ビット～３２ビット画像）
		ビットマップファイルを読み込んでビットマップ情報をFRAMEBUFFER構造体にセットして返す
		引数：
			const char* file_path：ビットマップ(*.bmp)ファイルのファイルパス
		戻り値：
			PFRAMEBUFFER：確保したFRAMEBUFFER構造体へのポインタ
				失敗の場合はNULLポインタ（値０）を返す
		概略：
			・返されるFRAMEBUFFER構造体にはBITMAPファイルのヘッダ情報が入っている
			・幅と高さは扱いやすい様にwidth,heightに入っている
			・パレット情報があればRGBQUADの配列とパレット数が入っている（パレットデータへのポインタ）
			・画像（ピクセル）データが変換されて入っている（画像データへのポインタ）
			・１～４ビット画像は１ピクセル１バイトに変換して入っている
			　例：４ビット画像の0xF5（２ピクセル分）は0x0F,0x05（２ピクセル分）の２バイトに変換されている
	*/
	PFRAMEBUFFER bmp_load(const char* file_path)
	{
		//BMP構造体のメモリ領域を確保
		PFRAMEBUFFER pfb = (PFRAMEBUFFER)calloc(1, sizeof(FRAMEBUFFER));	//callocで確保しているので領域は０クリア済み(^_^）
		if (!pfb) {
			//NULLポインタ（値０）が返ってきたら失敗(;_;)
			return pfb;	//BMP構造体の確保失敗
		}
		//BMP構造体にファイル名をコピー
		int path_len = strlen(file_path) + 1;	//パス文字列長(+1は'\0'の分)
		//パス名文字列のコピー先メモリ領域をを確保
		pfb->file_path = (char*)calloc(path_len, sizeof(char));
		if (!pfb->file_path) {
			//NULLポインタ（値０）が返ってきたら失敗(;_;)
			free(pfb);	//失敗したのでFRAMEBUFFERは解放する
			return (PFRAMEBUFFER)NULL;	//パス名文字列領域の確保失敗
		}
		//確保した領域にパス名文字列をコピーする
		strcpy_s(pfb->file_path, path_len, file_path);
		//-------------------------------------------------------------
		//BMPファイルを開く（ファイルオープン）
		pfb->fp = fopen(pfb->file_path, "rb");	//リード・バイナリモード。
		if (!pfb->fp) {
			bmp_end(pfb);
			return (PFRAMEBUFFER)NULL;
		}
		//BITMAPFILEHEADER(14byte)の部分を読み込む。
		//	WORD	bfType;			//0x4D42(文字コード'B'と'M'が入っている。WORD（16ビット値）で見ると)
		//	DWORD	bfSize;			//BMPファイルのバイト数(131190)
		//	WORD	bfReserved1;	//予約（未使用部分）
		//	WORD	bfReserved2;	//予約（未使用部分）
		//	DWORD	bfOffBits;		//ピクセル部分までのバイト数
		int er = fread(&pfb->bfh, sizeof(BYTE), sizeof(BITMAPFILEHEADER), pfb->fp);
		if (!er) {
			bmp_end(pfb);
			//NULLポインタ（値０）が返ってきたら失敗(;_;)
			return (PFRAMEBUFFER)NULL;
		}
		//BITMAPINFOHEADER(40byte)部分を読み込む
		//以下のメンバ変数の詳細は：MSのドキュメント<"https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapinfoheader">参照
		//	DWORD	   biSize;			//この構造体（BITMAPINFOHEADER）のサイズ（バイト数）
		//	LONG	   biWidth;			//この画像の幅（ピクセル数）
		//	LONG	   biHeight;		//この画像の高さ（ピクセル数）※プラス（＋）値なら「ボトムアップ（左下隅から）」マイナス（－）値なら「トップダウン（左上隅から）」でピクセルが並んでいる
		//	WORD	   biPlanes;		//常に１
		//	WORD	   biBitCount;		//１ピクセルあたりのビット数（bpp)
		//	DWORD	   biCompression;	//※bmp_utlでは圧縮形式は扱っていないので、非圧縮形式のBI_RGBだけ扱っている（BI_BITFIELDSもあるが一般的に使われていないので非対応）
		//	DWORD	   biSizeImage;		//非圧縮RGBビットマップの場合は0に出来るので、値が入っていても参照しない。biSizeImage の正しい値は biWidth，biHeight，biBitCount から計算できる
		//	LONG	   biXPelsPerMeter;	//水平解像度：単位は1m当たりのがぞ数（画素数/ｍ）※０の場合もあるので参照しない
		//	LONG	   biYPelsPerMeter;	//垂直解像度：単位は1m当たりのがぞ数（画素数/ｍ）※０の場合もあるので参照しない
		//	DWORD	   biClrUsed;		//パレットの数（カラーテーブル数）：０ならbiBitCountのビット数で表現できる最大値がテーブル数となる※詳細はMSのドキュメント参照
		//	DWORD	   biClrImportant;	//この画像を表示するのに必要な色数（パレット数）０なら全ての色が必要※bmp_utlではこの値は無視して全色必要としている
		er = fread(&pfb->bih, sizeof(BYTE), sizeof(BITMAPINFOHEADER), pfb->fp);
		if (!er) {
			bmp_end(pfb);
			//NULLポインタ（値０）が返ってきたら失敗(;_;)
			return (PFRAMEBUFFER)NULL;
		}

		///*
		//パレット位置に読み出し位置を移動する
		//*/
		//er = fseek(pfb->fp, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), SEEK_SET);
		//if (!er) {
		//	//bmp_end(pfb);
		//	//return (PFRAMEBUFFER)NULL;
		//}

		//パレット（カラーテーブル）が存在すればその領域を確保して読み込む。
		//パレットがあるかないかはbiBitCountを見て判断する
		pfb->pal_count = 0;	//判断前はパレット無し（０）にしておく
		pfb->ppal = (RGBQUAD*)NULL;	//判断前はパレット無し（NULL）にしておく
		//１～８はパレット在り、それ以外は1ピクセル＝16ビット｜24ビット｜32ビット
		if (pfb->bih.biBitCount >= 1 && pfb->bih.biBitCount <= 8) {
			//１bit：２色、４bit：１６色、８bit：２５６色
			pfb->pal_count = (1 << pfb->bih.biBitCount);	//このビット数分左すれば必要な最大パレット数になる
			//RGB値を表すRGBQUAD値の配列をパレット数の数だけ確保する
			//１色はDWORDでバイト並び順は[B][G][R][A]として格納されている
			//１色のデータをRGBQUAD構造体として読み込む※リトルエンディアンで格納されているのでDWORD型で読み込むと0xAARRGGBBとなる
			COLORREF cref;
			pfb->ppal = (RGBQUAD*)calloc(pfb->pal_count, sizeof(RGBQUAD));
			if (!pfb->ppal) {
				//NULLポインタ（値０）が返ってきたら失敗(;_;)
				bmp_end(pfb);
				return (PFRAMEBUFFER)NULL;
			}
			//確保したサイズ分パレットデータ（RGBQUAD×パレット数）をファイルから読み込む。
			int er = fread(pfb->ppal, pfb->pal_count, sizeof(RGBQUAD), pfb->fp);
			if (!er) {
				bmp_end(pfb);
				return (PFRAMEBUFFER)NULL;
			}
		}
		//残りはピクセル値なので、残りを全部読み込む
		//BITMAPFILEHEADER＋BITMAPINFOHEADER（＋パレット配列）部分を除く残りのサイズ分を全て読み込んでファイルを閉じる。
		DWORD pixel_data_size = (pfb->bfh.bfSize - pfb->bfh.bfOffBits);
		PBYTE pixel_data = (PBYTE)calloc(pixel_data_size, sizeof(BYTE));
		if (!pixel_data) {
			//NULLポインタ（値０）が返ってきたら失敗(;_;)
			bmp_end(pfb);
			return (PFRAMEBUFFER)NULL;
		}
		//pfb->ppx_size = pixel_data_size;
		er = fread(pixel_data, sizeof(BYTE), pixel_data_size, pfb->fp);
		if (!er) {
			free(pixel_data);
			bmp_end(pfb);
			return (PFRAMEBUFFER)NULL;
		}
		//ファイルの読み込みは終了したのでファイルは閉じておく。
		if (pfb->fp) {
			fclose(pfb->fp);
			pfb->fp = NULL;
		}
		//画像ピクセル部分を扱いやすいデータに変換：
		//	「１～４ビット／ピクセル」＝＞「１バイト／ピクセル」
		//	「８ビット／ピクセル」＝＞そのまま
		//
		PBYTE tmp_px_data = pixel_data;// + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*pfb->pal_count);	//画像データの先頭にする。
		//(((biWidth * biBitCount + 31)& ~31)>>3) : ＋３１(1F)と＆~３１(E0)で０～３１ビットの値は切り上げて、>>3でビット数をバイト数にする。
		pfb->ppx_stride = (((pfb->bih.biWidth * pfb->bih.biBitCount + 31)& ~31) >> 3);
		LONG height = pfb->bih.biHeight;
		//
		int top_down = (height < 0);	//負数の場合はトップダウン。
		LONG add_stride = pfb->ppx_stride;	//Line毎に加算する値。1行分（4バイトアライメント）
		if (top_down) {
			//負の数はtop-down DIB.
			height = (-height);	//トップダウンなので正の数に補正。
		}
		else {
			//正の数はbottom-up DIB.
			tmp_px_data += (pfb->ppx_stride * (height - 1));	//一番下の行先頭にポイント。
			add_stride = (-add_stride);	//下から上に１行づつ減算してゆく。
		}
		//画素（ピクセル）のビット数に対応した変換用の関数ポインタを選択する。
		DWORD bpp = pfb->bih.biBitCount;
		FUNC_NUM	bpp_num = BPP_1;
		switch (bpp) {
		case	1:	bpp_num = BPP_1;	break;
		case	4:	bpp_num = BPP_4;	break;
		case	8:	bpp_num = BPP_8;	break;
		case	16:	bpp_num = BPP_16;	break;
		case	24:	bpp_num = BPP_24;	break;
		case	32:	bpp_num = BPP_32;	break;
		default:	bpp_num = BPP_8;	break;
		}
		pfb->bypp = (bpp <= 8) ? 1 : bpp / 8;	//１ピクセル当たりのバイト数
		//(幅 × 高さ × 1ピクセル当たりのバイト数（但し8ビット以下は全て1バイト扱い））の領域を確保し、変換してから格納する
		pfb->ppx_size = (pfb->bih.biWidth * height * pfb->bypp);
		//pixel_data = (PBYTE)calloc(pfb->ppx_size, sizeof(BYTE));
		//PBYTE pixcel_buffer = pixel_data;
		//PBYTE pixel_buffer = (PBYTE)calloc(pfb->ppx_size, sizeof(BYTE));
		pfb->ppx = (PBYTE)calloc(pfb->ppx_size, sizeof(BYTE));
		PBYTE tmp_ppx = pfb->ppx;
		if (!tmp_ppx) {
			//NULLポインタ（値０）が返ってきたら失敗(;_;)
			free(pixel_data);
			bmp_end(pfb);
			return (PFRAMEBUFFER)NULL;
		}
		//１ラインづつ画素（ピクセル）のビット数に対応したデータに変換しながらコピーする。
		for (int y = 0; y < height; y++) {
			//(int w_pixels, PBYTE dst_pxbuf, int stride, PBYTE src_ppx)
			print_line[bpp_num](pfb->bih.biWidth, tmp_ppx, pfb->ppx_stride, tmp_px_data);
			tmp_ppx += (pfb->bih.biWidth * pfb->bypp);
			tmp_px_data += add_stride;
		}
		//変換し終わったので元データは解放する
		free(pixel_data);
		//画像のサイズ（ピクセル単位）を使いやすい様に別の変数にコピーする
		pfb->width = pfb->bih.biWidth;
		pfb->height = pfb->bih.biHeight;
		//正常に読み込めたのでFRAMEBUFFER構造体へのポインタを返す
		return pfb;
	}	//bmp_load
	//----------------------------------------------------------------
	/*
		画像（ピクセル）データの変換：
			３原色データ位置の並べ替え
			RGB -> BGR / BGR -> RGB
	*/
	PBYTE bmp_swapRB(PFRAMEBUFFER pfb) {
		if (pfb->bypp != 3) {
			return (PBYTE)NULL;	//24ビット／ピクセルじゃない。
		}
		for (int n = 0; n < pfb->ppx_size; n += 3) {
			BYTE tmp = pfb->ppx[n + 2];	//R
			pfb->ppx[n + 2] = pfb->ppx[n + 0];	//<- B
			pfb->ppx[n + 0] = tmp;	//<- R
		}
		return pfb->ppx;
	}
	//----------------------------------------------------------------
	/*
		画像（ピクセル）データの変換：
			24ビット画像を32ビット画像に変換する
			BGR:24bit -> RGBA:32bit
	*/
	PBYTE bmp_conv2432(PFRAMEBUFFER pfb) {
		if (pfb->bypp != 3) {
			return (PBYTE)NULL;	//24ビット／ピクセルじゃない。
		}
		PBYTE pix = (PBYTE)calloc(pfb->bih.biWidth * pfb->bih.biHeight, sizeof(RGBQUAD));
		PBYTE tmp_p = pix;
		for (int n = 0; n < pfb->ppx_size; n += 3) {
			tmp_p[0] = pfb->ppx[n + 0];	//B
			tmp_p[1] = pfb->ppx[n + 1];	//G
			tmp_p[2] = pfb->ppx[n + 2];	//R
			tmp_p += sizeof(RGBQUAD);
		}
		free(pfb->ppx);
		pfb->ppx = pix;
		return pfb->ppx;
	}
	//----------------------------------------------------------------
	/*
		ビットマップ操作：終了
		使ったメモリ領域を解放する
		ファイルを閉じる
		引数：PFRAMEBUFFER pfb：FRAMEBUFFERへのポインタ
		戻り値：無し
	*/
	void bmp_end(PFRAMEBUFFER pfb)
	{
		if (!pfb) {
			return;
		}
		if (pfb->fp) {
			fclose(pfb->fp);
			pfb->fp = NULL;
		}
		
		SAFE_FREE(pfb->ppal);//パレット領域（Pallette buffer）を解放
		SAFE_FREE(pfb->ppx);//ピクセル領域（Pixel buffer）を解放
		SAFE_FREE(pfb->file_path);//ファイルパス名領域（File path）を解放
		SAFE_FREE(pfb);//BMP構造体領域（BMP）を解放

	}	//bmp_end

	//----------------------------------------------------------------
	/*
		画像データをBMPファイルへ保存する
		pfb->ppx:画像データへのポインタ
		pfb->width:幅（ピクセル）
		pfb->height:高さ（ピクセル）
		pfb->ppal:16色パレット（色数は16固定）
	*/
	PFRAMEBUFFER bmp_save(const char* file_path, int width, int height, const char* ppx, RGBQUAD* palette_table)
	{
		//
		FILE* fp = fopen(file_path, "wb");
		if (!fp) {
			return (PFRAMEBUFFER)NULL;
		}
		//BITMAPFILEHEADER(14byte)の部分を読み込む。
		//	WORD	bfType;			//0x4D42(文字コード+0:'B'(0x42)と+1:'M'(0x4D)が入っている。WORD（16ビット値）で見ると'0x4D42')
		//	DWORD	bfSize;			//BMPファイルのバイト数(131190)
		//	WORD	bfReserved1;	//予約（未使用部分）
		//	WORD	bfReserved2;	//予約（未使用部分）
		//	DWORD	bfOffBits;		//ピクセル部分までのバイト数
		BITMAPFILEHEADER	bfh;
		ZeroMemory(&bfh, sizeof(BITMAPFILEHEADER));
		bfh.bfType = 0x4D42;
		bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 16);
		bfh.bfSize = bfh.bfOffBits + (width*height);
		//BITMAPINFOHEADER(40byte)部分を読み込む
		//	DWORD	   biSize;			//この構造体（BITMAPINFOHEADER）のサイズ（バイト数）
		//	LONG	   biWidth;			//この画像の幅（ピクセル数）
		//	LONG	   biHeight;		//この画像の高さ（ピクセル数）※プラス（＋）値なら「ボトムアップ（左下隅から）」マイナス（－）値なら「トップダウン（左上隅から）」でピクセルが並んでいる
		//	WORD	   biPlanes;		//常に１
		//	WORD	   biBitCount;		//１ピクセルあたりのビット数（bpp)
		//	DWORD	   biCompression;	//※bmp_utlでは圧縮形式は扱っていないので、非圧縮形式のBI_RGBだけ扱っている（BI_BITFIELDSもあるが一般的に使われていないので非対応）
		//	DWORD	   biSizeImage;		//非圧縮RGBビットマップの場合は0に出来るので、値が入っていても参照しない。biSizeImage の正しい値は biWidth，biHeight，biBitCount から計算できる
		//	LONG	   biXPelsPerMeter;	//水平解像度：単位は1m当たりのがぞ数（画素数/ｍ）※０の場合もあるので参照しない
		//	LONG	   biYPelsPerMeter;	//垂直解像度：単位は1m当たりのがぞ数（画素数/ｍ）※０の場合もあるので参照しない
		//	DWORD	   biClrUsed;		//パレットの数（カラーテーブル数）：０ならbiBitCountのビット数で表現できる最大値がテーブル数となる※詳細はMSのドキュメント参照
		//	DWORD	   biClrImportant;	//この画像を表示するのに必要な色数（パレット数）０なら全ての色が必要※bmp_utlではこの値は無視して全色必要としている
		BITMAPINFOHEADER	bih;
		ZeroMemory(&bih, sizeof(BITMAPINFOHEADER));
		bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biWidth = width;
		bih.biHeight = height;
		bih.biPlanes = 1;
		bih.biBitCount = 8;
		bih.biClrUsed = 16;
		//
		int er;
		if (er = fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fp)) {
			if (er = fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fp)) {
				if (er = fwrite(palette_table, sizeof(RGBQUAD), 16, fp)) {
					er = fwrite(ppx, sizeof(char), width*height, fp);
				}
			}
		}
		if (!er) {
			printf("Bitmap write error:%s\n", file_path);
		}
		fclose(fp);
		//
		return (PFRAMEBUFFER)1;
	}
	//----------------------------------------------------------------
	/*
		パレットデータの表示
	*/
	void bmp_pal_dump(PFRAMEBUFFER pfb)
	{
		if (pfb) {
			for (int n = 0; n < pfb->pal_count; n++)
			{
				RGBQUAD rgbq = pfb->ppal[n];
				printf("{0x%02x,", rgbq.rgbBlue);
				printf("0x%02x,", rgbq.rgbGreen);
				printf("0x%02x,", rgbq.rgbRed);
				printf("0x%02x},\n", rgbq.rgbReserved);
				//if (((n + 1) % 16) == 0) {
				//	putchar('\n');
				//}
			}
		}
	}
	//----------------------------------------------------------------
	/*
		ビットマップ情報の表示
			BITMAPヘッダ情報の表示
			パレットデータのHEXダンプ
			画像データのHEXダンプ
	*/
	void bmp_info(PFRAMEBUFFER pfb)
	{
		//BITMAPINFO:
		//	BITMAPINFOHEADER	bmiHeader;
		//	RGBQUAD	bmiColors[1];
		//BITMAPFILEHEADERとRGBQUAD[]のパレットの先頭（1個だけ定義されている）
		printf("---BITMAPFILEHEADER(%d)---\n", sizeof(BITMAPFILEHEADER));
		for (int n = 0; n < sizeof(BITMAPFILEHEADER); n++) {
			printf("%02X,", ((BYTE*)&pfb->bfh)[n]);
			if (((n + 1) % 16) == 0) {
				putchar('\n');
			}
		}
		putchar('\n');
		printf("bfType:%04X\n", pfb->bfh.bfType);
		printf("bfSize:%d\n", pfb->bfh.bfSize);
		printf("bfReserved1:%d\n", pfb->bfh.bfReserved1);
		printf("bfReserved2:%d\n", pfb->bfh.bfReserved2);
		printf("bfOffBits:%d\n", pfb->bfh.bfOffBits);
		//	DWORD	   biSize;	//
		//	LONG	   biWidth;
		//	LONG	   biHeight;
		//	WORD	   biPlanes;
		//	WORD	   biBitCount;
		//	DWORD	   biCompression;
		//	DWORD	   biSizeImage;
		//	LONG	   biXPelsPerMeter;
		//	LONG	   biYPelsPerMeter;
		//	DWORD	   biClrUsed;
		//	DWORD	   biClrImportant;
		printf("---BITMAPINFO(%d)---\n", sizeof(BITMAPINFO));
		for (int n = 0; n < sizeof(BITMAPINFO); n++) {
			printf("%02X,", ((PBYTE)&pfb->bih)[n]);
			if (((n + 1) % 16) == 0) {
				putchar('\n');
			}
		}
		putchar('\n');
		printf("biSize:%d\n", pfb->bih.biSize);	//個のヘッダのサイズ
		printf("biWidth:%d\n", pfb->bih.biWidth);	//幅
		printf("biHeight:%d\n", pfb->bih.biHeight);	//高さ
		printf("biPlanes:%d\n", pfb->bih.biPlanes);	//プレーン数
		printf("biBitCount:%d\n", pfb->bih.biBitCount);	//１画素のビット数
		printf("biCompression:%d\n", pfb->bih.biCompression);	//圧縮形式
		printf("biSizeImage:%d\n", pfb->bih.biSizeImage);	//画像部分のサイズ
		printf("biXPelsPerMeter:%d\n", pfb->bih.biXPelsPerMeter);
		printf("biYPelsPerMeter:%d\n", pfb->bih.biYPelsPerMeter);
		printf("biClrUsed:%d\n", pfb->bih.biClrUsed);
		printf("biClrImportant:%d\n", pfb->bih.biClrImportant);
		//int ctbl_siz = pfb->bfh.bfOffBits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);	//color table size.
		//printf("ColorTable-size:%d = (FILE-OFF:%d - (BFH:%d + BIH:%d))\n", ctbl_siz, pfb->bfh.bfOffBits, sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));
		//
		//色数判定。
		//パレット表示。
		printf("--- COLOR PALLET #RRGGBB ---\n");
		for (int n = 0; n < pfb->pal_count; n++) {
			if (((n % 8) == 0) && (n != 0)) {
				putchar('\n');
			}
			printf("0x%02X%02X%02X,", pfb->ppal[n].rgbRed, pfb->ppal[n].rgbGreen, pfb->ppal[n].rgbBlue);
		}
		putchar('\n');
		//圧縮形式は扱っていない。
		if (pfb->bih.biCompression != BI_RGB) {
			printf("Not support : Compression format.");
			return;
		}
		//補完の入った幅を表示
		printf("stride : %d Bytes\n", pfb->ppx_stride);
		//ピクセル部分をバイト／ピクセルの単位で表示
		BYTE* pixcel_buffer = pfb->ppx;
		for (int y = 0; y < pfb->bih.biHeight; y++) {
			for (int lx = 0; lx < pfb->bih.biWidth; lx++) {
				if (pfb->bypp == 1) {
					//１ピクセル＝１バイト
					printf("%X,", pixcel_buffer[lx]);
				}
				else if (pfb->bypp == 2) {
					//１ピクセル＝２バイト
					printf("%04X,", ((WORD*)pixcel_buffer)[lx]);
				}
				else if (pfb->bypp == 3) {
					//１ピクセル＝３バイト
					printf("%06X,", *((DWORD*)(&pixcel_buffer[lx * 3])) & 0x00FFFFFF);
				}
				else if (pfb->bypp == 4) {
					//１ピクセル＝４バイト
					printf("%08X,", ((DWORD*)pixcel_buffer)[lx]);
				}
			}
			putchar('\n');
			pixcel_buffer += (pfb->bih.biWidth * pfb->bypp);
		}
	}	//bmp_info





