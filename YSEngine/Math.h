#pragma once
#include "YSConsoleGameEngine.h"
#define PI 3.141592

typedef struct
{
	float x, y;
}Vector2_F;

typedef struct
{
	int x, y;

}Vector2_I;

typedef struct
{
	float x, y, z;
}Vector3;

//float型の絶対値を返す
float absf(float x);