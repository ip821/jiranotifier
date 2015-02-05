// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define __JIRACONNECTION__
#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW
#define _CRT_SECURE_NO_WARNINGS

#ifdef _DEBUG
	#define _ATL_DEBUG_INTERFACES
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlconv.h>
#include <atlctl.h>
#include <atlstr.h>
#include <iterator>
#include <vector>
#include <set>
#include <iostream>
#include <memory>
#include <string>

#include <curl/curl.h>
#include <curl/easy.h>
#include <Winhttp.h>

#include "JiraFields.h"
#include "JiraMethods.h"

#include <initguid.h>
#include "..\model-libs\ObjMdl\Macros.h"
#include "..\model-libs\ObjMdl\Plugins.h"
#include "..\model-libs\ObjMdl\Functions.h"
#include "..\model-libs\ObjMdl\Metadata.h"

#define STREAM_INFO _T("INFO")
#define STREAM_DATA _T("DATA")

#include "..\model-libs\json\src\JSON.h"
#include "..\model-libs\json\src\JSONValue.h"
#include "..\model-libs\json\src\urlencode.h"
#include "..\model-libs\json\src\base64.h"

using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wldap32.lib")
