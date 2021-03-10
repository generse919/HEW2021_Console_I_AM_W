#include "YellowSoul.h"

static PS_YellowSoul p_bs = NULL;
static GAMESTATE Start(PS_YellowSoul p_this);
static GAMESTATE Update(PS_YellowSoul p_this);
static void Stop(PS_YellowSoul p_this);

static Vector2_F destination;


static FILE *fi;
static FILE *fp;

static float fdest_theta;
static float speed;

void Yellow(GAMESTATE *gstate)
{
	if (*gstate == YELLOW_INIT)
	{
		p_bs = (PS_YellowSoul)calloc(1, sizeof(S_YellowSoul));
		Create(&p_bs->SP_YellowSoul, YELLOWSOUL_PIC, YELLOWSOUL);
		if (!(fi = fopen("../Release/scripts/playerlog.txt", "r")))
		{
			Stop(p_bs);
			*gstate = APP_EXIT;
			return;
		}
		if (!(fp = fopen("../Release/scripts/titlelog.txt", "w")))
		{
			Stop(p_bs);
			*gstate = APP_EXIT;
			return;
		}
		*gstate = Start(p_bs);

	}
	else if (*gstate == YELLOW)
	{
		*gstate = Update(p_bs);
	}

}

GAMESTATE Start(PS_YellowSoul p_this)
{
	p_this->SP_YellowSoul.pos = { (float)(DesktopWidth() - WindowWidth()),(float)(DesktopHeight() - WindowHeight()) };

	//if (!fscanf(fi, "x:%f,y:%f", &destination.x, &destination.y))	return APP_EXIT;
	
	DWORD player_pid = 0;
	RECT player_rect = {0};
	if (!fscanf(fi, "%ld", &player_pid))
	{
		fputs("111",fp);
		return APP_EXIT;
	}
	//HWND player_hwnd = GetTopWindow(NULL);
	HWND player_hwnd = GetWindowHandle(player_pid, NULL);
	if (player_hwnd == NULL)
	{
		fputs("222", fp);
		return APP_EXIT;
	}
	GetWindowRect(player_hwnd, &player_rect);
	destination = { (float)(player_rect.right - ((player_rect.right - player_rect.left) / 2)),(float)(player_rect.bottom - ((player_rect.bottom - player_rect.top) / 2)) };

	fclose(fi);

	fdest_theta = 0.0f;
	speed = 200.0f;
	return YELLOW;
}
GAMESTATE Update(PS_YellowSoul p_this)
{

	float dx = -(p_this->SP_YellowSoul.pos.x -destination.x);
	float dy = -(p_this->SP_YellowSoul.pos.y -destination.y);
	fdest_theta = atan2(dy,dx);


	//if (p_this->SP_YellowSoul.pos.x > destination.x)	p_this->SP_YellowSoul.pos.x -= speed * cosf(fdest_theta)*GetDeltaTime();

	//if (p_this->SP_YellowSoul.pos.x < destination.x)	p_this->SP_YellowSoul.pos.x += speed * cosf(fdest_theta)*GetDeltaTime();
	p_this->SP_YellowSoul.pos.x += speed * cosf(fdest_theta)*GetDeltaTime();

	//if (p_this->SP_YellowSoul.pos.y > destination.y)	p_this->SP_YellowSoul.pos.y -= speed * sinf(fdest_theta)*GetDeltaTime();

	p_this->SP_YellowSoul.pos.y += speed * sinf(fdest_theta)*GetDeltaTime();
	//if (p_this->SP_YellowSoul.pos.y < destination.y)	p_this->SP_YellowSoul.pos.y += speed * sinf(fdest_theta)*GetDeltaTime();




	MoveWin(p_this->SP_YellowSoul.pos.x, p_this->SP_YellowSoul.pos.y);

	
	float distance = sqrtf(pow(dx, 2.0) + pow(dy, 2.0));

	//if ((absf((float)(p_this->SP_YellowSoul.pos.x - destination.x)) < 5.0f) && (absf((float)(p_this->SP_YellowSoul.pos.y - destination.y)) < 5.0f))
	if (distance <= 5.0f)
	{
		Stop(p_this);
		return APP_EXIT;
	}

	for (int y = 0; y < ScreenHeight(); y++)
		for (int x = 0; x < ScreenWidth(); x++)
		{
			Draw(x, y, GetColor(&p_this->SP_YellowSoul, x, y));
		}

	return YELLOW;
}
void Stop(PS_YellowSoul p_this)
{
	Destroy(&p_bs->SP_YellowSoul);
	SAFE_CLOSE(fi);
	SAFE_CLOSE(fp);
	SAFE_FREE(p_this);

}