// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "JiraConnection_i.h"
#include "dllmain.h"

CJiraConnectionModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
	BOOL bRes = _AtlModule.DllMain(dwReason, lpReserved); 
	//if(dwReason == DLL_PROCESS_ATTACH)
	//{
	//	_CrtSetBreakAlloc(3101);
	//}
#ifdef _DEBUG
	if(dwReason == DLL_PROCESS_DETACH)
	{
		_CrtDumpMemoryLeaks();
	}
#endif

	return bRes;
}
