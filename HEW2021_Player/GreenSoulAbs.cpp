#include "GreenSoulAbs.h"

static PS_GreenAbs scn = NULL;
static GAMESTATE Start(PS_GreenAbs p_this);
static GAMESTATE Update(PS_GreenAbs p_this);
static void Stop(PS_GreenAbs p_this);


static FILE *_fp;
static FILE *fs;
//
static Vector2_F playerMidPos;
static float speed;
//�`��p�Ɗp
static float ftheta;

//GreenSoulAppear�p�ϐ�
static STARTUPINFO si;
static PROCESS_INFORMATION pi;


#if 0
/*
*
*@brief		�O���[���\�E���v���Z�X�����s����
*/
void GreenSoulAppear()
{
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	CreateProcess(NULL, (LPTSTR)("HEW2021_Soul.exe"), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);


	// �R�[���o�b�N�֐��̌Ăяo���B
	EnumWindows(EnumWindowsProc, (LPARAM)&pi);


	//�����ŏ����������~�߂Ȃ��Ƃ��܂������Ȃ����Ƃ�����
	Sleep(2000);
	
	



}
/*
*
*@brief		�O���[���\�E�����z���������ǂ���(�v���Z�X���I���������ǂ���)���m�F����
*/
BOOL isGreenSoulAbsorb()
{
	if (WaitForSingleObject(pi.hProcess, 1) == WAIT_OBJECT_0)
	{
		TerminateProcess(pi.hProcess, 0);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return TRUE;

	}
	return FALSE;
}

#endif

void Green_Abs(GAMESTATE * gstate)
{
	if (*gstate == GREEN_SOUL_INIT)
	{
		scn = (PS_GreenAbs)calloc(1, sizeof(S_GreenAbs));
		Create(&scn->Absorb_Y, PIC_YELLOWSOUL, PLAYER_MODE_ONE);
		for (int i = 0; i < 15; i++)
		{
			Create(&scn->Clock[i], PIC_CLOCK_ZERO + i, PLAYER_CLOCK_ZERO + i);
		}
		/*if (!(f = fopen("../Release/scripts/gamelog.txt", "r")))
		{
			return;
		}*/
		//�ŏ��͓ǂݍ���

		if (!(_fp = fopen("../Release/scripts/playerlog.txt", "r")))
		{
			*gstate = APP_EXIT;
			return;
		}

		if (!(fs = fopen("../Release/scripts/soullog.txt", "r")))
		{
			*gstate = APP_EXIT;
			return;
		}


		//�X�^�[�g�֐��̎��s
		*gstate = Start(scn);

	}
	else if (*gstate == GREEN_SOUL)
	{
		*gstate = Update(scn);
	}

	return;
}

