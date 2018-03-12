
// UPEToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UPETool.h"
#include "UPEToolDlg.h"
#include "afxdialogex.h"
#include "PartionInfo.h"
#include "Util.h"
#include "CmdExecuter.h"
#include "DialogImagexBackup.h"
#include "UserMsg.h"
#include <boost/regex.hpp>
#include "Markup.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "util/Logger.h"
#include "util/StringEx.h"
// CUPEToolDlg �Ի���
using std::string;
#include "DlgImagexRestore.h"

CUPEToolDlg::CUPEToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUPEToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bIsWIMInstall = FALSE;
	m_bIsISOWinXPInstall = FALSE;
}

void CUPEToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_Cols, m_listPartions);
	DDX_Control(pDX, IDC_COMBO_IsoGhost, m_comboIsoGho);
}

BEGIN_MESSAGE_MAP(CUPEToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &CUPEToolDlg::OnBnClickedButtonExpore)
	ON_BN_CLICKED(IDOK, &CUPEToolDlg::OnBnClickedOk)
	ON_MESSAGE(WM_MYMSG_FindISOEnd,&CUPEToolDlg::OnMesgFindISOEnd)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_Cols, &CUPEToolDlg::OnNMRClickListCols)
	ON_BN_CLICKED(IDC_RADIO_BACKUP_GHOST, &CUPEToolDlg::OnBnClickedRadioBackupGhost)
	ON_BN_CLICKED(IDC_RADIO_RESTORE, &CUPEToolDlg::OnBnClickedRadioRestore)
	ON_BN_CLICKED(IDC_RADIO_BACKUP_IMAGEX, &CUPEToolDlg::OnBnClickedRadioBackupImagex)
	ON_BN_SETFOCUS(IDC_RADIO_RESTORE, &CUPEToolDlg::OnBnSetfocusRadioRestore)
	ON_BN_SETFOCUS(IDC_RADIO_BACKUP_GHOST, &CUPEToolDlg::OnBnSetfocusRadioBackupGhost)
	ON_BN_SETFOCUS(IDC_RADIO_BACKUP_IMAGEX, &CUPEToolDlg::OnBnSetfocusRadioBackupImagex)
	ON_MESSAGE(WM_MYMSG_GHOST_BACKUP_END,&CUPEToolDlg::OnGhostBackUpEnd)
	ON_MESSAGE(WM_MYMSG_GHOST_RESTORE_END,&CUPEToolDlg::OnRestoreDataEnd)
	ON_COMMAND(ID__32771, &CUPEToolDlg::OnMenuOpenPartion)
	ON_COMMAND(ID__32772, &CUPEToolDlg::OnMenuFormatPartion)
	ON_CBN_SELCHANGE(IDC_COMBO_IsoGhost, &CUPEToolDlg::OnCbnSelchangeComboIsoghost)
	ON_BN_CLICKED(IDCANCEL, &CUPEToolDlg::OnBnClickedCancel)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CUPEToolDlg ��Ϣ�������
void __cdecl ThreadFunFindIsoGhostFile(void* ThreadParam)
{
	LOG_DEBUG("��������ISO/GHOST�ļ����߳�");
	CUPEToolDlg* hMainDlg = (CUPEToolDlg*)ThreadParam;
	for (int i=0; i!=hMainDlg->m_arr.size(); i++)
	{
		LOG_DEBUG("��Ҫ���� %s",String::fromStdWString(hMainDlg->m_arr[i].m_PartionName).c_str());
	}
	for (int i = 0; i!= hMainDlg->m_arr.size(); i++)
	{
		vector<wstring> vect = CUtil::FindIsoGhoFiles(hMainDlg->m_arr[i].m_PartionName);
		hMainDlg->m_ghoAndIsoArr.insert(hMainDlg->m_ghoAndIsoArr.end(),vect.begin(),vect.end());
	}
	hMainDlg->PostMessageW(WM_MYMSG_FindISOEnd,0,0);
}

void __cdecl ThreadFunBackupGhost(void* ThreadParam)
{
	CUPEToolDlg* pMainDlg = (CUPEToolDlg*)ThreadParam;
	ASSERT(pMainDlg->m_strGhoBackupFilePath != _T(""));
	ASSERT(pMainDlg->m_strGhostBackupSrc != _T(""));
	CString strGhoFilePath = _T("\"");
	strGhoFilePath += pMainDlg->m_strGhoBackupFilePath;
	strGhoFilePath += _T("\"");
	CExtractCmdExecuter shell;
	shell.Extract(IDR_BIN_ghopwd_exe,L"BIN",L"ghopwd.exe");
	shell.Extract(IDR_BIN_Ghost32_exe,L"BIN",L"Ghost32.exe");
	INT iRet = shell.ExecCommand(L"ghopwd.exe",(LPCWSTR)strGhoFilePath);
	if (iRet != 0)
	{
		pMainDlg->PostMessageW(WM_MYMSG_GHOST_BACKUP_END,1,iRet);
		return;
	}
	else
	{
		CString strCmdLine = L"-clone,mode=pdump,src=";
		strCmdLine += pMainDlg->m_strGhostBackupSrc;
		strCmdLine += L",dst=";
		strCmdLine += strGhoFilePath;
		strCmdLine += L" -sure -fx -fro -z3 -auto";
		iRet = shell.ExecCommand(L"Ghost32.exe",strCmdLine);
		if(iRet)
			pMainDlg->PostMessageW(WM_MYMSG_GHOST_BACKUP_END,2,iRet);
		else
			pMainDlg->PostMessageW(WM_MYMSG_GHOST_BACKUP_END);
	}
}

