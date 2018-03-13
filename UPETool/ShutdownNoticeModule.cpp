#include "StdAfx.h"
#include "ShutdownNoticeModule.h"


CShutdownNoticeModule::CShutdownNoticeModule(void)
	:IComfirmDlgModule(_T("关机提示"),_T("系统安装完毕，是否立即关机？"),_T("关机"),10)
{
}


CShutdownNoticeModule::~CShutdownNoticeModule(void)
{
}

void CShutdownNoticeModule::DoWhileOk()
{
	WinExec("X:\\Windows\\system32\\cmd.exe /c shutdown /s /t 0",SW_HIDE);
}
