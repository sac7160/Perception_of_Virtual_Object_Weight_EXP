#include "Timer1.h"
#include <process.h>
cTimer1::cTimer1()
{
	m_bStop = true;
	m_msTimeout = -1;
	m_hThreadDone = NULL;
	m_hThreadDone = CreateEvent(NULL, FALSE, FALSE, NULL);
//	ASSERT(m_hThreadDone);
	SetEvent(m_hThreadDone);
}

cTimer1::~cTimer1()
{
	//dont destruct until the thread is done
	m_bStop = true; //ok make it stop
	DWORD ret = WaitForSingleObject(m_hThreadDone, INFINITE);
//	ASSERT(ret == WAIT_OBJECT_0);
//	Sleep(500);
}
//
//void cTimer1::Tick()
//{
//	//Will be overriden by subclass
//}

void cTimer1::startTicking()
{
	if (m_bStop == false)
		return; ///ignore, it is already ticking...
	m_bStop = false;
	ResetEvent(m_hThreadDone);
	_beginthreadex(0, 0, &cTimer1::tickerThread, this, 0, 0);
//	AfxBeginThread(TickerThread, this);
}

unsigned int __stdcall cTimer1::tickerThread(LPVOID pParam)
{
	cTimer1* me = (cTimer1*)pParam;
//	ASSERT(me->m_msTimeout != -1);
	while (!me->m_bStop)
	{
		Sleep(me->getTimeout());
		me->Tick();
	}
	SetEvent(me->m_hThreadDone);
	return 0;
}
void cTimer1::stopTicking()
{
	if (m_bStop == true)
		return; ///ignore, it is not ticking...
	m_bStop = true; //ok make it stop
	WaitForSingleObject(m_hThreadDone, INFINITE);
	//The above ensures that we do not return UNTIL the thread
	//has finished. This way we dont allow the user to start multiple
	//threads that will execute Tick() at the same time
}