void __cdecl ThreadFunRestoreGho(void* ThreadParam)
{
	CUPEToolDlg* pDlg = (CUPEToolDlg*)ThreadParam;
	ASSERT(pDlg->m_strGhostRestoreSrc != _T(""));
	ASSERT(pDlg->m_strGhostRestoreDest != _T(""));
	CString strGhoFilePath = _T("\"");
	strGhoFilePath += pDlg->m_strGhostRestoreSrc;
	strGhoFilePath += _T(":1\"");
	CExtractCmdExecuter shell;
	shell.Extract(IDR_BIN_ghopwd_exe,L"BIN",L"ghopwd.exe");
	shell.Extract(IDR_BIN_Ghost32_exe,L"BIN",L"Ghost32.exe");
	INT iRet = shell.ExecCommand(L"ghopwd.exe",(LPCWSTR)strGhoFilePath);
	if (iRet != 0)
	{
		pDlg->PostMessageW(WM_MYMSG_GHOST_RESTORE_END, 1, iRet);
		return;
	}
	else
	{
		CString strCmdLine = L"-clone,mode=pload,src=";
		strCmdLine += strGhoFilePath;
		strCmdLine += L",dst=";
		strCmdLine += pDlg->m_strGhostRestoreDest;
		strCmdLine += L" -sure -fx -fro";
		iRet = shell.ExecCommand(L"Ghost32.exe",strCmdLine);
		if(iRet)
			pDlg->PostMessageW(WM_MYMSG_GHOST_RESTORE_END,2,iRet);
		else
			pDlg->PostMessageW(WM_MYMSG_GHOST_RESTORE_END);
	}
}

BOOL CUPEToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_listPartions.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_SHOWSELALWAYS);
	//m_comboIsoGho.ModifyStyle(0,CBS_AUTOHSCROLL);
	CButton* pRadioRestore = (CButton*)GetDlgItem(IDC_RADIO_RESTORE);
	pRadioRestore->SetCheck(TRUE);

	m_listPartions.InsertColumn(0,_T("����"),0,80);
	m_listPartions.InsertColumn(1,_T("���"),0,60);
	m_listPartions.InsertColumn(2,_T("����"),0,80);
	m_listPartions.InsertColumn(3,_T("���"),0,120);
	m_listPartions.InsertColumn(4,_T("���ÿռ�"),0,80);
	m_listPartions.InsertColumn(5,_T("��������"),0,80);
	CPartionInfo::GetPartionsInfo(m_arr);
	for(int i=0; i!=m_arr.size(); i++)
	{
		CDriverInfo dv = m_arr[i];
		m_listPartions.InsertItem(i,dv.m_PartionName.c_str());
		wchar_t szText[256] = {0};
		swprintf(szText,L"%d:%d",dv.m_deviceNumber,dv.m_partitionNumber);
		m_listPartions.SetItemText(i,1,szText);
		m_listPartions.SetItemText(i,2,L"δʵ��");
		m_listPartions.SetItemText(i,3,dv.m_volName.c_str());
		float gbFree = dv.m_partionFreeSize/(1024*1024*1024.0);
		swprintf(szText,L"%.1fGB",gbFree);
		m_listPartions.SetItemText(i,4,szText);
		float gbTotal = dv.m_partionTotalSize/(1024*1024*1024.0);
		swprintf(szText,L"%.1fGB",gbTotal);
		m_listPartions.SetItemText(i,5,szText);
	}
	m_comboIsoGho.InsertString(0,L"���ڲ����С���");
	m_comboIsoGho.SetCurSel(0);
	m_comboIsoGho.EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_GHOST)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_IMAGEX)->ShowWindow(SW_HIDE);
	LOG_DEBUG("��ʼѰ��iso��gho�ļ�");
	_beginthread(ThreadFunFindIsoGhostFile,0,(void*)this);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CUPEToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CUPEToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUPEToolDlg::OnBnClickedButtonExpore()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (((CButton*)GetDlgItem(IDC_RADIO_RESTORE))->GetCheck() == BST_CHECKED)
	{
		BOOL isOpen = TRUE;				//�Ƿ��(����Ϊ����)  
		CString defaultDir = L"";		//Ĭ�ϴ򿪵��ļ�·��  
		CString fileName = L"";         //Ĭ�ϴ򿪵��ļ���  
		CString filter = L"ӳ���ļ� (*.GHO; *.WIM; *.ESD; *.ISO)|*.GHO;*.WIM;*.ESD;*.ISO||";   //�ļ����ǵ�����  
		CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY|OFN_READONLY, filter, NULL);  
		//openFileDlg.GetOFN().lpstrInitialDir = L"E:\\FileTest\\test.doc";  
		INT_PTR result = openFileDlg.DoModal();  
		CString filePath = L""; 
		if(result == IDOK) {  
			filePath = openFileDlg.GetPathName();
			CString text2Compare = filePath.MakeUpper();
			//1 ����Ѿ���combox���ô������Ϊѡ��
			BOOL isFound = FALSE;
			for (int i=0; i!=m_comboIsoGho.GetCount(); i++)
			{
				CString text;
				m_comboIsoGho.GetLBText(i,text);
				text = text.MakeUpper();
				if (text == text2Compare)
				{
					isFound = TRUE;
					m_comboIsoGho.SetCurSel(i);
					break;
				}
			}
			// if the path selected is not exist,insert into front and select it.
			if (!isFound)
			{
				m_comboIsoGho.InsertString(0,filePath);
				m_comboIsoGho.SetCurSel(0);
			}
		} 
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO_BACKUP_GHOST))->GetCheck() == BST_CHECKED)
	{
		BOOL isOpen = FALSE;     //�Ƿ��(����Ϊ����)  
		CString defaultDir = L"";   //Ĭ�ϴ򿪵��ļ�·��  
		CString fileName = L"";         //Ĭ�ϴ򿪵��ļ���  
		CString filter = L"GHO�ļ�(*.GHO)|*.GHO||";   //�ļ����ǵ�����,����Ϊ.GHO
		CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY|OFN_READONLY, filter, NULL);  
		//openFileDlg.GetOFN().lpstrInitialDir = L"E:\\FileTest\\test.doc";  
		INT_PTR result = openFileDlg.DoModal();  
		CString filePath = L""; 
		if(result == IDOK) {  
			filePath = openFileDlg.GetPathName();
			SetDlgItemText(IDC_EDIT_GHOST,filePath);
		} 
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO_BACKUP_IMAGEX))->GetCheck() == BST_CHECKED)
	{
		BOOL isOpen = TRUE;     //�Ƿ��(����Ϊ����)  
		CString defaultDir = L"";   //Ĭ�ϴ򿪵��ļ�·��  
		CString fileName = L"";         //Ĭ�ϴ򿪵��ļ���  
		CString filter = L"WIM�ļ� (*WIM)|*.WIM||";   //�ļ����ǵ�����  
		CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY|OFN_READONLY, filter, NULL);  
		//openFileDlg.GetOFN().lpstrInitialDir = L"E:\\FileTest\\test.doc";  
		INT_PTR result = openFileDlg.DoModal();  
		CString filePath = L""; 
		if(result == IDOK) {  
			filePath = openFileDlg.GetPathName();
			SetDlgItemText(IDC_EDIT_IMAGEX,filePath);
		} 
	}
}


