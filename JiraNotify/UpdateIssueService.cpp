// UpdateIssueService.cpp : Implementation of CUpdateIssueService

#include "stdafx.h"
#include "UpdateIssueService.h"
#include "..\JiraConnection\Plugins.h"
#include "..\JiraConnection\JiraFields.h"

// CUpdateIssueService

STDMETHODIMP CUpdateIssueService::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_pSettings = pSettings;
	}
	return S_OK;
}

STDMETHODIMP CUpdateIssueService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ThreadService, &m_pThreadService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_TimerService, &m_pTimerService));
	return S_OK;
}

STDMETHODIMP CUpdateIssueService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	m_pSettings.Release();
	m_pThreadService.Release();
	m_pTimerService.Release();
	return S_OK;
}

STDMETHODIMP CUpdateIssueService::OnStart(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);
	RETURN_IF_FAILED(m_pTimerService->StopTimer());
	return S_OK;
}

STDMETHODIMP CUpdateIssueService::OnRun(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);
	CComPtr<ISettings> pSettings;
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		pSettings = m_pSettings;
	}
	CComBSTR bstrServer;
	RETURN_IF_FAILED(HrSettingsGetBSTR(pSettings, KEY_SERVER, &bstrServer));

	CComPtr<IJiraConnection> pJiraXmlRpcConnection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_JiraConnection, &pJiraXmlRpcConnection));

	RETURN_IF_FAILED(pJiraXmlRpcConnection->OpenConnection(bstrServer));

	CComBSTR bstrUser;
	RETURN_IF_FAILED(HrSettingsGetBSTR(pSettings, KEY_USER, &bstrUser));

	CComBSTR bstrPwd;
	RETURN_IF_FAILED(HrSettingsGetBSTR(pSettings, KEY_PASSWORD, &bstrPwd));

	HRESULT hr = pJiraXmlRpcConnection->Login(bstrUser, bstrPwd);
	if (FAILED(hr))
	{
		HandleJiraError(pJiraXmlRpcConnection, pResult);
		return hr;
	}

	CComPtr<IJiraObjectsCollection> pServerIssues;
	hr = pJiraXmlRpcConnection->GetIssuesByCriteria(FILTER_ASSIGNED_TO_ME, &pServerIssues);
	if (FAILED(hr))
	{
		HandleJiraError(pJiraXmlRpcConnection, pResult);
		return hr;
	}

	CComPtr<IObjectStorageManager> pObjectStorageManager;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectStorageManager, &pObjectStorageManager));

	CComPtr<IObjectStorage> pObjectStorage;
	RETURN_IF_FAILED(pObjectStorageManager->OpenObjectStorage(_T("Issues"), TRUE, &pObjectStorage));

	{ //Read
		CComPtr<IStorage> pStorage;
		hr = pObjectStorage->OpenStorage(FILTER_ASSIGNED_TO_ME_NAME, FALSE, &pStorage);
		if (FAILED(hr))
		{
			RETURN_IF_FAILED(pResult->SetVariantValue(KEY_HAS_CHANGES, &CComVariant(true)));
		}
		else
		{
			CComPtr<IJiraObjectsCollection> pSavedIssues;
			RETURN_IF_FAILED(HrCoCreateInstance(CLSID_JiraObjectsCollection, &pSavedIssues));
			CComQIPtr<IPersistStorage> pPersist = pSavedIssues;
			RETURN_IF_FAILED(pPersist->Load(pStorage));
			if (FAILED(hr))
			{
				RETURN_IF_FAILED(pResult->SetVariantValue(KEY_HAS_CHANGES, &CComVariant(true)));
			}
			else
			{
				RETURN_IF_FAILED(Diff(pSavedIssues, pServerIssues, pResult));
			}
		}
	}

	CComVariant vHasChanges;
	RETURN_IF_FAILED(pResult->GetVariantValue(KEY_HAS_CHANGES, &vHasChanges));

	if (vHasChanges.vt == VT_BOOL && vHasChanges.boolVal)
	{ //Write
		RETURN_IF_FAILED(pResult->SetVariantValue(KEY_ISSUES, &CComVariant(pServerIssues)));
	}

	return S_OK;
}

STDMETHODIMP CUpdateIssueService::OnFinish(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);
	RETURN_IF_FAILED(m_pTimerService->ResumeTimer());
	return S_OK;
}

void CUpdateIssueService::HandleJiraError(IJiraConnection *pJiraXmlRpcConnection, IVariantObject* pResultObject)
{
	CComPtr<IJiraConnection> pJiraXmlRpcConnectionSafe = pJiraXmlRpcConnection;
	CComQIPtr<IErrorInfo> pErrorInfo = pJiraXmlRpcConnectionSafe;
	if (!pErrorInfo)
		return;

	CComBSTR bstr;
	pErrorInfo->GetDescription(&bstr);
	pResultObject->SetVariantValue(KEY_HAS_MESSAGE, &CComVariant(bstr));
}

HRESULT CUpdateIssueService::Diff(IJiraObjectsCollection* pFirstCollection, IJiraObjectsCollection* pSecondCollection, IVariantObject* pResultObject)
{
	CHECK_E_POINTER(pFirstCollection);
	CHECK_E_POINTER(pSecondCollection);
	CHECK_E_POINTER(pResultObject);
	TStringVector firstKeys;
	HRESULT hr = FillKeysVector(firstKeys, pFirstCollection);
	if (FAILED(hr))
		return hr;

	sort(firstKeys.begin(), firstKeys.end());

	TStringVector secondKeys;
	hr = FillKeysVector(secondKeys, pSecondCollection);
	if (FAILED(hr))
		return hr;

	sort(secondKeys.begin(), secondKeys.end());

	TStringVector result(secondKeys.size());
	auto end = set_difference(
		secondKeys.begin(),
		secondKeys.end(),
		firstKeys.begin(),
		firstKeys.end(),
		result.begin()
		);
	result.resize(end - result.begin());

	if (result.size())
	{
		RETURN_IF_FAILED(pResultObject->SetVariantValue(KEY_HAS_CHANGES, &CComVariant(true)));
	}
	return S_OK;
}

HRESULT CUpdateIssueService::FillKeysVector(TStringVector& destVector, IJiraObjectsCollection* pSourceCollection)
{
	CHECK_E_POINTER(pSourceCollection);
	CComQIPtr<IObjArray> pSafe = pSourceCollection;

	UINT ulCount = 0;
	HRESULT hr = pSafe->GetCount(&ulCount);
	if (FAILED(hr))
		return hr;

	for (UINT i = 0; i < ulCount; i++)
	{
		CComPtr<IUnknown> pUnk;
		RETURN_IF_FAILED(pSafe->GetAt(i, __uuidof(IUnknown), (LPVOID*)&pUnk));
		CComQIPtr<IJiraObject> pJiraObject = pUnk;

		CComQIPtr<IVariantObject> pVariantObject = pJiraObject;
		CComVariant vValue;
		hr = pVariantObject->GetVariantValue(JF_ID, &vValue);
		if (FAILED(hr))
			return hr;

		CComBSTR bstrKey;
		if (vValue.vt == VT_BSTR)
			destVector.push_back(CString(vValue.bstrVal));
		else
			destVector.push_back(L"");
	}

	return S_OK;
}
