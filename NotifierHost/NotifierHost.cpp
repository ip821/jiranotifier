// JiraNotifier.cpp : main source file for JiraNotifier.exe
//

#include "stdafx.h"

#include "resource.h"

#include "MainDlg.h"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	int nRet = 0;
	{
		CAppSingleInstance m_appSingleInstance;
		if(m_appSingleInstance.IsAnotherInstanceRunning())
		{
			CString msgAlreadyRunning;
			msgAlreadyRunning.LoadString(IDS_ALREADYRUNNING);
			CString strCaption;
			strCaption.LoadString(IDS_CAPTION);
			MessageBox(NULL, msgAlreadyRunning, strCaption, 0);
			return 1;
		}

		CMessageLoop theLoop;
		_Module.AddMessageLoop(&theLoop);

		SetCurrentDirectoryToApplicationRoot();
		g_hObjMdl = LoadLibrary(L"objmdl.dll");
		GetPluginManager()->InitializePluginLibraryByHandle(g_hObjMdl);
		GetPluginManager()->InitializePluginLibraryByName(L"viewmdl.dll");
		GetPluginManager()->InitializePluginLibraryByName(L"asyncsvc.dll");
		GetPluginManager()->LoadPlugins(NULL);

		CComObject<CMainDlg>* pMainDlg;
		CComObject<CMainDlg>::CreateInstance(&pMainDlg);
		HWND hWnd = 0;

		if (FAILED(pMainDlg->CreateEx(NULL, &hWnd)))
		{
			ATLTRACE(_T("Main dialog creation failed!\n"));
			return 0;
		}

		::ShowWindow(hWnd, SW_HIDE);

		nRet = theLoop.Run();

		_Module.RemoveMessageLoop();
	}

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
