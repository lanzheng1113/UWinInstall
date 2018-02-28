// DlgImagexRestore.cpp : ʵ���ļ�
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
// CDlgImagexRestore �Ի���

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


// CDlgImagexRestore ��Ϣ�������


BOOL CDlgImagexRestore::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_ComboxFormatType.AddString(_T("NTFS [�Զ�]"));
	m_ComboxFormatType.AddString(_T("FAT32 [�Զ�]"));
	m_ComboxFormatType.AddString(_T("�Զ��� [�ֶ�]"));
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
			str += L" [�Ƽ�]";
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
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CDlgImagexRestore::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// CDialogEx::OnOK();
	// ע��Combox����Ŀ������ETypeFormat��ֵ��˳����ͬ��
	if (m_eFormatType == -1)
	{
		MessageBox(_T("��ʽ�����ʹ���������"));
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
	// TODO: �ڴ����ר�ô����/����û���
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
			LOG_INFO("�����߼����󣬵�ǰѡ�еĸ�ʽ����ʽδ����");
			PostUnexpectedError(1);
			return 1;
		}
		m_eFormatType = (ETypeFormat)FmtSel;
		//��ʽ��
		ASSERT(EFT_NTFS == m_eFormatType || EFT_USERDEFINE == m_eFormatType || EFT_FAT32 == m_eFormatType);
		if (m_eFormatType == EFT_USERDEFINE)
		{
			LOG_WARN("�ͻ�ѡ�����Զ���ģʽ�������κθ�ʽ����");
		}
		else
		{
			ASSERT(m_RestoreCfg.m_strRestoreDestPartionName.GetLength() >= 2); //C:
			CString TargetPartion = m_RestoreCfg.m_strRestoreDestPartionName.Left(2);
			std::wstring wstrCmdLine = L"/c format.com /q /x /y ";
			wstrCmdLine += (LPCWSTR)TargetPartion;
			if (m_eFormatType == EFT_NTFS)
			{
				LOG_INFO("�û�ѡ����NTFS��ʽ��");
				wstrCmdLine += L" /fs:NTFS";
			}
			else if (m_eFormatType == EFT_FAT32)
			{
				LOG_INFO("�û�ѡ����FAT32��ʽ��");
				wstrCmdLine += L" /fs:FAT32";
			}
			// X:\\Windows\\system32\\cmd.exe /c format.com /q /x /y C: /fs:NTFS
			CCmdExecuter exec;
			std::wstring Result;
			INT cmdR = exec.ExecCommandWithResultText(L"X:\\Windows\\system32\\cmd.exe",wstrCmdLine.c_str(),Result);
			LOG_INFO("ִ�и�ʽ���Ľ��Ϊ��%d",cmdR);
		}
	}

	//����IMAGEX.EXEִ��ϵͳ��װ imagex.exe /apply "Z:\sources\install.wim" 1 "C:"
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
		//ִ�гɹ�
		LOG_INFO("ִ��imagex��װ�ɹ�");
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
			LOG_INFO("ִ��bcdboot.exe��������ɹ�");
		}else{
			LOG_INFO("ִ��bcdboot.exe�������ʧ��");
		}
	}
	else
	{
		//ִ��ʧ��
		LOG_INFO("ִ��imagexʧ��");
	}
	return (DWORD)-1;
}

void CDlgImagexRestore::PostUnexpectedError( UINT ErrorCode )
{
	PostMessage(WM_UNEXCEPT_IMAGEX_RESTORE_ERROR,0,(LPARAM)ErrorCode);
}

LRESULT CDlgImagexRestore::OnUnexpectError( WPARAM ,LPARAM lParam )
{
	// TODO : �����ﴦ�����lParam�Ǵ���ţ�ͨ������WM_UNEXCEPT_IMAGEX_RESTORE_ERROR�����ÿ���֪�����﷢���˴���
	if (lParam == 0)
	{
	}
	return 0;
}
// NOTE: ��������ڹ����߳������У���Ҫ�����������ִ�н��������
void CDlgImagexRestore::ExecCmdCallBack( const std::string& text )
{
	//text��ִ�������е������������imagex /apply��װϵͳ�Ĺ���
	int stringBeginPos = text.find_last_of('[');
	std::string subStringToMatch = text.substr(stringBeginPos,text.length()-stringBeginPos);
	LOG_INFO("ƥ���ַ�����%s",subStringToMatch.c_str());
	// ��ƥ���ʣ��ʱ���
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
		//ע������ĸ���Ϊƥ������+1���磺[   7% ] Applying progress: 2:25 mins remaining  7 2 25 
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
			LOG_DEBUG("��ƥ��mins");
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
			LOG_DEBUG("��ƥ��secs");
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
			LOG_DEBUG("��ƥ����remain");
		}
	}
	// д�뵽������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}


void CDlgImagexRestore::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
