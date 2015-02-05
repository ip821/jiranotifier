#include "stdafx.h"
#include "JiraRestConnection.h"
#include "..\model-libs\objmdl\StringUtils.h"

void CJiraConnection::InitStoredValuesMap()
{
	m_supportedValues.insert(JF_ID);
	m_supportedValues.insert(JF_NAME);
	m_supportedValues.insert(JF_DISPLAYNAME);
	m_supportedValues.insert(JF_ID);
	m_supportedValues.insert(JF_KEY);
	m_supportedValues.insert(JF_SUMMARY);
	m_supportedValues.insert(JF_PRIORITY);
	m_supportedValues.insert(JF_STATUS);
	m_supportedValues.insert(JF_RESOLUTIONDATE);
}

STDMETHODIMP CJiraConnection::OpenConnection(BSTR bstrUri)
{
	if(bstrUri == NULL)
		return INET_E_INVALID_URL;

	CString strRpcUri = bstrUri;
	int iLen = strRpcUri.GetLength();
	if(iLen && strRpcUri[iLen - 1] != '/')
	{
		strRpcUri += '/';
	}
	strRpcUri += _T("rest/api");
	m_strUri = strRpcUri;
	return S_OK;
}

STDMETHODIMP CJiraConnection::Login(BSTR bstrUser, BSTR bstrPwd)
{
	if(bstrUser == NULL || bstrPwd == NULL)
		return COMADMIN_E_USERPASSWDNOTVALID;

	m_user = bstrUser;
	m_passwd = bstrPwd;

	return S_OK;
}

STDMETHODIMP CJiraConnection::Logout()
{
	return S_OK;
}

STDMETHODIMP CJiraConnection::CloseConnection()
{
	return S_OK;
}

void CJiraConnection::CopyToStringMap(const JSONObject& source, std::map<std::wstring, std::wstring>& dest)
{
	for(auto it = source.begin(); it != source.end(); it++)
	{
		if(!it->second->IsString())
			continue;

		std::wstring key = it->first;
		if(m_supportedValues.find(key) == m_supportedValues.end())
			continue;
		std::wstring val = it->second->AsString();
		dest[key] = val;
	}
}

int CJiraConnection::CurlCallback(char* data, size_t size, size_t nmemb, CJiraConnection* pObj)
{
	if (pObj && data)
	{
		/* Save http response in twitcurl object's buffer */
		return pObj->SaveLastWebResponse(data, (size*nmemb));
	}
	return 0;
}

int CJiraConnection::SaveLastWebResponse(char*& data, size_t size)
{
	if (data && size)
	{
		/* Append data in our internal buffer */
		m_callbackData.append(data, size);
		return (int)size;
	}
	return 0;
}

STDMETHODIMP CJiraConnection::RemoteCall(string& query)
{
	m_callbackData.clear();

	CURL* curl = curl_easy_init();
	if (!curl)
		return E_FAIL;

	char errorBuffer[1024] = { 0 };

	USES_CONVERSION;

	curl_easy_setopt(curl, CURLOPT_URL, query.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1);
	curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, NULL);
	curl_easy_setopt(curl, CURLOPT_PROXYAUTH, (long)CURLAUTH_ANY);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL);
	curl_easy_setopt(curl, CURLOPT_ENCODING, "");
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

	std::string str(CString(W2A(m_user)) + ":" + W2A(m_passwd));
	std::vector<unsigned char> vec(str.cbegin(), str.cend());

	struct curl_slist* pHeaderList = NULL;
	pHeaderList = curl_slist_append(pHeaderList, string("Authorization: Basic " + base64_encode((const unsigned char*)str.c_str(), str.size())).c_str());
	curl_slist_append(pHeaderList, "User-Agent: Jira Notifier");

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, pHeaderList);

	{
		WINHTTP_CURRENT_USER_IE_PROXY_CONFIG proxyConfig = { 0 };
		if (WinHttpGetIEProxyConfigForCurrentUser(&proxyConfig))
		{
			std::vector<CString> proxies;
			StrSplit(proxyConfig.lpszProxy, L";", proxies);
			for (auto it = proxies.begin(); it != proxies.end(); it++)
			{
				std::vector<CString> values;
				StrSplit(*it, L"=", values);
				if (values.size() == 2 && values[0] == L"http")
				{
					string strValue = (CW2A(values[1]));
					curl_easy_setopt(curl, CURLOPT_PROXY, strValue.c_str());
					break;
				}
			}
			if (proxyConfig.lpszAutoConfigUrl)
				GlobalFree(proxyConfig.lpszAutoConfigUrl);
			if (proxyConfig.lpszProxy)
				GlobalFree(proxyConfig.lpszProxy);
			if (proxyConfig.lpszProxyBypass)
				GlobalFree(proxyConfig.lpszProxyBypass);
		}
	}

	auto res = curl_easy_perform(curl);
	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

	HRESULT curlHr = S_OK;
	
	if (curlHr == S_OK)
	{
		switch (res)
		{
		case CURLE_COULDNT_CONNECT:
			curlHr = HRESULT_FROM_WIN32(ERROR_NETWORK_UNREACHABLE);
		}
	}

	if (curlHr == S_OK)
	{
		switch (http_code)
		{
		case 401:
			curlHr = HRESULT_FROM_WIN32(COMADMIN_E_USERPASSWDNOTVALID);
		}
	}

	if (pHeaderList)
	{
		curl_slist_free_all(pHeaderList);
	}

	curl_easy_cleanup(curl);

	if (FAILED(curlHr))
	{
		m_strLastErrorMsg = _com_error(curlHr).ErrorMessage();
		SetErrorInfo(0, this);
		return E_CALL_FAILED;
	}

	return S_OK;
}

