#include "stdafx.h"
#include "SettingsFactory.h"

// CSettingsFactory

#define SETTINGS_PATH L"Software\\IP\\JiraNotifier"

STDMETHODIMP CSettingsFactory::CreateSettings(ISettings** ppSettings)
{
	CHECK_E_POINTER(ppSettings);
	RETURN_IF_FAILED(HrOpenSettings(SETTINGS_PATH, ppSettings));
	return S_OK;
}