#include "stdafx.h"
#include "PluginTable.h"
#include "Plugins.h"

CPluginTable::CPluginTable(void)
{
}

HRESULT CPluginTable::FinalConstruct()
{
	AddObject(CLSID_ObjectStorageManager, L"Object storage manager");
	AddObject(CLSID_SettingsControl, L"Settings control");
	AddObject(CLSID_TrayNotifyManager, L"Tray notify manager");
	AddObject(CLSID_SettingsFactory, L"Settings factory");
	AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_NOTIFIER_SERVICE, CLSID_UpdateIssueService, CLSID_UpdateIssueService, L"Update issues service");
	AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_NOTIFIER_SERVICE, CLSID_UpdateViewService, CLSID_UpdateViewService, L"Update systray service");
	AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_NOTIFIER_COMMAND, CLSID_OpenJiraCommand, CLSID_OpenJiraCommand, L"Open Jira command");
	AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_NOTIFIER_SERVICE, CLSID_TimerService, CLSID_TimerService, L"Timer service");
	AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_NOTIFIER_SERVICE, CLSID_ThreadService, CLSID_ThreadService, L"Thread service");
	return S_OK;
}
