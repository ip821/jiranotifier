#include "stdafx.h"
#include "ShowWindowCommand.h"
#include "NotifierHost_contract_i.h"

STDMETHODIMP CShowWindowCommand::GetCommandText(REFGUID guidCommand, BSTR* bstrText)
{
	CHECK_E_POINTER(bstrText);
	*bstrText = CComBSTR(L"Settings...");
	return S_OK;
}

STDMETHODIMP CShowWindowCommand::InstallMenu(IMenu* pMenu)
{
	CHECK_E_POINTER(pMenu);
	return pMenu->AddMenuCommand(COMMAND_FILE, COMMAND_EXIT, this);
}

STDMETHODIMP CShowWindowCommand::Invoke(REFGUID guidCommand)
{
	CComQIPtr<IMainDialog> pMainDialog = m_pControl;
	RETURN_IF_FAILED(pMainDialog->ShowWindow(SW_SHOW, NULL));
	return S_OK;
}