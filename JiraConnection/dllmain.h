// dllmain.h : Declaration of module class.

class CJiraConnectionModule : public ATL::CAtlDllModuleT< CJiraConnectionModule >
{
public :
	DECLARE_LIBID(LIBID_JiraConnectionLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_JIRACONNECTION, "{2205AE40-2B76-4E5B-9E7C-2227C3545A48}")
};

extern class CJiraConnectionModule _AtlModule;
