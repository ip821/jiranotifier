#pragma once
#include "resource.h"       // main symbols
#include "..\ViewMdl\IInitializeWithControlImpl.h"
#include "Plugins.h"

using namespace ATL;

// CShowWindowCommand

class ATL_NO_VTABLE CShowWindowCommand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CShowWindowCommand, &CLSID_ShowWindowCommand>,
	public IInitializeWithControlImpl,
	public ICommand
{
public:
	CShowWindowCommand()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CShowWindowCommand)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ICommand)
	END_COM_MAP()

public:

	STDMETHOD(GetCommandText)(REFGUID guidCommand, BSTR* bstrText);
	STDMETHOD(InstallMenu)(IMenu* pMenu);
	STDMETHOD(Invoke)(REFGUID guidCommand);
};

OBJECT_ENTRY_AUTO(CLSID_ShowWindowCommand, CShowWindowCommand)
