// DlgConfirmWithDefault.cpp : 实现文件
//

#include "stdafx.h"
#include "UPETool.h"
#include "DlgConfirmWithDefault.h"
#include "afxdialogex.h"
#include "comfirmdlgmodule.h"

#define TIMER_ID_OK_BUTTON (2)

// CDlgConfirmWithDefault 对话框

IMPLEMENT_DYNAMIC(CDlgConfirmWithDefault, CDialogEx)

CDlgConfirmWithDefault::CDlgConfirmWithDefault(IComfirmDlgModule* pModule, CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgConfirmWithDefault::IDD, pParent)
{
	m_pModule = pModule;
}

CDlgConfirmWithDefault::~CDlgConfirmWithDefault()
{
}

void CDlgConfirmWithDefault::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgConfirmWithDefault, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgConfirmWithDefault::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgConfirmWithDefault::OnBnClickedCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgConfirmWithDefault 消息处理程序


BOOL CDlgConfirmWithDefault::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	ASSERT(m_pModule);
	SetWindowText(m_pModule->DialogCaption());
	m_TimeOut = m_pModule->TimeOut();
	CString strButtonTextOrignal = m_pModule->ButtonText();
	CString strTimeOut;
	strTimeOut.Format(_T("(%d)"),m_pModule->TimeOut());
	strButtonTextOrignal += strTimeOut;
	((CButton*)GetDlgItem(IDOK))->SetWindowText(strButtonTextOrignal);
	GetDlgItem(IDC_STATIC_COMP)->SetWindowText(m_pModule->Lable());
	SetTimer(TIMER_ID_OK_BUTTON,1000,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgConfirmWithDefault::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	m_pModule->DoWhileOk();
}


void CDlgConfirmWithDefault::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	m_pModule->DoWhileCancel();
	CDialogEx::OnCancel();
}


void CDlgConfirmWithDefault::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_TimeOut--;
	if (m_TimeOut <= 0)
	{
		m_pModule->DoWhileOk();
		KillTimer(TIMER_ID_OK_BUTTON);
	}else{
		CString strButtonTextOrignal = m_pModule->ButtonText();
		CString strTimeOut;
		strTimeOut.Format(_T("(%d)"),m_TimeOut);
		strButtonTextOrignal += strTimeOut;
		((CButton*)GetDlgItem(IDOK))->SetWindowText(strButtonTextOrignal);
	}
	CDialogEx::OnTimer(nIDEvent);
}


BOOL CDlgConfirmWithDefault::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN))
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}