GAMESTATE Start(PS_GreenAbs p_this)
{
#if 0
	//���x�͏������ݗp
	if (!fscanf(_fp, "x:%f,y:%f", &p_this->Absorb_Y.pos.x, &p_this->Absorb_Y.pos.y))
		return APP_EXIT;
	fclose(_fp);
	

	if (!(_fp = fopen("../Release/scripts/playerlog.txt", "w")))
	{
		return APP_EXIT;
	}
#endif
	p_this->Absorb_Y.isActive = TRUE;

	p_this->Absorb_Y.pos.x = DesktopWidth() / 2;
	p_this->Absorb_Y.pos.y = DesktopHeight() / 2;
	speed = 50.0f;
	ftheta = 0.0f;

	//soul.exe�����s
	//GreenSoulAppear();


	return GREEN_SOUL;
}
GAMESTATE Update(PS_GreenAbs p_this)
{
	if (p_this->Absorb_Y.isActive)
	{
		if (HeldKey(PK_A))
		{
			p_this->Absorb_Y.pos.x -= 60.0f* GetDeltaTime();
		}

		if (HeldKey(PK_D))
		{
			p_this->Absorb_Y.pos.x += 60.0f* GetDeltaTime();
		}

		if (HeldKey(PK_W))
		{
			p_this->Absorb_Y.pos.y -= 60.0f* GetDeltaTime();
		}
		if (HeldKey(PK_S))
		{
			p_this->Absorb_Y.pos.y += 60.0f* GetDeltaTime();
		}

		
		

		
		playerMidPos = { (float)p_this->Absorb_Y.pos.x + (WindowWidth() / 2), (float)(float)p_this->Absorb_Y.pos.y + (WindowHeight() / 2) };
		float dx = (float)GetDesktopMousePosX() - playerMidPos.x;
		float dy = (float)GetDesktopMousePosY() - playerMidPos.y;
		float distance = sqrtf(pow(dx, 2.0) + pow(dy, 2.0));


		float fdest_theta = atan2(dy, dx);
		
		p_this->Absorb_Y.vec.x = distance * speed * cos(fdest_theta)*GetDeltaTime()/50;
		p_this->Absorb_Y.vec.y = distance * speed * sin(fdest_theta)*GetDeltaTime()/50;

		p_this->Absorb_Y.pos.x += p_this->Absorb_Y.vec.x;
		p_this->Absorb_Y.pos.y += p_this->Absorb_Y.vec.y;

		

	}


	//�t�@�C���Ƀv���C���[�ʒu���X�V����
#if 0
	char buf_p[100];
	freopen("../Release/scripts/playerlog.txt", "w", _fp);
	snprintf(buf_p, sizeof(buf_p), "x:%f,y:%f", playerMidPos.x, playerMidPos.y);
	fputs(buf_p, _fp);
	fflush(_fp);

	//�O���[���\�E�����z�����������肷��(�u���[�V�[����)
	//ZeroMemory(&buf, sizeof(buf));
	char buf_s[100];
	freopen("../Release/scripts/soullog.txt", "r", fs);
	while (fscanf(fs, "%s", &buf_s) != EOF)
	{
		if (!strcmp(buf_s, "AbsorbedGreenSoul"))
		{
			/*freopen("../Release/scripts/playerlog.txt", "w", _fp);
			snprintf(buf_p, sizeof(buf_p), "x:%f,y:%f", playerMidPos.x, playerMidPos.y);
			fputs(buf_p, _fp);*/
			Stop(p_this);
			return BLUE_SOUL_INIT;
		}
	}
#endif
	ftheta += GetDeltaTime();
	if (ftheta >= 2 * PI) ftheta = 0.0f;

	//���Ԏ擾
	unsigned char timeData[3];

	time_t t = time(NULL);

	timeData[0] = (t / 3600 + 9) % 12;//����(0~11)
	timeData[1] = t / 60 % 60;//��(0~59)
	timeData[2] = t % 60;//�b(0~59)

	//snprintf(timeData[0], 3, "%02ld\n", (t / 3600 + 9) % 24);//�b
	//snprintf(timeData[1], 3, "%02ld\n", t / 60 % 60);//��
	//snprintf(timeData[2], 3, "%02ld\n", t % 60);//����
	

	SetWindowPos(Get_HWND(), HWND_TOP, p_this->Absorb_Y.pos.x, p_this->Absorb_Y.pos.y, 0, 0, (SWP_NOSIZE | SWP_NOOWNERZORDER));

	//�\�E���`��
	for (int y = 0; y < ScreenHeight(); y++)
		for (int x = 0; x < ScreenWidth(); x++)
		{

			RGBQUAD Blight = { 255,245,223,0 };
			RGBQUAD Dark = GetColor(&p_this->Absorb_Y, x, y);

			/*if (Dark.rgbBlue != 0 || Dark.rgbGreen != 0 || Dark.rgbRed != 0 )
			{
				SetColor(&scn->Player, x, y, { (BYTE)((float)Dark.rgbBlue + ((float)(Blight.rgbBlue - Dark.rgbBlue) *0.2* absf(sinf(ftheta)))),
				(BYTE)((float)Dark.rgbGreen + ((float)(Blight.rgbGreen - Dark.rgbGreen) * 0.2*absf(sinf(ftheta)))),
				(BYTE)((float)Dark.rgbRed + ((float)(Blight.rgbRed - Dark.rgbRed) * 0.2*absf(sinf(ftheta)))) });
			}*/
			if (Dark.rgbBlue != 0 || Dark.rgbGreen != 0 || Dark.rgbRed != 0)
			{

				Draw(x, y,
					{ (BYTE)((float)Dark.rgbBlue + ((float)(Blight.rgbBlue - Dark.rgbBlue) * absf(sinf(ftheta)))),
					(BYTE)((float)Dark.rgbGreen + ((float)(Blight.rgbGreen - Dark.rgbGreen) *absf(sinf(ftheta)))),
				(BYTE)((float)Dark.rgbRed + ((float)(Blight.rgbRed - Dark.rgbRed)*absf(sinf(ftheta)))) });

			}
			else
			{
				Draw(x, y, GetColor(&scn->Absorb_Y, x, y));
			}



		}
	//�j�̕`��
	for (int i = 0; i < 3; i++)
	{
		if (i == 0) timeData[i] *= 5;//hour������5�{����
		RGBQUAD Blight = { 255,245,223,0 };
		RGBQUAD Dark;
		//BaseSprite tmpBS = p_this->Clock[timeData[i] % 15];
		
		for (int y = 0; y < ScreenHeight(); y++)
			for (int x = 0; x < ScreenWidth(); x++)
			{
				switch (timeData[i] / 15)
				{
					//0~14
				case 0:Dark = GetColor(&p_this->Clock[timeData[i] % 15], x, y);
					break;
					//15~29
				case 1:Dark = GetColor(&p_this->Clock[timeData[i] % 15], y, ScreenHeight() - x);
					break;
					//30~44
				case 2:Dark = GetColor(&p_this->Clock[timeData[i] % 15], ScreenWidth() - x, ScreenHeight() - y);
					break;
					//45~59
				case 3:Dark = GetColor(&p_this->Clock[timeData[i] % 15], ScreenWidth() - y, x);
					break;
				default:break;
				}

				if ((Dark.rgbBlue == 255 && Dark.rgbGreen == 0 && Dark.rgbRed == 0))//�j�̕`��
				{
					switch (i)
					{
					case 0:Draw(x, y, {0,0,255,0}); break;//����
					case 1:Draw(x, y, Dark); break;//��
					case 2:Draw(x, y, {0,255,0,0}); break;//�b
					default:Draw(x, y, { 0,0,0,0 }); break;
					}

				}

			}
		
	}

#if 0
	for (int y = 0; y < ScreenHeight(); y++)
		for (int x = 0; x < ScreenWidth(); x++)
		{

			RGBQUAD Blight = { 255,245,223,0 };
			RGBQUAD Dark = GetColor(&p_this->Absorb_Y, x, y);

			/*if (Dark.rgbBlue != 0 || Dark.rgbGreen != 0 || Dark.rgbRed != 0 )
			{
				SetColor(&scn->Player, x, y, { (BYTE)((float)Dark.rgbBlue + ((float)(Blight.rgbBlue - Dark.rgbBlue) *0.2* absf(sinf(ftheta)))),
				(BYTE)((float)Dark.rgbGreen + ((float)(Blight.rgbGreen - Dark.rgbGreen) * 0.2*absf(sinf(ftheta)))),
				(BYTE)((float)Dark.rgbRed + ((float)(Blight.rgbRed - Dark.rgbRed) * 0.2*absf(sinf(ftheta)))) });
			}*/
			if (Dark.rgbBlue != 0 || Dark.rgbGreen != 0 || Dark.rgbRed != 0)
			{

				Draw(x, y,
					{ (BYTE)((float)Dark.rgbBlue + ((float)(Blight.rgbBlue - Dark.rgbBlue) * absf(sinf(ftheta)))),
					(BYTE)((float)Dark.rgbGreen + ((float)(Blight.rgbGreen - Dark.rgbGreen) *absf(sinf(ftheta)))),
				(BYTE)((float)Dark.rgbRed + ((float)(Blight.rgbRed - Dark.rgbRed)*absf(sinf(ftheta)))) });

			}
			else
			{
				Draw(x, y, GetColor(&scn->Absorb_Y, x, y));
			}



		}
#endif

	return GREEN_SOUL;
}
void Stop(PS_GreenAbs p_this)
{
	Destroy(&p_this->Absorb_Y);
	for (int i = 0; i < 15; i++)Destroy(&scn->Clock[i]);
	SAFE_CLOSE(_fp);
	SAFE_CLOSE(fs);
	SAFE_FREE(scn);
}
