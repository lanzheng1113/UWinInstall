// DlgImagexRestore.cpp : 实现文件
//

#include "stdafx.h"
#include "UPETool.h"
#include "DlgImagexRestore.h"
#include "afxdialogex.h"
#include "PartionInfo.h"
#include "UserMsg.h"
#include "CmdExecuter.h"
#include "util/StringEx.h"
#include "util/Logger.h"
#include "boost/regex.hpp"
// CDlgImagexRestore 对话框

#define TIMER_COUNT_USED_TIME 1 

IMPLEMENT_DYNAMIC(CDlgImagexRestore, CDialogEx)

CDlgImagexRestore::CDlgImagexRestore(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgImagexRestore::IDD, pParent)
	, m_strDestPartionName(_T(""))
	, m_strSourceMain(_T(""))
	, m_strSourceSub(_T(""))
	, m_bIsFormat(FALSE)
	, m_bIsAddBoot(FALSE)
	, m_bIsFileCheck(FALSE)
	, m_bReboot(FALSE)
	, m_bHalt(FALSE)
	, m_strCompletePercent(_T(""))
	, m_strTimeRemain(_T(""))
	, m_strTimeUsed(_T(""))
	, m_strDestPartionId(_T(""))
{
	m_iTimerCount = 0;
}

CDlgImagexRestore::~CDlgImagexRestore()
{
}

void CDlgImagexRestore::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_DEST_PARTION_NAME, m_strDestPartionName);
	DDX_Control(pDX, IDC_STATIC_DEST_PARTION_NUMBER, m_LableDestPartionId);
	DDX_Text(pDX, IDC_STATIC_SOURCE_SYSTEM, m_strSourceMain);
	DDX_Text(pDX, IDC_STATIC_SUB_SOURCE_SYSTEM, m_strSourceSub);
	DDX_Check(pDX, IDC_CHECK_IS_FORMAT, m_bIsFormat);
	DDX_Check(pDX, IDC_CHECK_ADD_BOOT, m_bIsAddBoot);
	DDX_Check(pDX, IDC_CHECK_SUM_CHECK, m_bIsFileCheck);
	DDX_Check(pDX, IDC_CHECK_BOOT_AFTER, m_bReboot);
	DDX_Check(pDX, IDC_CHECK_HALT_AFTER, m_bHalt);
	DDX_Text(pDX, IDC_EDIT_COMPLETE_PERCENT, m_strCompletePercent);
	DDX_Text(pDX, IDC_EDIT_TIME_REMAIN, m_strTimeRemain);
	DDX_Text(pDX, IDC_EDIT_TIME_USED, m_strTimeUsed);
	DDX_Control(pDX, IDC_COMBO_FORMAT_TYPE, m_ComboxFormatType);
	DDX_Control(pDX, IDC_COMBO_BOOT_PARTION, m_ComboxBootPartion);
	DDX_Text(pDX, IDC_STATIC_DEST_PARTION_NUMBER, m_strDestPartionId);
}


