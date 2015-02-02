// ObjectStorageManager.cpp : Implementation of CObjectStorageManager

#include "stdafx.h"
#include "ObjectStorage.h"
#include "ObjectStorageManager.h"


// CObjectStorageManager

STDMETHODIMP CObjectStorageManager::OpenObjectStorage(BSTR bstrStoreName, BOOL bWrite, IObjectStorage** ppObjectStorage)
{
	if(bstrStoreName == NULL)
		return E_INVALIDARG;

	if(ppObjectStorage == NULL)
		return E_POINTER;

	CString strAppFolderPath;

	LPTSTR lpszPath =  strAppFolderPath.GetBuffer(MAX_PATH);
	BOOL bRes = SHGetSpecialFolderPath(
		NULL,
		lpszPath,
		CSIDL_APPDATA,
		TRUE
		);

	PathAppend(lpszPath, _T("JiraNotifier"));

	if(!PathFileExists(lpszPath))
	{
		CreateDirectory(lpszPath, NULL);
	}
	
	PathAppend(lpszPath, bstrStoreName);
	PathAddExtension(lpszPath, _T(".dat"));
	strAppFolderPath.ReleaseBuffer();

	ULONG stgmMode = STGM_READ|STGM_SHARE_EXCLUSIVE;
	if(bWrite)
	{
		stgmMode |= STGM_READWRITE;
	}

	CComPtr<IStorage> pStorage;

	HRESULT hr = S_OK;
	if(!PathFileExists(lpszPath))
	{
		if(!bWrite)
			return STG_E_FILENOTFOUND;

		stgmMode |= STGM_CREATE;
		hr = StgCreateStorageEx(
			strAppFolderPath, 
			stgmMode, 
			STGFMT_STORAGE, 
			0,
			NULL,
			NULL,
			IID_IStorage,
			(LPVOID*)&pStorage);
	}
	else
	{
		hr = StgOpenStorageEx(
			strAppFolderPath, 
			stgmMode, 
			STGFMT_STORAGE, 
			0,
			NULL,
			NULL,
			IID_IStorage,
			(LPVOID*)&pStorage);
	}

	if(FAILED(hr))
		return hr;

	CComObject<CObjectStorage>* pObjectStorage;
	hr = CComObject<CObjectStorage>::CreateInstance(&pObjectStorage);
	if(FAILED(hr))
		return hr;

	hr = pObjectStorage->SetStorage(pStorage);
	if(FAILED(hr))
		return hr;

	hr = pObjectStorage->QueryInterface(ppObjectStorage);
	if(FAILED(hr))
		return hr;
	
	return S_OK;
}
