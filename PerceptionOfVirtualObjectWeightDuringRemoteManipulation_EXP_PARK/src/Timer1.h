#pragma once
#include <windows.h>
/*
Win32 c++ Timer class based on
https://www.codeguru.com/cpp/misc/misc/timers/article.php/c341/A-Threaded-Timer-Class.htm
Jaeyoung Park, Feb. 10. 2020
*/
class cTimer1
{
public:
	void stopTicking();
	void startTicking();
	cTimer1();
	virtual ~cTimer1();
	/*Retrieves the timer period in miliseconds*/
	int getTimeout() { return m_msTimeout; }
	/*Sets the timer period in miliseconds*/
	void setTimeout(int t) { m_msTimeout = t; }
protected:
	int m_msTimeout;
	HANDLE m_hThreadDone;
	virtual void Tick() {};
private:
	bool m_bStop;
	///static UINT 
	static unsigned int __stdcall tickerThread(LPVOID pParam);

};