#pragma once
#include <string>
using std::wstring;

//
// CCmdExecuter�����ڵ�ǰĿ¼ִ��һ�����򣬵ȴ���������󷵻�ִ�н����
// ����ȡ�ó���ı�׼����ͱ�׼�����
// 
class CCmdExecuter
{
public:
	// �ڵ�ǰĿ¼ִ��һ������/���򣬷���ִ�н����������/��������������Result��
	INT ExecCommandWithResultText(LPCWSTR szExe, LPCWSTR szCmd, OUT wstring& Result);
protected:
private:
};

//
// CExtractCmdExecuter ��������Դ���ͷ�һ����ִ�г�����ʱĿ¼��ִ������
// ���������ٺ�ɾ����ʱĿ¼��
// 
class CExtractCmdExecuter
{
public:
	CExtractCmdExecuter(void);
	~CExtractCmdExecuter(void);
public:
	// �ͷ��ļ�����ʱĿ¼
	BOOL Extract(UINT ResourceId, LPCWSTR rcType, LPCWSTR szTargetName);
	// ����ʱĿ¼ִ�г��򣬷���ִ�н��.��������ض���ָ�����ļ���
	INT	ExecCommand(LPCWSTR szExe, LPCWSTR szCMD , LPCWSTR lpszStdOutFileName = NULL, 
		BOOL IsChangeCurDir = TRUE, BOOL fHide = TRUE,BOOL bWaitUntilFinish = TRUE);
	// ����ʱĿ¼ִ��һ������/���򣬷���ִ�н����������/��������������Result��
	INT ExecCommandWithResultText(LPCWSTR szExe, LPCWSTR szCmd, OUT wstring& Result);
private:
	BOOL InitEnv();
	BOOL ClearEnv();
	wstring m_pwd;
};

