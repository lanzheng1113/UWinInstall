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
// CDlgImagexRestore �Ի���

class CDlgImagexRestore : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgImagexRestore)

public:
	CDlgImagexRestore(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgImagexRestore();

// �Ի�������
	enum { IDD = IDD_DIALOG_ONEKEY_IMAGEX_RESTORE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
	// �ؼ���������Ӧ����ɱ������༭��
	CString m_strCompletePercent;
	// �ؼ���������Ӧ��ʣ��ʱ�䡱�༭��
	CString m_strTimeRemain;
	// �ؼ���������Ӧ������ʱ�䡱�༭��
	CString m_strTimeUsed;
	afx_msg void OnBnClickedOk();
	CComboBox m_ComboxFormatType;
	CComboBox m_ComboxBootPartion;
	// �ؼ����� ��Ӧ������ſؼ�
	CString m_strDestPartionId;
	int m_iWimIndex;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
