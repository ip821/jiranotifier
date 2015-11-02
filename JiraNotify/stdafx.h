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
#include <boost/thread/condition_variable.hpp>

#include <initguid.h>
#include "..\model-libs\ObjMdl\Plugins.h"
#include "..\model-libs\ObjMdl\Macros.h"
#include "..\model-libs\ObjMdl\Functions.h"
#include "..\model-libs\ObjMdl\Metadata.h"
#include "..\model-libs\ViewMdl\Plugins.h"
#include "..\model-libs\ViewMdl\Functions.h"
#include "..\model-libs\asyncmdl\Plugins.h"
#include "..\model-libs\asyncmdl\Metadata.h"
#include "..\BaseNotifier\NotifierHost\Plugins.h"
#include "..\JiraConnection\Plugins.h"

#define KEY_HAS_CHANGES L"HasChanges"
#define KEY_HAS_MESSAGE L"HasMessage"
#define KEY_ISSUES L"Issues"

using namespace IP;

#define FILTER_ASSIGNED_TO_ME L"assignee = currentUser() AND status in(Open, \"In Progress\", Reopened) ORDER BY priority DESC, key DESC"
#define FILTER_ASSIGNED_TO_ME_NAME L"Assigned to me"
