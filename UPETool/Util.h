#pragma once
#include <string>
using std::wstring;
#include <vector>
using std::vector;

class CUtil
{
public:
	CUtil(void);
	~CUtil(void);
	static wstring StringToWString( const char* szPartionName );
	static std::string WStringToString( const std::wstring& wstr );

	static wstring ErrorMessageW(LPWSTR lpszFunction);
	static std::wstring fromUnsignedLongLong(unsigned long long number);
	static vector<wstring> FindIsoGhoFiles(const wstring& root);
	static BOOL isStringEndsWith(const wstring& strToFind, const wstring& strEnd);
	static BOOL isStringBeginsWith(const wstring& strToFind, const wstring& strEnd);
	//释放资源文件
	//bxx = ExtractResource(MAKEINTRESOURCE(IDR_DAT1),L"DAT",(LPCTSTR)str,NULL);
	static bool ExtractResource(const wchar_t * szResName,const wchar_t * szResType,const wchar_t * szPathOut,HMODULE hModule=NULL);
	//递归删除一个目录
	static BOOL RemoveDirectoryRecursively(LPCTSTR lpszDir);
	static BOOL DeepDeleteFile( LPCTSTR strFile );
	static wstring CurTimeStamp();
	static std::wstring UTF8ToUnicode( const std::string& utfString );
private:
	static vector<wstring> FindIsoGhoFileInternal(const wstring& root,int FolderLevel);
};

