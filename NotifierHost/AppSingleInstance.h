#pragma once
class CAppSingleInstance
{
protected:
  DWORD  m_dwLastError;
  HANDLE m_hMutex;

public:
	CAppSingleInstance();
	virtual ~CAppSingleInstance();
	BOOL IsAnotherInstanceRunning();
};

