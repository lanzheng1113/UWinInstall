#include "StdAfx.h"
#include "ShutdownNoticeModule.h"


CShutdownNoticeModule::CShutdownNoticeModule(void)
	:IComfirmDlgModule(_T("�ػ���ʾ"),_T("ϵͳ��װ��ϣ��Ƿ������ػ���"),_T("�ػ�"),10)
{
}


CShutdownNoticeModule::~CShutdownNoticeModule(void)
{
}

void CShutdownNoticeModule::DoWhileOk()
{
	WinExec("X:\\Windows\\system32\\cmd.exe /c shutdown /s /t 0",SW_HIDE);
}
