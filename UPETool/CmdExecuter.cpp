#include "StdAfx.h"
#include "CmdExecuter.h"
#include "Util.h"
#include "util/File.h"
#include "util/StringEx.h"
#include "util/Logger.h"

CExtractCmdExecuter::CExtractCmdExecuter(void)
{
	if (!InitEnv())
	{
		//do something like logging error message. here.
	}
	
}


CExtractCmdExecuter::~CExtractCmdExecuter(void)
{
	if (!ClearEnv())
	{
		//do something like logging error message. here.
	}
}

BOOL CExtractCmdExecuter::InitEnv()
{
	WCHAR szTempPath[MAX_PATH] = {0};
	GetTempPathW(MAX_PATH,szTempPath);
	PathAddBackslash(szTempPath);
	char szFolderName[9] = "~";
	srand(GetTickCount());
	for (int i=1; i!=8; i++)
	{
		szFolderName[i] = 'a'+rand()%26;
	}
	szFolderName[8] = 0;
	wstring str = CUtil::StringToWString(szFolderName);
	wcscat(szTempPath,str.c_str());
	wcscat(szTempPath,L"\\");
	if (PathFileExistsW(szTempPath))
	{
		CUtil::RemoveDirectoryRecursively(szTempPath);
	}
	CreateDirectory(szTempPath,NULL);
	m_pwd = szTempPath;
	return TRUE;
}

BOOL CExtractCmdExecuter::ClearEnv()
{
	if (!m_pwd.empty())
	{
		return CUtil::RemoveDirectoryRecursively(m_pwd.c_str());
	}
	return FALSE;
}

BOOL CExtractCmdExecuter::Extract( UINT ResourceId, LPCWSTR rcType, LPCWSTR szTargetName )
{
	if (m_pwd.empty())
	{
		return false;
	}
	return CUtil::ExtractResource(MAKEINTRESOURCE(ResourceId),rcType,(m_pwd + szTargetName).c_str(),NULL);
}

