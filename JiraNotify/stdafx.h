// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW
#define _WTL_NO_CSTRING
#define __JIRANOTIFY__

#include "resource.h"
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
#include  <vector>
#include  <list>
#include  <algorithm>
#include  <mutex>

#include <initguid.h>
#include "..\ObjMdl\Plugins.h"
#include "..\ObjMdl\Macros.h"
#include "..\ObjMdl\Functions.h"
#include "..\ViewMdl\Plugins.h"
#include "..\ViewMdl\Functions.h"
#include "..\NotificationServices\Plugins.h"
#include "..\NotifierHost\Plugins.h"
#include "..\JiraConnection\Plugins.h"

#define KEY_SERVER L"Server"
#define KEY_USER L"User"
#define KEY_PASSWORD L"Password"
#define KEY_HAS_CHANGES L"HasChanges"
#define KEY_HAS_MESSAGE L"HasMessage"
