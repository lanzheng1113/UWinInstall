#pragma once

// 这个类加载已安装系统的注册表,在HKLM\...\run下增加一个启动项，
// 释放ExtraSetup并赋予一个随机的名字，将这个程序写入启动项
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
	// 释放文件并添加启动项，成功返回0否则返回一个错误值
	UINT AddExtraSetup();
private:
	// 启用或者禁用一个Token上的某个权限
	BOOL SetPrivilege( HANDLE hToken, LPCWSTR nameOfPrivilege,  BOOL bEnablePrivilege );
};

