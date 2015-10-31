// UpdateIssueService.h : Declaration of the CUpdateIssueService

#pragma once
#include "resource.h"       // main symbols
#include "JiraNotify_i.h"
#include "..\NotifierHost\SettingKeys.h"

typedef std::vector<CString> TStringVector;

using namespace ATL;


// CUpdateIssueService

class ATL_NO_VTABLE CUpdateIssueService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUpdateIssueService, &CLSID_UpdateIssueService>,
	public IUpdateIssueService,
	public IPluginSupportNotifications,
	public IThreadServiceEventSink,
	public IInitializeWithSettings
{
public:
	CUpdateIssueService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_UPDATEISSUESERVICE)

	BEGIN_COM_MAP(CUpdateIssueService)
		COM_INTERFACE_ENTRY(IUpdateIssueService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
	END_COM_MAP()

private:
	CComPtr<ISettings> m_pSettings;
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<ITimerService> m_pTimerService;
	DWORD m_dwAdvice = 0;
	boost::mutex m_mutex;

	HRESULT Diff(IJiraObjectsCollection* pFirstCollection, IJiraObjectsCollection* pSecondCollection, IVariantObject* pResultObject);
	HRESULT FillKeysVector(TStringVector& destVector, IJiraObjectsCollection* pSourceCollection);
	void HandleJiraError(IJiraConnection *pJiraXmlRpcConnection, IVariantObject* pResultObject);

public:

	STDMETHOD(Load)(ISettings *pSettings);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject* pResult);
	STDMETHOD(OnRun)(IVariantObject* pResult);
	STDMETHOD(OnFinish)(IVariantObject* pResult);
};

OBJECT_ENTRY_AUTO(__uuidof(UpdateIssueService), CUpdateIssueService)
