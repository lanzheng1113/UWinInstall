// DlgImagexRestore.cpp : 实现文件
//

#include "stdafx.h"
#include "UPETool.h"
#include "DlgImagexRestore.h"
#include "afxdialogex.h"
#include "PartionInfo.h"

// CDlgImagexRestore 对话框

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
}
