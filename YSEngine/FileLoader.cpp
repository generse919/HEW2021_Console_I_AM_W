#include "FileLoader.h"




	typedef struct tagBMP_ERR {
		char* p_msg = NULL;
		struct tagBMP_ERR* p_next;
	} BMP_ERR;
	BMP_ERR* g_pBmpErr = NULL;
	BMP_ERR* g_pBmpErrRoot = NULL;
	/*
		�G���[���F������
	*/
	void bmp_err_init(void)
	{
		g_pBmpErr = (BMP_ERR*)calloc(1, sizeof(BMP_ERR));
		g_pBmpErrRoot = g_pBmpErr;
	}	//bmp_err_init
	/*
		�G���[���F�ǉ�
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
		�G���[���F�I��
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
	//	�r�b�g�}�b�v�t�@�C�����샆�[�e�B���e�B�[
	//================================================================
	//--------------------------------
	//�摜�̃r�b�g�𑜓x�ʕϊ��֐��B
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
		�𑜓x�ʕ\���֐��̃|�C���^�e�[�u���i�z��j
		void (*fp)();	//�֐��|�C���^
		fp();	//�ďo
		void (*fpp[])();	//�֐��|�C���^�z��
		fpp[n]();	//�ďo
	��*/
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
		�֐��z��C���f�b�N�X�p�F�񋓎q
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
		�r�b�g�}�b�v����F�ǂݍ��݁F�e��r�b�g���̉摜�f�[�^�ɑΉ����Ă���i�P�r�b�g�`�R�Q�r�b�g�摜�j
		�r�b�g�}�b�v�t�@�C����ǂݍ���Ńr�b�g�}�b�v����FRAMEBUFFER�\���̂ɃZ�b�g���ĕԂ�
		�����F
			const char* file_path�F�r�b�g�}�b�v(*.bmp)�t�@�C���̃t�@�C���p�X
		�߂�l�F
			PFRAMEBUFFER�F�m�ۂ���FRAMEBUFFER�\���̂ւ̃|�C���^
				���s�̏ꍇ��NULL�|�C���^�i�l�O�j��Ԃ�
		�T���F
			�E�Ԃ����FRAMEBUFFER�\���̂ɂ�BITMAP�t�@�C���̃w�b�_��񂪓����Ă���
			�E���ƍ����͈����₷���l��width,height�ɓ����Ă���
			�E�p���b�g��񂪂����RGBQUAD�̔z��ƃp���b�g���������Ă���i�p���b�g�f�[�^�ւ̃|�C���^�j
			�E�摜�i�s�N�Z���j�f�[�^���ϊ�����ē����Ă���i�摜�f�[�^�ւ̃|�C���^�j
			�E�P�`�S�r�b�g�摜�͂P�s�N�Z���P�o�C�g�ɕϊ����ē����Ă���
			�@��F�S�r�b�g�摜��0xF5�i�Q�s�N�Z�����j��0x0F,0x05�i�Q�s�N�Z�����j�̂Q�o�C�g�ɕϊ�����Ă���
	*/
	PFRAMEBUFFER bmp_load(const char* file_path)
	{
		//BMP�\���̂̃������̈���m��
		PFRAMEBUFFER pfb = (PFRAMEBUFFER)calloc(1, sizeof(FRAMEBUFFER));	//calloc�Ŋm�ۂ��Ă���̂ŗ̈�͂O�N���A�ς�(^_^�j
		if (!pfb) {
			//NULL�|�C���^�i�l�O�j���Ԃ��Ă����玸�s(;_;)
			return pfb;	//BMP�\���̂̊m�ێ��s
		}
		//BMP�\���̂Ƀt�@�C�������R�s�[
		int path_len = strlen(file_path) + 1;	//�p�X������(+1��'\0'�̕�)
		//�p�X��������̃R�s�[�惁�����̈�����m��
		pfb->file_path = (char*)calloc(path_len, sizeof(char));
		if (!pfb->file_path) {
			//NULL�|�C���^�i�l�O�j���Ԃ��Ă����玸�s(;_;)
			free(pfb);	//���s�����̂�FRAMEBUFFER�͉������
			return (PFRAMEBUFFER)NULL;	//�p�X��������̈�̊m�ێ��s
		}
		//�m�ۂ����̈�Ƀp�X����������R�s�[����
		strcpy_s(pfb->file_path, path_len, file_path);
		//-------------------------------------------------------------
		//BMP�t�@�C�����J���i�t�@�C���I�[�v���j
		pfb->fp = fopen(pfb->file_path, "rb");	//���[�h�E�o�C�i�����[�h�B
		if (!pfb->fp) {
			bmp_end(pfb);
			return (PFRAMEBUFFER)NULL;
		}
		//BITMAPFILEHEADER(14byte)�̕�����ǂݍ��ށB
		//	WORD	bfType;			//0x4D42(�����R�[�h'B'��'M'�������Ă���BWORD�i16�r�b�g�l�j�Ō����)
		//	DWORD	bfSize;			//BMP�t�@�C���̃o�C�g��(131190)
		//	WORD	bfReserved1;	//�\��i���g�p�����j
		//	WORD	bfReserved2;	//�\��i���g�p�����j
		//	DWORD	bfOffBits;		//�s�N�Z�������܂ł̃o�C�g��
		int er = fread(&pfb->bfh, sizeof(BYTE), sizeof(BITMAPFILEHEADER), pfb->fp);
		if (!er) {
			bmp_end(pfb);
			//NULL�|�C���^�i�l�O�j���Ԃ��Ă����玸�s(;_;)
			return (PFRAMEBUFFER)NULL;
		}
		//BITMAPINFOHEADER(40byte)������ǂݍ���
		//�ȉ��̃����o�ϐ��̏ڍׂ́FMS�̃h�L�������g<"https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapinfoheader">�Q��
		//	DWORD	   biSize;			//���̍\���́iBITMAPINFOHEADER�j�̃T�C�Y�i�o�C�g���j
		//	LONG	   biWidth;			//���̉摜�̕��i�s�N�Z�����j
		//	LONG	   biHeight;		//���̉摜�̍����i�s�N�Z�����j���v���X�i�{�j�l�Ȃ�u�{�g���A�b�v�i����������j�v�}�C�i�X�i�|�j�l�Ȃ�u�g�b�v�_�E���i���������j�v�Ńs�N�Z��������ł���
		//	WORD	   biPlanes;		//��ɂP
		//	WORD	   biBitCount;		//�P�s�N�Z��������̃r�b�g���ibpp)
		//	DWORD	   biCompression;	//��bmp_utl�ł͈��k�`���͈����Ă��Ȃ��̂ŁA�񈳏k�`����BI_RGB���������Ă���iBI_BITFIELDS�����邪��ʓI�Ɏg���Ă��Ȃ��̂Ŕ�Ή��j
		//	DWORD	   biSizeImage;		//�񈳏kRGB�r�b�g�}�b�v�̏ꍇ��0�ɏo����̂ŁA�l�������Ă��Ă��Q�Ƃ��Ȃ��BbiSizeImage �̐������l�� biWidth�CbiHeight�CbiBitCount ����v�Z�ł���
		//	LONG	   biXPelsPerMeter;	//�����𑜓x�F�P�ʂ�1m������̂������i��f��/���j���O�̏ꍇ������̂ŎQ�Ƃ��Ȃ�
		//	LONG	   biYPelsPerMeter;	//�����𑜓x�F�P�ʂ�1m������̂������i��f��/���j���O�̏ꍇ������̂ŎQ�Ƃ��Ȃ�
		//	DWORD	   biClrUsed;		//�p���b�g�̐��i�J���[�e�[�u�����j�F�O�Ȃ�biBitCount�̃r�b�g���ŕ\���ł���ő�l���e�[�u�����ƂȂ遦�ڍׂ�MS�̃h�L�������g�Q��
		//	DWORD	   biClrImportant;	//���̉摜��\������̂ɕK�v�ȐF���i�p���b�g���j�O�Ȃ�S�Ă̐F���K�v��bmp_utl�ł͂��̒l�͖������đS�F�K�v�Ƃ��Ă���
		er = fread(&pfb->bih, sizeof(BYTE), sizeof(BITMAPINFOHEADER), pfb->fp);
		if (!er) {
			bmp_end(pfb);
			//NULL�|�C���^�i�l�O�j���Ԃ��Ă����玸�s(;_;)
			return (PFRAMEBUFFER)NULL;
		}

		///*
		//�p���b�g�ʒu�ɓǂݏo���ʒu���ړ�����
		//*/
		//er = fseek(pfb->fp, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), SEEK_SET);
		//if (!er) {
		//	//bmp_end(pfb);
		//	//return (PFRAMEBUFFER)NULL;
		//}

		//�p���b�g�i�J���[�e�[�u���j�����݂���΂��̗̈���m�ۂ��ēǂݍ��ށB
		//�p���b�g�����邩�Ȃ�����biBitCount�����Ĕ��f����
		pfb->pal_count = 0;	//���f�O�̓p���b�g�����i�O�j�ɂ��Ă���
		pfb->ppal = (RGBQUAD*)NULL;	//���f�O�̓p���b�g�����iNULL�j�ɂ��Ă���
		//�P�`�W�̓p���b�g�݂�A����ȊO��1�s�N�Z����16�r�b�g�b24�r�b�g�b32�r�b�g
		if (pfb->bih.biBitCount >= 1 && pfb->bih.biBitCount <= 8) {
			//�Pbit�F�Q�F�A�Sbit�F�P�U�F�A�Wbit�F�Q�T�U�F
			pfb->pal_count = (1 << pfb->bih.biBitCount);	//���̃r�b�g����������ΕK�v�ȍő�p���b�g���ɂȂ�
			//RGB�l��\��RGBQUAD�l�̔z����p���b�g���̐������m�ۂ���
			//�P�F��DWORD�Ńo�C�g���я���[B][G][R][A]�Ƃ��Ċi�[����Ă���
			//�P�F�̃f�[�^��RGBQUAD�\���̂Ƃ��ēǂݍ��ށ����g���G���f�B�A���Ŋi�[����Ă���̂�DWORD�^�œǂݍ��ނ�0xAARRGGBB�ƂȂ�
			COLORREF cref;
			pfb->ppal = (RGBQUAD*)calloc(pfb->pal_count, sizeof(RGBQUAD));
			if (!pfb->ppal) {
				//NULL�|�C���^�i�l�O�j���Ԃ��Ă����玸�s(;_;)
				bmp_end(pfb);
				return (PFRAMEBUFFER)NULL;
			}
			//�m�ۂ����T�C�Y���p���b�g�f�[�^�iRGBQUAD�~�p���b�g���j���t�@�C������ǂݍ��ށB
			int er = fread(pfb->ppal, pfb->pal_count, sizeof(RGBQUAD), pfb->fp);
			if (!er) {
				bmp_end(pfb);
				return (PFRAMEBUFFER)NULL;
			}
		}
		//�c��̓s�N�Z���l�Ȃ̂ŁA�c���S���ǂݍ���
		//BITMAPFILEHEADER�{BITMAPINFOHEADER�i�{�p���b�g�z��j�����������c��̃T�C�Y����S�ēǂݍ���Ńt�@�C�������B
		DWORD pixel_data_size = (pfb->bfh.bfSize - pfb->bfh.bfOffBits);
		PBYTE pixel_data = (PBYTE)calloc(pixel_data_size, sizeof(BYTE));
		if (!pixel_data) {
			//NULL�|�C���^�i�l�O�j���Ԃ��Ă����玸�s(;_;)
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
		//�t�@�C���̓ǂݍ��݂͏I�������̂Ńt�@�C���͕��Ă����B
		if (pfb->fp) {
			fclose(pfb->fp);
			pfb->fp = NULL;
		}
		//�摜�s�N�Z�������������₷���f�[�^�ɕϊ��F
		//	�u�P�`�S�r�b�g�^�s�N�Z���v�����u�P�o�C�g�^�s�N�Z���v
		//	�u�W�r�b�g�^�s�N�Z���v�������̂܂�
		//
		PBYTE tmp_px_data = pixel_data;// + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*pfb->pal_count);	//�摜�f�[�^�̐擪�ɂ���B
		//(((biWidth * biBitCount + 31)& ~31)>>3) : �{�R�P(1F)�Ɓ�~�R�P(E0)�łO�`�R�P�r�b�g�̒l�͐؂�グ�āA>>3�Ńr�b�g�����o�C�g���ɂ���B
		pfb->ppx_stride = (((pfb->bih.biWidth * pfb->bih.biBitCount + 31)& ~31) >> 3);
		LONG height = pfb->bih.biHeight;
		//
		int top_down = (height < 0);	//�����̏ꍇ�̓g�b�v�_�E���B
		LONG add_stride = pfb->ppx_stride;	//Line���ɉ��Z����l�B1�s���i4�o�C�g�A���C�����g�j
		if (top_down) {
			//���̐���top-down DIB.
			height = (-height);	//�g�b�v�_�E���Ȃ̂Ő��̐��ɕ␳�B
		}
		else {
			//���̐���bottom-up DIB.
			tmp_px_data += (pfb->ppx_stride * (height - 1));	//��ԉ��̍s�擪�Ƀ|�C���g�B
			add_stride = (-add_stride);	//�������ɂP�s�Â��Z���Ă䂭�B
		}
		//��f�i�s�N�Z���j�̃r�b�g���ɑΉ������ϊ��p�̊֐��|�C���^��I������B
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
		pfb->bypp = (bpp <= 8) ? 1 : bpp / 8;	//�P�s�N�Z��������̃o�C�g��
		//(�� �~ ���� �~ 1�s�N�Z��������̃o�C�g���i�A��8�r�b�g�ȉ��͑S��1�o�C�g�����j�j�̗̈���m�ۂ��A�ϊ����Ă���i�[����
		pfb->ppx_size = (pfb->bih.biWidth * height * pfb->bypp);
		//pixel_data = (PBYTE)calloc(pfb->ppx_size, sizeof(BYTE));
		//PBYTE pixcel_buffer = pixel_data;
		//PBYTE pixel_buffer = (PBYTE)calloc(pfb->ppx_size, sizeof(BYTE));
		pfb->ppx = (PBYTE)calloc(pfb->ppx_size, sizeof(BYTE));
		PBYTE tmp_ppx = pfb->ppx;
		if (!tmp_ppx) {
			//NULL�|�C���^�i�l�O�j���Ԃ��Ă����玸�s(;_;)
			free(pixel_data);
			bmp_end(pfb);
			return (PFRAMEBUFFER)NULL;
		}
		//�P���C���Â�f�i�s�N�Z���j�̃r�b�g���ɑΉ������f�[�^�ɕϊ����Ȃ���R�s�[����B
		for (int y = 0; y < height; y++) {
			//(int w_pixels, PBYTE dst_pxbuf, int stride, PBYTE src_ppx)
			print_line[bpp_num](pfb->bih.biWidth, tmp_ppx, pfb->ppx_stride, tmp_px_data);
			tmp_ppx += (pfb->bih.biWidth * pfb->bypp);
			tmp_px_data += add_stride;
		}
		//�ϊ����I������̂Ō��f�[�^�͉������
		free(pixel_data);
		//�摜�̃T�C�Y�i�s�N�Z���P�ʁj���g���₷���l�ɕʂ̕ϐ��ɃR�s�[����
		pfb->width = pfb->bih.biWidth;
		pfb->height = pfb->bih.biHeight;
		//����ɓǂݍ��߂��̂�FRAMEBUFFER�\���̂ւ̃|�C���^��Ԃ�
		return pfb;
	}	//bmp_load
	//----------------------------------------------------------------
	/*
		�摜�i�s�N�Z���j�f�[�^�̕ϊ��F
			�R���F�f�[�^�ʒu�̕��בւ�
			RGB -> BGR / BGR -> RGB
	*/
	PBYTE bmp_swapRB(PFRAMEBUFFER pfb) {
		if (pfb->bypp != 3) {
			return (PBYTE)NULL;	//24�r�b�g�^�s�N�Z������Ȃ��B
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
		�摜�i�s�N�Z���j�f�[�^�̕ϊ��F
			24�r�b�g�摜��32�r�b�g�摜�ɕϊ�����
			BGR:24bit -> RGBA:32bit
	*/
	PBYTE bmp_conv2432(PFRAMEBUFFER pfb) {
		if (pfb->bypp != 3) {
			return (PBYTE)NULL;	//24�r�b�g�^�s�N�Z������Ȃ��B
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
		�r�b�g�}�b�v����F�I��
		�g�����������̈���������
		�t�@�C�������
		�����FPFRAMEBUFFER pfb�FFRAMEBUFFER�ւ̃|�C���^
		�߂�l�F����
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
		
		SAFE_FREE(pfb->ppal);//�p���b�g�̈�iPallette buffer�j�����
		SAFE_FREE(pfb->ppx);//�s�N�Z���̈�iPixel buffer�j�����
		SAFE_FREE(pfb->file_path);//�t�@�C���p�X���̈�iFile path�j�����
		SAFE_FREE(pfb);//BMP�\���̗̈�iBMP�j�����

	}	//bmp_end

	//----------------------------------------------------------------
	/*
		�摜�f�[�^��BMP�t�@�C���֕ۑ�����
		pfb->ppx:�摜�f�[�^�ւ̃|�C���^
		pfb->width:���i�s�N�Z���j
		pfb->height:�����i�s�N�Z���j
		pfb->ppal:16�F�p���b�g�i�F����16�Œ�j
	*/
	PFRAMEBUFFER bmp_save(const char* file_path, int width, int height, const char* ppx, RGBQUAD* palette_table)
	{
		//
		FILE* fp = fopen(file_path, "wb");
		if (!fp) {
			return (PFRAMEBUFFER)NULL;
		}
		//BITMAPFILEHEADER(14byte)�̕�����ǂݍ��ށB
		//	WORD	bfType;			//0x4D42(�����R�[�h+0:'B'(0x42)��+1:'M'(0x4D)�������Ă���BWORD�i16�r�b�g�l�j�Ō����'0x4D42')
		//	DWORD	bfSize;			//BMP�t�@�C���̃o�C�g��(131190)
		//	WORD	bfReserved1;	//�\��i���g�p�����j
		//	WORD	bfReserved2;	//�\��i���g�p�����j
		//	DWORD	bfOffBits;		//�s�N�Z�������܂ł̃o�C�g��
		BITMAPFILEHEADER	bfh;
		ZeroMemory(&bfh, sizeof(BITMAPFILEHEADER));
		bfh.bfType = 0x4D42;
		bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 16);
		bfh.bfSize = bfh.bfOffBits + (width*height);
		//BITMAPINFOHEADER(40byte)������ǂݍ���
		//	DWORD	   biSize;			//���̍\���́iBITMAPINFOHEADER�j�̃T�C�Y�i�o�C�g���j
		//	LONG	   biWidth;			//���̉摜�̕��i�s�N�Z�����j
		//	LONG	   biHeight;		//���̉摜�̍����i�s�N�Z�����j���v���X�i�{�j�l�Ȃ�u�{�g���A�b�v�i����������j�v�}�C�i�X�i�|�j�l�Ȃ�u�g�b�v�_�E���i���������j�v�Ńs�N�Z��������ł���
		//	WORD	   biPlanes;		//��ɂP
		//	WORD	   biBitCount;		//�P�s�N�Z��������̃r�b�g���ibpp)
		//	DWORD	   biCompression;	//��bmp_utl�ł͈��k�`���͈����Ă��Ȃ��̂ŁA�񈳏k�`����BI_RGB���������Ă���iBI_BITFIELDS�����邪��ʓI�Ɏg���Ă��Ȃ��̂Ŕ�Ή��j
		//	DWORD	   biSizeImage;		//�񈳏kRGB�r�b�g�}�b�v�̏ꍇ��0�ɏo����̂ŁA�l�������Ă��Ă��Q�Ƃ��Ȃ��BbiSizeImage �̐������l�� biWidth�CbiHeight�CbiBitCount ����v�Z�ł���
		//	LONG	   biXPelsPerMeter;	//�����𑜓x�F�P�ʂ�1m������̂������i��f��/���j���O�̏ꍇ������̂ŎQ�Ƃ��Ȃ�
		//	LONG	   biYPelsPerMeter;	//�����𑜓x�F�P�ʂ�1m������̂������i��f��/���j���O�̏ꍇ������̂ŎQ�Ƃ��Ȃ�
		//	DWORD	   biClrUsed;		//�p���b�g�̐��i�J���[�e�[�u�����j�F�O�Ȃ�biBitCount�̃r�b�g���ŕ\���ł���ő�l���e�[�u�����ƂȂ遦�ڍׂ�MS�̃h�L�������g�Q��
		//	DWORD	   biClrImportant;	//���̉摜��\������̂ɕK�v�ȐF���i�p���b�g���j�O�Ȃ�S�Ă̐F���K�v��bmp_utl�ł͂��̒l�͖������đS�F�K�v�Ƃ��Ă���
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
		�p���b�g�f�[�^�̕\��
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
		�r�b�g�}�b�v���̕\��
			BITMAP�w�b�_���̕\��
			�p���b�g�f�[�^��HEX�_���v
			�摜�f�[�^��HEX�_���v
	*/
	void bmp_info(PFRAMEBUFFER pfb)
	{
		//BITMAPINFO:
		//	BITMAPINFOHEADER	bmiHeader;
		//	RGBQUAD	bmiColors[1];
		//BITMAPFILEHEADER��RGBQUAD[]�̃p���b�g�̐擪�i1������`����Ă���j
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
		printf("biSize:%d\n", pfb->bih.biSize);	//�̃w�b�_�̃T�C�Y
		printf("biWidth:%d\n", pfb->bih.biWidth);	//��
		printf("biHeight:%d\n", pfb->bih.biHeight);	//����
		printf("biPlanes:%d\n", pfb->bih.biPlanes);	//�v���[����
		printf("biBitCount:%d\n", pfb->bih.biBitCount);	//�P��f�̃r�b�g��
		printf("biCompression:%d\n", pfb->bih.biCompression);	//���k�`��
		printf("biSizeImage:%d\n", pfb->bih.biSizeImage);	//�摜�����̃T�C�Y
		printf("biXPelsPerMeter:%d\n", pfb->bih.biXPelsPerMeter);
		printf("biYPelsPerMeter:%d\n", pfb->bih.biYPelsPerMeter);
		printf("biClrUsed:%d\n", pfb->bih.biClrUsed);
		printf("biClrImportant:%d\n", pfb->bih.biClrImportant);
		//int ctbl_siz = pfb->bfh.bfOffBits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);	//color table size.
		//printf("ColorTable-size:%d = (FILE-OFF:%d - (BFH:%d + BIH:%d))\n", ctbl_siz, pfb->bfh.bfOffBits, sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));
		//
		//�F������B
		//�p���b�g�\���B
		printf("--- COLOR PALLET #RRGGBB ---\n");
		for (int n = 0; n < pfb->pal_count; n++) {
			if (((n % 8) == 0) && (n != 0)) {
				putchar('\n');
			}
			printf("0x%02X%02X%02X,", pfb->ppal[n].rgbRed, pfb->ppal[n].rgbGreen, pfb->ppal[n].rgbBlue);
		}
		putchar('\n');
		//���k�`���͈����Ă��Ȃ��B
		if (pfb->bih.biCompression != BI_RGB) {
			printf("Not support : Compression format.");
			return;
		}
		//�⊮�̓���������\��
		printf("stride : %d Bytes\n", pfb->ppx_stride);
		//�s�N�Z���������o�C�g�^�s�N�Z���̒P�ʂŕ\��
		BYTE* pixcel_buffer = pfb->ppx;
		for (int y = 0; y < pfb->bih.biHeight; y++) {
			for (int lx = 0; lx < pfb->bih.biWidth; lx++) {
				if (pfb->bypp == 1) {
					//�P�s�N�Z�����P�o�C�g
					printf("%X,", pixcel_buffer[lx]);
				}
				else if (pfb->bypp == 2) {
					//�P�s�N�Z�����Q�o�C�g
					printf("%04X,", ((WORD*)pixcel_buffer)[lx]);
				}
				else if (pfb->bypp == 3) {
					//�P�s�N�Z�����R�o�C�g
					printf("%06X,", *((DWORD*)(&pixcel_buffer[lx * 3])) & 0x00FFFFFF);
				}
				else if (pfb->bypp == 4) {
					//�P�s�N�Z�����S�o�C�g
					printf("%08X,", ((DWORD*)pixcel_buffer)[lx]);
				}
			}
			putchar('\n');
			pixcel_buffer += (pfb->bih.biWidth * pfb->bypp);
		}
	}	//bmp_info





