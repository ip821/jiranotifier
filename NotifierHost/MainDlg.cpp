// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "MainDlg.h"
#include "ShowWindowCommand.h"

CMainDlg::CMainDlg()
{
}

STDMETHODIMP CMainDlg::GetHWND(HWND *hWnd)
{
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CMainDlg::CreateEx(HWND hWndParent, HWND *hWnd)
{
	*hWnd = Create(hWndParent);
	return S_OK;
}

STDMETHODIMP CMainDlg::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
	*pbResult = PreTranslateMessage(pMsg);
	return S_OK;
}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CAxDialogImpl<CMainDlg>::OnInitDialog(uMsg, wParam, lParam, bHandled);

	CenterWindow();

    CMenu sysMenu = GetSystemMenu(FALSE);
    sysMenu.EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SettingsFactory, &m_pSettingsFactory));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SystrayManager, &m_pSystrayManager));
	RETURN_IF_FAILED(m_pSystrayManager->CreateSystrayWindow(m_hWnd, &m_pSystrayWindow));

	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_PluginSupport, &m_pPluginSupport));
	RETURN_IF_FAILED(m_pPluginSupport->InitializePlugins(PNAMESP_HOSTFORM, PVIEWTYPE_NOTIFIER_COMMAND));
	RETURN_IF_FAILED(m_pPluginSupport->InitializePlugins(PNAMESP_HOSTFORM, PVIEWTYPE_NOTIFIER_SERVICE));
	RETURN_IF_FAILED(InitializePlugins());

	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_CommandSupport, &m_pCommandSupport));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_AboutCommand, &m_pAboutCommand));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ExitCommand, &m_pExitCommand));
	RETURN_IF_FAILED(CComObject<CShowWindowCommand>::_CreatorClass::CreateInstance(NULL, IID_ICommand, (LPVOID*)&m_pShowCommand));

	CComPtr<IUnknown> pThis;
	RETURN_IF_FAILED(QueryInterface(IID_IUnknown, (LPVOID*)&pThis));
	RETURN_IF_FAILED(HrInitializeWithControl(m_pShowCommand, pThis));
	RETURN_IF_FAILED(HrInitializeWithControl(m_pExitCommand, pThis));
	RETURN_IF_FAILED(HrInitializeWithControl(m_pCommandSupport, pThis));

	m_menu.CreatePopupMenu();

	RETURN_IF_FAILED(m_pCommandSupport->EnableStdCommands(FALSE));
	RETURN_IF_FAILED(m_pCommandSupport->SetMenu(m_menu));
	RETURN_IF_FAILED(m_pCommandSupport->InstallCommand(m_pShowCommand));
	RETURN_IF_FAILED(m_pCommandSupport->InstallCommand(m_pAboutCommand));
	RETURN_IF_FAILED(m_pCommandSupport->InstallCommand(m_pExitCommand));
	RETURN_IF_FAILED(m_pCommandSupport->InstallCommands(m_pPluginSupport));
	RETURN_IF_FAILED(m_pSystrayWindow->SetMenu(m_menu));

	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SettingsControl, &m_pSettingsControl));
	RETURN_IF_FAILED(InsertToGroupboxPlaceholder(m_pSettingsControl, IDC_PLACEHOLDER, FALSE));
	return TRUE;
}

LRESULT CMainDlg::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_pPluginSupport)
		return 0;

	bHandled = FALSE;
	CComPtr<IObjArray> pObjectArray;
	RETURN_IF_FAILED(m_pPluginSupport->GetPlugins(&pObjectArray));
	UINT cb = 0;
	RETURN_IF_FAILED(pObjectArray->GetCount(&cb));
	for (UINT i = 0; i < cb; i++)
	{
		CComPtr<IMsgHandler> pMsgHandler;
		HRESULT hr = pObjectArray->GetAt(i, IID_IMsgHandler, (LPVOID*)&pMsgHandler);
		if (hr == E_NOINTERFACE)
			continue;

		if (pMsgHandler)
		{
			LRESULT lResult = 0;
			BOOL bResult = FALSE;
			pMsgHandler->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, &lResult, &bResult);
			if (bResult)
				return bResult;
		}
	}
	return 0;
}

LRESULT CMainDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	RETURN_IF_FAILED(m_pPluginSupport->OnShutdown());
	LRESULT lResult = 0;
	BOOL b = FALSE;
	m_pSystrayWindow.Release();
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);
	DestroyWindow();
	PostQuitMessage(0);
	return 0;
}

