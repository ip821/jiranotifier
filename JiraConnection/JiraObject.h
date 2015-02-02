// JiraObject.h : Declaration of the CJiraObject

#pragma once

#include <map>

#include "resource.h"       // main symbols
#include "JiraConnection_i.h"
#include "..\ObjMdl\Plugins.h"

using namespace ATL;


// CJiraObject

class ATL_NO_VTABLE CJiraObject :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CJiraObject, &CLSID_JiraObject>,
	public IPersistStream,
	public IJiraObject
{
public:
	CJiraObject()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_JIRAOBJECT)

	BEGIN_COM_MAP(CJiraObject)
		COM_INTERFACE_ENTRY(IJiraObject)
		COM_INTERFACE_ENTRY(IPersistStream)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IVariantObject, m_pVariantObject)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return HrCoCreateInstance(CLSID_VariantObject, &m_pVariantObject);
	}

	void FinalRelease()
	{
	}

private:
	CComPtr<IVariantObject> m_pVariantObject;
public:
	STDMETHOD(GetClassID(CLSID *pClassID));
	STDMETHOD(IsDirty());
	STDMETHOD(Load(IStream *pStm));
	STDMETHOD(Save(IStream *pStm, BOOL fClearDirty));
	STDMETHOD(GetSizeMax(ULARGE_INTEGER *pcbSize));

	STDMETHOD(SetItem(std::map<std::wstring, std::wstring>& m_value));

};

OBJECT_ENTRY_AUTO(__uuidof(JiraObject), CJiraObject)
