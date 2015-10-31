#pragma once 

#include "resource.h"       // main symbols
#include "..\ViewMdl\IInitializeWithControlImpl.h"
#include "Plugins.h"
#include "JiraNotify_i.h"
#include "..\NotifierHost\SettingKeys.h"

using namespace ATL;

// CShowWindowCommand

class ATL_NO_VTABLE COpenJiraCommand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<COpenJiraCommand, &CLSID_ShowWindowCommand>,
	public IInitializeWithControlImpl,
	public ICommand,
	public IPluginSupportNotifications,
	public IInitializeWithSettings
{
public:
	COpenJiraCommand()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(COpenJiraCommand)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ICommand)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
	END_COM_MAP()

private:
	CComPtr<IUpdateViewService> m_pUpdateViewService;
	CComPtr<ISettings> m_pSettings;
public:

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(Load)(ISettings *pSettings);

	STDMETHOD(GetCommandText)(REFGUID guidCommand, BSTR* bstrText);
	STDMETHOD(InstallMenu)(IMenu* pMenu);
	STDMETHOD(Invoke)(REFGUID guidCommand);
};

OBJECT_ENTRY_AUTO(CLSID_OpenJiraCommand, COpenJiraCommand)
