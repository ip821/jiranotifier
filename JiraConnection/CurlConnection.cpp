#include "stdafx.h"
#include "CurlConnection.h"
#include "..\model-libs\objmdl\StringUtils.h"

CCurlConnection::CCurlConnection()
{
}

CCurlConnection::~CCurlConnection()
{
}

STDMETHODIMP CCurlConnection::RemoteCall(string strQuery, string strUser, string strPass, string& strResult)
{
	CURL* curl = curl_easy_init();
	if (!curl)
		return E_FAIL;

	char errorBuffer[1024] = { 0 };

	USES_CONVERSION;

	curl_easy_setopt(curl, CURLOPT_URL, strQuery.c_str());
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

	string strAuthData(strUser + string(":") + strPass);
	string strAuthDataBaseEncoded(base64_encode((const unsigned char*)strAuthData.c_str(), strAuthData.size()));

	struct curl_slist* pHeaderList = NULL;
	pHeaderList = curl_slist_append(pHeaderList, string("Authorization: Basic " + strAuthDataBaseEncoded).c_str());
	curl_slist_append(pHeaderList, "User-Agent: Jira Notifier");

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, pHeaderList);

	{
		WINHTTP_CURRENT_USER_IE_PROXY_CONFIG proxyConfig = { 0 };
		if (WinHttpGetIEProxyConfigForCurrentUser(&proxyConfig))
		{
			vector<CString> proxies;
			StrSplit(proxyConfig.lpszProxy, L";", proxies);
			for (auto it = proxies.begin(); it != proxies.end(); it++)
			{
				vector<CString> values;
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
	strResult = m_callbackData;
	m_callbackData.clear();

	return curlHr;
}

int CCurlConnection::CurlCallback(char* data, size_t size, size_t nmemb, CCurlConnection* pObj)
{
	if (pObj && data)
	{
		/* Save http response in twitcurl object's buffer */
		return pObj->SaveLastWebResponse(data, (size*nmemb));
	}
	return 0;
}

int CCurlConnection::SaveLastWebResponse(char*& data, size_t size)
{
	if (data && size)
	{
		/* Append data in our internal buffer */
		m_callbackData.append(data, size);
		return (int)size;
	}
	return 0;
}
