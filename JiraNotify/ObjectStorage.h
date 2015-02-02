// ObjectStorage.h : Declaration of the CObjectStorage

#pragma once
#include "resource.h"       // main symbols
#include "JiraNotify_i.h"

using namespace ATL;

// CObjectStorage

class ATL_NO_VTABLE CObjectStorage :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CObjectStorage, &CLSID_ObjectStorage>,
	public ISupportErrorInfo,
	public IObjectStorage
{
public:
	CObjectStorage()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CObjectStorage)
		COM_INTERFACE_ENTRY(IObjectStorage)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
	END_COM_MAP()

	// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

private:
	CComQIPtr<IStorage> m_pStorage;
public:
	STDMETHOD(SetStorage(IStorage* pStorage));
	STDMETHOD(OpenStorage(BSTR bstrStorage, BOOL bWrite, IStorage** ppStorage));
};

OBJECT_ENTRY_AUTO(__uuidof(ObjectStorage), CObjectStorage)
