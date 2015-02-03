#include "StdAfx.h"
#include "resource.h"
#include "TrayNotifyManager.h"
#include "viewmdl_contract_i.h"

CTrayNotifyManager::CTrayNotifyManager()
{
	m_trayIcon.LoadIcon(IDR_MAINFRAME, 16, 16);
	m_trayUpdateIcon.LoadIcon(IDI_UPDATEICON, 16, 16);
	m_trayErrorIcon.LoadIcon(IDI_ERRORICON, 16, 16);
	m_trayNewItemsIcon.LoadIcon(IDI_NEWICON, 16, 16);
}

STDMETHODIMP CTrayNotifyManager::Shutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pSystrayWindow, __uuidof(ISystrayWindowEventSink), dw_advice));
	m_pSystrayWindow.Release();
	return S_OK;
}

STDMETHODIMP CTrayNotifyManager::Initialize(ISystrayWindow* pSystrayWindow)
{
	CHECK_E_POINTER(pSystrayWindow);
	m_pSystrayWindow = pSystrayWindow;
	m_pSystrayWindow->SetTooltip(L"Jira notifier");
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	AtlAdvise(m_pSystrayWindow, pUnk, __uuidof(ISystrayWindowEventSink), &dw_advice);
	return S_OK;
}

void CTrayNotifyManager::ChangeIcon(HICON hIcon)
{
	m_currentIcon = hIcon;
	m_pSystrayWindow->SetIcon(hIcon);
}

STDMETHODIMP CTrayNotifyManager::OnTrayNotification(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT lpResult, BOOL pbHandled)
{
	if (lParam == NIN_BALLOONSHOW)
	{
		m_bBaloonIsVisible = true;
	}

	if (lParam == NIN_BALLOONHIDE)
	{
		m_bBaloonIsVisible = false;
	}

	if (lParam == NIN_BALLOONTIMEOUT)
	{
		m_bBaloonIsVisible = false;
	}

	if (lParam == NIN_BALLOONUSERCLICK)
	{
		m_bBaloonIsVisible = false;
	}

	return S_OK;
}

STDMETHODIMP CTrayNotifyManager::ShowBaloon(LPCTSTR lpszText, LPCTSTR lpszCaption, SystrayBalloonStyle style, UINT uTimeout)
{
	if (!m_bBaloonIsVisible)
	{
		RETURN_IF_FAILED(m_pSystrayWindow->ShowBaloon(CComBSTR(lpszText), CComBSTR(lpszCaption), style, uTimeout));
	}
	return S_OK;
}

STDMETHODIMP CTrayNotifyManager::ResetIconToNormalIfPossible()
{
	if (!m_currentIcon || m_currentIcon == m_trayNewItemsIcon)
		ChangeIcon(m_trayIcon);
	return S_OK;
}

STDMETHODIMP CTrayNotifyManager::ShowErrorIcon()
{
	ChangeIcon(m_trayErrorIcon);
	return S_OK;
}

STDMETHODIMP CTrayNotifyManager::ShowNewItemsIcon()
{
	ChangeIcon(m_trayNewItemsIcon);
	return S_OK;
}

STDMETHODIMP CTrayNotifyManager::ShowNormalIcon()
{
	ChangeIcon(m_trayIcon);
	return S_OK;
}

STDMETHODIMP CTrayNotifyManager::ShowUpdateIcon()
{
	ChangeIcon(m_trayUpdateIcon);
	return S_OK;
}
