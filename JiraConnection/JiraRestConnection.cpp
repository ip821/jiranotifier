#include "stdafx.h"
#include "JiraRestConnection.h"

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

void CJiraConnection::CopyToStringMap(web::json::object& source, std::map<std::wstring, std::wstring>& dest)
{
	for(auto it = source.begin(); it != source.end(); it++)
	{
		if(it->second.type() != web::json::value::value_type::String)
			continue;

		std::wstring key = it->first;
		if(m_supportedValues.find(key) == m_supportedValues.end())
			continue;
		std::wstring val = it->second.as_string();
		dest[key] = val;
	}
}

STDMETHODIMP CJiraConnection::RemoteCall(std::wstring& query, web::json::value& value)
{
	try
	{
		//http_client_config config;
		//config.set_credentials(credentials(std::wstring(m_user), std::wstring(m_passwd)));
		http_client client(query);
		http_request req(methods::GET);

		USES_CONVERSION;
		std::string str(CString(W2A(m_user)) + ":" + W2A(m_passwd));
		std::vector<unsigned char> vec(str.cbegin(), str.cend());
		req.headers().add(L"Authorization", L"Basic " + conversions::to_base64(vec));

		CString strErrorMessage;
		client
			.request(req)
			.then([&](http_response response)
		{
			USES_CONVERSION;
			if (response.status_code() == status_codes::OK)
			{
				value = response.extract_json().get();
			}
			else
			{
				strErrorMessage = response.reason_phrase().c_str();
				auto data = response.extract_vector().get(); //due to bug in the casablanca i have to read the data anyway... check works in release!
			}
		})
			.wait();

		if (!strErrorMessage.IsEmpty())
		{
			m_strLastErrorMsg = strErrorMessage;
			SetErrorInfo(0, this);
			return E_CALL_FAILED;
		}
	}
	catch (const http_exception& ex)
	{
		auto errCode = ex.error_code();
		auto errCodeValue = errCode.value();
		if (errCodeValue)
		{
			m_strLastErrorMsg = CA2W(ex.error_code().message().c_str());
		}
		else
		{
			m_strLastErrorMsg = CA2W(ex.what());
		}
		SetErrorInfo(0, this);
		return E_CALL_FAILED;
	}
	catch (const std::exception& ex)
	{
		m_strLastErrorMsg = CA2W(ex.what());
		SetErrorInfo(0, this);
		return E_CALL_FAILED;
	}

	return S_OK;
}

STDMETHODIMP CJiraConnection::GetCurrentUser(IJiraObject** ppObject)
{
	if (ppObject == NULL)
		return E_POINTER;

	web::json::value value;
	RETURN_IF_FAILED(RemoteCall(m_strUri + L"/2/myself", value));
	
	std::map<std::wstring, std::wstring> valueMap;
	CopyToStringMap(value.as_object(), valueMap);

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

	uri_builder builder(m_strUri);
	builder.append_path(L"/2/search");
	builder.append_query(L"fields", std::wstring(strFields));
	builder.append_query(L"jql", searchQuery);
	web::json::value value;
	RETURN_IF_FAILED(RemoteCall(builder.to_string(), value));

	auto issues = value.as_object()[L"issues"].as_array();

	CComObject<CJiraObjectsCollection>* pCollectionClass;
	RETURN_IF_FAILED(CComObject<CJiraObjectsCollection>::CreateInstance(&pCollectionClass));
	CComPtr<IObjCollection> pCollection;
	RETURN_IF_FAILED(pCollectionClass->QueryInterface(&pCollection));

	for (auto it = issues.begin(); it != issues.end(); it++)
	{
		std::map<std::wstring, std::wstring> valueMap;
		web::json::object& issue = it->as_object();
		CopyToStringMap(issue, valueMap);
		valueMap[(BSTR)JF_SUMMARY] = issue[L"fields"][L"summary"].as_string();
		valueMap[(BSTR)JF_STATUS] = issue[L"fields"][L"status"][L"name"].as_string();
		valueMap[(BSTR)JF_PRIORITY] = issue[L"fields"][L"priority"][L"name"].as_string();
		auto resolutionDate = issue[L"fields"][L"resolutiondate"];
		if (resolutionDate.is_null())
			valueMap[(BSTR)JF_RESOLUTIONDATE] = L"";
		else
		{
			auto resolutionDateStr = resolutionDate.as_string();
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

	web::json::value value;
	RETURN_IF_FAILED(RemoteCall(m_strUri + L"/2/filter/favourite", value));

	CComObject<CJiraObjectsCollection>* pCollectionClass;
	RETURN_IF_FAILED(CComObject<CJiraObjectsCollection>::CreateInstance(&pCollectionClass));
	CComPtr<IObjCollection> pCollection;
	RETURN_IF_FAILED(pCollectionClass->QueryInterface(&pCollection));

	auto filters = value.as_array();
	for(auto it = filters.begin(); it != filters.end(); it++)
	{
		std::map<std::wstring, std::wstring> valueMap;
		CopyToStringMap(it->as_object(), valueMap);

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

