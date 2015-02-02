// stdafx.cpp : source file that includes just the standard includes
//	JiraNotifier.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

HMODULE g_hObjMdl;

void ShellExecute(LPCTSTR lpszUrl)
{
	::ShellExecute(NULL, _T("open"), lpszUrl, NULL, NULL, SW_SHOW);
}
