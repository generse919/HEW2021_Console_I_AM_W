#include "GameTimer.h"


PGameTimer pgt = NULL;

BOOL Init(PGameTimer const p_this)
{

	pgt = p_this;
	timeBeginPeriod(1);
	pgt->t1 = timeGetTime();
	pgt->t2 = timeGetTime();
	pgt->m_fElapsedTime = 0;
	pgt->m_frameCount = 0;
	
	return TRUE;
}

void StartTimer(PGameTimer const p_this)
{
	//PGameTimer pgt = p_this;
#ifdef MULTI_THREAD
	pgt->t1 = omp_get_wtime();
#else
	pgt->t1 = timeGetTime();

	
	
#endif
	
}
void FrameCalculation(PGameTimer const p_this)
{
#ifdef MULTI_THREAD
	pgt->t2 = omp_get_wtime();

	int processingTime = (pgt->t2 - pgt->t1) * 1000;

	if (pgt->m_frameCount % 3 == 0)
	{
		pgt->m_fElapsedTime = 34 - ((int)processingTime);
		pgt->m_frameCount = 0;
	}
	else
	{
		pgt->m_fElapsedTime = 33 - ((int)processingTime);
	}

	if(!(pgt->m_fElapsedTime > 34)) Sleep(pgt->m_fElapsedTime);

	pgt->m_frameCount += 1;

#else
	pgt->t2 = timeGetTime();


	float processingTime = (float)(pgt->t2 - pgt->t1);

	//30fps‚Ü‚Å‚Ío‚é‚æ‚¤‚Éİ’èÏ‚İ
	if (processingTime < 33)
	{
		if (pgt->m_frameCount % 3 == 0)
		{
			pgt->m_fElapsedTime = 33;
			//pgt->m_fElapsedTime = 66;

			pgt->m_frameCount = 0;
		}
		else
		{
			pgt->m_fElapsedTime = 34;
		}
		Sleep(pgt->m_fElapsedTime - processingTime);
	}
	else
	{
		pgt->m_fElapsedTime = processingTime;
	}


	
	pgt->m_frameCount += 1;
	timeEndPeriod(1);
	
#endif
	
}

float GetDeltaTime()
{
	return float(pgt->m_fElapsedTime / 1000);
}

float GetFPS(PGameTimer const p_this)
{
	return (1000.0f / pgt->m_fElapsedTime);
}