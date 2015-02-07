// UpdateViewService.cpp : Implementation of CUpdateViewService

#include "stdafx.h"
#include "UpdateViewService.h"
#include "JiraNotify_i.h"

// CUpdateViewService

STDMETHODIMP CUpdateViewService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ThreadService, &m_pThreadService));
	
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));
	
	CComQIPtr<IMainDialog> pMainDialog = m_pControl;
	RETURN_IF_FAILED(pMainDialog->GetSystrayWindow(&m_pSystrayWindow));
	
	RETURN_IF_FAILED(AtlAdvise(m_pSystrayWindow, pUnk, __uuidof(ISystrayWindowEventSink), &m_dwAdvice2));

	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TrayNotifyManager, &m_pTrayNotifyManager));
	RETURN_IF_FAILED(m_pTrayNotifyManager->Initialize(m_pSystrayWindow));
	RETURN_IF_FAILED(m_pTrayNotifyManager->ResetIconToNormalIfPossible());

	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	RETURN_IF_FAILED(pMainDialog->SetIcon(hIcon, TRUE));
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	RETURN_IF_FAILED(pMainDialog->SetIcon(hIconSmall, FALSE));

	UINT uTimeout = 10;
	CComVariant vInterval;
	if (SUCCEEDED(m_pSettings->GetVariantValue(AsyncServices::Metadata::Timer::Interval, &vInterval)) && vInterval.vt == VT_I4)
		uTimeout = vInterval.intVal;

	CComPtr<ITimerService> pTimerService;
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_TimerService, &pTimerService));
	RETURN_IF_FAILED(pTimerService->StartTimer(uTimeout * 1000 * 60));

	return S_OK;
}

STDMETHODIMP CUpdateViewService::GetTrayNotifyManager(ITrayNotifyManager** ppTrayNotifyManager)
{
	CHECK_E_POINTER(ppTrayNotifyManager);
	RETURN_IF_FAILED(m_pTrayNotifyManager->QueryInterface(ppTrayNotifyManager));
	return S_OK;
}

STDMETHODIMP CUpdateViewService::OnShutdown()
{
	RETURN_IF_FAILED(m_pTrayNotifyManager->Shutdown());
	RETURN_IF_FAILED(AtlUnadvise(m_pSystrayWindow, __uuidof(ISystrayWindowEventSink), m_dwAdvice2));
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	m_pTrayNotifyManager.Release();
	m_pThreadService.Release();
	m_pSystrayWindow.Release();
	m_pSettings.Release();
	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());
	return S_OK;
}

STDMETHODIMP CUpdateViewService::OnStart(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);
	CComPtr<ITrayNotifyManager> pTrayManager;
	RETURN_IF_FAILED(m_pTrayNotifyManager->ShowUpdateIcon());
	return S_OK;
	return S_OK;
}

STDMETHODIMP CUpdateViewService::OnRun(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);
	return S_OK;
}

STDMETHODIMP CUpdateViewService::OnFinish(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);

	m_strLastCaption.Empty();
	m_strLastMessage.Empty();

	RETURN_IF_FAILED(m_pTrayNotifyManager->ShowNormalIcon());

	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));
	HRESULT hr = S_OK;
	if (vHr.vt == VT_I4)
		hr = vHr.lVal;

	if (hr == S_OK)
	{
		CComVariant vHasChanges;
		if (SUCCEEDED(pResult->GetVariantValue(KEY_HAS_CHANGES, &vHasChanges)) && vHasChanges.vt == VT_BOOL && vHasChanges.boolVal)
		{
			RETURN_IF_FAILED(m_pTrayNotifyManager->ShowNewItemsIcon());
			CString strMessage;
			strMessage.LoadString(IDS_CHANGESMESSAGE);
			CString strCaption;
			strCaption.LoadString(IDS_CAPTIONCHANGES);
			RETURN_IF_FAILED(m_pTrayNotifyManager->ShowBaloon(strMessage, strCaption, SystrayBalloonStyle::Info, INFINITE));

			m_strLastCaption = strCaption;
			m_strLastMessage = strMessage;

			CComVariant vServerIssues;
			RETURN_IF_FAILED(pResult->GetVariantValue(KEY_ISSUES, &vServerIssues));
			if (vServerIssues.vt == VT_UNKNOWN)
			{
				m_pJiraObjectsCollection = vServerIssues.punkVal;
			}
		}
	}
	else
	{
		RETURN_IF_FAILED(m_pTrayNotifyManager->ShowErrorIcon());
		CString strMessage = _com_error(hr).ErrorMessage();
		CString strLastErrorMessage;
		CComVariant vMessage;
		if (SUCCEEDED(pResult->GetVariantValue(KEY_HAS_MESSAGE, &vMessage)) && vMessage.vt == VT_BSTR)
		{
			strLastErrorMessage = vMessage.bstrVal;
		}

		if (!strLastErrorMessage.IsEmpty())
		{
			strMessage = strLastErrorMessage;
		}

		CString strCaption;
		strCaption.LoadString(IDS_CAPTIONUPDATE);
		m_pTrayNotifyManager->ShowBaloon(strMessage, strCaption, SystrayBalloonStyle::Warning, 10);
		m_strLastCaption = strCaption;
		m_strLastMessage = strMessage;
	}

	return S_OK;
}

STDMETHODIMP CUpdateViewService::OnTrayNotification(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT lpResult, BOOL pbHandled)
{
	if (lParam == WM_MOUSEFIRST && !m_strLastCaption.IsEmpty() && !m_strLastMessage.IsEmpty())
	{
		m_pTrayNotifyManager->ShowBaloon(m_strLastMessage, m_strLastCaption, SystrayBalloonStyle::Warning, 10);
	}

	if (lParam == NIN_BALLOONUSERCLICK)
	{
		CComQIPtr<ICommandSupport> pCommandSupport = m_pControl;
		RETURN_IF_FAILED(pCommandSupport->QueueCommandExecution(CLSID_OpenJiraCommand, NULL));
	}

	return S_OK;
}


STDMETHODIMP CUpdateViewService::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	m_pSettings = pSettings;
	return S_OK;
}

STDMETHODIMP CUpdateViewService::ResetIcon()
{
	if (m_pJiraObjectsCollection)
	{
		CComPtr<IObjectStorageManager> pObjectStorageManager;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectStorageManager, &pObjectStorageManager));

		CComPtr<IObjectStorage> pObjectStorage;
		RETURN_IF_FAILED(pObjectStorageManager->OpenObjectStorage(_T("Issues"), TRUE, &pObjectStorage));

		CComPtr<IStorage> pStorage;
		RETURN_IF_FAILED(pObjectStorage->OpenStorage(FILTER_ASSIGNED_TO_ME_NAME, TRUE, &pStorage));

		CComQIPtr<IPersistStorage> pPersistStorage = m_pJiraObjectsCollection;
		RETURN_IF_FAILED(pPersistStorage->Save(pStorage, FALSE));

		m_pJiraObjectsCollection.Release();
	}
	RETURN_IF_FAILED(m_pTrayNotifyManager->ResetIconToNormalIfPossible());

	return S_OK;
}