void CUPEToolDlg::DoGhostRestore(const CString& strFilePath)
{
	if (strFilePath == L"" || !PathFileExists(strFilePath))
	{
		MessageBox(L"ԴGHO�ļ�������");
		return;
	}
	else
	{
		m_strGhostRestoreSrc = strFilePath;
		m_strGhostRestoreDest = _T("");
		POSITION pos = m_listPartions.GetFirstSelectedItemPosition();
		if (pos == NULL)
		{
			MessageBox(_T("��ѡ��Ҫ�ѱ����ļ��ָ����ĸ�������"));
			return;
		}
		int nItem = m_listPartions.GetNextSelectedItem(pos);
		m_strGhostRestoreDest = m_listPartions.GetItemText(nItem,1);
		ASSERT(nItem != -1 && m_strGhostRestoreDest != _T(""));
		CloseHandle((HANDLE)_beginthread(ThreadFunRestoreGho, 0, this));
	}
}

void CUPEToolDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// ��鵱ǰѡ�е��ǻ�ԭ�������Ǳ��ݷ�����GHOST����IMAGEX
	if (((CButton*)GetDlgItem(IDC_RADIO_RESTORE))->GetCheck() == BST_CHECKED)
	{
		//��ԭ����
		//�ж���Դ��ISO/GHOST/WIM
		int isel = m_comboIsoGho.GetCurSel();
		CString strFilePath;
		m_comboIsoGho.GetLBText(isel,strFilePath);
		CString temp = strFilePath.MakeUpper();
		if (CUtil::isStringEndsWith((LPCTSTR)strFilePath,L".GHO"))//����GHOST������ֻ��Ҫ������Ļ�ԭ�Ϳ�����
		{
			DoGhostRestore(strFilePath);
		}
		else if (CUtil::isStringEndsWith((LPCTSTR)strFilePath,L".WIM")) //�ݲ�֧��
		{
			MessageBox(L"�ݲ�֧��WIM��ʽ�Ļ�ԭ");
			return;
		}
		else if (CUtil::isStringEndsWith((LPCTSTR)strFilePath,L".ISO"))
		{
			//����ISO,Ҫ�������������
			//1 ���ISO��ԭ��ϵͳ
			//2 ���ISO�Ǳ��˷�װ�õ�GHOSTϵͳ
			//ISOѡ��ʱӦ��Ҫ������Ӧ��WIM��GHOST�У����ǷŹ���
			//��Щ�߼���������Ͽ��on_sel_change�д������Բ�Ӧ�û�ѡ��ISO.�������㵽�����������������ô�������¼���֮
			OnCbnSelchangeComboIsoghost();
			LOG_INFO("δ��ʶ���ISO���������¼��غ�����һ��");
		}
		else
		{
			//����GHO,WIM,ISO�����ͣ�ȷ��һ�µ�ǰѡ�е�COMBOX���ǲ�������
			int nItem = m_comboIsoGho.GetCurSel();
			CString strCurrentSel;
			m_comboIsoGho.GetLBText(nItem,strCurrentSel);
			//LOG_INFO("��ǰѡ�е�����Ϊ��%s",String::fromStdWString((LPCTSTR)strCurrentSel).c_str());
			//ȷ���ǲ�������
			for (int i=0; i!=m_ExtraItems.size(); i++)
			{
				//LOG_INFO("Ѱ�ҵ�ǰ��ƥ�������,����=%s",String::fromStdWString((LPCTSTR)m_ExtraItems[i].m_strName).c_str());
				if (m_ExtraItems[i].m_strName == strCurrentSel)
				{
					LOG_INFO("�ҵ������� %s",String::fromStdWString((LPCTSTR)strCurrentSel).c_str());
					//��������ж��ǲ���Ŀ¼
					if (m_ExtraItems[i].m_bIsFolder)
					{
						//�����Ŀ¼�������κ�����
						MessageBox(_T("��ѡ����Ҫ��װ��ϵͳ��"));
						return;
					}
					else
					{
						m_strGhostRestoreSrc = strFilePath;
						POSITION pos = m_listPartions.GetFirstSelectedItemPosition();
						if (pos == NULL)
						{
							MessageBox(_T("��ѡ��Ҫ�ѱ����ļ��ָ����ĸ�������"));
							return;
						}
						int nItem = m_listPartions.GetNextSelectedItem(pos);
						CString strRestoreDestPartionName = m_listPartions.GetItemText(nItem,0);
						CString strRestoreDestPartionIDs = m_listPartions.GetItemText(nItem,1);
						ASSERT(nItem != -1 && strRestoreDestPartionIDs != _T(""));
						//ȷ��������������
						if (m_bIsWIMInstall)
						{
							//win7 8 10�İ�װ��
							LOG_INFO("׼��win7 8 10�İ�װ");
							CDlgImagexRestore dlg;
							LOG_INFO("id = %d",m_ExtraItems[i].m_iWimIndex);
							LOG_INFO("Ŀ��������� = %s",String::fromStdWString((LPCTSTR)strRestoreDestPartionName).c_str());
							LOG_INFO("Ŀ�������ID = %s",String::fromStdWString((LPCTSTR)strRestoreDestPartionIDs).c_str());
							LOG_INFO("��Դ���� = %s",String::fromStdWString((LPCTSTR)m_ExtraItems[i].m_strSourceMain).c_str());
							LOG_INFO("Ŀ������ = %s",String::fromStdWString((LPCTSTR)m_ExtraItems[i].m_strSourceSub).c_str());
							dlg.SetOneKeyImageStoreCfg(m_ExtraItems[i].m_iWimIndex,strRestoreDestPartionName,strRestoreDestPartionIDs,m_ExtraItems[i].m_strSourceMain,m_ExtraItems[i].m_strSourceSub);
							dlg.DoModal();
							return;
						}
						else if (m_bIsISOWinXPInstall)
						{
							//xpԭ��İ�װ��
							return;
						}
						else
							return;
					}
				}
			}
			//��Ӧ�ڴˣ��д�������
			MessageBox(_T("�߼�����û����ϵͳ�б��е�ǰ�İ�װ������ԡ�"));
		}

	}
	else if (((CButton*)GetDlgItem(IDC_RADIO_BACKUP_GHOST))->GetCheck() == BST_CHECKED)
	{
		//Ghost���ݷ���
		//����������
		CString strGhoFilePath;
		GetDlgItemText(IDC_EDIT_GHOST,strGhoFilePath);
		if (strGhoFilePath == _T(""))
		{
			MessageBox(_T("����·������Ϊ��"),NULL,MB_OK);
			return;
		}
		m_strGhoBackupFilePath = strGhoFilePath;
		m_strGhostBackupSrc = "";
		POSITION pos = m_listPartions.GetFirstSelectedItemPosition();
		if (pos == NULL)
		{
			MessageBox(_T("��ѡ����Ҫ���ݵķ�����"));
			return;
		}
		int nItem = m_listPartions.GetNextSelectedItem(pos);
		m_strGhostBackupSrc = m_listPartions.GetItemText(nItem,1);
		ASSERT(nItem != -1 && m_strGhostBackupSrc != _T(""));
		CloseHandle((HANDLE)_beginthread(ThreadFunBackupGhost,0,this));
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO_BACKUP_IMAGEX))->GetCheck() == BST_CHECKED)
	{
		//Imagex���ݷ���
		CString strImgFilePath;
		GetDlgItemText(IDC_EDIT_IMAGEX,strImgFilePath);
		if (strImgFilePath == _T(""))
		{
			MessageBox(_T("����·������Ϊ��"),NULL,MB_OK);
			return;
		}
		m_strImgFileDest = strImgFilePath;
		POSITION pos = m_listPartions.GetFirstSelectedItemPosition();
		if (pos == NULL)
		{
			MessageBox(_T("��ѡ����Ҫ���ݵķ�����"));
			return;
		}
		int nItem = m_listPartions.GetNextSelectedItem(pos);
		m_strImgFileSrce = m_listPartions.GetItemText(nItem,0);
		ASSERT(nItem != -1 && m_strImgFileSrce != _T(""));
		CDialogImagexBackup dlg;
		dlg.m_strVolNameSrc = m_strImgFileSrce;
		dlg.m_strNameDest = m_strImgFileDest;
		dlg.m_strPartionNumber = m_listPartions.GetItemText(nItem,1);
		dlg.DoModal();
	}
}

