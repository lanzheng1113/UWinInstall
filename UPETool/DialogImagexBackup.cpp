// DialogImagexBackup.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UPETool.h"
#include "DialogImagexBackup.h"
#include "afxdialogex.h"
#include "CmdExecuter.h"
#include "UserMsg.h"
#include <ctime>
#include "Util.h"
#include <sstream>

using std::wstringstream;

// CDialogImagexBackup �Ի���

IMPLEMENT_DYNAMIC(CDialogImagexBackup, CDialogEx)

CDialogImagexBackup::CDialogImagexBackup(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogImagexBackup::IDD, pParent)
	, m_strVolNameSrc(_T(""))
	, m_strPartionNumber(_T(""))
	, m_strNameDest(_T(""))
	, m_strImageName(_T(""))
	, m_bAddtionBackup(FALSE)
	, m_strProgressPercent(_T(""))
	, m_strTimeLeft(_T(""))
	, m_strTimeUsed(_T(""))
	, m_bIsFileCheck(FALSE)
	, m_bRebootWhileSuccess(FALSE)
	, m_bHaltWhileSuccess(FALSE)
	, m_state(_T(""))
{

}

CDialogImagexBackup::~CDialogImagexBackup()
{
}

void CDialogImagexBackup::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_PARTION, m_strVolNameSrc);
	DDV_MaxChars(pDX, m_strVolNameSrc, 4);
	DDX_Text(pDX, IDC_STATIC_PARTION_ID, m_strPartionNumber);
	DDV_MaxChars(pDX, m_strPartionNumber, 6);
	DDX_Text(pDX, IDC_STATIC_TARGET, m_strNameDest);
	DDV_MaxChars(pDX, m_strNameDest, 256);
	DDX_Text(pDX, IDC_EDIT_IMAGENAME, m_strImageName);
	DDV_MaxChars(pDX, m_strImageName, 32);
	DDX_Control(pDX, IDC_COMBO_COMPRESS, m_ComboCompressType);
	DDX_Check(pDX, IDC_CHECK_ADD, m_bAddtionBackup);
	DDX_Text(pDX, IDC_EDIT_Percent, m_strProgressPercent);
	DDX_Text(pDX, IDC_EDIT_TIME_LEFT, m_strTimeLeft);
	DDX_Text(pDX, IDC_EDIT_TIME_COLLAPSE, m_strTimeUsed);
	DDX_Check(pDX, IDC_CHECK_FILE_EXAM, m_bIsFileCheck);
	DDX_Check(pDX, IDC_CHECK_REBOOT_WHILE_END, m_bRebootWhileSuccess);
	DDX_Check(pDX, IDC_CHECK_HALT_WHILE_END, m_bHaltWhileSuccess);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Text(pDX, IDC_STATIC_STATE, m_state);
}


BEGIN_MESSAGE_MAP(CDialogImagexBackup, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDialogImagexBackup::OnBnClickedOk)
	ON_MESSAGE(WM_MYMSG_IMAGEX_BACKUP_END,&CDialogImagexBackup::OnBackupEnd)
END_MESSAGE_MAP()


// CDialogImagexBackup ��Ϣ�������


BOOL CDialogImagexBackup::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	ASSERT(m_strNameDest != _T("") && m_strVolNameSrc != _T("") && m_strPartionNumber != _T(""));
	m_step = EImagexBackupInited;
	m_ComboCompressType.SetCurSel(1);
	m_strImageName = m_strVolNameSrc.Left(1);
	m_strImageName += _T("_WZTK");
	
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void __cdecl ThreadFunBackupImgex(void* ThreadParam)
{
	CDialogImagexBackup* pDlg = (CDialogImagexBackup*)ThreadParam;
	ASSERT(pDlg->m_strNameDest != _T(""));
	ASSERT(pDlg->m_strVolNameSrc != _T(""));
	CString strImgFilePathSrc = _T("\"");
	strImgFilePathSrc += pDlg->m_strVolNameSrc.Left(2);
	strImgFilePathSrc += _T("\"");

	CString strImgFilePathDest = _T("\"");
	strImgFilePathDest += pDlg->m_strNameDest;
	strImgFilePathDest += _T("\"");

	CExtractCmdExecuter shell;
	shell.Extract(IDR_BIN_imagex_exe,L"BIN",L"imagex.exe");
	CString strCmdLine = L"/capture ";

	pDlg->m_step = EImagexBackupRuning;
	CString strTimeStamp = CUtil::CurTimeStamp().c_str();
	ASSERT(pDlg->m_strImageName != _T("") && strTimeStamp != _T(""));
	strCmdLine = strCmdLine + strImgFilePathSrc + _T(" ") + strImgFilePathDest 
		+ _T(" \"") + pDlg->m_strImageName + _T("\" \"") + strTimeStamp 
		+ _T("\" ") + _T("/Compress fast");
	
	INT iRet = shell.ExecCommand(L"imagex.exe",(LPCWSTR)strCmdLine);
	pDlg->PostMessageW(WM_MYMSG_IMAGEX_BACKUP_END, 0, iRet);
}

void CDialogImagexBackup::OnBnClickedOk()
{
	UpdateData(TRUE);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_step == EImagexBackupInited)
	{
		if (m_strNameDest == _T("") || m_strVolNameSrc == _T("") 
			|| m_strImageName == _T("") || m_strPartionNumber == _T(""))
		{
			MessageBox(_T("��ȷ��ӳ�����ƣ����ݷ�����������ţ�ӳ���ļ��Ƿ�����"));
			return;
		}
		m_strProgressPercent = L"���ڼ��㡭";
		m_state = L"����ִ�б���" + m_strVolNameSrc;
		UpdateData(FALSE);
		CloseHandle((HANDLE)_beginthread(ThreadFunBackupImgex,0,this));
		m_btnOK.EnableWindow(FALSE);
	}
	else
		CDialogEx::OnOK();
}

LRESULT CDialogImagexBackup::OnBackupEnd( WPARAM wParam, LPARAM lParam )
{
	m_btnOK.EnableWindow(TRUE);
	m_btnOK.SetWindowTextW(L"���");
	m_strProgressPercent = _T("100%");
	m_step = EImagexBackupFinish;
	if (lParam == 0)
	{
		wstringstream wss;
		wss << L"�ɹ�����[" << (LPCTSTR)m_strVolNameSrc << L"]";
		m_state = wss.str().c_str();
		//�������ӵ���ʱ�����Ի���
		if (m_bRebootWhileSuccess)
		{
			//CDialogDaoJiShi dlg;
			//dlg.doModule();
		}
		if (m_bHaltWhileSuccess)
		{
			//...
		}
	}
	else
	{
		//���������Ϣ
		wstringstream wss;
		wss << L"�ڱ���[" << (LPCTSTR)m_strVolNameSrc << L"]ʱ��������" << lParam;
		m_state = wss.str().c_str();
	}
	UpdateData(FALSE);
	return 0;
}


BOOL CDialogImagexBackup::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN))
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}
