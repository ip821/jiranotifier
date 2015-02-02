// ObjectStorageManager.h : Declaration of the CObjectStorageManager

#pragma once
#include "resource.h"       // main symbols
#include "JiraNotify_i.h"

using namespace ATL;

// CObjectStorageManager

class ATL_NO_VTABLE CObjectStorageManager :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CObjectStorageManager, &CLSID_ObjectStorageManager>,
	public IObjectStorageManager
{
public:
	CObjectStorageManager()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CObjectStorageManager)
		COM_INTERFACE_ENTRY(IObjectStorageManager)
	END_COM_MAP()

public:
	STDMETHOD(OpenObjectStorage(BSTR bstrStoreName, BOOL bWrite, IObjectStorage** ppObjectStorage));

};

OBJECT_ENTRY_AUTO(__uuidof(ObjectStorageManager), CObjectStorageManager)
