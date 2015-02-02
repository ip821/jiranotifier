// JiraObject.cpp : Implementation of CJiraObject

#include "stdafx.h"
#include "JiraObject.h"


// CJiraObject

STDMETHODIMP CJiraObject::SetItem(std::map<std::wstring,std::wstring>& val)
{
	for (auto it = val.cbegin(); it != val.cend(); it++)
	{
		RETURN_IF_FAILED(m_pVariantObject->SetVariantValue(CComBSTR(it->first.c_str()), &CComVariant(CComBSTR(it->second.c_str()))));
	}

	return S_OK;
}

STDMETHODIMP CJiraObject::IsDirty()
{
	return S_OK;
}

STDMETHODIMP CJiraObject::Load(IStream *pStm)
{
	if(pStm == NULL)
		return E_POINTER;

	int iSize = 0;
	ULONG uRead = 0;
	RETURN_IF_FAILED(pStm->Read(&iSize, sizeof(int), &uRead));

	USES_CONVERSION;
	for(int i = 0; i < iSize; i++)
	{
		CComBSTR strKey;
		CComBSTR strValue;

		RETURN_IF_FAILED(strKey.ReadFromStream(pStm));
		RETURN_IF_FAILED(strValue.ReadFromStream(pStm));
		RETURN_IF_FAILED(m_pVariantObject->SetVariantValue(strKey, &CComVariant(strValue)));
	}

	return S_OK;
}

STDMETHODIMP CJiraObject::Save(IStream *pStm, BOOL fClearDirty)
{
	if(pStm == NULL)
		return E_POINTER;

	UINT_PTR uiCount = 0;
	RETURN_IF_FAILED(m_pVariantObject->GetCount(&uiCount));
	int iSize = static_cast<int>(uiCount);
	ULONG uWritten = 0;
	RETURN_IF_FAILED(pStm->Write(&iSize, sizeof(int), &uWritten));

	for(UINT_PTR i = 0; i < uiCount; i++)
	{
		CComBSTR strKey;
		RETURN_IF_FAILED(m_pVariantObject->GetKeyByIndex(i, &strKey));
		CComVariant vValue;
		RETURN_IF_FAILED(m_pVariantObject->GetVariantValue(strKey, &vValue));
		CComBSTR strValue;
		if (vValue.vt == VT_BSTR)
			strValue = vValue.bstrVal;
		RETURN_IF_FAILED(strKey.WriteToStream(pStm));
		RETURN_IF_FAILED(strValue.WriteToStream(pStm));
	}

	return S_OK;
}

STDMETHODIMP CJiraObject::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	return E_NOTIMPL;
}

STDMETHODIMP CJiraObject::GetClassID(CLSID *pClassID)
{
	if(pClassID == NULL)
		return E_POINTER;

	*pClassID = GetObjectCLSID();
	return S_OK;
}
