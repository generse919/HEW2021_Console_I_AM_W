#include "BlueSoul.h"

static PS_BlueSoul p_bs = NULL;
static GAMESTATE Start(PS_BlueSoul p_this);
static GAMESTATE Update(PS_BlueSoul p_this);
static void Stop(PS_BlueSoul p_this);

static Vector2_F destination;
static Vector2_F playerMidPos;
static FILE *fs;
static FILE *fp;

static float fdest_theta;
static float speed;
static BOOL isReached;

void Blue(GAMESTATE * gstate)
{
	if (*gstate == BLUE_INIT)
	{
		p_bs = (PS_BlueSoul)calloc(1, sizeof(S_BlueSoul));
		Create(&p_bs->SP_BlueSoul, BLUESOUL_PIC, BLUESOUL);
		if (!(fp = fopen("../Release/scripts/playerlog.txt", "r")))
		{
			*gstate = APP_EXIT;
			return;
		}
		if (!(fs = fopen("../Release/scripts/soullog.txt", "w")))
		{
			*gstate = APP_EXIT;
			return;
		}
		*gstate = Start(p_bs);

	}
	else if (*gstate == BLUE)
	{
		*gstate = Update(p_bs);
	}
}

GAMESTATE Start(PS_BlueSoul p_this)
{
	p_this->SP_BlueSoul.pos = {300.0f,0.0f};
	p_this->SP_BlueSoul.isActive = FALSE;
	fdest_theta = 0.0f;
	speed = 200.0f;
	isReached = FALSE;
	destination = {(float)DesktopWidth() + 100,(float)(DesktopHeight()/2)};
	return BLUE;
}
GAMESTATE Update(PS_BlueSoul p_this)
{

	if (!p_this->SP_BlueSoul.isActive)
	{
		float dx = -(p_this->SP_BlueSoul.pos.x - destination.x);
		float dy = -(p_this->SP_BlueSoul.pos.y - destination.y);
		fdest_theta = atan2(dy, dx);

		


		p_this->SP_BlueSoul.pos.x += speed * cosf(fdest_theta)*GetDeltaTime();
		p_this->SP_BlueSoul.pos.y += speed * sinf(fdest_theta)*GetDeltaTime();


		float distance = sqrtf(pow(dx, 2.0) + pow(dy, 2.0));

		if (distance <= 5.0f)
		{
			p_this->SP_BlueSoul.isActive = TRUE;
		}
	}
	else
	{
		//プレイヤーの座標を取得し続ける
		freopen("../Release/scripts/playerlog.txt", "r", fp);
		if (!fscanf(fp, "x:%f,y:%f", &playerMidPos.x, &playerMidPos.y))
		{
			return APP_EXIT;
		}

		//壁を越えたらついてくる
		if (playerMidPos.x >= p_this->SP_BlueSoul.pos.x)isReached = TRUE;

		if (isReached)
		{
			p_this->SP_BlueSoul.pos.x = playerMidPos.x;
			p_this->SP_BlueSoul.pos.y = playerMidPos.y;

			//デスクトップxが3/4の位置に来たら終了
			if (playerMidPos.x <= DesktopWidth() * 0.75)
			{
				//WaitMessage
				freopen("../Release/scripts/soullog.txt", "w", fs);
				fputs("AbsorbedBlueSoul", fs);
				fflush(fs);
				Stop(p_this);
				return APP_EXIT;
			}
		}
	}
	
	
	
	
	SetWindowPos(Get_HWND(), HWND_NOTOPMOST, (int)p_this->SP_BlueSoul.pos.x, (int)p_this->SP_BlueSoul.pos.y, 0, 0, (SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER));
	

	for (int y = 0; y < ScreenHeight(); y++)
		for (int x = 0; x < ScreenWidth(); x++)
		{
			Draw(x, y, GetColor(&p_this->SP_BlueSoul, x, y));
		}

	return BLUE;
}
void Stop(PS_BlueSoul p_this)
{
	Destroy(&p_this->SP_BlueSoul);
	SAFE_CLOSE(fp);
	SAFE_CLOSE(fs);
	SAFE_FREE(p_this);
}