BEGIN_MESSAGE_MAP(CDlgImagexRestore, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgImagexRestore::OnBnClickedOk)
	ON_MESSAGE(WM_UNEXCEPT_IMAGEX_RESTORE_ERROR,&CDlgImagexRestore::OnUnexpectError)
	ON_MESSAGE(WM_PROGRESS_IMAGEX_RESTORE,&CDlgImagexRestore::OnUpdateProgress)
	ON_BN_CLICKED(IDCANCEL, &CDlgImagexRestore::OnBnClickedCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CDlgImagexRestore::SetOneKeyImageStoreCfg( int iWimIndex, const CString& strRestoreDestPartionName, const CString& strRestoreDestPartionIDs, const CString& strSourceMain, const CString& strSourceSub )
{
	m_RestoreCfg.m_iWimIndex = iWimIndex;
	m_RestoreCfg.m_strRestoreDestPartionIDs = strRestoreDestPartionIDs;
	m_RestoreCfg.m_strRestoreDestPartionName = strRestoreDestPartionName;
	m_RestoreCfg.m_strSourceMain = strSourceMain;
	m_RestoreCfg.m_strSourceSub = strSourceSub;
}


// CDlgImagexRestore 消息处理程序


BOOL CDlgImagexRestore::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ComboxFormatType.AddString(_T("NTFS [自动]"));
	m_ComboxFormatType.AddString(_T("FAT32 [自动]"));
	m_ComboxFormatType.AddString(_T("自定义 [手动]"));
	m_ComboxFormatType.SetCurSel(0);
	std::vector<CDriverInfo> arr;
	CPartionInfo::GetPartionsInfo(arr);
	BOOL bFirstValide = TRUE;
	for(int i=0; i!=arr.size(); i++)
	{
		CDriverInfo dv = arr[i];
		wstring str = dv.m_PartionName.substr(0,2).c_str();
		if (str[0] == L'X' || str[0] == L'Z')
		{
			continue;
		}
		if (bFirstValide)
		{
			str += L" [推荐]";
			bFirstValide = FALSE;
		}
		m_ComboxBootPartion.AddString(str.c_str());
	}
	m_ComboxBootPartion.SetCurSel(0);

	m_bIsFileCheck = FALSE;
	m_bIsAddBoot = TRUE;
	m_bHalt = FALSE;
	m_bReboot = TRUE;
	m_bIsFormat = TRUE;
	m_strCompletePercent = _T("0");
	m_strTimeRemain = _T("00:00:00");
	m_strTimeUsed = _T("00:00:00");
	m_strDestPartionId = m_RestoreCfg.m_strRestoreDestPartionIDs;
	m_strDestPartionName = m_RestoreCfg.m_strRestoreDestPartionName;
	m_strSourceMain = m_RestoreCfg.m_strSourceMain;
	m_strSourceSub = m_RestoreCfg.m_strSourceSub;
	m_iWimIndex = m_RestoreCfg.m_iWimIndex;
	
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgImagexRestore::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	// CDialogEx::OnOK();
	// 注：Combox的项目设置与ETypeFormat数值的顺序相同。
	if (m_eFormatType == -1)
	{
		MessageBox(_T("格式化类型错误，请重试"));
		return;
	}
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	m_iTimerCount = 0;
	SetTimer(TIMER_COUNT_USED_TIME,1000,NULL);
	m_ComboxBootPartion.GetLBText(m_ComboxBootPartion.GetCurSel(),m_strCurSelBootPartion);
	ASSERT(m_strCurSelBootPartion.GetLength()>=2);
	m_strCurSelBootPartion = m_strCurSelBootPartion.Left(2);
	AfxBeginThread(ThreadFunDoImagexRestore,this);
}


BOOL CDlgImagexRestore::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN))
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}

UINT CDlgImagexRestore::ThreadFunDoImagexRestore( LPVOID lpThreadParam )
{
	CDlgImagexRestore* pDlg = (CDlgImagexRestore*)lpThreadParam;
	UINT retCode = pDlg->DoImagexRestoreInternal();
	pDlg->GetDlgItem(IDOK)->EnableWindow(TRUE);
	pDlg->KillTimer(TIMER_COUNT_USED_TIME);
	return retCode;
}

