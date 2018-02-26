
// UPEToolDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include <vector>
using std::vector;
#include "DriverInfo.h"
#include "VirtualCDDriver.h"
#include "afxwin.h"
#include "Common.h"

// 这个类仅用于在组合框中显示可选择安装的系统列表
// 一个ISO中的额外项，一个ISO加载到虚拟光驱后，这个ISO里可能有一个或者多个安装项
class CExtraItem
{
public:
	// 显示在下拉框中的名字，可能是以下之一：
	// 1、目录Z:\source\install.wim
	// 2、WIM子项，如 "1 Windows 7 旗舰版[10.52GB]"
	// 3、GHOST子项
	CString m_strName;	
	// 如果是真，那么这个项是如“Z:\source\install.wim”这种目录
	// 如果是假，那么是如同Windows 7 旗舰版这种子项。当选中目录时，自动跳转到第一个子项。
	// GHOST的ISO没有目录项。
	BOOL m_bIsFolder;
	// 如果是子节点，那么这个值指向它的父目录.如果节点是目录则指向目录的第一个子节点.只有在VIM时会有目录。
	CString m_strParentOrFirstChild;
	// 如果是GHOST系统，那么只有一个子项,如 Z:\SYSTEM\GWIN7.GHO(没有目录项)
	BOOL m_bIsGhost;
	// 这个项对应的命令行参数(仅适用于非GHOST系统，即当m_bIsGhost==FALSE时才有效。)
	CString m_strCmdLine;
	// 系统子ID
	int m_iWimIndex;
	// 源ISO的路径，如“Z:\source\install.wim”
	CString m_strSourceMain;
	// 系统信息，如1 Windows 7 旗舰版[10.52GB]
	CString m_strSourceSub;
protected:
private:
};

// CUPEToolDlg 对话框
class CUPEToolDlg : public CDialogEx
{
// 构造
public:
	CUPEToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_UPETOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	LRESULT OnMesgFindISOEnd(WPARAM,LPARAM);
	LRESULT OnGhostBackUpEnd(WPARAM wParam,LPARAM lParam);
	LRESULT OnRestoreDataEnd(WPARAM wParam,LPARAM lParam);
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonExpore();
	afx_msg void OnBnClickedOk();
	CListCtrl m_listPartions;
	std::vector<CDriverInfo> m_arr;
	std::vector<wstring> m_ghoAndIsoArr;
	CComboBox m_comboIsoGho;
	afx_msg void OnNMRClickListCols(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioBackupGhost();
	afx_msg void OnBnClickedRadioRestore();
	afx_msg void OnBnClickedRadioBackupImagex();
	afx_msg void OnBnSetfocusRadioRestore();
	afx_msg void OnBnSetfocusRadioBackupGhost();
	afx_msg void OnBnSetfocusRadioBackupImagex();
	//Ghost备份时的目的备份文件
	CString m_strGhoBackupFilePath;
	//Ghost备份时的来源(磁盘分区号，例如1:1)
	CString m_strGhostBackupSrc;
	//Ghost还原时的来源文件
	CString m_strGhostRestoreSrc;
	//Ghost还原时的目的分区(磁盘分区号，例如1:1)
	CString m_strGhostRestoreDest;
	//Imagex备份时的目的文件
	CString m_strImgFileDest;
	//Imagex备份时的源分区
	CString m_strImgFileSrce;
	// 加载ISO后生成的子项
	vector<CExtraItem> m_ExtraItems;
	// 是否为Wim
	BOOL m_bIsWIMInstall;
	// 是不是XP系统的原版系统安装
	BOOL m_bIsISOWinXPInstall;
	afx_msg void OnMenuOpenPartion();
	afx_msg void OnMenuFormatPartion();
	CVirtualCDDriver m_virualCDD_Z;
private:
	void DoGhostRestore(const CString& strFilePath);
	void DoGhostSetup();
	BOOL FindWIM(LPCWSTR WimFilePath ,vector<CExtraItem>& vec);
public:
	afx_msg void OnCbnSelchangeComboIsoghost();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
