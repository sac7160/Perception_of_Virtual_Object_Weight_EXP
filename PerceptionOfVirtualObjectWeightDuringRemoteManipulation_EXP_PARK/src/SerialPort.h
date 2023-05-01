#pragma once

#include "stdafx.h"
#include <stdio.h>
#include <atlstr.h>
#include <iostream>
#include <windows.h>

using namespace std;

class CSerialPort
{
public:
	CSerialPort(void);
	virtual ~CSerialPort(void);

private:
	HANDLE			m_hComm;
	DCB				m_dcb;
	COMMTIMEOUTS	m_CommTimeouts;
	BOOL			m_bPortReady;
	BOOL			m_bWriteRC;
	BOOL			m_bReadRC;
	DWORD			m_iBytesWritten;
	DWORD			m_iBytesRead;
	DWORD			m_dwBytesRead;

public:
	bool OpenPort(CString portname);
	void ClosePort();
	bool ReadByte(BYTE &resp);
	bool ReadByte(BYTE* &resp, UINT size);
	bool WriteByte(BYTE bybyte);
	bool SetCommunicationTimeouts(DWORD ReadIntervalTimeout, DWORD ReadTotalTimeoutMultiplier, DWORD ReadTotalTimeoutConstant, DWORD WriteTotalTimeoutMultiplier, DWORD WriteTotalTimeoutConstant);
	bool ConfigurePort(DWORD BaudRate, BYTE ByteSize, DWORD fParity, BYTE Parity, BYTE StopBits);
};