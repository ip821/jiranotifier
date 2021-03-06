// JiraXmlRpcConnection2.h : Declaration of the CJiraConnection

#pragma once
#include "resource.h"       // main symbols
#include "JiraConnection_i.h"
#include "JiraObjectsCollection.h"
#include "JiraObject.h"
#include "netmdl_contract_i.h"
#include "..\model-libs\netmdl\Plugins.h"
#include "..\model-libs\netmdl\Metadata.h"

using namespace ATL;

#define E_CALL_FAILED E_FAIL

class ATL_NO_VTABLE CJiraConnection :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IErrorInfo,
	public ISupportErrorInfoImpl<&IID_IJiraConnection>,
	public CComCoClass<CJiraConnection, &CLSID_JiraConnection>,
	public IJiraConnection
{
public:
	CJiraConnection()
	{
		InitStoredValuesMap();
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_JIRAXMLRPCCONNECTION2)

	BEGIN_COM_MAP(CJiraConnection)
		COM_INTERFACE_ENTRY(IJiraConnection)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
		COM_INTERFACE_ENTRY(IErrorInfo)
	END_COM_MAP()

private:
    CComBSTR m_bstrUser;
	CComBSTR m_bstrPasswd;
	CString m_strLastErrorMsg;
	std::set<std::wstring> m_supportedValues;
    CComBSTR m_bstrUri;
    CComPtr<IHttpConnection> m_pHttpConnection;

	void CopyToStringMap(const JSONObject& source, std::map<std::wstring, std::wstring>& dest);
	void InitStoredValuesMap();
	STDMETHOD(RemoteCall)(BSTR bstrQuery, BSTR* pbstrResult);

public:

	STDMETHOD(OpenConnection)(BSTR bstrUri);
	STDMETHOD(Login(BSTR bstrUser, BSTR bstrPwd));
	STDMETHOD(Logout());
	STDMETHOD(CloseConnection());
	STDMETHOD(GetFavoriteFilters(IJiraObjectsCollection** ppCollection));
	STDMETHOD(GetIssuesByFilter(BSTR bstrFilterId, IJiraObjectsCollection** ppCollection));
	STDMETHOD(GetCurrentUser)(IJiraObject** ppObject);
	STDMETHOD(GetIssuesByCriteria)(BSTR bstrSearchCriteria, IJiraObjectsCollection** ppCollection);

	STDMETHOD(GetGUID(GUID *pGUID));
	STDMETHOD(GetSource(BSTR *pBstrSource));
	STDMETHOD(GetDescription(BSTR *pBstrDescription));
	STDMETHOD(GetHelpFile(BSTR *pBstrHelpFile));
	STDMETHOD(GetHelpContext(DWORD *pdwHelpContext));

};

OBJECT_ENTRY_AUTO(__uuidof(JiraConnection), CJiraConnection)
