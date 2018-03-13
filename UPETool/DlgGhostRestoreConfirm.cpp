// DlgGhostRestoreConfirm.cpp : 实现文件
//

#include "stdafx.h"
#include "UPETool.h"
#include "DlgGhostRestoreConfirm.h"
#include "afxdialogex.h"
#include "CmdExecuter.h"
#include "UserMsg.h"
//安装完成后的重启倒计时
#include "RebootNoticeModule.h"
#include "ShutdownNoticeModule.h"
#include "DlgConfirmWithDefault.h "
#include "util/Logger.h"
#include "ExtraSetup.h"
// CDlgGhostRestoreConfirm 对话框

IMPLEMENT_DYNAMIC(CDlgGhostRestoreConfirm, CDialogEx)

CDlgGhostRestoreConfirm::CDlgGhostRestoreConfirm(const CString& strPartionName,const CString& strPartionIndex,const CString& strImageFilePath,CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgGhostRestoreConfirm::IDD, pParent)
	, m_strPartionName(strPartionName)
	, m_strPartionIndex(strPartionIndex)
	, m_strImageFilePath(strImageFilePath)
	, m_bReboot(TRUE)
	, m_bHalt(FALSE)
{
	m_buss = NULL;
}

CDlgGhostRestoreConfirm::~CDlgGhostRestoreConfirm()
{
	if (m_buss)
	{
		delete m_buss;
	}
}

void CDlgGhostRestoreConfirm::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_PARTION_NAME, m_strPartionName);
	DDX_Text(pDX, IDC_STATIC_PARTION_INDEX, m_strPartionIndex);
	DDX_Text(pDX, IDC_STATIC_IMAGE_PATH, m_strImageFilePath);
	DDX_Check(pDX, IDC_CHECK_REBOOT_COMPLETE, m_bReboot);
	DDX_Check(pDX, IDC_CHECK_HALT_COMPLETE, m_bHalt);
}


BEGIN_MESSAGE_MAP(CDlgGhostRestoreConfirm, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgGhostRestoreConfirm::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_REBOOT_COMPLETE, &CDlgGhostRestoreConfirm::OnBnClickedCheckRebootComplete)
	ON_BN_CLICKED(IDC_CHECK_HALT_COMPLETE, &CDlgGhostRestoreConfirm::OnBnClickedCheckHaltComplete)
	ON_MESSAGE(WM_MYMSG_GHOST_RESTORE_END,&CDlgGhostRestoreConfirm::OnRestoreDataEnd)
END_MESSAGE_MAP()


// CDlgGhostRestoreConfirm 消息处理程序


void CDlgGhostRestoreConfirm::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
	m_buss->Start(m_strPartionName,m_strPartionIndex,m_strImageFilePath,GetSafeHwnd());
	((CButton*)GetDlgItem(IDOK))->EnableWindow(FALSE);
}


BOOL CDlgGhostRestoreConfirm::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
	m_buss = new CBussGhostRestore();
	ASSERT(m_buss);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CDlgGhostRestoreConfirm::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN))
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDlgGhostRestoreConfirm::OnBnClickedCheckRebootComplete()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_bReboot && m_bHalt)
	{
		m_bHalt = FALSE;
		UpdateData(FALSE);
	}
}


void CDlgGhostRestoreConfirm::OnBnClickedCheckHaltComplete()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_bHalt && m_bReboot)
	{
		m_bReboot = FALSE;
		UpdateData(FALSE);
	}
}


