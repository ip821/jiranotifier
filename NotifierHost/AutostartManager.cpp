#include "StdAfx.h"
#include "AutostartManager.h"
#include "..\ObjMdl\StringUtils.h"

HRESULT CAutostartManager::SetAutostart(BOOL bEnable)
{
	CString strShortcutName;
	strShortcutName.Format(L"%s.lnk", StrGetAppExeName());

	TCHAR lpszStartupShortcutPath[MAX_PATH] = {0};
	BOOL bRes = SHGetSpecialFolderPath(NULL, (LPTSTR)&lpszStartupShortcutPath, CSIDL_STARTUP, FALSE);

	if(!bRes)
		return HRESULT_FROM_WIN32(GetLastError());

	bRes = PathAppend(lpszStartupShortcutPath, strShortcutName);
	if(!bRes)
		return HRESULT_FROM_WIN32(GetLastError());

	if(!bEnable)
	{
		DeleteFile(lpszStartupShortcutPath);
		return S_OK;
	}

	CComPtr<IShellLink> pShellLink;
	HRESULT hr = pShellLink.CoCreateInstance(CLSID_ShellLink);
	if(FAILED(hr))
		return hr;

	TCHAR lpszExePath[MAX_PATH] = {0};
	bRes = GetModuleFileName(NULL, (LPTSTR)&lpszExePath, MAX_PATH);
	if(!bRes)
		return HRESULT_FROM_WIN32(GetLastError());

	hr = pShellLink->SetPath(lpszExePath);
	if(FAILED(hr))
		return hr;

	hr = pShellLink->SetIconLocation(lpszExePath, 0);
	if(FAILED(hr))
		return hr;

	bRes = PathRemoveFileSpec(lpszExePath);
	if(!bRes)
		return HRESULT_FROM_WIN32(GetLastError());

	hr = pShellLink->SetWorkingDirectory(lpszExePath);
	if(FAILED(hr))
		return hr;

	CComQIPtr<IPersistFile> pPersistFile = pShellLink;
	hr = pPersistFile->Save(lpszStartupShortcutPath, TRUE);
	if(FAILED(hr))
		return hr;

	return S_OK;
}