LRESULT CUPEToolDlg::OnMesgFindISOEnd( WPARAM,LPARAM )
{
	for (int i=m_comboIsoGho.GetCount(); i!=0; i--)
	{
		m_comboIsoGho.DeleteString(i-1);
	}
	
	for (int i=0; i!=m_ghoAndIsoArr.size(); i++)
	{
		m_comboIsoGho.InsertString(i,m_ghoAndIsoArr[i].c_str());
	}
	m_comboIsoGho.SetCurSel(0);
	m_comboIsoGho.EnableWindow(TRUE);

	//��������һ��OnCbnSelchangeComboIsoghost������ISO������������ж��ǲ�������Ŀ¼
	OnCbnSelchangeComboIsoghost();
	return 0;
}


void CUPEToolDlg::OnNMRClickListCols(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;  
	if (pNMListView->iItem != -1)  
	{  
		DWORD dwPos = GetMessagePos();  
		CPoint point(LOWORD(dwPos), HIWORD(dwPos));  
		CMenu menu;   
		VERIFY(menu.LoadMenu(IDR_MENU_LIST_CTRL));           //������������1�ж����MENU���ļ�����  
		CMenu* popup = menu.GetSubMenu(0);  
		ASSERT(popup != NULL);  
		popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);  

		//��������д�����Ҫ��Ϊ�˺���Ĳ���Ϊ׼����  
		//��ȡ�б���ͼ�ؼ��е�һ����ѡ�����λ��    
		//POSITION m_pstion = GetFirstSelectedItemPosition();  
		//�ú�����ȡ��posָ�����б����������Ȼ��pos����Ϊ��һ��λ�õ�POSITIONֵ  
		//m_nIndex = GetNextSelectedItem(m_pstion);  
		//
		//CString str;  
		//str.Format(L" m_nIndex = %d", m_nIndex);  
		//MessageBox(str, str, 0);  
	}  
	*pResult = 0;
}


