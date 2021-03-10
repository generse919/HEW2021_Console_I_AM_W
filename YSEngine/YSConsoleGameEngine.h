#pragma once
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "xinput.lib")


//#define _OPENMP
//#define MT

#include<stdio.h>
#include <stdlib.h>
#include<Windows.h>
#include <mbstring.h>
#include <xinput.h>
#include<time.h>
#include <math.h>
#include <tchar.h>

#include "Input.h"
#include "FileLoader.h"
#include "ConsoleManager.h"
#include "GameTimer.h"
#include "Math.h"

#ifdef _OPENMP
#include<omp.h>
#endif

//#define SAFE_FREE(ptr) {free(ptr);ptr = NULL;}

//ダブルフリーを回避するマクロ
#define SAFE_FREE(ptr) if(ptr != NULL)free(ptr);ptr = NULL;
#define SAFE_CLOSE(fp) if(fp != NULL)fclose(fp);fp = NULL;


#pragma warning(disable: 4996)



