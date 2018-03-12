#include "StdAfx.h"
#include "RebootNoticeModule.h"


CRebootNoticeModule::CRebootNoticeModule(void)
	:IComfirmDlgModule(_T("重启确认"),_T("系统安装完毕，是否立即重启？"),_T("重启"),10)
{

}


CRebootNoticeModule::~CRebootNoticeModule(void)
{

}

void CRebootNoticeModule::DoWhileOk()
{
	WinExec("X\\Windows\\system32\\cmd.exe /c shutdown /r /t 0",SW_HIDE);
}