INT CExtractCmdExecuter::ExecCommand( LPCWSTR szExe, LPCWSTR szCMD, 
	BOOL IsChangeCurDir/*=TRUE*/, BOOL fHide /*= TRUE*/,BOOL bWaitUntilFinish /*= TRUE*/,ICallBackRestore* cbk /*=NULL*/ )
{
	// 设置需要将标准输出重定位到哪个临时文件
	WCHAR szTempPath[MAX_PATH] = {0};
	GetTempPath(MAX_PATH,szTempPath);
	WCHAR szTempFileName[MAX_PATH] = {0};
	GetTempFileName(szTempPath,L"~l",0,szTempFileName);
	// 子进程启动信息设置
	STARTUPINFOW si;  
	si.cb = sizeof(STARTUPINFO);  
	GetStartupInfoW(&si);    
	si.wShowWindow = fHide ? SW_HIDE : SW_SHOW;
	si.dwFlags     = STARTF_USESHOWWINDOW;  
	HANDLE hOutPutFile = 0;
	if (szTempFileName[0])
	{
		si.dwFlags = si.dwFlags | STARTF_USESTDHANDLES;
		SECURITY_ATTRIBUTES psa={sizeof(psa),NULL,TRUE};;  
		psa.bInheritHandle=TRUE;
		hOutPutFile = CreateFile(szTempFileName, GENERIC_WRITE, 
			FILE_SHARE_READ|FILE_SHARE_WRITE, &psa, 
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  
		si.hStdOutput = hOutPutFile;
		si.hStdError = hOutPutFile;  
	}
	// 运行子进程并等待其结束
	PROCESS_INFORMATION pi;   
	WCHAR szCurPath[MAX_PATH] = {0};
	
	if (IsChangeCurDir)
	{
		GetCurrentDirectoryW(MAX_PATH,szCurPath);
		SetCurrentDirectoryW(m_pwd.c_str());
	}
	CString FullCmdLine;
	if (!(szExe == NULL || CString(szExe) == ""))
	{
		FullCmdLine = CString(szExe) + L" " + szCMD;
	}else{
		FullCmdLine = szCMD;
	}
// 	MessageBox(NULL,FullCmdLine,NULL,MB_OK);
// 	return 0;
	BOOL flag = CreateProcessW(NULL, (LPWSTR)(LPCWSTR)FullCmdLine, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi);  
	if (!flag)
	{
		SetCurrentDirectoryW(szCurPath);
		LOG_INFO("执行命令%s时发生了错误，错误ID为%d：",String::fromStdWString((LPCWSTR)FullCmdLine).c_str(),GetLastError());
		return GetLastError();
	}
	std::string fileContent;
	if (bWaitUntilFinish)
	{
		if (cbk && szTempFileName[0])
		{
			int x = 0;
			while (WaitForSingleObject(pi.hProcess, 1000) == WAIT_TIMEOUT)
			{
				FileReader fr(String::fromStdWString(szTempFileName));
				fr.open();
				fileContent = fr.read();
				cbk->ExecCmdCallBack(fileContent);
				fr.close();
				//LOG_DEBUG("%d\n%s",x++,str.c_str());
			}
			FileReader fr(String::fromStdWString(szTempFileName));
			fr.open();
			fileContent = fr.read();
			cbk->ExecCmdCallBack(fileContent);
			fr.close();
			//LOG_DEBUG("%d\n%s",x++,str.c_str());
		}else{
			WaitForSingleObject(pi.hProcess, INFINITE);  
		}
		
		// 获取子进程返回值,在等待期间其他线程可以打开lpszStdOutFileName读取内容。例如进度之类的东西。
		DWORD dwExitCode = 0;
		flag = GetExitCodeProcess(pi.hProcess, &dwExitCode);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		SetCurrentDirectoryW(szCurPath);
		LOG_INFO("执行命令%s的结果为：",String::fromStdWString((LPCTSTR)FullCmdLine).c_str());
		LOG_INFO("%s",fileContent.c_str());
		if (hOutPutFile)
		{
			//DeleteFile(szTempFileName); //先注释掉这行，用于保留着原始数据
			CloseHandle(hOutPutFile);
		}
		return dwExitCode;
	}
	SetCurrentDirectoryW(szCurPath);
	if (hOutPutFile)
	{
		//DeleteFile(szTempFileName); //先注释掉这行，用于保留着原始数据
		CloseHandle(hOutPutFile);
	}
	return 0;
}

INT CExtractCmdExecuter::ExecCommandWithResultText( LPCWSTR szExe, LPCWSTR szCmd, OUT wstring& Result, int ResultEncodeing/*=REncodingANSI*/ )
{
	Result = L"";
	WCHAR szTempPath[MAX_PATH] = {0};
	GetTempPath(MAX_PATH,szTempPath);
	WCHAR szTempFileName[MAX_PATH] = {0};
	GetTempFileName(szTempPath,L"~l",0,szTempFileName);
	//子进程启动信息设置
	STARTUPINFOW si;  
	si.cb = sizeof(STARTUPINFO);  
	GetStartupInfoW(&si);    
	si.wShowWindow = SW_HIDE;
	si.dwFlags     = STARTF_USESHOWWINDOW;  
	HANDLE hOutPutFile = 0;
	if (szTempFileName[0])
	{
		si.dwFlags = si.dwFlags | STARTF_USESTDHANDLES;
		SECURITY_ATTRIBUTES psa={sizeof(psa),NULL,TRUE};;  
		psa.bInheritHandle=TRUE;
		wstring wstr = szTempFileName;
		hOutPutFile = CreateFile((LPCWSTR)wstr.c_str(), GENERIC_WRITE, 
			FILE_SHARE_READ|FILE_SHARE_WRITE, &psa, 
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  
		si.hStdOutput = hOutPutFile;
		si.hStdError = hOutPutFile;  
	}
	// 运行子进程并等待其结束
	PROCESS_INFORMATION pi;   
	WCHAR szCurPath[MAX_PATH] = {0};

	GetCurrentDirectoryW(MAX_PATH,szCurPath);
	SetCurrentDirectoryW(m_pwd.c_str());
	
	CString FullCmdLine = CString(szExe) + L" " + szCmd;
	// 	MessageBox(NULL,FullCmdLine,NULL,MB_OK);
	// 	return 0;
	BOOL flag = CreateProcessW(NULL, (LPWSTR)(LPCWSTR)FullCmdLine, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi);  
	if (!flag)
	{
		SetCurrentDirectoryW(szCurPath);
		LOG_INFO("执行命令%s时发生了错误，错误ID为%d：",String::fromStdWString((LPCWSTR)FullCmdLine).c_str(),GetLastError());
		return GetLastError();
	}
	
	WaitForSingleObject(pi.hProcess, INFINITE);  

	DWORD dwExitCode = 0;
	flag = GetExitCodeProcess(pi.hProcess, &dwExitCode);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	SetCurrentDirectoryW(szCurPath);
	if (hOutPutFile)
	{
		CloseHandle(hOutPutFile);
	}
	FileReader fr(String::fromStdWString(szTempFileName));
	if (fr.open())
	{
		std::string str = fr.read();
		if (ResultEncodeing == REncodingUTF8)
		{
			Result = CUtil::UTF8ToUnicode(str);
		}
		else
			Result = String(str).toStdWString();
	}
	LOG_INFO("执行命令%s的结果为：",String::fromStdWString(wstring(szExe)+wstring(szCmd)).c_str());
	LOG_INFO("%s",String::fromStdWString(Result).c_str());
	return dwExitCode;
}

INT CCmdExecuter::ExecCommandWithResultText( LPCWSTR szExe, LPCWSTR szCmd, OUT wstring& Result )
{
	//子进程启动信息设置
	wstring wstrEXE;
	if (szExe)
	{
		wstrEXE = szExe;
	}else{
		wstrEXE = L"";
	}
	Result = L"";
	WCHAR szTempPath[MAX_PATH] = {0};
	GetTempPath(MAX_PATH,szTempPath);
	WCHAR szTempFileName[MAX_PATH] = {0};
	GetTempFileName(szTempPath,L"~l",0,szTempFileName);
	//子进程启动信息设置
	STARTUPINFOW si;  
	si.cb = sizeof(STARTUPINFO);  
	GetStartupInfoW(&si);    
	si.wShowWindow = SW_HIDE;
	si.dwFlags     = STARTF_USESHOWWINDOW;  
	HANDLE hOutPutFile = 0;
	if (szTempFileName[0])
	{
		si.dwFlags = si.dwFlags | STARTF_USESTDHANDLES;
		SECURITY_ATTRIBUTES psa={sizeof(psa),NULL,TRUE};;  
		psa.bInheritHandle=TRUE;
		wstring wstr = szTempFileName;
		hOutPutFile = CreateFile((LPCWSTR)wstr.c_str(), GENERIC_WRITE, 
			FILE_SHARE_READ|FILE_SHARE_WRITE, &psa, 
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  
		si.hStdOutput = hOutPutFile;
		si.hStdError = hOutPutFile;  
	}
	// 运行子进程并等待其结束
	PROCESS_INFORMATION pi;  
	CString FullCmdLine = CString(wstrEXE.c_str()) + L" " + szCmd;
	// 	MessageBox(NULL,FullCmdLine,NULL,MB_OK);
	// 	return 0;
	BOOL flag = CreateProcessW(NULL, (LPWSTR)(LPCWSTR)FullCmdLine, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi);  
	if (!flag)
	{
		LOG_INFO("执行命令%s时发生了错误，错误ID为%d：",String::fromStdWString((LPCWSTR)FullCmdLine).c_str(),GetLastError());
		return GetLastError();
	}

	WaitForSingleObject(pi.hProcess, INFINITE);  

	DWORD dwExitCode = 0;
	flag = GetExitCodeProcess(pi.hProcess, &dwExitCode);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	if (hOutPutFile)
	{
		CloseHandle(hOutPutFile);
	}
	FileReader fr(String::fromStdWString(szTempFileName));
	if (fr.open())
	{
		std::string str = fr.read();
		Result = String(str).toStdWString();
	}
	LOG_INFO("执行命令%s的结果为：",String::fromStdWString(wstrEXE+wstring(szCmd)).c_str());
	LOG_INFO("%s",String::fromStdWString(Result).c_str());
	return dwExitCode;
}