void CUPEToolDlg::OnBnClickedRadioBackupGhost()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (((CButton*)GetDlgItem(IDC_RADIO_BACKUP_GHOST))->GetCheck() == BST_CHECKED)
	{
		GetDlgItem(IDC_COMBO_IsoGhost)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_IMAGEX)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_GHOST)->ShowWindow(SW_SHOW);
	}
}


void CUPEToolDlg::OnBnClickedRadioRestore()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (((CButton*)GetDlgItem(IDC_RADIO_RESTORE))->GetCheck() == BST_CHECKED)
	{
		GetDlgItem(IDC_EDIT_IMAGEX)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_GHOST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_IsoGhost)->ShowWindow(SW_SHOW);
	}
}


void CUPEToolDlg::OnBnClickedRadioBackupImagex()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (((CButton*)GetDlgItem(IDC_RADIO_BACKUP_IMAGEX))->GetCheck() == BST_CHECKED)
	{
		GetDlgItem(IDC_EDIT_GHOST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_IsoGhost)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_IMAGEX)->ShowWindow(SW_SHOW);
	}
}


void CUPEToolDlg::OnBnSetfocusRadioRestore()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CUPEToolDlg::OnBnSetfocusRadioBackupGhost()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CUPEToolDlg::OnBnSetfocusRadioBackupImagex()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

LRESULT CUPEToolDlg::OnGhostBackUpEnd( WPARAM wParam,LPARAM )
{
	if (wParam == 1)
	{
		MessageBox(CUtil::ErrorMessageW(L"����GHO�ļ�").c_str());
		return 1;
	}
	return 0;
}


void CUPEToolDlg::OnMenuOpenPartion()
{
	// TODO: �ڴ���������������
	POSITION pos = m_listPartions.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_listPartions.GetNextSelectedItem(pos);
		ASSERT(-1 != nItem);
		CString strVolName = m_listPartions.GetItemText(nItem,0);
		if (!CUtil::isStringEndsWith((LPCWSTR)strVolName,L"\\"))
		{
			strVolName += L"\\";
		}
		ShellExecute(NULL, _T("open"), strVolName, NULL, NULL, SW_SHOW);
	}
}


