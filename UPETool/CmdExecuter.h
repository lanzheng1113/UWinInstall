#pragma once
#include <string>
using std::wstring;
#include "InterfaceCallBack.h"

//
// CCmdExecuter用于在当前目录执行一个程序，等待程序结束后返回执行结果。
// 并获取该程序的标准错误和标准输出。
// 
class CCmdExecuter
{
public:
	// 在当前目录执行一个命令/程序，返回执行结果并将命令/程序的输出到参数Result。
	INT ExecCommandWithResultText(LPCWSTR szExe, LPCWSTR szCmd, OUT wstring& Result);
protected:
private:
};

void ExecCmdCallBack(const std::string& text);
typedef void *(funtypeCmdCallback)(const std::string& text);
//
// CExtractCmdExecuter 用于在资源中释放一个可执行程序到临时目录并执行它。
// 当对象被销毁后删除临时目录。
// 
#define REncodingANSI 0
#define REncodingUTF8 1
class CExtractCmdExecuter
{
public:
	CExtractCmdExecuter(void);
	~CExtractCmdExecuter(void);
public:
	// 释放文件到临时目录
	BOOL Extract(UINT ResourceId, LPCWSTR rcType, LPCWSTR szTargetName);
	// 在临时目录执行程序，返回执行结果.错误输出重定向到指定的文件。
	INT	ExecCommand(LPCWSTR szExe, LPCWSTR szCMD, 
		BOOL IsChangeCurDir = TRUE, BOOL fHide = TRUE,BOOL bWaitUntilFinish = TRUE,ICallBackRestore* cbk=NULL);
	// 在临时目录执行一个命令/程序，返回执行结果并将命令/程序的输出到参数Result。
	INT ExecCommandWithResultText(LPCWSTR szExe, LPCWSTR szCmd, OUT wstring& Result, int ResultEncodeing=REncodingANSI);
private:
	BOOL InitEnv();
	BOOL ClearEnv();
	wstring m_pwd;
};

