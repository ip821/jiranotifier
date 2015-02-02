// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW
#define _WTL_NO_CSTRING

#include <atlstr.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlcom.h>
#include <atlctl.h>


#include <atlhost.h>
#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlctrlx.h>
#include <atlmisc.h>

extern CAppModule _Module;

#include <vector>
#include <set>
#include <list>
#include <algorithm>
#include <functional>
#include <Shobjidl.h>

#include <initguid.h>
#include "..\model-libs\ObjMdl\Plugins.h"
#include "..\model-libs\ObjMdl\Functions.h"
#include "..\model-libs\ObjMdl\Metadata.h"

#include "..\model-libs\ViewMdl\Plugins.h"
#include "..\model-libs\ViewMdl\Functions.h"

#include "..\JiraConnection\Plugins.h"
#include "..\JiraConnection\JiraFields.h"

#include "Plugins.h"
#include "AppSingleInstance.h"
#include "AutostartManager.h"

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

void ShellExecute(LPCTSTR lpszUrl);