
// UPEToolDlg.cpp : 实现文件
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
// CUPEToolDlg 对话框
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


// CUPEToolDlg 消息处理程序
void __cdecl ThreadFunFindIsoGhostFile(void* ThreadParam)
{
	LOG_DEBUG("启动查找ISO/GHOST文件的线程");
	CUPEToolDlg* hMainDlg = (CUPEToolDlg*)ThreadParam;
	for (int i=0; i!=hMainDlg->m_arr.size(); i++)
	{
		LOG_DEBUG("需要查找 %s",String::fromStdWString(hMainDlg->m_arr[i].m_PartionName).c_str());
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_listPartions.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_SHOWSELALWAYS);
	//m_comboIsoGho.ModifyStyle(0,CBS_AUTOHSCROLL);
	CButton* pRadioRestore = (CButton*)GetDlgItem(IDC_RADIO_RESTORE);
	pRadioRestore->SetCheck(TRUE);

	m_listPartions.InsertColumn(0,_T("分区"),0,80);
	m_listPartions.InsertColumn(1,_T("编号"),0,60);
	m_listPartions.InsertColumn(2,_T("类型"),0,80);
	m_listPartions.InsertColumn(3,_T("卷标"),0,120);
	m_listPartions.InsertColumn(4,_T("可用空间"),0,80);
	m_listPartions.InsertColumn(5,_T("分区容量"),0,80);
	CPartionInfo::GetPartionsInfo(m_arr);
	for(int i=0; i!=m_arr.size(); i++)
	{
		CDriverInfo dv = m_arr[i];
		m_listPartions.InsertItem(i,dv.m_PartionName.c_str());
		wchar_t szText[256] = {0};
		swprintf(szText,L"%d:%d",dv.m_deviceNumber,dv.m_partitionNumber);
		m_listPartions.SetItemText(i,1,szText);
		m_listPartions.SetItemText(i,2,L"未实现");
		m_listPartions.SetItemText(i,3,dv.m_volName.c_str());
		float gbFree = dv.m_partionFreeSize/(1024*1024*1024.0);
		swprintf(szText,L"%.1fGB",gbFree);
		m_listPartions.SetItemText(i,4,szText);
		float gbTotal = dv.m_partionTotalSize/(1024*1024*1024.0);
		swprintf(szText,L"%.1fGB",gbTotal);
		m_listPartions.SetItemText(i,5,szText);
	}
	m_comboIsoGho.InsertString(0,L"正在查找中……");
	m_comboIsoGho.SetCurSel(0);
	m_comboIsoGho.EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_GHOST)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_IMAGEX)->ShowWindow(SW_HIDE);
	LOG_DEBUG("开始寻找iso和gho文件");
	_beginthread(ThreadFunFindIsoGhostFile,0,(void*)this);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUPEToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CUPEToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUPEToolDlg::OnBnClickedButtonExpore()
{
	// TODO: 在此添加控件通知处理程序代码
	if (((CButton*)GetDlgItem(IDC_RADIO_RESTORE))->GetCheck() == BST_CHECKED)
	{
		BOOL isOpen = TRUE;				//是否打开(否则为保存)  
		CString defaultDir = L"";		//默认打开的文件路径  
		CString fileName = L"";         //默认打开的文件名  
		CString filter = L"映像文件 (*.GHO; *.WIM; *.ESD; *.ISO)|*.GHO;*.WIM;*.ESD;*.ISO||";   //文件过虑的类型  
		CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY|OFN_READONLY, filter, NULL);  
		//openFileDlg.GetOFN().lpstrInitialDir = L"E:\\FileTest\\test.doc";  
		INT_PTR result = openFileDlg.DoModal();  
		CString filePath = L""; 
		if(result == IDOK) {  
			filePath = openFileDlg.GetPathName();
			CString text2Compare = filePath.MakeUpper();
			//1 如果已经在combox里，那么将它设为选中
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
		BOOL isOpen = FALSE;     //是否打开(否则为保存)  
		CString defaultDir = L"";   //默认打开的文件路径  
		CString fileName = L"";         //默认打开的文件名  
		CString filter = L"GHO文件(*.GHO)|*.GHO||";   //文件过虑的类型,保存为.GHO
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
		BOOL isOpen = TRUE;     //是否打开(否则为保存)  
		CString defaultDir = L"";   //默认打开的文件路径  
		CString fileName = L"";         //默认打开的文件名  
		CString filter = L"WIM文件 (*WIM)|*.WIM||";   //文件过虑的类型  
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
		MessageBox(L"源GHO文件不存在");
		return;
	}
	else
	{
		m_strGhostRestoreSrc = strFilePath;
		m_strGhostRestoreDest = _T("");
		POSITION pos = m_listPartions.GetFirstSelectedItemPosition();
		if (pos == NULL)
		{
			MessageBox(_T("请选择将要把备份文件恢复到哪个分区。"));
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
	// TODO: 在此添加控件通知处理程序代码
	// 检查当前选中的是还原分区还是备份分区，GHOST还是IMAGEX
	if (((CButton*)GetDlgItem(IDC_RADIO_RESTORE))->GetCheck() == BST_CHECKED)
	{
		//还原分区
		//判断来源是ISO/GHOST/WIM
		int isel = m_comboIsoGho.GetCurSel();
		CString strFilePath;
		m_comboIsoGho.GetLBText(isel,strFilePath);
		CString temp = strFilePath.MakeUpper();
		if (CUtil::isStringEndsWith((LPCTSTR)strFilePath,L".GHO"))//对于GHOST，我们只需要做纯粹的还原就可以了
		{
			DoGhostRestore(strFilePath);
		}
		else if (CUtil::isStringEndsWith((LPCTSTR)strFilePath,L".WIM")) //暂不支持
		{
			MessageBox(L"暂不支持WIM格式的还原");
			return;
		}
		else if (CUtil::isStringEndsWith((LPCTSTR)strFilePath,L".ISO"))
		{
			//对于ISO,要分两种情况处理
			//1 这个ISO是原版系统
			//2 这个ISO是别人封装好的GHOST系统
			//ISO选中时应该要跳到对应的WIM和GHOST中，我们放过它
			//这些逻辑都有在组合框的on_sel_change中处理，所以不应该会选中ISO.如果真的陪到了这种意外情况，那么尝试重新加载之
			OnCbnSelchangeComboIsoghost();
			LOG_INFO("未能识别的ISO，将他重新加载后再试一次");
		}
		else
		{
			//不是GHO,WIM,ISO的类型，确认一下当前选中的COMBOX项是不是子项
			int nItem = m_comboIsoGho.GetCurSel();
			CString strCurrentSel;
			m_comboIsoGho.GetLBText(nItem,strCurrentSel);
			//LOG_INFO("当前选中的子项为：%s",String::fromStdWString((LPCTSTR)strCurrentSel).c_str());
			//确认是不是子项
			for (int i=0; i!=m_ExtraItems.size(); i++)
			{
				//LOG_INFO("寻找当前项匹配的子项,子项=%s",String::fromStdWString((LPCTSTR)m_ExtraItems[i].m_strName).c_str());
				if (m_ExtraItems[i].m_strName == strCurrentSel)
				{
					LOG_INFO("找到了子项 %s",String::fromStdWString((LPCTSTR)strCurrentSel).c_str());
					//是子项，再判断是不是目录
					if (m_ExtraItems[i].m_bIsFolder)
					{
						//这个是目录，不做任何事情
						MessageBox(_T("请选择需要安装的系统。"));
						return;
					}
					else
					{
						m_strGhostRestoreSrc = strFilePath;
						POSITION pos = m_listPartions.GetFirstSelectedItemPosition();
						if (pos == NULL)
						{
							MessageBox(_T("请选择将要把备份文件恢复到哪个分区。"));
							return;
						}
						int nItem = m_listPartions.GetNextSelectedItem(pos);
						CString strRestoreDestPartionName = m_listPartions.GetItemText(nItem,0);
						CString strRestoreDestPartionIDs = m_listPartions.GetItemText(nItem,1);
						ASSERT(nItem != -1 && strRestoreDestPartionIDs != _T(""));
						//确认这个子项的类型
						if (m_bIsWIMInstall)
						{
							//win7 8 10的安装。
							LOG_INFO("准备win7 8 10的安装");
							CDlgImagexRestore dlg;
							LOG_INFO("id = %d",m_ExtraItems[i].m_iWimIndex);
							LOG_INFO("目标分区名字 = %s",String::fromStdWString((LPCTSTR)strRestoreDestPartionName).c_str());
							LOG_INFO("目标分区的ID = %s",String::fromStdWString((LPCTSTR)strRestoreDestPartionIDs).c_str());
							LOG_INFO("来源主项 = %s",String::fromStdWString((LPCTSTR)m_ExtraItems[i].m_strSourceMain).c_str());
							LOG_INFO("目标子项 = %s",String::fromStdWString((LPCTSTR)m_ExtraItems[i].m_strSourceSub).c_str());
							dlg.SetOneKeyImageStoreCfg(m_ExtraItems[i].m_iWimIndex,strRestoreDestPartionName,strRestoreDestPartionIDs,m_ExtraItems[i].m_strSourceMain,m_ExtraItems[i].m_strSourceSub);
							dlg.DoModal();
							return;
						}
						else if (m_bIsISOWinXPInstall)
						{
							//xp原版的安装。
							return;
						}
						else
							return;
					}
				}
			}
			//不应在此，有错误发生了
			MessageBox(_T("逻辑错误，没有在系统列表中当前的安装项。请重试。"));
		}

	}
	else if (((CButton*)GetDlgItem(IDC_RADIO_BACKUP_GHOST))->GetCheck() == BST_CHECKED)
	{
		//Ghost备份分区
		//检查基本参数
		CString strGhoFilePath;
		GetDlgItemText(IDC_EDIT_GHOST,strGhoFilePath);
		if (strGhoFilePath == _T(""))
		{
			MessageBox(_T("备份路径不能为空"),NULL,MB_OK);
			return;
		}
		m_strGhoBackupFilePath = strGhoFilePath;
		m_strGhostBackupSrc = "";
		POSITION pos = m_listPartions.GetFirstSelectedItemPosition();
		if (pos == NULL)
		{
			MessageBox(_T("请选择需要备份的分区。"));
			return;
		}
		int nItem = m_listPartions.GetNextSelectedItem(pos);
		m_strGhostBackupSrc = m_listPartions.GetItemText(nItem,1);
		ASSERT(nItem != -1 && m_strGhostBackupSrc != _T(""));
		CloseHandle((HANDLE)_beginthread(ThreadFunBackupGhost,0,this));
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO_BACKUP_IMAGEX))->GetCheck() == BST_CHECKED)
	{
		//Imagex备份分区
		CString strImgFilePath;
		GetDlgItemText(IDC_EDIT_IMAGEX,strImgFilePath);
		if (strImgFilePath == _T(""))
		{
			MessageBox(_T("备份路径不能为空"),NULL,MB_OK);
			return;
		}
		m_strImgFileDest = strImgFilePath;
		POSITION pos = m_listPartions.GetFirstSelectedItemPosition();
		if (pos == NULL)
		{
			MessageBox(_T("请选择需要备份的分区。"));
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

	//主动调用一次OnCbnSelchangeComboIsoghost，载入ISO到虚拟光驱，判断是不是有子目录
	OnCbnSelchangeComboIsoghost();
	return 0;
}


void CUPEToolDlg::OnNMRClickListCols(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;  
	if (pNMListView->iItem != -1)  
	{  
		DWORD dwPos = GetMessagePos();  
		CPoint point(LOWORD(dwPos), HIWORD(dwPos));  
		CMenu menu;   
		VERIFY(menu.LoadMenu(IDR_MENU_LIST_CTRL));           //这里是我们在1中定义的MENU的文件名称  
		CMenu* popup = menu.GetSubMenu(0);  
		ASSERT(popup != NULL);  
		popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);  

		//下面的两行代码主要是为了后面的操作为准备的  
		//获取列表视图控件中第一个被选择项的位置    
		//POSITION m_pstion = GetFirstSelectedItemPosition();  
		//该函数获取由pos指定的列表项的索引，然后将pos设置为下一个位置的POSITION值  
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
	// TODO: 在此添加控件通知处理程序代码
	if (((CButton*)GetDlgItem(IDC_RADIO_BACKUP_GHOST))->GetCheck() == BST_CHECKED)
	{
		GetDlgItem(IDC_COMBO_IsoGhost)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_IMAGEX)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_GHOST)->ShowWindow(SW_SHOW);
	}
}


