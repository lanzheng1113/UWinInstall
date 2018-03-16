// DlgXPInstall.cpp : 实现文件
//

#include "stdafx.h"
#include "UPETool.h"
#include "DlgXPInstall.h"
#include "afxdialogex.h"

#include "DlgConfirmWithDefault.h"
#include "ComfirmDlgModule.h"
#include "RebootNoticeModule.h"
#include "ShutdownNoticeModule.h"

#include "DriverInfo.h"
#include <vector>
#include "PartionInfo.h"

#include "util/StringEx.h"
#include "UserMsg.h"

#include "util.h"
#include "CmdExecuter.h"
#include "util/File.h"
#include "util/Logger.h"

// CDlgXPInstall 对话框

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
	m_hThread = NULL;
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
	ON_MESSAGE(WM_INSTALL_XP_RESULT,&CDlgXPInstall::OnFinishInstall)
	ON_BN_CLICKED(IDCANCEL, &CDlgXPInstall::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgXPInstall 消息处理程序

BOOL CDlgXPInstall::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_bIsFormat = TRUE;
	m_bIsBoot = TRUE;

	m_bReboot = TRUE;
	m_bHalt = FALSE;
	m_comboForamtType.AddString(_T("NTFS [自动]"));
	m_comboForamtType.AddString(_T("FAT32 [自动]"));
	m_comboForamtType.AddString(_T("自定义 [手动]"));
	m_comboForamtType.SetCurSel(0);

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
		m_comboBoot.AddString(str.c_str());
	}
	m_comboBoot.SetCurSel(0);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CDlgXPInstall::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN))
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDlgXPInstall::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
	// lllll-mmmmm-nnnnn-ooooo-ppppp
	UpdateData(TRUE);
	m_strKeyTotal = m_strKey1 + _T("-") + m_strKey2 + _T("-") + m_strKey3 + _T("-") + m_strKey4 + _T("-") + m_strKey5;
	m_hThread = AfxBeginThread(ThreadFunXpSetup,this)->m_hThread;
	GetDlgItem(IDOK)->EnableWindow(FALSE);
}

UINT CDlgXPInstall::ThreadFunXpSetup( LPVOID lpThreadParam )
{
	CDlgXPInstall* pDlg = (CDlgXPInstall*) lpThreadParam;
	UINT result = pDlg->doXpSetup();
	return result;
}

