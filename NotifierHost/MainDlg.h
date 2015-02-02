// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "viewmdl_contract_i.h"
#include "NotifierHost_contract_i.h"
#include "..\ViewMdl\ChildDialogControl.h"
#include "Plugins.h"

DEFINE_GUID(CLSID_SettingsControl, 0x11f756d4, 0x7bd4, 0x4a51, 0xb9, 0xf1, 0x29, 0x73, 0x62, 0xe0, 0x8c, 0x1a);

class CMainDlg : 
	public CAxDialogImpl<CMainDlg>, 
	public CUpdateUI<CMainDlg>,
	public CMessageFilter, 
	public CIdleHandler,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMainDlg, &CLSID_CMainDlg>,
	public IMainDialog,
	public CChildDialogControlSupport<CMainDlg>
{
private:

	CComPtr<ISystrayManager> m_pSystrayManager;
	CComPtr<ISystrayWindow> m_pSystrayWindow;
	CComPtr<IPluginSupport> m_pPluginSupport;
	CComPtr<ICommandSupport> m_pCommandSupport;
	CComPtr<ICommand> m_pShowCommand;
	CComPtr<ICommand> m_pAboutCommand;
	CComPtr<ICommand> m_pExitCommand;
	CComPtr<IControl> m_pSettingsControl;
	CComPtr<ISettingsFactory> m_pSettingsFactory;
	CMenu m_menu;

public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	DECLARE_NO_REGISTRY()
	BEGIN_COM_MAP(CMainDlg)
		COM_INTERFACE_ENTRY(IMainDialog)
		COM_INTERFACE_ENTRY(IMainWindow)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_ICommandSupport, m_pCommandSupport)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IMenu, m_pCommandSupport)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IToolbar, m_pCommandSupport)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IServiceProvider, m_pPluginSupport)
	END_COM_MAP()

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_TRAYNOTIFY, OnTrayNotify)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		bHandled = TRUE;
		lResult = OnMessage(uMsg, wParam, lParam, bHandled);
		if (bHandled)
			return TRUE;
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTrayNotify(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	CMainDlg();

	STDMETHOD(InitializePlugins)();

	STDMETHOD(GetSystrayWindow)(ISystrayWindow** ppSystrayWindow);

	STDMETHOD(GetMessageLoop)(IMessageLoop** ppMessageLoop);
	STDMETHOD(SetMessageLoop)(IMessageLoop* pMessageLoop);
	STDMETHOD(ShowWindow)(int cmd, BOOL* bResult);
	STDMETHOD(GetContainerControl)(IContainerControl** ppContainerControl);
	STDMETHOD(SetContainerControl)(IContainerControl* pContainerControl);
	STDMETHOD(SetIcon)(HICON hIcon, BOOL bLarge);
	STDMETHOD(GetMainMenu)(IMenu** ppMenu);
	STDMETHOD(GetToolbar)(IToolbar** ppToolbar);
	STDMETHOD(GetStatusBar)(IStatusBar** ppStatusBar);
	METHOD_EMPTY(STDMETHOD(SetFlags)(MainWindowFlags flags));

	STDMETHOD(GetHWND)(HWND *hWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult);
};

OBJECT_ENTRY_AUTO(CLSID_CMainDlg, CMainDlg)