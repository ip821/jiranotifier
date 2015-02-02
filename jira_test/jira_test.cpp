// jira_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

HRESULT PrintFilterCollection(IJiraObjectsCollection* pFilterCollection);

int _tmain(int argc, _TCHAR* argv[])
{
	USES_CONVERSION;

	HRESULT hr = CoInitialize(NULL);

	CComPtr<IObjectStorageManager> pStorageManager;
	hr = pStorageManager.CoCreateInstance(CLSID_ObjectStorageManager, NULL, CLSCTX_INPROC_SERVER);
	
	CComPtr<IObjectStorage> pObjectStorage;
	hr = pStorageManager->OpenObjectStorage(_T("Default"), TRUE, &pObjectStorage);

	CComPtr<IJiraXmlRpcConnection> pJiraXmlRpcConnection;
	hr = pJiraXmlRpcConnection.CoCreateInstance(CLSID_JiraXmlRpcConnection, NULL, CLSCTX_INPROC_SERVER);

	hr = pJiraXmlRpcConnection->OpenConnection(_T("http://support.nsure.dk:8082//rpc//xmlrpc"));
	hr = pJiraXmlRpcConnection->Login(_T("igor"), _T("ip1982"));

	CComPtr<IJiraObjectsCollection> pFilterCollection;
	hr = pJiraXmlRpcConnection->GetFavoriteFilters(&pFilterCollection);

	hr = pJiraXmlRpcConnection->Logout();
	hr = pJiraXmlRpcConnection->CloseConnection();

	return 0;
}

HRESULT PrintFilterCollection(IJiraObjectsCollection* pFilterCollection)
{
	USES_CONVERSION;
	ULONG ulCount = 0;
	HRESULT hr = pFilterCollection->GetCount(&ulCount);
	for(ULONG i = 0; i < ulCount; i++)
	{
		CComPtr<IJiraObject> pObj;
		hr = pFilterCollection->GetItem(i, &pObj);
		if(FAILED(hr))
			return hr;

		CComBSTR bstrKey;
		CComBSTR bstrDisplayName;
		hr = pObj->GetStringItem(JF_FILTER_ID, &bstrKey);
		hr = pObj->GetStringItem(JF_FILTER_NAME, &bstrDisplayName);
		std::cout << "Filter: " << W2A(bstrKey) << ": " << W2A(bstrDisplayName) << std::endl;

		if(i == 0)
		{
			//currentFilterId = bstrKey;
		}
	}
	return S_OK;
}