void CUPEToolDlg::OnBnClickedRadioRestore()
{
	// TODO: 在此添加控件通知处理程序代码
	if (((CButton*)GetDlgItem(IDC_RADIO_RESTORE))->GetCheck() == BST_CHECKED)
	{
		GetDlgItem(IDC_EDIT_IMAGEX)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_GHOST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_IsoGhost)->ShowWindow(SW_SHOW);
	}
}


void CUPEToolDlg::OnBnClickedRadioBackupImagex()
{
	// TODO: 在此添加控件通知处理程序代码
	if (((CButton*)GetDlgItem(IDC_RADIO_BACKUP_IMAGEX))->GetCheck() == BST_CHECKED)
	{
		GetDlgItem(IDC_EDIT_GHOST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_IsoGhost)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_IMAGEX)->ShowWindow(SW_SHOW);
	}
}


void CUPEToolDlg::OnBnSetfocusRadioRestore()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CUPEToolDlg::OnBnSetfocusRadioBackupGhost()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CUPEToolDlg::OnBnSetfocusRadioBackupImagex()
{
	// TODO: 在此添加控件通知处理程序代码
}

LRESULT CUPEToolDlg::OnGhostBackUpEnd( WPARAM wParam,LPARAM )
{
	if (wParam == 1)
	{
		MessageBox(CUtil::ErrorMessageW(L"创建GHO文件").c_str());
		return 1;
	}
	return 0;
}


