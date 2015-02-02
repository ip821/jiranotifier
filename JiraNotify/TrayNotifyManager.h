#pragma once

#include <atlapp.h>
#include "JiraNotify_i.h"

class CTrayNotifyManager :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTrayNotifyManager, &CLSID_TrayNotifyManager>,
	public ITrayNotifyManager,
	public ISystrayWindowEventSink
{
private:
	CIcon m_trayIcon;
	CIcon m_trayUpdateIcon;
	CIcon m_trayErrorIcon;
	CIcon m_trayNewItemsIcon;
	HICON m_currentIcon = 0;
	bool m_bBaloonIsVisible = false;
	CComPtr<ISystrayWindow> m_pSystrayWindow;
	DWORD dw_advice = 0;

	void ChangeIcon(HICON hIcon);

public:
	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CTrayNotifyManager)
		COM_INTERFACE_ENTRY(ITrayNotifyManager)
		COM_INTERFACE_ENTRY(ISystrayWindowEventSink)
	END_COM_MAP()

	CTrayNotifyManager();

	STDMETHOD(Initialize)(ISystrayWindow* pSystrayWindow);
	STDMETHOD(Shutdown)();
	STDMETHOD(OnTrayNotification)(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT lResult, BOOL pbHandled);
	STDMETHOD(ShowBaloon)(LPCTSTR lpszText, LPCTSTR lpszCaption, SystrayBalloonStyle style, UINT uTimeout);
	STDMETHOD(ShowErrorIcon)();
	STDMETHOD(ShowNewItemsIcon)();
	STDMETHOD(ShowNormalIcon)();
	STDMETHOD(ShowUpdateIcon)();
	STDMETHOD(ResetIconToNormalIfPossible)();
};

OBJECT_ENTRY_AUTO(CLSID_TrayNotifyManager, CTrayNotifyManager)