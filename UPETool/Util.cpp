#include "StdAfx.h"
#include "Util.h"
#include <strsafe.h>
#include <sstream>
using std::stringstream;
using std::wstringstream;
#include <algorithm>
#include <stdexcept>
#include <fstream>
using std::ofstream;
using std::runtime_error;
#include <ctime>
#include "util/Logger.h"

CUtil::CUtil(void)
{
}


CUtil::~CUtil(void)
{
}

std::string CUtil::WStringToString( const std::wstring& wstr )
{
	if (wstr.empty()){
		return std::string("");
	}

	std::string result;

	int nLen=WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)wstr.c_str(), -1, NULL, 0, NULL, NULL );
	if(nLen<=0)
	{
		return std::string( "" );
	}
	char *presult=new char[nLen];
	if ( NULL == presult ) 
	{
		return std::string("");
	}
	WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)wstr.c_str(), -1, presult, nLen, NULL, NULL );
	presult[nLen-1]=0;
	result = presult;
	delete [] presult;

	return result;
}

wstring CUtil::StringToWString( const char* szPartionName )
{
	if (!szPartionName){
		return std::wstring(L"");
	}

	int nLen = strlen(szPartionName);
	if(!nLen)
		return std::wstring(L"");

	int nSize = MultiByteToWideChar( CP_ACP, 0, (LPCSTR)szPartionName, nLen, 0, 0 );
	if ( nSize<=0 ) {
		return std::wstring( L"" );
	}
	wchar_t *pDst=new wchar_t[nSize+1];
	if ( NULL == pDst )
	{
		return NULL;
	}
	MultiByteToWideChar( CP_ACP, 0, (LPCSTR)szPartionName, nLen, pDst, nSize );
	pDst[nSize]=0;
	if ( 0xFEFF == pDst[0] )
	{
		for (int i=0;i<nSize;i++)
		{
			pDst[i]=pDst[i+1];
		}
	}
	std::wstring wcstr(pDst);
	delete [] pDst;
	return wcstr;
}

std::wstring CUtil::fromUnsignedLongLong(unsigned long long number) 
{
	stringstream ss;
	// 	std::locale cloc("C");
	// 	ss.imbue(cloc);
	ss << number;
	return StringToWString(ss.str().c_str());
}


std::wstring CUtil::ErrorMessageW(LPWSTR lpszFunction)
{ 
	// Retrieve the system error message for the last-error code
	std::wstring ret;
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError(); 

	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR) &lpMsgBuf,
		0, NULL );

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
		(wcslen((LPCWSTR)lpMsgBuf) + wcslen((LPCWSTR)lpszFunction) + 40) * sizeof(WCHAR)); 

	StringCchPrintfW((LPWSTR)lpDisplayBuf, 
		LocalSize(lpDisplayBuf) / sizeof(WCHAR),
		L"%s failed with error %d: %s", 
		lpszFunction, dw, lpMsgBuf); 

	//MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 
	ret = (LPCWSTR)lpDisplayBuf;

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);

	return ret;
}

vector<wstring> CUtil::FindIsoGhoFiles( const wstring& root )
{
	return FindIsoGhoFileInternal(root,0);
}

