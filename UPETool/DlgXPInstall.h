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
private:
	typedef enum
	{
		EFT_NTFS,
		EFT_FAT32,
		EFT_USERDEFINE
	}ETypeFormatXP;
	enum
	{
		XP_INSTALL_SUCCESS,
		XP_INSTALL_ERR_EXTRACT_SIF,
		XP_INSTALL_ERR_INSTALL,
		XP_INSTALL_ERR_BOOT_SECT,
		XP_INSTALL_ERR_FORMAT
	};
	static UINT ThreadFunXpSetup(LPVOID lpThreadParam);
	UINT doXpSetup();
	CString m_strKeyTotal;
	HANDLE m_hThread;
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
	LRESULT OnFinishInstall(WPARAM wParam,LPARAM lParam);
	afx_msg void OnBnClickedCancel();
};