UINT CDlgImagexRestore::DoImagexRestoreInternal()
{
	if (m_bIsFormat)
	{
		int FmtSel = m_ComboxFormatType.GetCurSel();
		if (FmtSel == -1)
		{
			LOG_INFO("发生逻辑错误，当前选中的格式化方式未定义");
			PostUnexpectedError(1);
			return 1;
		}
		m_eFormatType = (ETypeFormat)FmtSel;
		//格式化
		ASSERT(EFT_NTFS == m_eFormatType || EFT_USERDEFINE == m_eFormatType || EFT_FAT32 == m_eFormatType);
		if (m_eFormatType == EFT_USERDEFINE)
		{
			LOG_WARN("客户选中了自定义模式，不做任何格式化。");
		}
		else
		{
			ASSERT(m_RestoreCfg.m_strRestoreDestPartionName.GetLength() >= 2); //C:
			CString TargetPartion = m_RestoreCfg.m_strRestoreDestPartionName.Left(2);
			std::wstring wstrCmdLine = L"/c format.com /q /x /y ";
			wstrCmdLine += (LPCWSTR)TargetPartion;
			if (m_eFormatType == EFT_NTFS)
			{
				LOG_INFO("用户选择了NTFS格式化");
				wstrCmdLine += L" /fs:NTFS";
			}
			else if (m_eFormatType == EFT_FAT32)
			{
				LOG_INFO("用户选择了FAT32格式化");
				wstrCmdLine += L" /fs:FAT32";
			}
			// X:\\Windows\\system32\\cmd.exe /c format.com /q /x /y C: /fs:NTFS
			CCmdExecuter exec;
			std::wstring Result;
			INT cmdR = exec.ExecCommandWithResultText(L"X:\\Windows\\system32\\cmd.exe",wstrCmdLine.c_str(),Result);
			LOG_INFO("执行格式化的结果为：%d",cmdR);
		}
	}

	//调用IMAGEX.EXE执行系统安装 imagex.exe /apply "Z:\sources\install.wim" 1 "C:"
	CExtractCmdExecuter execImagex;
	execImagex.Extract(IDR_BIN_imagex_exe,L"BIN",L"imagex.exe");
	wstring wstrCmd = L"/apply \"";						// /apply "
	wstrCmd += (LPCTSTR)m_RestoreCfg.m_strSourceMain;
	wstrCmd += L"\" ";									// /apply "Z:\sources\install.wim" 
	wstrCmd += String(String::fromNumber(m_RestoreCfg.m_iWimIndex)).toStdWString();
	wstrCmd += L" \"";									// /apply "Z:\sources\install.wim" 1 "
	ASSERT(m_RestoreCfg.m_strRestoreDestPartionName.GetLength() >= 2); //C:
	CString TargetPartion = m_RestoreCfg.m_strRestoreDestPartionName.Left(2);
	wstrCmd += (LPCTSTR)TargetPartion;					// /apply "Z:\sources\install.wim" 1 "C:
	wstrCmd += L"\"";
	INT iResultImagexExecute = execImagex.ExecCommand(L"imagex.exe",wstrCmd.c_str(),TRUE,TRUE,TRUE,this);
	if (iResultImagexExecute == 0)
	{
		//执行成功
		LOG_INFO("执行imagex安装成功");
		//bcdboot.exe C:\Windows /s C: /f ALL /l zh-CN
		CExtractCmdExecuter exec;
		exec.Extract(IDR_BIN_bcdboot_exe,L"BIN",L"bcdboot.exe");
		wstring wstrCmdBcdbootCmd;
		wstrCmdBcdbootCmd = (LPCTSTR)TargetPartion;
		wstrCmdBcdbootCmd += L"\\Windows"; //"C:\Windows "
		wstrCmdBcdbootCmd += L" /s ";
		wstrCmdBcdbootCmd += m_strCurSelBootPartion;
		wstrCmdBcdbootCmd += L" /f ALL /l zh-CN";
		wstring ResultStr;
		INT iResult = exec.ExecCommandWithResultText(L"bcdboot.exe",wstrCmdBcdbootCmd.c_str(),ResultStr);
		if (iResult == 0)
		{
			LOG_INFO("执行bcdboot.exe添加引导成功");
		}else{
			LOG_INFO("执行bcdboot.exe添加引导失败");
		}
	}
	else
	{
		//执行失败
		LOG_INFO("执行imagex失败");
	}
	return (DWORD)-1;
}

void CDlgImagexRestore::PostUnexpectedError( UINT ErrorCode )
{
	PostMessage(WM_UNEXCEPT_IMAGEX_RESTORE_ERROR,0,(LPARAM)ErrorCode);
}