LRESULT CDlgGhostRestoreConfirm::OnRestoreDataEnd( WPARAM wParam,LPARAM lParam )
{
	if (1 == wParam)
	{
		//在执行ghopwd.exe时发生了错误
		LOG_ERROR("在执行ghopwd.exe时发生了错误%d",lParam);
		MessageBox(L"发生了一个意外错误（1）程序终止系统安装过程。");
		EndDialog(IDCANCEL);
	}else if (2 == wParam){
		//在执行Ghost32.exe时发生了错误
		LOG_ERROR("在执行Ghost32.exe时发生了错误%d",lParam);
		MessageBox(L"发生了一个意外错误（2）程序终止系统安装过程。");
		EndDialog(IDCANCEL);
	}else if (3 == wParam){
		//在添加启动项时发生了错误
		LOG_ERROR("在添加启动项时发生了错误%d",lParam);
		MessageBox(L"发生了一个意外错误（3）程序终止系统安装过程。");
		EndDialog(IDCANCEL);
	}else{
		IComfirmDlgModule* pModule = NULL;
		if (m_bReboot)
		{
			pModule = new CRebootNoticeModule;
		}
		else if (m_bHalt)
		{
			pModule = new CShutdownNoticeModule;
		}
		if (pModule)
		{
			CDlgConfirmWithDefault dlg(pModule, NULL);
			if (dlg.DoModal() == IDCANCEL)
			{
				EndDialog(IDCANCEL);
			}else
				EndDialog(IDOK);
			delete pModule;
		}

		((CButton*)GetDlgItem(IDOK))->EnableWindow(FALSE);
	}
	
	return 0;
}
//////////////////////////////////////////////////////////////////////////

CBussGhostRestore::CBussGhostRestore()
{
}

CBussGhostRestore::~CBussGhostRestore()
{

}

UINT CBussGhostRestore::ThreadFunGhostRestore( LPVOID lpThreadParam )
{
	CBussGhostRestore* pDlg = (CBussGhostRestore*)lpThreadParam;
	ASSERT(pDlg->m_strImageFilePath != _T(""));
	ASSERT(pDlg->m_strPartionIndex != _T(""));
	CString strGhoFilePath = _T("\"");
	strGhoFilePath += pDlg->m_strImageFilePath;
	strGhoFilePath += _T(":1\"");
	CExtractCmdExecuter shell;
	shell.Extract(IDR_BIN_ghopwd_exe,L"BIN",L"ghopwd.exe");
	shell.Extract(IDR_BIN_Ghost32_exe,L"BIN",L"Ghost32.exe");
	INT iRet = shell.ExecCommand(L"ghopwd.exe",(LPCTSTR)(CString(_T("\"")) + pDlg->m_strImageFilePath + CString(_T("\""))));
	if (iRet != 0)
	{
		PostMessageW(pDlg->m_hWnd ,WM_MYMSG_GHOST_RESTORE_END, 1, iRet);
		return 1;
	}
	else
	{
		CString strCmdLine = L"-clone,mode=pload,src=";
		strCmdLine += strGhoFilePath;
		strCmdLine += L",dst=";
		strCmdLine += pDlg->m_strPartionIndex;
		strCmdLine += L" -sure -fx -fro";
		iRet = shell.ExecCommand(L"Ghost32.exe",strCmdLine);
		if(iRet){
			PostMessageW(pDlg->m_hWnd, WM_MYMSG_GHOST_RESTORE_END,2,iRet);
			return 2;
		}
		CExtraSetup st;
		UINT AddExtra = st.AddExtraSetup();
		if (AddExtra == 0)
		{
			PostMessageW(pDlg->m_hWnd, WM_MYMSG_GHOST_RESTORE_END,0,0);
		}else{
			PostMessageW(pDlg->m_hWnd, WM_MYMSG_GHOST_RESTORE_END,3,AddExtra);
			return 3;
		}
	}
	return 0;
}

void CBussGhostRestore::Start( const CString& strPartionName,const CString& strPartionIndex,const CString& strImageFilePath,HWND hWnd )
{
	m_hWnd = hWnd;
	m_strPartionName = strPartionName;
	m_strPartionIndex = strPartionIndex;
	m_strImageFilePath = strImageFilePath;
	AfxBeginThread(&ThreadFunGhostRestore,this);
}