vector<wstring> CUtil::FindIsoGhoFileInternal( const wstring& root,int FolderLevel )
{
	//LOG_DEBUG("查找层数 level (%d)",FolderLevel);
	vector<wstring> ret;
	if (FolderLevel >= 2)
	{
		return ret;
	}
	WIN32_FIND_DATA FindFileData;
	memset(&FindFileData,0,sizeof(WIN32_FIND_DATA));
	WCHAR PathToSearchInto[MAX_PATH] = {0};
	wcscpy(PathToSearchInto,root.c_str());
	PathAddBackslash(PathToSearchInto);
	wcscat(PathToSearchInto,L"*");
	//LOG_DEBUG("查找ISO/Ghost (2)");
	//LOG_DEBUG("查找目标 %s",String::fromStdWString(PathToSearchInto).c_str());
	HANDLE hFind = FindFirstFile(PathToSearchInto,&FindFileData); // find the first file
	if(hFind == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR("在执行FindFirstFile时发生了错误:%d",GetLastError());
		return ret;
	}
	//LOG_DEBUG("查找ISO/Ghost (3)");
	do // until we finds an entry
	{
		//LOG_DEBUG("查找文件");
		// Don't care about . and ..
		//if(IsDots(FindFileData.cFileName))
		if ((_tcscmp(FindFileData.cFileName, _T(".")) == 0) ||
			(_tcscmp(FindFileData.cFileName, _T("..")) == 0))
			continue;

		// We have found a directory
		if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			wstring wstrTemp = FindFileData.cFileName;
			//LOG_DEBUG("目录：%s",String::fromStdWString(wstrTemp).c_str());
			transform(wstrTemp.begin(),wstrTemp.end(),wstrTemp.begin(),towlower);
			if (wstrTemp == L"$recycle.bin")
			{
				//LOG_DEBUG("跳过回收站的文件");
				continue;
			}
			WCHAR RelativePathNewDirFound[MAX_PATH] = {0};
			wcscpy(RelativePathNewDirFound, root.c_str());
			PathAddBackslash(RelativePathNewDirFound);
			wcscat(RelativePathNewDirFound, FindFileData.cFileName);
			// Recursive call with the new directory found
			vector<wstring> recursiveRet;
			recursiveRet = FindIsoGhoFileInternal(RelativePathNewDirFound, FolderLevel+1);
			if (!recursiveRet.empty())
			{
				ret.insert(ret.end(),   recursiveRet.begin(),   recursiveRet.end());   
			}
		}
		else
		{
			// We have found a file
			wstring strCompare = FindFileData.cFileName;
			//LOG_DEBUG("查找ISO/Ghost (%s)",String::fromStdWString(strCompare).c_str());
			transform(strCompare.begin(), strCompare.end(), strCompare.begin(), towlower);
			if (CUtil::isStringEndsWith(strCompare,L".gho") || CUtil::isStringEndsWith(strCompare,L".iso"))
			{
				//LOG_DEBUG("We Get one ,OK we need this iso");
				TCHAR AbsolutePathOfNewFile[MAX_PATH] = {0};
				wcscpy(AbsolutePathOfNewFile,root.c_str());
				PathAddBackslash(AbsolutePathOfNewFile);
				wcscat(AbsolutePathOfNewFile,FindFileData.cFileName);
				ret.push_back(AbsolutePathOfNewFile);
				//LOG_DEBUG("找到了一个GHO/ISO文件%s",String::fromStdWString(AbsolutePathOfNewFile).c_str());
			}//else{
				//LOG_DEBUG("不需要");
			//}
		}
		
	}while (FindNextFile(hFind,&FindFileData));// do - while
	
	if(GetLastError() != ERROR_NO_MORE_FILES) // no more files there
	{
		// some error occured, close the handle and return FALSE
		LOG_ERROR("查找时FindNextFile发生错误 %d",GetLastError());
		FindClose(hFind);
		return ret;
	}

	FindClose(hFind); // closing file handle
	return ret;  
}

BOOL CUtil::isStringEndsWith( const wstring& strToFind, const wstring& strEnd )
{
	if (strToFind.size() < strEnd.size()) {
		return false;
	} else {
		return (strToFind.rfind(strEnd) == (strToFind.size() - strEnd.size()));
	}
}

BOOL CUtil::isStringBeginsWith( const wstring& strToFind, const wstring& strEnd )
{
	return (strToFind.find(strEnd) == 0);
}

bool CUtil::ExtractResource(const wchar_t * szResName,const wchar_t * szResType,const wchar_t * szPathOut,HMODULE hModule/*=NULL*/)	//释放资源文件
{

	bool bRet=false;
	HRSRC hResInfo=NULL;
	HGLOBAL hResLoad=NULL;
	LPVOID lpRes=NULL;

	try
	{
		hResInfo=FindResourceW(hModule,szResName,szResType);
		if(!hResInfo)
		{
			throw runtime_error("ExtractResource FindResourceW");
		}

		hResLoad=LoadResource(hModule,hResInfo);
		if(!hResLoad)
		{
			throw runtime_error("ExtractResource LoadResource");
		}

		lpRes=LockResource(hResLoad);
		if(!lpRes)
		{
			throw runtime_error("ExtractResource LockResource");
		}

		//-----------------------------------
		//创建文件，并将资源数据写入
		ofstream out_file(szPathOut,std::ios::out|std::ios::binary);
		if(!out_file)
		{
			throw runtime_error("ExtractResource 文件创建失败");
		}

		DWORD dwResSize=SizeofResource(hModule,hResInfo);
		if(out_file.write(static_cast<const char *>(lpRes),dwResSize))
		{
			bRet=true;
		}
		else
		{
			bRet=false;
		}
	}
	catch (std::exception & e)
	{
		OutputDebugStringA(e.what());
	}
	catch(...)
	{
		OutputDebugStringA("ExtractResource中捕捉到一个异常");
	}

	return bRet;
}