LRESULT CDlgImagexRestore::OnUnexpectError( WPARAM ,LPARAM lParam )
{
	// TODO : 在这里处理错误。lParam是错误号，通过查找WM_UNEXCEPT_IMAGEX_RESTORE_ERROR的引用可以知道哪里发生了错误。
	if (lParam == 0)
	{
	}
	return 0;
}
// NOTE: 这个函数在工作线程中运行，不要在这个函数里执行界面操作。
void CDlgImagexRestore::ExecCmdCallBack( const std::string& text )
{
	//text是执行命令行的输出，这里是imagex /apply安装系统的过程
	int stringBeginPos = text.find_last_of('[');
	std::string subStringToMatch = text.substr(stringBeginPos,text.length()-stringBeginPos);
	LOG_INFO("匹配字符串：%s",subStringToMatch.c_str());
	// 先匹配带剩余时间的
	// [   7% ] Applying progress: 2:25 mins remaining 
	// [  66% ] Applying progress: 58 secs remaining
	//
	bool bHasCalculateRemainTime = false;
	std::string strPercent = "0";
	std::string strTimeRemainMinutes, strTimeRemainSeconds;
	if (subStringToMatch.find("mins") != std::string::npos)
	{
		//\[.*(\d)+% \] Applying progress: (\d+):(\d+) mins remaining.*
		boost::smatch mat;
		boost::regex reg( "\\[[ ]*(\\d+)% \\] Applying progress: (\\d+):(\\d+) mins remaining.*" );
		bool r = boost::regex_match( subStringToMatch, mat, reg);
		//注意子项的个数为匹配数量+1，如：[   7% ] Applying progress: 2:25 mins remaining  7 2 25 
		if (r && mat.size() == 4)
		{
			strPercent = mat[1];
			strTimeRemainMinutes = mat[2];
			if (strTimeRemainMinutes.length() == 1)
			{
				strTimeRemainMinutes = std::string("0") + strTimeRemainMinutes;
			}
			strTimeRemainSeconds = mat[3];
			if (strTimeRemainSeconds.length() == 1)
			{
				strTimeRemainSeconds = std::string("0") + strTimeRemainSeconds;
			}
			bHasCalculateRemainTime = true;
		}else{
			LOG_DEBUG("不匹配mins");
		}
	}
	else if (subStringToMatch.find("secs") != std::string::npos)
	{
		//\[.*(\d)+% \] Applying progress: (\d+) secs remaining.*
		boost::smatch mat;
		boost::regex reg( "\\[[ ]*(\\d+)% \\] Applying progress: (\\d+) secs remaining.*" );
		bool r = boost::regex_match( subStringToMatch, mat, reg);
		if (r && mat.size() == 3)
		{
			strPercent = mat[1];
			strTimeRemainMinutes = "00";
			strTimeRemainSeconds = mat[2];
			if (strTimeRemainSeconds.length() == 1)
			{
				strTimeRemainSeconds = std::string("0") + strTimeRemainSeconds;
			}
			bHasCalculateRemainTime = true;
		}else{
			LOG_DEBUG("不匹配secs");
		}
	}
	else
	{
		//\[.*(\d)+% \] Applying progress.*
		boost::smatch mat;
		boost::regex reg( "\\[[ ]*(\\d+)% \\] Applying progress.*" );
		bool r = boost::regex_match( subStringToMatch, mat, reg);
		if (r && mat.size() == 2)
		{
			strPercent = mat[1];
			strTimeRemainMinutes = "0";
			strTimeRemainSeconds = "0";
			bHasCalculateRemainTime = false;
		}else{
			LOG_DEBUG("不匹配无remain");
		}
	}
	// 写入到变量中
	m_strCompletePercent = String(strPercent + "%").toStdWString().c_str();
	if (bHasCalculateRemainTime)
	{
		m_strTimeRemain = String(strTimeRemainMinutes + ":" + strTimeRemainSeconds).toStdWString().c_str();
	}
	else
	{
		m_strTimeRemain = L"--:--";
	}
	PostMessage(WM_PROGRESS_IMAGEX_RESTORE);
}

LRESULT CDlgImagexRestore::OnUpdateProgress( WPARAM,LPARAM )
{
	UpdateData(FALSE);
	return 0;
}

void CDlgImagexRestore::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}


void CDlgImagexRestore::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == TIMER_COUNT_USED_TIME)
	{
		m_iTimerCount ++;
		UINT hour = m_iTimerCount / 3600;
		UINT minute = m_iTimerCount % 3600 / 60;
		UINT seconds = m_iTimerCount % 60;
		m_strTimeUsed = String(String::fromNumber(hour,2) + ":" + String::fromNumber(minute,2) + ":" + String::fromNumber(seconds,2)).toStdWString().c_str();
		UpdateData(FALSE);
	}
	__super::OnTimer(nIDEvent);
}
