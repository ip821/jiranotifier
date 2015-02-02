// JiraObjectsCollection.h : Declaration of the CJiraObjectsCollection

#pragma once
#include "resource.h"       // main symbols
#include "JiraConnection_i.h"

using namespace ATL;

class ATL_NO_VTABLE CJiraObjectsCollection :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CJiraObjectsCollection, &CLSID_JiraObjectsCollection>,
	public IPersistStorage,
	public IJiraObjectsCollection
{
public:
	CJiraObjectsCollection()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_JIRAOBJECTSCOLLECTION)

	BEGIN_COM_MAP(CJiraObjectsCollection)
		COM_INTERFACE_ENTRY(IJiraObjectsCollection)
		COM_INTERFACE_ENTRY(IPersistStorage)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IObjectCollection, m_pCollection)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IObjectArray, m_pCollection)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return m_pCollection.CoCreateInstance(CLSID_EnumerableObjectCollection);
	}

	void FinalRelease()
	{
	}

private:
	CComPtr<IObjectCollection> m_pCollection;
public:

	STDMETHOD(GetClassID(CLSID *pClassID));
	STDMETHOD(IsDirty(void));
	STDMETHOD(InitNew(IStorage *pStg));
	STDMETHOD(Load(IStorage *pStg));
	STDMETHOD(Save(IStorage *pStgSave, BOOL fSameAsLoad));
	STDMETHOD(SaveCompleted(IStorage *pStgNew));
	STDMETHOD(HandsOffStorage());
};

OBJECT_ENTRY_AUTO(__uuidof(JiraObjectsCollection), CJiraObjectsCollection)