STDMETHODIMP CJiraConnection::GetCurrentUser(IJiraObject** ppObject)
{
	if (ppObject == NULL)
		return E_POINTER;

	USES_CONVERSION;

	string strUrl(W2A(m_strUri.c_str()));
	auto strFull(strUrl + string("/2/myself"));
	RETURN_IF_FAILED(RemoteCall(strFull));
	auto value = shared_ptr<JSONValue>(JSON::Parse(m_callbackData.c_str()));
	
	std::map<std::wstring, std::wstring> valueMap;
	CopyToStringMap(value->AsObject(), valueMap);

	CComObject<CJiraObject>* pJiraObjectClass;
	RETURN_IF_FAILED(CComObject<CJiraObject>::CreateInstance(&pJiraObjectClass));
	RETURN_IF_FAILED(pJiraObjectClass->SetItem(valueMap));
	return pJiraObjectClass->QueryInterface(ppObject);
}

STDMETHODIMP CJiraConnection::GetIssuesByCriteria(BSTR bstrSearchCriteria, IJiraObjectsCollection** ppCollection)
{
	std::wstring searchQuery = bstrSearchCriteria;

	CString strFields;
	for (auto& item : m_supportedValues)
	{
		strFields += CString(item.c_str()) + L",";
	}
	if (!strFields.IsEmpty())
		strFields = strFields.Left(strFields.GetLength() - 1);

	USES_CONVERSION;
	string strUrl(W2A(m_strUri.c_str()));
	strUrl += string("/2/search");
	string strFieldsValue(W2A(strFields));
	strUrl += string("?fields=") + urlencode(strFieldsValue);
	string searchQueryValue(W2A(searchQuery.c_str()));
	strUrl += string("&jql=") + urlencode(searchQueryValue);
	RETURN_IF_FAILED(RemoteCall(strUrl));
	auto value = shared_ptr<JSONValue>(JSON::Parse(m_callbackData.c_str()));

	auto obj = value->AsObject();
	auto issuesObject = obj[L"issues"];
	JSONArray issues = issuesObject->AsArray();

	CComObject<CJiraObjectsCollection>* pCollectionClass;
	RETURN_IF_FAILED(CComObject<CJiraObjectsCollection>::CreateInstance(&pCollectionClass));
	CComPtr<IObjCollection> pCollection;
	RETURN_IF_FAILED(pCollectionClass->QueryInterface(&pCollection));

	for (size_t it = 0; it < issues.size(); it++)
	{
		std::map<std::wstring, std::wstring> valueMap;
		auto issue = issues[it]->AsObject();
		CopyToStringMap(issue, valueMap);
		auto fieldsObject = issue[L"fields"]->AsObject();
		valueMap[(BSTR)JF_SUMMARY] = fieldsObject[L"summary"]->AsString();
		auto status = fieldsObject[L"status"]->AsObject();
		valueMap[(BSTR)JF_STATUS] = status[L"name"]->AsString();
		auto priority = fieldsObject[L"priority"]->AsObject();
		valueMap[(BSTR)JF_PRIORITY] = priority[L"name"]->AsString();
		auto resolutionDate = fieldsObject[L"resolutiondate"];
		if (resolutionDate->IsNull())
			valueMap[(BSTR)JF_RESOLUTIONDATE] = L"";
		else
		{
			auto resolutionDateStr = resolutionDate->AsString();
			int day, month, year;
			swscanf(
				resolutionDateStr.c_str(),
				L"%4d-%2d-%2d",
				&year,
				&month,
				&day
				);
			CString strFormat;
			strFormat.Format(L"%04d-%02d-%02d", year, month, day);
			valueMap[(BSTR)JF_RESOLUTIONDATE] = strFormat;
		}

		CComObject<CJiraObject>* pJiraObjectClass;
		RETURN_IF_FAILED(CComObject<CJiraObject>::CreateInstance(&pJiraObjectClass));
		RETURN_IF_FAILED(pJiraObjectClass->SetItem(valueMap));
		CComPtr<IJiraObject> pJiraObject;
		RETURN_IF_FAILED(pJiraObjectClass->QueryInterface(&pJiraObject));
		RETURN_IF_FAILED(pCollection->AddObject(pJiraObject));
	}

	RETURN_IF_FAILED(pCollectionClass->QueryInterface(IID_IJiraObjectsCollection, (LPVOID*)ppCollection));
	return S_OK;
}

