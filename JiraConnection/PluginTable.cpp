#include "stdafx.h"
#include "PluginTable.h"

CPluginTable::CPluginTable(void)
{
}

HRESULT CPluginTable::FinalConstruct()
{
	RETURN_IF_FAILED(AddObject(CLSID_JiraConnection, _T("Jira connection")));
	RETURN_IF_FAILED(AddObject(CLSID_JiraObjectsCollection, _T("Jira objects collection")));
	RETURN_IF_FAILED(AddObject(CLSID_JiraObject, _T("Jira object")));
	return S_OK;
}
