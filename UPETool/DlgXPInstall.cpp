// DlgXPInstall.cpp : ʵ���ļ�
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
		m_comboBoot.AddString(str.c_str());
	}
	m_comboBoot.SetCurSel(0);
	UpdateData(FALSE);
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
			LOG_INFO("�����߼����󣬵�ǰѡ�еĸ�ʽ����ʽδ����");
			return 1;
		}
		ETypeFormatXP eFormatType = (ETypeFormatXP)FmtSel;
		//��ʽ��
		ASSERT(EFT_NTFS == eFormatType || EFT_USERDEFINE == eFormatType || EFT_FAT32 == eFormatType);
		if (eFormatType == EFT_USERDEFINE)
		{
			LOG_WARN("�ͻ�ѡ�����Զ���ģʽ�������κθ�ʽ����");
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
				LOG_INFO("�û�ѡ����NTFS��ʽ��");
				wstrCmdLine += L" /fs:NTFS";
			}
			else if (eFormatType == EFT_FAT32)
			{
				LOG_INFO("�û�ѡ����FAT32��ʽ��");
				wstrCmdLine += L" /fs:FAT32";
			}
			// X:\\Windows\\system32\\cmd.exe /c format.com /q /x /y C: /fs:NTFS
			CCmdExecuter exec;
			std::wstring Result;
			INT cmdR = exec.ExecCommandWithResultText(L"X:\\Windows\\system32\\cmd.exe",wstrCmdLine.c_str(),Result);
			LOG_INFO("ִ�и�ʽ���Ľ��Ϊ��%d",cmdR);
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
	//sif�ļ�������KEY�������滻Ϊ�û������KEY
	std::wstring sifPath = L"X:\\Temp\\winnt.sif";
	CUtil::ExtractResource(MAKEINTRESOURCE(IDR_BIN_XP_SIF),L"BIN",sifPath.c_str(),NULL);
	FileReader fr(String::fromStdWString(sifPath));
	if (!fr.open())
	{
		SetCurrentDirectoryW(szCurPath);
		LOG_ERROR("��ȡ�ͷŵ�sif�ļ�ʱ�����˴���%d",GetLastError());
		PostMessage(WM_INSTALL_XP_RESULT,XP_INSTALL_ERR_EXTRACT_SIF,1);
		return 1;
	}else{
		// �滻key
		std::string strContentOfSif = fr.read();
		fr.close();
		std::string toReplace = "lllll-mmmmm-nnnnn-ooooo-ppppp";
		std::string replaceText = String::fromStdWString((LPCTSTR)m_strKeyTotal);
		String TempContent(strContentOfSif);
		TempContent.replace(toReplace,replaceText);
		strContentOfSif = TempContent;
		// �����ȥ
		FileWriter fw(String::fromStdWString(sifPath));
		if (!fw.open())
		{
			LOG_ERROR("д�밲װ���к�ʱ�����˴���%d",GetLastError());
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
		LOG_ERROR("��װϵͳʱ�����˴���ִ��winnt32�����˴���%d",Iresult);
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
		LOG_ERROR("��װϵͳʱ�����˴���ִ��bootsect�����˴���%d",Iresult);
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
			MessageBox(_T("���޷�������ȡ���к��ļ�����װϵͳʧ���ˡ�"));
		}
		else if (lParam == 2)
		{
			MessageBox(_T("���޷�����д�����кţ���װϵͳʧ���ˡ�"));
		}else{
			ASSERT(0);
		}
		break;
	case XP_INSTALL_ERR_INSTALL:
		{
			MessageBox(_T("��װʧ��"));
		}
		break;
	case XP_INSTALL_ERR_BOOT_SECT:
		MessageBox(_T("��������������˴��󣬰�װʧ�ܡ�"));
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_hThread)
	{
		TerminateProcess(m_hThread,1);
		WaitForSingleObject(m_hThread,1000);
	}
	CDialogEx::OnCancel();
}