STDMETHODIMP CJiraConnection::GetFavoriteFilters(IJiraObjectsCollection** ppCollection)
{
	if(ppCollection == NULL)
		return E_POINTER;

	USES_CONVERSION;

	string strUrl(W2A(m_strUri.c_str()));
	RETURN_IF_FAILED(RemoteCall(strUrl + string("/2/filter/favourite")));
	auto value = shared_ptr<JSONValue>(JSON::Parse(m_callbackData.c_str()));

	CComObject<CJiraObjectsCollection>* pCollectionClass;
	RETURN_IF_FAILED(CComObject<CJiraObjectsCollection>::CreateInstance(&pCollectionClass));
	CComPtr<IObjCollection> pCollection;
	RETURN_IF_FAILED(pCollectionClass->QueryInterface(&pCollection));

	auto filters = value->AsArray();
	for(size_t it = 0; it < filters.size(); it++)
	{
		std::map<std::wstring, std::wstring> valueMap;
		CopyToStringMap(filters[it]->AsObject(), valueMap);

		CComObject<CJiraObject>* pJiraObjectClass;
		RETURN_IF_FAILED(CComObject<CJiraObject>::CreateInstance(&pJiraObjectClass));
		RETURN_IF_FAILED(pJiraObjectClass->SetItem(valueMap));
		CComPtr<IJiraObject> pJiraObject;
		RETURN_IF_FAILED(pJiraObjectClass->QueryInterface(&pJiraObject));
		RETURN_IF_FAILED(pCollection->AddObject(pJiraObject));
	}

	RETURN_IF_FAILED(pCollectionClass->QueryInterface(IID_IJiraObjectsCollection, (LPVOID*)ppCollection));
	return S_OK;
}

STDMETHODIMP CJiraConnection::GetIssuesByFilter(BSTR bstrFilterId, IJiraObjectsCollection** ppCollection)
{
	if(bstrFilterId == NULL)
		return E_POINTER;

	if(ppCollection == NULL)
		return E_POINTER;

	CComBSTR bstrSearchCriteria(L"filter=\"" + CString(bstrFilterId) + L"\"");
	RETURN_IF_FAILED(GetIssuesByCriteria(bstrSearchCriteria, ppCollection));
	return S_OK;
}

STDMETHODIMP CJiraConnection::GetGUID(GUID *pGUID)
{
	return E_NOTIMPL;
}

STDMETHODIMP CJiraConnection::GetSource(BSTR *pBstrSource)
{
	return E_NOTIMPL;
}

STDMETHODIMP CJiraConnection::GetDescription(BSTR *pBstrDescription)
{
	CComBSTR bstr = m_strLastErrorMsg;
	*pBstrDescription = bstr;
	return S_OK;
}

STDMETHODIMP CJiraConnection::GetHelpFile(BSTR *pBstrHelpFile)
{
	return E_NOTIMPL;
}

STDMETHODIMP CJiraConnection::GetHelpContext(DWORD *pdwHelpContext)
{
	return E_NOTIMPL;
}

// CJiraConnection

