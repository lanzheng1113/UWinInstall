#pragma once

// ���������Ѱ�װϵͳ��ע���,��HKLM\...\run������һ�������
// �ͷ�ExtraSetup������һ����������֣����������д��������
class CExtraSetup
{
public:
	CExtraSetup(void);
	~CExtraSetup(void);
public:
	enum ErrorCode
	{
		Succeeded,
		ExtractFileFailed,
		GetAccessTokenFailed,
		LoadKeyFailed,
	};
public:
	// �ͷ��ļ������������ɹ�����0���򷵻�һ������ֵ
	UINT AddExtraSetup();
private:
	// ���û��߽���һ��Token�ϵ�ĳ��Ȩ��
	BOOL SetPrivilege( HANDLE hToken, LPCWSTR nameOfPrivilege,  BOOL bEnablePrivilege );
};

