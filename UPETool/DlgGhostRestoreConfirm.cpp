// DlgGhostRestoreConfirm.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UPETool.h"
#include "DlgGhostRestoreConfirm.h"
#include "afxdialogex.h"
#include "CmdExecuter.h"
#include "UserMsg.h"
//��װ��ɺ����������ʱ
#include "RebootNoticeModule.h"
#include "ShutdownNoticeModule.h"
#include "DlgConfirmWithDefault.h "
#include "util/Logger.h"
#include "ExtraSetup.h"
// CDlgGhostRestoreConfirm �Ի���

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


// CDlgGhostRestoreConfirm ��Ϣ�������


void CDlgGhostRestoreConfirm::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//CDialogEx::OnOK();
	m_buss->Start(m_strPartionName,m_strPartionIndex,m_strImageFilePath,GetSafeHwnd());
	((CButton*)GetDlgItem(IDOK))->EnableWindow(FALSE);
}


BOOL CDlgGhostRestoreConfirm::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_buss = new CBussGhostRestore();
	ASSERT(m_buss);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CDlgGhostRestoreConfirm::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN))
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDlgGhostRestoreConfirm::OnBnClickedCheckRebootComplete()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_bReboot && m_bHalt)
	{
		m_bHalt = FALSE;
		UpdateData(FALSE);
	}
}


void CDlgGhostRestoreConfirm::OnBnClickedCheckHaltComplete()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		//��ִ��ghopwd.exeʱ�����˴���
		LOG_ERROR("��ִ��ghopwd.exeʱ�����˴���%d",lParam);
		MessageBox(L"������һ���������1��������ֹϵͳ��װ���̡�");
		EndDialog(IDCANCEL);
	}else if (2 == wParam){
		//��ִ��Ghost32.exeʱ�����˴���
		LOG_ERROR("��ִ��Ghost32.exeʱ�����˴���%d",lParam);
		MessageBox(L"������һ���������2��������ֹϵͳ��װ���̡�");
		EndDialog(IDCANCEL);
	}else if (3 == wParam){
		//�����������ʱ�����˴���
		LOG_ERROR("�����������ʱ�����˴���%d",lParam);
		MessageBox(L"������һ���������3��������ֹϵͳ��װ���̡�");
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
