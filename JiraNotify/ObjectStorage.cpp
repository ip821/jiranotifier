// ObjectStorage.cpp : Implementation of CObjectStorage

#include "stdafx.h"
#include "ObjectStorage.h"


// CObjectStorage

STDMETHODIMP CObjectStorage::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* const arr[] = 
	{
		&IID_IObjectStorage
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CObjectStorage::OpenStorage(BSTR bstrStorage, BOOL bWrite, IStorage** ppStorage)
{
	if(bstrStorage == NULL)
		return E_INVALIDARG;

	if(ppStorage == NULL)
		return E_POINTER;

	if(m_pStorage == NULL)
		return E_UNEXPECTED;

	ULONG stgmMode = STGM_READ|STGM_SHARE_EXCLUSIVE;
	if(bWrite)
	{
		stgmMode |= STGM_CREATE|STGM_WRITE;
	}

	CComPtr<IStorage> pStorage;
	HRESULT hr = S_OK;
	
	if(bWrite)
	{
		hr = m_pStorage->CreateStorage(
			bstrStorage,
			stgmMode,
			0,
			0,
			&pStorage
			);
	}
	else
	{
		hr = m_pStorage->OpenStorage(
			bstrStorage,
			NULL,
			stgmMode,
			0,
			0,
			&pStorage
		);
	}

	if(FAILED(hr))
		return hr;

	hr = pStorage->QueryInterface(ppStorage);

	if(FAILED(hr))
		return hr;

	return S_OK;
}

STDMETHODIMP CObjectStorage::SetStorage(IStorage* pStorage)
{
	if(pStorage == NULL)
		return E_POINTER;

	m_pStorage = pStorage;
	return S_OK;
}