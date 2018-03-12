#pragma once
#include "comfirmdlgmodule.h"
class CShutdownNoticeModule :
	public IComfirmDlgModule
{
public:
	CShutdownNoticeModule(void);
	~CShutdownNoticeModule(void);
	virtual void DoWhileOk() override;
};