//删除文件
BOOL CUtil::DeepDeleteFile( const LPCTSTR strFile )
{
	int nR = _taccess( strFile, 0 );//Existence only

	if ( nR != 0 || PathIsDirectory(strFile))//不存在
	{
		return TRUE;
	}
	SetFileAttributes(strFile, FILE_ATTRIBUTE_NORMAL);//去掉只读属性

	BOOL bRet = ::DeleteFile( strFile );
	if (!bRet)
	{
		TCHAR strDstTmp[MAX_PATH] = {0};
		_tcscpy(strDstTmp,strFile);
		_tcscat(strDstTmp,_T(".tmp"));
		MoveFileEx( strFile, strDstTmp, MOVEFILE_REPLACE_EXISTING );
		if (!DeleteFile(strDstTmp))
		{
			MoveFileEx( strDstTmp, NULL, MOVEFILE_DELAY_UNTIL_REBOOT );
		}
	}
	return bRet;
}

BOOL CUtil::RemoveDirectoryRecursively(LPCTSTR lpszDir)
{
	BOOL bRet = TRUE;
	HANDLE hFindFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA struFindFileData={0};

	TCHAR strToFind[ MAX_PATH ] = _T( "" );
	_sntprintf( strToFind, MAX_PATH - 1, _T( "%s\\%s" ), lpszDir, _T( "*.*" ));
	hFindFile=FindFirstFile( strToFind, &struFindFileData );
	if ( hFindFile == INVALID_HANDLE_VALUE )
		return FALSE;
	do
	{
		if ( !_tcscmp( struFindFileData.cFileName, _T( "." ) ) )
			continue;
		if ( !_tcscmp( struFindFileData.cFileName, _T( ".." ) ) )
			continue;
		if ( struFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			TCHAR strNewSource[ MAX_PATH ] = _T( "" );
			_sntprintf( strNewSource, MAX_PATH - 1, _T( "%s\\%s" ), lpszDir, struFindFileData.cFileName );
			bRet = RemoveDirectoryRecursively( strNewSource);
		}
		else
		{
			TCHAR strFile[ MAX_PATH ] = _T( "" );
			_sntprintf( strFile, MAX_PATH - 1, _T( "%s\\%s" ), lpszDir, struFindFileData.cFileName );
			bRet = DeepDeleteFile( strFile );
		}
	}
	while ( FindNextFile( hFindFile, &struFindFileData ) );
	FindClose(hFindFile);
	hFindFile = INVALID_HANDLE_VALUE;
	SetFileAttributes(lpszDir, FILE_ATTRIBUTE_ARCHIVE);
	bRet = RemoveDirectory( lpszDir );
	return bRet;
}

using namespace std;
wstring CUtil::CurTimeStamp()
{
	std::time_t curTime = time(NULL);
	struct std::tm * timeinfo = std::localtime(&curTime);
	wstringstream wss;
	wss << timeinfo->tm_year + 1900;
	wss.width(2);
	wss.fill(L'0');
	wss << timeinfo->tm_mon + 1 << timeinfo->tm_mday << timeinfo->tm_hour << timeinfo->tm_min << timeinfo->tm_sec;
	return wss.str();
}

std::wstring CUtil::UTF8ToUnicode( const string& utfString )
{
	int nwLen;
	wchar_t* pwBuf;
	std::wstring wszUnicode;
	wchar_t wszBuffer[512+1];
	if (utfString.empty())
	{
		return wszUnicode;
	}
	nwLen = MultiByteToWideChar(CP_UTF8, 0, utfString.c_str(), -1, NULL, 0); 
	if (nwLen <= 0){
		return wszUnicode;
	}
	if (nwLen <= 512)
	{
		pwBuf = wszBuffer;
	}
	else
	{
		pwBuf = new wchar_t[nwLen + 1];
	}
	if (pwBuf)
	{
		memset(pwBuf, 0, (nwLen  + 1)* 2); 
		MultiByteToWideChar(CP_UTF8, 0, utfString.c_str(), -1, pwBuf, nwLen); 
		wszUnicode = pwBuf;
	}
	if (pwBuf != wszBuffer)
	{
		delete[] pwBuf;
	}
	pwBuf = NULL; 
	return wszUnicode;
}