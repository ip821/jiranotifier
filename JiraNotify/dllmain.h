// dllmain.h : Declaration of module class.

class CJiraNotifyModule : public ATL::CAtlDllModuleT< CJiraNotifyModule >
{
public :
	DECLARE_LIBID(LIBID_JiraNotifyLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_JIRANOTIFY, "{A6FC65FE-0838-46A3-8D00-9EA660B3E745}")
};

extern class CJiraNotifyModule _AtlModule;
