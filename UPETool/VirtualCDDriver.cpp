#include "StdAfx.h"
#include "VirtualCDDriver.h"
#include "CmdExecuter.h"
#include "Util.h"
#include <algorithm>
#include "util/Logger.h"
#include "util/StringEx.h"

CVirtualCDDriver::CVirtualCDDriver(void)
{
}


CVirtualCDDriver::~CVirtualCDDriver(void)
{
	
}

BOOL CVirtualCDDriver::Install(CExtractCmdExecuter& cmder)
{
	m_strCurIso = L"";
	wstring wstrResult;
	cmder.ExecCommandWithResultText(L"isocmd.exe",L"-print",wstrResult);
	cmder.ExecCommandWithResultText(L"isocmd.exe",L"-i",wstrResult);
	cmder.ExecCommandWithResultText(L"isocmd.exe",L"-number 1",wstrResult);
	return 0 == cmder.ExecCommandWithResultText(L"isocmd.exe", L"-change 1 Z:" ,wstrResult);
}

BOOL CVirtualCDDriver::Mount( const wchar_t* pszIsoPath, CExtractCmdExecuter& cmder )
{
	m_strCurIso = pszIsoPath;
	wstring wstr = L"-infile -mount Z: \"";
	wstr += pszIsoPath;
	wstr += L"\"";
	wstring strRet;
	return 0 == cmder.ExecCommandWithResultText(L"isocmd.exe",wstr.c_str(),strRet);
}

BOOL CVirtualCDDriver::EJect( CExtractCmdExecuter& cmder )
{
	m_strCurIso = L"";
	wstring strRet;
	return 0 == cmder.ExecCommandWithResultText(L"isocmd.exe",L"-eject Z:",strRet);
}

BOOL CVirtualCDDriver::Remove( CExtractCmdExecuter& cmder )
{
	m_strCurIso = L"";
	wstring strRet;
	return 0 == cmder.ExecCommandWithResultText(L"isocmd.exe", L"-r",strRet);
}

vector<wstring> CVirtualCDDriver::FindInternal( const wstring& root,int FolderLevel )
{
	vector<wstring> ret;
	if (FolderLevel >= 2)
	{
		return ret;
	}
	WIN32_FIND_DATA FindFileData;
	WCHAR PathToSearchInto [MAX_PATH] = {0};
	wcscpy(PathToSearchInto,root.c_str());
	PathAddBackslash(PathToSearchInto);
	wcscat(PathToSearchInto,L"*.*");
	HANDLE hFind = FindFirstFile(PathToSearchInto,&FindFileData); // find the first file
	if(hFind == INVALID_HANDLE_VALUE)
	{
		return ret;
	}

	bool bSearch = true;
	while(bSearch) // until we finds an entry
	{
		if(FindNextFile(hFind,&FindFileData))
		{
			// Don't care about . and ..
			//if(IsDots(FindFileData.cFileName))
			if ((_tcscmp(FindFileData.cFileName, _T(".")) == 0) ||
				(_tcscmp(FindFileData.cFileName, _T("..")) == 0))
				continue;

			// We have found a directory
			if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				WCHAR RelativePathNewDirFound[MAX_PATH] = {0};
				wcscpy(RelativePathNewDirFound, root.c_str());
				PathAddBackslash(RelativePathNewDirFound);
				wcscat(RelativePathNewDirFound, FindFileData.cFileName);

				// Recursive call with the new directory found
				vector<wstring> recursiveRet;
				recursiveRet = FindInternal(RelativePathNewDirFound, FolderLevel+1);
				if (!recursiveRet.empty())
				{
					ret.insert(ret.end(),   recursiveRet.begin(),   recursiveRet.end());   
				}
			}
			// We have found a file
			else
			{
				wstring strCompare = FindFileData.cFileName;
				transform(strCompare.begin(), strCompare.end(), strCompare.begin(), towlower);
				if (CUtil::isStringEndsWith(strCompare,L".gho") || strCompare == L"install.wim")
				{
					if (root.find(L"$Recycle")==-1)
					{
						TCHAR AbsolutePathOfNewFile[MAX_PATH] = {0};
						wcscpy(AbsolutePathOfNewFile,root.c_str());
						PathAddBackslash(AbsolutePathOfNewFile);
						wcscat(AbsolutePathOfNewFile,FindFileData.cFileName);
						ret.push_back(AbsolutePathOfNewFile);
					}
				}
			}

		}//FindNextFile
		else
		{
			if(GetLastError() == ERROR_NO_MORE_FILES) // no more files there
				bSearch = false;
			else 
			{
				// some error occured, close the handle and return FALSE
				FindClose(hFind);
				return ret;
			}
		}
	}//while

	FindClose(hFind); // closing file handle
	return ret;  
}

ISOType CVirtualCDDriver::GetIsoType( wstring& WimOrGhoPath )
{
	if (PathFileExists(_T("Z:\\I386\\WINNT32.EXE")))
	{
		LOG_INFO("找到了WINNT32.EXE，判断为xp安装系统。");
		WimOrGhoPath = L"Z:\\I386\\WINNT32.EXE";
		return ISO_WINNT32;
	}
	vector<wstring> TargetsFound = FindInternal(_T("Z:\\"),0);
	vector<wstring> GhoList;
	for (int i = 0; i!=TargetsFound.size(); i++)
	{
		wstring wstrTemp = TargetsFound[i];
		transform(wstrTemp.begin(),wstrTemp.end(),wstrTemp.begin(), towlower);
		if (CUtil::isStringEndsWith(wstrTemp, L"\\install.wim"))
		{
			// 判断是否是window原版安装wim
			WimOrGhoPath = TargetsFound[i];
			return ISO_WIM;
		}
		if (CUtil::isStringEndsWith(wstrTemp, L".gho"))
		{
			GhoList.push_back(TargetsFound[i]);
		}
	}
	// 没有找到WIM也没有找到GHO，那么是一个未知的ISO。
	if (GhoList.empty())
	{
		return ISO_UNKNOW;
	}
	// 如果GHOST文件列表不为空，我们从GHO中挑选出一个最大的。
	wstring MaxSizeGho;
	__int64 i64MaxFileSize = 0;
	for (int i= 0; i!= GhoList.size(); i++)
	{
		WIN32_FILE_ATTRIBUTE_DATA fileAttr;
		fileAttr.nFileSizeHigh = 0;
		fileAttr.nFileSizeLow = 0;
		GetFileAttributesEx(GhoList[i].c_str(), GetFileExInfoStandard, &fileAttr);  
		__int64 nSize = ((__int64)fileAttr.nFileSizeHigh << 32) + fileAttr.nFileSizeLow;
		if (nSize > i64MaxFileSize)
		{
			i64MaxFileSize = nSize;
			MaxSizeGho = GhoList[i];
		}
	}
	if (!MaxSizeGho.empty())
	{
		WimOrGhoPath = MaxSizeGho;
		return ISO_GHOST;
	}
	else
	{
		return ISO_UNKNOW;
	}
}
