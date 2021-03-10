#pragma once



#include"YSConsoleGameEngine.h"

	//	BYTE�F�����Ȃ��W�r�b�g�����^�i�O�`�Q�T�T�j�i0x00�`0xFF�j
	//	PBYTE�FBYTE�^�ւ̃|�C���^
	typedef unsigned char	BYTE, *PBYTE;

	//�t���[���o�b�t�@����p�\���̂̒�`
	//	FRAMEBUFFER�\����
	//	PFRAMEBUFFER�\���̂ւ̃|�C���^
	typedef struct {
		BITMAPFILEHEADER	bfh;	//�r�b�g�}�b�v�t�@�C���w�b�_
		BITMAPINFOHEADER	bih;	//�r�b�g�}�b�v�摜���
		//�p���b�g����
		int	pal_count;	//�p���b�g�̐F���i�P�U�F�j
		RGBQUAD*	ppal;	//�p���b�g�z��iRGBQUAD�^�z��̃|�C���^�j
		//�摜�s�N�Z������
		DWORD	ppx_size;	//�t���[���o�b�t�@�̃T�C�Y�i�o�C�g���j
		PBYTE	ppx;	//�t���[���o�b�t�@�{�́i�P�o�C�g�^�s�N�Z���j
		//PBYTE	pixel_data;	//�t���[���o�b�t�@�{�́i�P�o�C�g�^�s�N�Z���j
		//�摜�s�N�Z�����g���₷���`���ɕϊ������f�[�^����
		LONG	ppx_stride;	//�R�Q�r�b�g�A���C�����g���Z�̉������o�C�g��
		int bypp;			//�P�s�N�Z��������̃o�C�g��(bpp(Byte Per Pixel))
		int	width;	//�摜���i�s�N�Z���j
		int	height;	//�摜���i�s�N�Z���j
		FILE*	fp;
		char*	file_path;	//�ǂݍ��񂾃t�@�C���p�X
	} FRAMEBUFFER, *PFRAMEBUFFER;

	//BITMAP�f�[�^�̓ǂݍ���
	PFRAMEBUFFER bmp_load(const char* file_path);
	//�s�N�Z����RGB���т̕��בւ�RED��BLUE������
	PBYTE bmp_swapRB(PFRAMEBUFFER pfb);
	//�Q�S�r�b�g�摜���R�Q�r�b�g�摜�ɕϊ�
	PBYTE bmp_conv2432(PFRAMEBUFFER pfb);
	//�摜�f�[�^�iFRAMEBUFFER�j�̎g�p�I��
	void bmp_end(PFRAMEBUFFER pfb);
	//�摜�f�[�^���t�@�C���ɕۑ�
	PFRAMEBUFFER bmp_save(const char* file_patgh, int width, int height, const char* ppx, RGBQUAD* palette_table);
	//==== HEX-DUMP ====
	//BITMAP�t�@�C�����̃_���v�\��
	void  bmp_info(PFRAMEBUFFER pfb);
	//�p���b�g�f�[�^���_���v�\��
	void bmp_pal_dump(PFRAMEBUFFER pfb);



