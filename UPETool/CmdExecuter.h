#pragma once
#include <string>
using std::wstring;

class CCmdExecuter
{
public:
	CCmdExecuter(void);
	~CCmdExecuter(void);
public:
	BOOL Extract(UINT ResourceId, LPCWSTR rcType, LPCWSTR szTargetName);
	//Exeʹ�����Ŀ¼������ִ�н��.��������ض���ָ�����ı���
	INT	ExecCommand(LPCWSTR szExe, LPCWSTR szCMD , LPCWSTR lpszStdOutFileName = NULL, 
		BOOL IsChangeCurDir = TRUE, BOOL fHide = TRUE,BOOL bWaitUntilFinish = TRUE);
	INT ExecCommandWithResultText(LPCWSTR szExe, LPCWSTR szCmd, OUT wstring& Result);
private:
	BOOL InitEnv();
	BOOL ClearEnv();
	wstring m_pwd;
};

