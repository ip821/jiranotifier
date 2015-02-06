#pragma once
class CCurlConnection
{
private:
	string m_callbackData;
	string m_strUrl;

	static int CurlCallback(char* data, size_t size, size_t nmemb, CCurlConnection* pObj);
	int SaveLastWebResponse(char*& data, size_t size);

public:
	CCurlConnection();
	~CCurlConnection();

	STDMETHOD(RemoteCall)(string strQuery, string strUser, string strPass, string& strResult);
};

