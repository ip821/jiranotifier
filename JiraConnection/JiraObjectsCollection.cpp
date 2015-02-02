// JiraObjectsCollection.cpp : Implementation of CJiraObjectsCollection

#include "stdafx.h"
#include "JiraObjectsCollection.h"


// CJiraObjectsCollection

STDMETHODIMP CJiraObjectsCollection::IsDirty()
{
	return S_OK;
}

STDMETHODIMP CJiraObjectsCollection::InitNew(IStorage *pStg)
{
	RETURN_IF_FAILED(m_pCollection->Clear());
	return S_OK;
}

STDMETHODIMP CJiraObjectsCollection::Load(IStorage *pStg)
{
	if(pStg == NULL)
		return E_POINTER;

	CComPtr<IStream> pInfoStream;
	RETURN_IF_FAILED(pStg->OpenStream(STREAM_INFO, 0, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pInfoStream));

	ULONG ulCount = 0;
	ULONG ulRead = 0;
	RETURN_IF_FAILED(pInfoStream->Read(&ulCount, sizeof(ULONG), &ulRead));

	CComPtr<IStream> pDataStream;
	RETURN_IF_FAILED(pStg->OpenStream(STREAM_DATA, 0, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pDataStream));

	for(ULONG i = 0; i < ulCount; i++)
	{
		CComPtr<IJiraObject> pJiraObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_JiraObject, &pJiraObject));
		CComQIPtr<IPersistStream> pPersistStream = pJiraObject;
		RETURN_IF_FAILED(pPersistStream->Load(pDataStream));
		RETURN_IF_FAILED(m_pCollection->AddObject(pJiraObject));
	}
	return S_OK;
}

STDMETHODIMP CJiraObjectsCollection::Save(IStorage *pStgSave, BOOL fSameAsLoad)
{
	if(pStgSave == NULL)
		return E_POINTER;

	CComPtr<IStream> pInfoStream;
	RETURN_IF_FAILED(pStgSave->CreateStream(STREAM_INFO, STGM_CREATE | STGM_WRITE | STGM_SHARE_EXCLUSIVE, 0, 0, &pInfoStream));

	UINT uiCount = 0;
	RETURN_IF_FAILED(m_pCollection->GetCount(&uiCount));

	ULONG ulWritten = 0;
	RETURN_IF_FAILED(pInfoStream->Write(&uiCount, sizeof(ULONG), &ulWritten));

	CComPtr<IStream> pDataStream;
	RETURN_IF_FAILED(pStgSave->CreateStream(STREAM_DATA, STGM_CREATE | STGM_WRITE | STGM_SHARE_EXCLUSIVE, 0, 0, &pDataStream));

	RETURN_IF_FAILED(m_pCollection->GetCount(&uiCount));

	for(UINT i = 0; i < uiCount; i++)
	{
		CComPtr<IUnknown> pUnk;
		RETURN_IF_FAILED(m_pCollection->GetAt(i, __uuidof(IUnknown), (LPVOID*)&pUnk));
		CComQIPtr<IJiraObject> pJiraObject = pUnk;

		CComQIPtr<IPersistStream> pPersistStream = pJiraObject;
		RETURN_IF_FAILED(pPersistStream->Save(pDataStream, TRUE));
	}
	return S_OK;
}

STDMETHODIMP CJiraObjectsCollection::SaveCompleted(IStorage *pStgNew)
{
	return S_OK;
}

STDMETHODIMP CJiraObjectsCollection::HandsOffStorage()
{
	return S_OK;
}

STDMETHODIMP CJiraObjectsCollection::GetClassID(CLSID *pClassID)
{
	if(pClassID == NULL)
		return E_POINTER;

	*pClassID = GetObjectCLSID();
	return S_OK;
}
