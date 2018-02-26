#pragma once
#include "afxwin.h"

class COneKeyImageRestoreTaskCfg
{
public:
	int m_iWimIndex;
	CString m_strRestoreDestPartionName;
	CString m_strRestoreDestPartionIDs;
	CString m_strSourceMain;
	CString m_strSourceSub;
protected:
private:

};
// CDlgImagexRestore 对话框

class CDlgImagexRestore : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgImagexRestore)

public:
	CDlgImagexRestore(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgImagexRestore();

// 对话框数据
	enum { IDD = IDD_DIALOG_ONEKEY_IMAGEX_RESTORE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	COneKeyImageRestoreTaskCfg m_RestoreCfg;
public:
	void SetOneKeyImageStoreCfg(int iWimIndex,
		const CString& strRestoreDestPartionName,
		const CString& strRestoreDestPartionIDs,
		const CString& strSourceMain,
		const CString& strSourceSub);

	virtual BOOL OnInitDialog();
	CString m_strDestPartionName;
	CStatic m_LableDestPartionId;
	CString m_strSourceMain;
	CString m_strSourceSub;
	BOOL m_bIsFormat;
	BOOL m_bIsAddBoot;
	BOOL m_bIsFileCheck;
	BOOL m_bReboot;
	BOOL m_bHalt;
	// 控件变量，对应“完成比例”编辑框
	CString m_strCompletePercent;
	// 控件变量，对应“剩余时间”编辑框
	CString m_strTimeRemain;
	// 控件变量，对应“已用时间”编辑框
	CString m_strTimeUsed;
	afx_msg void OnBnClickedOk();
	CComboBox m_ComboxFormatType;
	CComboBox m_ComboxBootPartion;
	// 控件变量 对应分区编号控件
	CString m_strDestPartionId;
	int m_iWimIndex;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
