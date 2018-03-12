#pragma once

#include "ComfirmDlgModule.h"
class CRebootNoticeModule : public IComfirmDlgModule
{
public:
	CRebootNoticeModule(void);
	~CRebootNoticeModule(void);
public:
	virtual void DoWhileOk() override;
};

