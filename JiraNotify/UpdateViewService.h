// UpdateViewService.h : Declaration of the CUpdateViewService

#pragma once
#include "resource.h"       // main symbols
#include "JiraNotify_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;

// CUpdateViewService

class ATL_NO_VTABLE CUpdateViewService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUpdateViewService, &CLSID_UpdateViewService>,
	public IUpdateViewService,
	public IPluginSupportNotifications,
	public IThreadServiceEventSink,
	public IInitializeWithControlImpl,
	public ISystrayWindowEventSink,
	public IInitializeWithSettings
{
public:
	CUpdateViewService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_UPDATEVIEWSERVICE)

	BEGIN_COM_MAP(CUpdateViewService)
		COM_INTERFACE_ENTRY(IUpdateViewService)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ISystrayWindowEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
	END_COM_MAP()

private:
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<ISystrayWindow> m_pSystrayWindow;
	CComPtr<ITrayNotifyManager> m_pTrayNotifyManager;
	CComPtr<ISettings> m_pSettings;
	DWORD m_dwAdvice = 0;
	DWORD m_dwAdvice2 = 0;
	CString m_strLastCaption;
	CString m_strLastMessage;

public:
	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject* pResult);
	STDMETHOD(OnRun)(IVariantObject* pResult);
	STDMETHOD(OnFinish)(IVariantObject* pResult);

	STDMETHOD(GetTrayNotifyManager)(ITrayNotifyManager** ppTrayNotifyManager);
	STDMETHOD(OnTrayNotification)(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT lResult, BOOL pbHandled);
	STDMETHOD(Load)(ISettings *pSettings);
};

OBJECT_ENTRY_AUTO(__uuidof(UpdateViewService), CUpdateViewService)
