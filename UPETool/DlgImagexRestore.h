#pragma once
#include "afxwin.h"
#include "InterfaceCallBack.h"

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

typedef enum
{
	EFT_NTFS,
	EFT_FAT32,
	EFT_USERDEFINE
}ETypeFormat;

// CDlgImagexRestore �Ի���

class CDlgImagexRestore : public CDialogEx ,ICallBackRestore
{
	DECLARE_DYNAMIC(CDlgImagexRestore)

public:
	CDlgImagexRestore(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgImagexRestore();

// �Ի�������
	enum { IDD = IDD_DIALOG_ONEKEY_IMAGEX_RESTORE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	static UINT ThreadFunDoImagexRestore(LPVOID lpThreadParam);
	DECLARE_MESSAGE_MAP()
private:
	COneKeyImageRestoreTaskCfg m_RestoreCfg;
	UINT DoImagexRestoreInternal();
	void PostUnexpectedError(UINT ErrorCode);
public:
	virtual void ExecCmdCallBack(const std::string& text) override;
	LRESULT OnUnexpectError(WPARAM ,LPARAM lParam);
	LRESULT OnImagexInstallSpecificUpdateProgress(WPARAM,LPARAM);
	LRESULT OnUpdateTotalProgress(WPARAM wParma,LPARAM lParam);
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
	// �ؼ���������Ӧ����ɱ������༭��
	CString m_strCompletePercent;
	// �ؼ���������Ӧ��ʣ��ʱ�䡱�༭��
	CString m_strTimeRemain;
	// �ؼ���������Ӧ������ʱ�䡱�༭��
	CString m_strTimeUsed;
	afx_msg void OnBnClickedOk();
	CComboBox m_ComboxFormatType;
	ETypeFormat m_eFormatType;
	CComboBox m_ComboxBootPartion;
	// �ؼ����� ��Ӧ������ſؼ�
	CString m_strDestPartionId;
	int m_iWimIndex;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int m_iTimerCount;
	CString m_strCurSelBootPartion;
	// �ؼ�������������ʾ��
	CString m_strNotice;
};
