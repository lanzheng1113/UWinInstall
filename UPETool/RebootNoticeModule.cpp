#include "StdAfx.h"
#include "RebootNoticeModule.h"


CRebootNoticeModule::CRebootNoticeModule(void)
	:IComfirmDlgModule(_T("����ȷ��"),_T("ϵͳ��װ��ϣ��Ƿ�����������"),_T("����"),10)
{

}


CRebootNoticeModule::~CRebootNoticeModule(void)
{

}

void CRebootNoticeModule::DoWhileOk()
{
	WinExec("X\\Windows\\system32\\cmd.exe /c shutdown /r /t 0",SW_HIDE);
}
