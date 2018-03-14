#pragma once
#include "afxwin.h"


// CDlgXPInstall 对话框

class CDlgXPInstall : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgXPInstall)

public:
	CDlgXPInstall(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgXPInstall();

// 对话框数据
	enum { IDD = IDD_DIALOG_XP_INSTALL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	void setDestPartionName(const CString& PartionName){
		m_strDestPartionName = PartionName;
	}
	void setDestPartionID(const CString& PartionId){
		m_strPartionId = PartionId;
	}
	void setSource(const CString& sourceMain, const CString& sourceSub){
		m_strSourceMain = sourceMain;
		m_strSourceSub = sourceSub;
	}
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString m_strDestPartionName;
	CString m_strPartionId;
	CString m_strSourceMain;
	CString m_strSourceSub;
	BOOL m_bIsFormat;
	BOOL m_bIsBoot;
	CComboBox m_comboForamtType;
	CComboBox m_comboBoot;
	CString m_strKey1;
	CString m_strKey2;
	CString m_strKey3;
	CString m_strKey4;
	CString m_strKey5;
	BOOL m_bReboot;
	BOOL m_bHalt;
	afx_msg void OnBnClickedOk();
};