STDMETHODIMP CMainDlg::InitializePlugins()
{
	RETURN_IF_FAILED(HrInitializeWithControl(m_pPluginSupport, this));

	CComPtr<ISettings> pSettings;
	RETURN_IF_FAILED(m_pSettingsFactory->CreateSettings(&pSettings));

	HRESULT hr = HrIteratePlugins<IInitializeWithSettings>(
		m_pPluginSupport,
		[&](IInitializeWithSettings* p)
	{
		RETURN_IF_FAILED(p->Load(pSettings));
		return S_OK;
	}
	);

	RETURN_IF_FAILED(hr);
	RETURN_IF_FAILED(m_pPluginSupport->OnInitialized());
	return S_OK;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	RETURN_IF_FAILED(m_pPluginSupport->OnShutdown());

	CComPtr<ISettings> pSettings;
	RETURN_IF_FAILED(m_pSettingsFactory->CreateSettings(&pSettings));
	CComQIPtr<IPersistSettings> pPersistSettings = m_pSettingsControl;
	RETURN_IF_FAILED(pPersistSettings->Save(pSettings));

	CButton wndCheckButton = GetDlgItem(IDC_CHECKAUTOSTART);
	BOOL bChecked = wndCheckButton.GetCheck();
	CAutostartManager::SetAutostart(bChecked);
	RETURN_IF_FAILED(pSettings->SetVariantValue(KEY_AUTOSTART, &CComVariant(bChecked)));

	RETURN_IF_FAILED(InitializePlugins());

	BOOL bResult = FALSE;
	ShowWindow(SW_HIDE, &bResult);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bResult = FALSE;
	ShowWindow(SW_HIDE, &bResult);
	return 0;
}

LRESULT CMainDlg::OnTrayNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lResult = 0;
	if (m_pSystrayWindow)
		m_pSystrayWindow->OnTrayNotify(uMsg, wParam, lParam, &lResult, &bHandled);
	return lResult;
}

LRESULT CMainDlg::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BOOL bResult = FALSE;
	LRESULT lResult = 0;
	RETURN_IF_FAILED(m_pCommandSupport->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, &lResult, &bResult)); //systray menu
	bHandled = bResult;
	return lResult;
}

STDMETHODIMP CMainDlg::GetSystrayWindow(ISystrayWindow** ppSystrayWindow)
{
	RETURN_IF_FAILED(m_pSystrayWindow->QueryInterface(__uuidof(ISystrayWindow), (LPVOID*)ppSystrayWindow));
	return S_OK;
}

STDMETHODIMP CMainDlg::GetMessageLoop(IMessageLoop** ppMessageLoop)
{
	return E_NOTIMPL;
}

STDMETHODIMP CMainDlg::SetMessageLoop(IMessageLoop* pMessageLoop)
{
	return E_NOTIMPL;
}

STDMETHODIMP CMainDlg::ShowWindow(int cmd, BOOL* bResult)
{
	switch (cmd)
	{
	case SW_SHOW:
	{
		CComPtr<ISettings> pSettings;
		RETURN_IF_FAILED(m_pSettingsFactory->CreateSettings(&pSettings));
		CComQIPtr<IPersistSettings> pPersistSettings = m_pSettingsControl;
		RETURN_IF_FAILED(pPersistSettings->Load(pSettings));

		CButton wndCheckButton = GetDlgItem(IDC_CHECKAUTOSTART);
		CComVariant vChecked;
		RETURN_IF_FAILED(pSettings->GetVariantValue(KEY_AUTOSTART, &vChecked));
		if (vChecked.vt == VT_I4)
		{
			wndCheckButton.SetCheck(vChecked.intVal > 0);
		}
	}
		goto lbl_hide;
		break;
	case SW_HIDE:
		lbl_hide:
		__super::ShowWindow(cmd);
		break;
	default:
		return E_INVALIDARG;
		break;
	}

	return 0;
}

STDMETHODIMP CMainDlg::GetContainerControl(IContainerControl** ppContainerControl)
{
	return E_NOTIMPL;
}

STDMETHODIMP CMainDlg::SetContainerControl(IContainerControl* pContainerControl)
{
	return E_NOTIMPL;
}

STDMETHODIMP CMainDlg::SetIcon(HICON hIcon, BOOL bLarge)
{
	CAxDialogImpl::SetIcon(hIcon, bLarge);
	return S_OK;
}

STDMETHODIMP CMainDlg::GetMainMenu(IMenu** ppMenu)
{
	CHECK_E_POINTER(ppMenu);
	return QueryInterface(IID_IMenu, (LPVOID*)ppMenu);
}

STDMETHODIMP CMainDlg::GetToolbar(IToolbar** ppToolbar)
{
	return E_NOTIMPL;
}

STDMETHODIMP CMainDlg::GetStatusBar(IStatusBar** ppStatusBar)
{
	return E_NOTIMPL;
}
