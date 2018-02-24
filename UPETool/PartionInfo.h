#pragma once
#include <vector>
#include "DriverInfo.h"

class CPartionInfo
{
public:
	CPartionInfo(void);
	~CPartionInfo(void);
public:
	static BOOL GetPartionsInfo(std::vector<CDriverInfo>& arrDrives);
};