UINT CDlgXPInstall::doXpSetup()
{
	if (m_bIsFormat)
	{
		int FmtSel = m_comboForamtType.GetCurSel();
		if (FmtSel == -1)
		{
			LOG_INFO("发生逻辑错误，当前选中的格式化方式未定义");
			return 1;
		}
		ETypeFormatXP eFormatType = (ETypeFormatXP)FmtSel;
		//格式化
		ASSERT(EFT_NTFS == eFormatType || EFT_USERDEFINE == eFormatType || EFT_FAT32 == eFormatType);
		if (eFormatType == EFT_USERDEFINE)
		{
			LOG_WARN("客户选中了自定义模式，不做任何格式化。");
		}
		else
		{
			//PostMessage(WM_MYMSG_IMAGEX_INSTALL_TOTAL_PROGRESS,STEP_START_FORMAT);
			ASSERT(m_strDestPartionName.GetLength() >= 2); //C:
			CString TargetPartion = m_strDestPartionName.Left(2);
			std::wstring wstrCmdLine = L"/c format.com /q /x /y ";
			wstrCmdLine += (LPCWSTR)TargetPartion;
			if (eFormatType == EFT_NTFS)
			{
				LOG_INFO("用户选择了NTFS格式化");
				wstrCmdLine += L" /fs:NTFS";
			}
			else if (eFormatType == EFT_FAT32)
			{
				LOG_INFO("用户选择了FAT32格式化");
				wstrCmdLine += L" /fs:FAT32";
			}
			// X:\\Windows\\system32\\cmd.exe /c format.com /q /x /y C: /fs:NTFS
			CCmdExecuter exec;
			std::wstring Result;
			INT cmdR = exec.ExecCommandWithResultText(L"X:\\Windows\\system32\\cmd.exe",wstrCmdLine.c_str(),Result);
			LOG_INFO("执行格式化的结果为：%d",cmdR);
			//PostMessage(WM_MYMSG_IMAGEX_INSTALL_TOTAL_PROGRESS,STEP_END_FORMAT,cmdR);
			if (cmdR != 0)
			{
				PostMessage(WM_INSTALL_XP_RESULT,XP_INSTALL_ERR_FORMAT,1);
				return cmdR;
			}
		}
	}
	// Current work path : [Z:\\I386\\]
	/*
	bootsect.exe /nt52 C:
	*/
	WCHAR szCurPath[MAX_PATH] = {0};
	GetCurrentDirectoryW(MAX_PATH,szCurPath);
	SetCurrentDirectoryW(L"Z:\\I386\\");
	//sif文件保存了KEY，将他替换为用户输入的KEY
	std::wstring sifPath = L"X:\\Temp\\winnt.sif";
	CUtil::ExtractResource(MAKEINTRESOURCE(IDR_BIN_XP_SIF),L"BIN",sifPath.c_str(),NULL);
	FileReader fr(String::fromStdWString(sifPath));
	if (!fr.open())
	{
		SetCurrentDirectoryW(szCurPath);
		LOG_ERROR("读取释放的sif文件时发生了错误%d",GetLastError());
		PostMessage(WM_INSTALL_XP_RESULT,XP_INSTALL_ERR_EXTRACT_SIF,1);
		return 1;
	}else{
		// 替换key
		std::string strContentOfSif = fr.read();
		fr.close();
		std::string toReplace = "lllll-mmmmm-nnnnn-ooooo-ppppp";
		std::string replaceText = String::fromStdWString((LPCTSTR)m_strKeyTotal);
		String TempContent(strContentOfSif);
		TempContent.replace(toReplace,replaceText);
		strContentOfSif = TempContent;
		// 保存回去
		FileWriter fw(String::fromStdWString(sifPath));
		if (!fw.open())
		{
			LOG_ERROR("写入安装序列号时发生了错误%d",GetLastError());
			PostMessage(WM_INSTALL_XP_RESULT,XP_INSTALL_ERR_EXTRACT_SIF,2);
			return 1;
		}else{
			fw.write(strContentOfSif);
			fw.close();
		}
	}

	std::wstring commandLine = L"Z:\\I386\\winnt32.exe /s:\"Z:\\i386\" /unattend:\"";
	commandLine += sifPath;
	commandLine += L"\" /makelocalsource /tempdrive:C /syspart:C";

	CExtractCmdExecuter ex;
	INT Iresult = ex.ExecCommand(NULL,commandLine.c_str());
	if (Iresult != 0)
	{
		SetCurrentDirectoryW(szCurPath);
		LOG_ERROR("安装系统时发生了错误，执行winnt32返回了错误%d",Iresult);
		PostMessage(WM_INSTALL_XP_RESULT,XP_INSTALL_ERR_INSTALL,Iresult);
		return Iresult;
	}

	CExtractCmdExecuter booter;
	booter.Extract(IDR_BIN_BOOTSECT,L"BIN",L"bootsect.exe");
	commandLine = L"bootsect.exe /nt52 C:";
	Iresult = booter.ExecCommand(NULL,commandLine.c_str());
	if (Iresult != 0)
	{
		SetCurrentDirectoryW(szCurPath);
		LOG_ERROR("安装系统时发生了错误，执行bootsect返回了错误%d",Iresult);
		PostMessage(WM_INSTALL_XP_RESULT,XP_INSTALL_ERR_BOOT_SECT,Iresult);
		return Iresult;
	}

	SetCurrentDirectoryW(szCurPath);
	PostMessage(WM_INSTALL_XP_RESULT,XP_INSTALL_SUCCESS,Iresult);
	return Iresult;
}


LRESULT CDlgXPInstall::OnFinishInstall( WPARAM wParam,LPARAM lParam )
{
	switch (wParam)
	{
	case XP_INSTALL_ERR_EXTRACT_SIF:
		if (lParam == 1)
		{
			MessageBox(_T("因无法正常读取序列号文件，安装系统失败了。"));
		}
		else if (lParam == 2)
		{
			MessageBox(_T("因无法正常写入序列号，安装系统失败了。"));
		}else{
			ASSERT(0);
		}
		break;
	case XP_INSTALL_ERR_INSTALL:
		{
			MessageBox(_T("安装失败"));
		}
		break;
	case XP_INSTALL_ERR_BOOT_SECT:
		MessageBox(_T("因添加引导发生了错误，安装失败。"));
		break;
	case XP_INSTALL_SUCCESS:
		{
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
		}
		break;
	default:
		break;
	}
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	return 1;
}


void CDlgXPInstall::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_hThread)
	{
		TerminateProcess(m_hThread,1);
		WaitForSingleObject(m_hThread,1000);
	}
	CDialogEx::OnCancel();
}
