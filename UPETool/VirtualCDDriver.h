#pragma once
#include <string>
using std::wstring;
class CExtractCmdExecuter;
#include "Common.h"
#include <vector>
using std::vector;

class CVirtualCDDriver
{
public:
	CVirtualCDDriver(void);
	~CVirtualCDDriver(void);
public:
	BOOL Install(CExtractCmdExecuter& cmder);
	BOOL Mount(const wchar_t* pszIsoPath, CExtractCmdExecuter& cmder);
	BOOL EJect(CExtractCmdExecuter& cmder);
	BOOL Remove(CExtractCmdExecuter& cmder);
	wstring GetCurLoadedISO() const { return m_strCurIso; }
	ISOType GetIsoType(wstring& WimOrGhoPath);
private:
	vector<wstring> FindInternal( const wstring& root,int FolderLevel );
private:
	wstring m_strCurIso;
	wstring m_strWimOrGhoPath;
};

