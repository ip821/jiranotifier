#include "StdAfx.h"
#include "AppSingleInstance.h"
#include "..\ObjMdl\StringUtils.h"

CAppSingleInstance::CAppSingleInstance()
{
	CString strMutexName;
	strMutexName.Format(L"%s_{ 3AD23D1E-4D4C-4DEF-AD93-46011B6D8D3C}", StrGetAppExeName());
	m_hMutex = CreateMutex(NULL, FALSE, strMutexName);
	m_dwLastError = GetLastError();
}

CAppSingleInstance::~CAppSingleInstance()
{
	if (m_hMutex)
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
}

BOOL CAppSingleInstance::IsAnotherInstanceRunning()
{
	return m_dwLastError == ERROR_ALREADY_EXISTS;
}
