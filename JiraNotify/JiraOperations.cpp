#include "StdAfx.h"
#include "JiraOperations.h"


CJiraOperations::CJiraOperations(void)
{
}


CJiraOperations::~CJiraOperations(void)
{
}

void CJiraOperations::OpenJiraStartPage()
{
	CComPtr<ISettingsObject> pSettingsObject;
	HRESULT hr = HrCoCreateInstance(CLSID_SettingsObject, &pSettingsObject);
	if(!FAILED(hr))
	{
		CComBSTR strServer;
		hr = pSettingsObject->GetServerString(&strServer);
		ShellExecute((LPCTSTR)strServer);
	}
}