void CUPEToolDlg::OnMenuFormatPartion()
{
	// TODO: �ڴ���������������
	POSITION pos = m_listPartions.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_listPartions.GetNextSelectedItem(pos);
		ASSERT(-1 != nItem);
		CString strVolName = m_listPartions.GetItemText(nItem,0);
		if (strVolName != L"" && strVolName[0] >= L'A' && strVolName[0] <= L'Z')
		{
			WCHAR drv = strVolName[0];
			SHFormatDrive(this->GetSafeHwnd(),drv-L'A',65535,0);
		}
	}
}

LRESULT CUPEToolDlg::OnRestoreDataEnd( WPARAM wParam,LPARAM lParam )
{
	return 0;
}

void CUPEToolDlg::OnCbnSelchangeComboIsoghost()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nItem = m_comboIsoGho.GetCurSel();
	CString strCurrentSel;
	m_comboIsoGho.GetLBText(nItem,strCurrentSel);
	//LOG_INFO("��ǰѡ�е�����Ϊ��%s",String::fromStdWString((LPCTSTR)strCurrentSel).c_str());
	//ȷ���ǲ�������
	for (int i=0; i!=m_ExtraItems.size(); i++)
	{
		//LOG_INFO("Ѱ�ҵ�ǰ��ƥ�������,����=%s",String::fromStdWString((LPCTSTR)m_ExtraItems[i].m_strName).c_str());
		if (m_ExtraItems[i].m_strName == strCurrentSel)
		{
			LOG_INFO("�ҵ������� %s",String::fromStdWString((LPCTSTR)strCurrentSel).c_str());
			//��������ж��ǲ���Ŀ¼
			if (m_ExtraItems[i].m_bIsFolder)
			{
				LOG_INFO("���������Ŀ¼");
				//�����Ŀ¼����ô��ת����һ���������
				CString subItemName = m_ExtraItems[i].m_strParentOrFirstChild;
				LOG_INFO("�������Ŀ¼�ĵ�һ������ %s",String::fromStdWString((LPCTSTR)subItemName).c_str());
				for (int j=0; j!=m_comboIsoGho.GetCount(); j++)
				{
					CString strComboItem;
					m_comboIsoGho.GetLBText(j,strComboItem);
					if (strComboItem == subItemName)
					{
						LOG_INFO("�ҵ��˵�һ�����λ��Ϊ%d",j);
						m_comboIsoGho.SetCurSel(j);
						return;
					}
				}
			}
			else{
				//�������κ���
				return;
			}
		}
	}
	CString temp = strCurrentSel.MakeUpper();
	// �����ISO������Ҫ�������ص���������
	if (CUtil::isStringEndsWith((LPCTSTR)temp,L".ISO"))
	{
		LOG_INFO("��ǰѡ�еĲ��Ƕ�����");
		// ��������������Ѿ����أ������������
		// ���Լ�����ѡ���ISO,���ѡ���ISO���ǵ�ǰ����������ISO����ôˢ�¶����б�
		if (TRUE/*CString(m_virualCDD_Z.GetCurLoadedISO().c_str()) != strFilePath*/) // ���ܵ�ǰ�������ǲ������ISO������ˢ��֮��
		{

			LOG_INFO("����ISO��");
			string astrFilePath = String::fromStdWString((LPCTSTR)strCurrentSel);
			LOG_INFO(astrFilePath.c_str());
			CExtractCmdExecuter Executer;
			
			Executer.Extract(IDR_BIN_isocmd_exe,L"BIN",L"isocmd.exe");
			Executer.Extract(IDR_BIN_ISODrive_sys,L"BIN",L"ISODrive.sys");
			// �Ѷ�������ɾ����,�Ӻ���ǰɾ��ֱ���ָ��ߡ�
			m_ExtraItems.clear();
			
			int count = m_comboIsoGho.GetCount();
			// ��ȷ��һ����û�� ----------------- ��Ϲɾ�����������ݶ�ɾ�����
			BOOL bNeedClear = FALSE;
			for (int j = 0; j != count; j++)
			{
				m_comboIsoGho.GetLBText(j,temp);
				if (temp.Find(_T("----------")) != -1)
				{
					bNeedClear = TRUE;
					break;
				}
			}
			if (bNeedClear)
			{
				for (int i=count; i!=0; i--)
				{
					m_comboIsoGho.GetLBText(i-1,temp);
					//LOG_DEBUG(String::fromStdWString((LPCTSTR)temp));
					m_comboIsoGho.DeleteString(i-1);
					if (temp.Find(_T("----------")) != -1)
					{
						break;
					}
				}
			}
			
			if (PathFileExists(L"Z:\\"))
			{
				LOG_INFO("������ǰ�������������");
				if (!m_virualCDD_Z.EJect(Executer))
				{
					MessageBox(L"�����������ʧ�ܣ������ԣ�");
					return;
				}
			}else{
				LOG_INFO("��װ����������񡭡�");
				if (!m_virualCDD_Z.Install(Executer))
				{
					MessageBox(L"��װ�������ʧ�ܣ������ԣ�");
					return;
				}
			}
			
			if (!m_virualCDD_Z.Mount((LPCTSTR)strCurrentSel,Executer))
			{
				MessageBox(_T("��ISO����ʧ��"));
				return;
			}else{
				LOG_INFO("׼������ISO����ȡISO����");
				wstring wimOrGhostPath;
				ISOType isoType = m_virualCDD_Z.GetIsoType(wimOrGhostPath);
				switch(isoType){
				case ISO_UNKNOW:
					LOG_INFO("ISO����Ϊδ֪����");
					m_bIsWIMInstall = FALSE;
					break;
				case ISO_GHOST: 
					{
						m_bIsWIMInstall = FALSE;
						LOG_INFO("ISO����ΪGHOST����");
						//����һ��-----------------------
						CExtraItem extMenu;
						extMenu.m_bIsFolder = TRUE;
						extMenu.m_bIsGhost = TRUE;
						extMenu.m_strName = "------------------------------------------------------";
						extMenu.m_strParentOrFirstChild = wimOrGhostPath.c_str();
						m_ExtraItems.push_back(extMenu);

						CExtraItem ext;
						ext.m_bIsFolder = FALSE;
						ext.m_bIsGhost = TRUE;
						ext.m_strName = wimOrGhostPath.c_str();
						ext.m_strParentOrFirstChild = strCurrentSel;
						ext.m_strCmdLine = L"";  //����GHOST����Ҫ�����У���Ϊm_strName�Ѿ�ָ�����ļ���
						m_ExtraItems.push_back(ext);  //����һ��Z:/xxx/xxx.gho��������ת����һ����Ϊ��ǰ�
						m_comboIsoGho.SetCurSel(m_comboIsoGho.AddString(wimOrGhostPath.c_str()));
						break;
					}
				case ISO_WIM:
					{
						LOG_INFO("ISO����ΪWIM����");
						m_bIsWIMInstall = TRUE;
						vector<CExtraItem> vec;
						if (FindWIM(wimOrGhostPath.c_str(), vec))
						{
							m_ExtraItems.insert(m_ExtraItems.end(),vec.begin(),vec.end());
							CString firstChild;
							for (int i=0; i!=vec.size(); i++)
							{
								m_comboIsoGho.AddString(vec[i].m_strName);
								if (firstChild.IsEmpty() && !vec[i].m_bIsFolder)
								{
									firstChild = vec[i].m_strName;
								}
							}
							if (firstChild.IsEmpty())
							{
								TRACE("��������û���ҵ�WIMISO�ĵ�һ���ӽڵ�");
								return;
							}
							for (int i=0; i!=m_comboIsoGho.GetCount(); i++)
							{
								CString temp;
								m_comboIsoGho.GetLBText(i,temp);
								if (temp == firstChild)
								{
									m_comboIsoGho.SetCurSel(i);
									break;
								}
							}
						}
					}
				}
			}
		}
	}
}
#include <fstream>
using std::ifstream;
#include <sstream>
#include <deque>
using std::deque;
BOOL CUPEToolDlg::FindWIM( LPCWSTR WimFilePath ,vector<CExtraItem>& vec )
{
	CExtractCmdExecuter execter;
	execter.Extract(IDR_BIN_imagex_exe,L"BIN",L"imagex.exe");
	wstring wstr = L"/info ";
	wstr += WimFilePath;
	wstring wstrContent;
	execter.ExecCommandWithResultText(L"imagex.exe",wstr.c_str(),wstrContent,REncodingUTF8);
	string contents = String::fromStdWString(wstrContent);
	/*WCHAR szTempPath[MAX_PATH] = {0};
	GetTempPath(MAX_PATH,szTempPath);
	WCHAR szTempFileName[MAX_PATH] = {0};
	GetTempFileName(szTempPath,L"~l",0,szTempFileName);
	execter.ExecCommand(L"imagex.exe",wstr.c_str(),szTempFileName);
	//���ļ�����ȡ�ļ�����
	ifstream ifs;
	ifs.open(szTempFileName);
	std::string contents;
	if (ifs.is_open())
	{
		std::stringstream buffer;  
		buffer << ifs.rdbuf();  
		contents = buffer.str();
		// UTF-8 to ansi
		wstring str = CUtil::UTF8ToUnicode(contents);
		contents = CUtil::WStringToString(str);
		//MessageBoxA(this->GetSafeHwnd(),contents.c_str(),NULL,MB_OK);
		LOG_INFO("imagex.exeִ�в�ѯ�Ľ��Ϊ��");
		LOG_INFO("%s",String::fromStdWString(WimFilePath).c_str());
		LOG_INFO("%s",contents.c_str());
		ifs.close();
	}*/
	//�����ļ�����contents�е�XML
	int posBeg = contents.find_first_of('<');
	int posEnd = contents.find_last_of('>');
	if (posBeg != -1 && posEnd != -1)
	{
		// parent index, as Z:\source\install.wim
		CExtraItem par;
		par.m_bIsFolder = TRUE;
		par.m_bIsGhost = FALSE;
		par.m_strName = WimFilePath;
		
		std::string strXml = contents.substr(posBeg,posEnd-posBeg+1);
		//	StarXML xmlObj(strXml);
		CMarkup xmlParser;
		std::deque<CExtraItem> tempVec;
		if (xmlParser.SetDoc((LPCTSTR)CString(strXml.c_str())))
		{
			while (xmlParser.FindChildElem(_T("IMAGE")))
			{
				wstring DisplayName;
				wstring strSize;
				long long cbSize = 0;
				xmlParser.IntoElem();
				wstring index = xmlParser.GetAttrib(_T("INDEX"));
				if (xmlParser.FindChildElem(_T("HARDLINKBYTES")))
				{
					xmlParser.IntoElem();
					strSize = xmlParser.GetData();
					std::wstringstream wss;
					wss << strSize;
					wss >> cbSize;
					//LOG_INFO("�����С = %s",String::fromStdWString(wss.str()).c_str());
					//LOG_INFO("ת���ַ��� = %s",String::fromLongLong(cbSize).c_str());
					double GbSize = cbSize / (1024*1024*1024.0);
					wchar_t szTemp[64] = {0};
					swprintf(szTemp,L"%.2f",GbSize);
					strSize = szTemp;
					xmlParser.OutOfElem();
				}
				if (xmlParser.FindChildElem(_T("DISPLAYNAME")))
				{
					xmlParser.IntoElem();
					DisplayName = xmlParser.GetData();
					xmlParser.OutOfElem();
				}
				xmlParser.OutOfElem();
				// �������������������
				if (index != L"" && DisplayName != L"" && strSize != L"")
				{
					CExtraItem sub;
					sub.m_bIsFolder = FALSE;
					sub.m_bIsGhost = FALSE;
					// ��CMD�м�¼����İ�װ����
//#pragma error "comma"
					sub.m_strCmdLine = WimFilePath; sub.m_strCmdLine += _T(" -i "); sub.m_strCmdLine += index.c_str();
					sub.m_strName = L"    -- ";
					sub.m_strName += (index + L" " + DisplayName + L" " + strSize + L"GB").c_str();
					sub.m_iWimIndex = String(String::fromStdWString(index)).toInteger();
					sub.m_strSourceMain = WimFilePath;
					sub.m_strSourceSub = (index + L" " + DisplayName + L" " + strSize + L"GB").c_str();
					tempVec.push_back(sub);
				}
			}
			// ������Ϊ�ա���ô����Щ���ͷ�����һ��Ŀ¼
			// ������ӵ�������������С�
			if (!tempVec.empty())
			{
				//����һ�¼��롰�����͡� �������� 
				BOOL bLast = TRUE;
				for (std::deque<CExtraItem>::reverse_iterator x = tempVec.rbegin(); x != tempVec.rend(); ++x)
				{
					if (bLast)
					{
						x->m_strName = CString(_T("��")) + x->m_strName;
						bLast = FALSE;
					}
					else
					{
						x->m_strName = CString(_T("��")) + x->m_strName;
					}
				}

				CExtraItem itemFolder;
				itemFolder.m_bIsFolder = TRUE;
				itemFolder.m_bIsGhost = FALSE;
				itemFolder.m_strCmdLine = "";
				itemFolder.m_strName = "Z:\\sources\\install.wim";
				itemFolder.m_strParentOrFirstChild = tempVec.front().m_strName;
				LOG_INFO("�趨����Ϊ��%s",String::fromStdWString((LPCTSTR)itemFolder.m_strParentOrFirstChild).c_str());
				tempVec.push_front(itemFolder);

				itemFolder.m_strName = "------------------------------------------------------";
				tempVec.push_front(itemFolder);

				for (std::deque<CExtraItem>::const_iterator x = tempVec.begin(); x != tempVec.end(); ++x)
				{
					vec.push_back(*x);
				}
			}
		}
		//ɾ���ļ�
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	return FALSE;
}

void CUPEToolDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	/*
	std::string strTest = "[   17% ] Applying progress: 2:25 mins remaining ";
	boost::smatch mat;
	boost::regex reg( "\\[[ ]*(\\d+)% \\] Applying progress: (\\d+):(\\d+) mins remaining.*" );
	bool r = boost::regex_match( strTest, mat, reg);
	if (r)
	{
		//std::string strPercent = mat[0];
		//std::string strTimeRemainMinutes = mat[1];
		//std::string strTimeRemainSeconds = mat[2];
		//MessageBox(String(strPercent + " " + strTimeRemainMinutes + " " + strTimeRemainSeconds).toStdWString().c_str());
		std::string str;
		for (int i=0; i!=mat.size(); i++)
		{
			str += mat[i]; str += " ";
		}
		MessageBoxA(NULL,str.c_str(),NULL,MB_OK);
	}else{
		MessageBox(L"��ƥ��mins");
	}
	*/
	CDialogEx::OnCancel();
}


void CUPEToolDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	CExtractCmdExecuter Executer;

	Executer.Extract(IDR_BIN_isocmd_exe,L"BIN",L"isocmd.exe");
	Executer.Extract(IDR_BIN_ISODrive_sys,L"BIN",L"ISODrive.sys");
	m_virualCDD_Z.EJect(Executer);
	m_virualCDD_Z.Remove(Executer);

}


BOOL CUPEToolDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN))
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}
