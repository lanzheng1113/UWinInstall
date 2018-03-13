#pragma once

// GHOST安装的业务对象，将他拆分出来避免和界面挤成一团。让CDlgGhostRestoreConfirm只管界面的交互
class CBussGhostRestore
{
public:
	CBussGhostRestore();
	~CBussGhostRestore();
public:
	// 开始安装
	void Start(const CString& strPartionName,const CString& strPartionIndex,const CString& strImageFilePath,HWND hWnd);
private:
	// 需要安装到哪个盘，如C:
	CString m_strPartionName;
	// 安装盘的磁盘加分区的ID，如1:1
	CString m_strPartionIndex;
	// 安装源，如Z:\GHOST\WIN7_1.GHO
	CString m_strImageFilePath;
	// 用于通知进度和结果的窗口句柄
	HWND m_hWnd;
	// 安装线程
	static UINT ThreadFunGhostRestore(LPVOID lpThreadParam);
};

// CDlgGhostRestoreConfirm 对话框

class CDlgGhostRestoreConfirm : 
	public CDialogEx
{
	DECLARE_DYNAMIC(CDlgGhostRestoreConfirm)

public:
	CDlgGhostRestoreConfirm(const CString& strPartionName,const CString& strPartionIndex,const CString& strImageFilePath,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgGhostRestoreConfirm();

// 对话框数据
	enum { IDD = IDD_DIALOG_GHOST_RESTORE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 安装逻辑
	CBussGhostRestore* m_buss;
	// 安装完成后是否需要重启
	BOOL m_bReboot;
	// 安装完成后是否需要关机
	BOOL m_bHalt;
	// 需要安装到哪个盘，如C:
	CString m_strPartionName;
	// 安装盘的磁盘加分区的ID，如1:1
	CString m_strPartionIndex;
	// 安装源，如Z:\GHOST\WIN7_1.GHO
	CString m_strImageFilePath;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedCheckRebootComplete();
	afx_msg void OnBnClickedCheckHaltComplete();
	LRESULT OnRestoreDataEnd(WPARAM wParam,LPARAM lParam);
};
