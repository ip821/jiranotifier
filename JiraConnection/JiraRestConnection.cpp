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

    RETURN_IF_FAILED(HrCoCreateInstance(CLSID_HttpConnection, &m_pHttpConnection));

	CString strRpcUri = bstrUri;
	int iLen = strRpcUri.GetLength();
	if(iLen && strRpcUri[iLen - 1] != '/')
	{
		strRpcUri += '/';
	}
	strRpcUri += _T("rest/api");
	m_bstrUri = CW2A(strRpcUri);
	return S_OK;
}

STDMETHODIMP CJiraConnection::Login(BSTR bstrUser, BSTR bstrPwd)
{
	if(bstrUser == NULL || bstrPwd == NULL)
		return COMADMIN_E_USERPASSWDNOTVALID;

	m_bstrUser = bstrUser;
	m_bstrPasswd = bstrPwd;

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

void CJiraConnection::CopyToStringMap(const JSONObject& source, map<wstring, wstring>& dest)
{
	for(auto it = source.begin(); it != source.end(); it++)
	{
		if(!it->second->IsString())
			continue;

		wstring key = it->first;
		if(m_supportedValues.find(key) == m_supportedValues.end())
			continue;
		wstring val = it->second->AsString();
		dest[key] = val;
	}
}

STDMETHODIMP CJiraConnection::RemoteCall(BSTR bstrQuery, BSTR* pbstrResult)
{
	auto hr = m_pHttpConnection->RemoteCall(bstrQuery, m_bstrUser, m_bstrPasswd, pbstrResult);

	if (FAILED(hr))
	{
		m_strLastErrorMsg = _com_error(hr).ErrorMessage();
		SetErrorInfo(0, this);
		return E_CALL_FAILED;
	}
	
	if (hr != S_OK)
	{
		m_strLastErrorMsg = CA2W(curl_easy_strerror(static_cast<CURLcode>(hr)));
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

	CComBSTR bstrUrl(m_bstrUri);
    bstrUrl.Append(L"/2/myself");
	CComBSTR bstrResult;
	RETURN_IF_FAILED(RemoteCall(bstrUrl, &bstrResult));
	auto value = shared_ptr<JSONValue>(JSON::Parse(bstrResult));
	ATLASSERT(value);
	
	map<wstring, wstring> valueMap;
	CopyToStringMap(value->AsObject(), valueMap);

	CComObject<CJiraObject>* pJiraObjectClass;
	RETURN_IF_FAILED(CComObject<CJiraObject>::CreateInstance(&pJiraObjectClass));
	RETURN_IF_FAILED(pJiraObjectClass->SetItem(valueMap));
	return pJiraObjectClass->QueryInterface(ppObject);
}

STDMETHODIMP CJiraConnection::GetIssuesByCriteria(BSTR bstrSearchCriteria, IJiraObjectsCollection** ppCollection)
{
	wstring searchQuery = bstrSearchCriteria;

	CString strFields;
	for (auto& item : m_supportedValues)
	{
		strFields += CString(item.c_str()) + L",";
	}
	if (!strFields.IsEmpty())
		strFields = strFields.Left(strFields.GetLength() - 1);

	USES_CONVERSION;
	wstring strUrl(m_bstrUri);
    strUrl += L"/2/search";

    strUrl += L"?fields="+ urlencodew(wstring(strFields));

    strUrl += L"&jql=" + urlencodew(searchQuery);

    CComBSTR bstrResult;
	RETURN_IF_FAILED(RemoteCall(CComBSTR(strUrl.c_str()), &bstrResult));
	auto value = shared_ptr<JSONValue>(JSON::Parse(bstrResult));

    if (!value)
        return HRESULT_FROM_WIN32(DNS_ERROR_INVALID_XML);

	auto valueObject = value->AsObject();
	auto issues = valueObject[L"issues"]->AsArray();

	CComObject<CJiraObjectsCollection>* pCollectionClass;
	RETURN_IF_FAILED(CComObject<CJiraObjectsCollection>::CreateInstance(&pCollectionClass));
	CComPtr<IObjCollection> pCollection;
	RETURN_IF_FAILED(pCollectionClass->QueryInterface(&pCollection));

	for (size_t i = 0; i < issues.size(); i++)
	{
		auto issue = issues[i]->AsObject();
		
		map<wstring, wstring> valueMap;
		CopyToStringMap(issue, valueMap);

		auto fieldsObject = issue[L"fields"]->AsObject();
		valueMap[JF_SUMMARY] = fieldsObject[L"summary"]->AsString();
		auto status = fieldsObject[L"status"]->AsObject();
		valueMap[JF_STATUS] = status[L"name"]->AsString();
		auto priority = fieldsObject[L"priority"]->AsObject();
		valueMap[JF_PRIORITY] = priority[L"name"]->AsString();
		auto resolutionDate = fieldsObject[L"resolutiondate"];
		if (resolutionDate->IsNull())
			valueMap[JF_RESOLUTIONDATE] = L"";
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
			valueMap[JF_RESOLUTIONDATE] = strFormat;
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

	CComBSTR bstrUrl(m_bstrUri);
    bstrUrl.Append(L"/2/filter/favourite");
    CComBSTR bstrResult;
	RETURN_IF_FAILED(RemoteCall(bstrUrl, &bstrResult));
	auto value = shared_ptr<JSONValue>(JSON::Parse(bstrResult));
	ATLASSERT(value);

	CComObject<CJiraObjectsCollection>* pCollectionClass;
	RETURN_IF_FAILED(CComObject<CJiraObjectsCollection>::CreateInstance(&pCollectionClass));
	CComPtr<IObjCollection> pCollection;
	RETURN_IF_FAILED(pCollectionClass->QueryInterface(&pCollection));

	auto filters = value->AsArray();
	for(size_t it = 0; it < filters.size(); it++)
	{
		map<wstring, wstring> valueMap;
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

