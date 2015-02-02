#pragma once

class CAutostartManager
{
private:
	CAutostartManager(void);
	virtual ~CAutostartManager(void);

public:
	static HRESULT SetAutostart(BOOL bEnable);
};

