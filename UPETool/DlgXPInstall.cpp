// DlgXPInstall.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UPETool.h"
#include "DlgXPInstall.h"
#include "afxdialogex.h"


// CDlgXPInstall �Ի���

IMPLEMENT_DYNAMIC(CDlgXPInstall, CDialogEx)

CDlgXPInstall::CDlgXPInstall(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgXPInstall::IDD, pParent)
	, m_strDestPartionName(_T(""))
	, m_strPartionId(_T(""))
	, m_strSourceMain(_T(""))
	, m_strSourceSub(_T(""))
	, m_bIsFormat(FALSE)
	, m_bIsBoot(FALSE)
	, m_strKey1(_T(""))
	, m_strKey2(_T(""))
	, m_strKey3(_T(""))
	, m_strKey4(_T(""))
	, m_strKey5(_T(""))
	, m_bReboot(FALSE)
	, m_bHalt(FALSE)
{

}

CDlgXPInstall::~CDlgXPInstall()
{
}

void CDlgXPInstall::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_DEST_PARTION_NAME, m_strDestPartionName);
	DDX_Text(pDX, IDC_STATIC_DEST_PARTION_ID, m_strPartionId);
	DDX_Text(pDX, IDC_STATIC_SOURCE_MAIN, m_strSourceMain);
	DDX_Text(pDX, IDC_STATIC_SOURCE_SUB, m_strSourceSub);
	DDX_Check(pDX, IDC_CHECK_FORMAT, m_bIsFormat);
	DDX_Check(pDX, IDC_CHECK_BOOT, m_bIsBoot);
	DDX_Control(pDX, IDC_COMBO_FORMAT_TYPE, m_comboForamtType);
	DDX_Control(pDX, IDC_COMBO2, m_comboBoot);
	DDX_Text(pDX, IDC_EDIT_KEY1, m_strKey1);
	DDV_MaxChars(pDX, m_strKey1, 5);
	DDX_Text(pDX, IDC_EDIT_KEY2, m_strKey2);
	DDV_MaxChars(pDX, m_strKey2, 5);
	DDX_Text(pDX, IDC_EDIT_KEY3, m_strKey3);
	DDV_MaxChars(pDX, m_strKey3, 5);
	DDX_Text(pDX, IDC_EDIT_KEY4, m_strKey4);
	DDV_MaxChars(pDX, m_strKey4, 5);
	DDX_Text(pDX, IDC_EDIT_KEY5, m_strKey5);
	DDV_MaxChars(pDX, m_strKey5, 5);
	DDX_Check(pDX, IDC_CHECK_REBOOT, m_bReboot);
	DDX_Check(pDX, IDC_CHECK_SHUTDOWN, m_bHalt);
}


BEGIN_MESSAGE_MAP(CDlgXPInstall, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgXPInstall::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgXPInstall ��Ϣ�������


BOOL CDlgXPInstall::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_bIsFormat = TRUE;
	m_bIsBoot = TRUE;

	m_bReboot = TRUE;
	m_bHalt = FALSE;
	m_comboForamtType.AddString(_T("NTFS [�Զ�]"));
	m_comboForamtType.AddString(_T("FAT32 [�Զ�]"));
	m_comboForamtType.AddString(_T("�Զ��� [�ֶ�]"));
	m_comboForamtType.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CDlgXPInstall::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN))
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDlgXPInstall::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
}
