#pragma once 

#include "stdafx.h"
#include "OpenJiraCommand.h"
#include "JiraNotify_i.h"

STDMETHODIMP COpenJiraCommand::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_UpdateViewService, &m_pUpdateViewService));
	return S_OK;
}

STDMETHODIMP COpenJiraCommand::OnShutdown()
{
	m_pUpdateViewService.Release();
	return S_OK;
}

STDMETHODIMP COpenJiraCommand::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	m_pSettings.Release();
	m_pSettings = pSettings;
	return S_OK;
}

STDMETHODIMP COpenJiraCommand::GetCommandText(REFGUID guidCommand, BSTR* bstrText)
{
	CHECK_E_POINTER(bstrText);
	*bstrText = CComBSTR(L"Open Jira...");
	return S_OK;
}

STDMETHODIMP COpenJiraCommand::InstallMenu(IMenu* pMenu)
{
	CHECK_E_POINTER(pMenu);
	return pMenu->InsertMenuCommand(GUID_NULL, CLSID_OpenJiraCommand, 0, this);
}

STDMETHODIMP COpenJiraCommand::Invoke(REFGUID guidCommand)
{
	RETURN_IF_FAILED(m_pUpdateViewService->ResetIcon());
	CComVariant vServer;
	if (SUCCEEDED(m_pSettings->GetVariantValue(KEY_SERVER, &vServer)) && vServer.vt == VT_BSTR)
	{
		ShellExecute(NULL, L"open", vServer.bstrVal, NULL, NULL, SW_SHOW);
	}
	return S_OK;
}