void CUPEToolDlg::OnMenuOpenPartion()
{
	// TODO: 在此添加命令处理程序代码
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
	// TODO: 在此添加命令处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
	int nItem = m_comboIsoGho.GetCurSel();
	CString strCurrentSel;
	m_comboIsoGho.GetLBText(nItem,strCurrentSel);
	//LOG_INFO("当前选中的子项为：%s",String::fromStdWString((LPCTSTR)strCurrentSel).c_str());
	//确认是不是子项
	for (int i=0; i!=m_ExtraItems.size(); i++)
	{
		//LOG_INFO("寻找当前项匹配的子项,子项=%s",String::fromStdWString((LPCTSTR)m_ExtraItems[i].m_strName).c_str());
		if (m_ExtraItems[i].m_strName == strCurrentSel)
		{
			LOG_INFO("找到了子项 %s",String::fromStdWString((LPCTSTR)strCurrentSel).c_str());
			//是子项，再判断是不是目录
			if (m_ExtraItems[i].m_bIsFolder)
			{
				LOG_INFO("这个子项是目录");
				//如果是目录，那么跳转到第一个子项并返回
				CString subItemName = m_ExtraItems[i].m_strParentOrFirstChild;
				LOG_INFO("这个子项目录的第一子项是 %s",String::fromStdWString((LPCTSTR)subItemName).c_str());
				for (int j=0; j!=m_comboIsoGho.GetCount(); j++)
				{
					CString strComboItem;
					m_comboIsoGho.GetLBText(j,strComboItem);
					if (strComboItem == subItemName)
					{
						LOG_INFO("找到了第一个子项，位置为%d",j);
						m_comboIsoGho.SetCurSel(j);
						return;
					}
				}
			}
			else{
				//否则不做任何事
				return;
			}
		}
	}
	CString temp = strCurrentSel.MakeUpper();
	// 如果是ISO，则需要将它加载到虚拟光驱里。
	if (CUtil::isStringEndsWith((LPCTSTR)temp,L".ISO"))
	{
		LOG_INFO("当前选中的不是额外项");
		// 如果是子项则函数已经返回，下面则不是子项。
		// 尝试加载新选择的ISO,如果选择的ISO不是当前虚拟光驱里的ISO，那么刷新额外列表。
		if (TRUE/*CString(m_virualCDD_Z.GetCurLoadedISO().c_str()) != strFilePath*/) // 不管当前光驱里是不是这个ISO都重新刷新之。
		{

			LOG_INFO("载入ISO：");
			string astrFilePath = String::fromStdWString((LPCTSTR)strCurrentSel);
			LOG_INFO(astrFilePath.c_str());
			CExtractCmdExecuter Executer;
			
			Executer.Extract(IDR_BIN_isocmd_exe,L"BIN",L"isocmd.exe");
			Executer.Extract(IDR_BIN_ISODrive_sys,L"BIN",L"ISODrive.sys");
			// 把额外项先删除掉,从后往前删，直到分割线。
			m_ExtraItems.clear();
			
			int count = m_comboIsoGho.GetCount();
			// 先确认一下有没有 ----------------- 别瞎删把正常的数据都删光光了
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
				LOG_INFO("弹出当前的虚拟光驱……");
				if (!m_virualCDD_Z.EJect(Executer))
				{
					MessageBox(L"弹出虚拟光驱失败，请重试！");
					return;
				}
			}else{
				LOG_INFO("安装虚拟光驱服务……");
				if (!m_virualCDD_Z.Install(Executer))
				{
					MessageBox(L"安装虚拟光驱失败，请重试！");
					return;
				}
			}
			
			if (!m_virualCDD_Z.Mount((LPCTSTR)strCurrentSel,Executer))
			{
				MessageBox(_T("此ISO载入失败"));
				return;
			}else{
				LOG_INFO("准备载入ISO，获取ISO类型");
				wstring wimOrGhostPath;
				ISOType isoType = m_virualCDD_Z.GetIsoType(wimOrGhostPath);
				switch(isoType){
				case ISO_UNKNOW:
					LOG_INFO("ISO类型为未知类型");
					m_bIsWIMInstall = FALSE;
					break;
				case ISO_GHOST: 
					{
						m_bIsWIMInstall = FALSE;
						LOG_INFO("ISO类型为GHOST类型");
						//增加一行-----------------------
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
						ext.m_strCmdLine = L"";  //对于GHOST不需要命令行，因为m_strName已经指定了文件名
						m_ExtraItems.push_back(ext);  //增加一项Z:/xxx/xxx.gho，并且跳转到这一项作为当前项。
						m_comboIsoGho.SetCurSel(m_comboIsoGho.AddString(wimOrGhostPath.c_str()));
						break;
					}
				case ISO_WIM:
					{
						LOG_INFO("ISO类型为WIM类型");
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
								TRACE("发生错误，没有找到WIMISO的第一个子节点");
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
	//打开文件并读取文件内容
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
		LOG_INFO("imagex.exe执行查询的结果为：");
		LOG_INFO("%s",String::fromStdWString(WimFilePath).c_str());
		LOG_INFO("%s",contents.c_str());
		ifs.close();
	}*/
	//处理文件内容contents中的XML
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
					//LOG_INFO("对象大小 = %s",String::fromStdWString(wss.str()).c_str());
					//LOG_INFO("转换字符串 = %s",String::fromLongLong(cbSize).c_str());
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
				// 将读出来的子项保存起来
				if (index != L"" && DisplayName != L"" && strSize != L"")
				{
					CExtraItem sub;
					sub.m_bIsFolder = FALSE;
					sub.m_bIsGhost = FALSE;
					// 在CMD中记录此项的安装命令
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
			// 如果子项不为空。那么在这些项的头部添加一个目录
			// 并且添加到输入参数引用中。
			if (!tempVec.empty())
			{
				//处理一下加入“├”和“ └”符号 
				BOOL bLast = TRUE;
				for (std::deque<CExtraItem>::reverse_iterator x = tempVec.rbegin(); x != tempVec.rend(); ++x)
				{
					if (bLast)
					{
						x->m_strName = CString(_T("└")) + x->m_strName;
						bLast = FALSE;
					}
					else
					{
						x->m_strName = CString(_T("├")) + x->m_strName;
					}
				}

				CExtraItem itemFolder;
				itemFolder.m_bIsFolder = TRUE;
				itemFolder.m_bIsGhost = FALSE;
				itemFolder.m_strCmdLine = "";
				itemFolder.m_strName = "Z:\\sources\\install.wim";
				itemFolder.m_strParentOrFirstChild = tempVec.front().m_strName;
				LOG_INFO("设定子项为：%s",String::fromStdWString((LPCTSTR)itemFolder.m_strParentOrFirstChild).c_str());
				tempVec.push_front(itemFolder);

				itemFolder.m_strName = "------------------------------------------------------";
				tempVec.push_front(itemFolder);

				for (std::deque<CExtraItem>::const_iterator x = tempVec.begin(); x != tempVec.end(); ++x)
				{
					vec.push_back(*x);
				}
			}
		}
		//删除文件
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
	// TODO: 在此添加控件通知处理程序代码
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
		MessageBox(L"不匹配mins");
	}
	*/
	CDialogEx::OnCancel();
}


void CUPEToolDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	CExtractCmdExecuter Executer;

	Executer.Extract(IDR_BIN_isocmd_exe,L"BIN",L"isocmd.exe");
	Executer.Extract(IDR_BIN_ISODrive_sys,L"BIN",L"ISODrive.sys");
	m_virualCDD_Z.EJect(Executer);
	m_virualCDD_Z.Remove(Executer);

}


BOOL CUPEToolDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN))
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}
