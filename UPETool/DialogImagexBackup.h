#pragma once
#include "afxwin.h"


// CDialogImagexBackup �Ի���
enum{
	EImagexBackupInited,
	EImagexBackupRuning,
	EImagexBackupFinish
};
class CDialogImagexBackup : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogImagexBackup)

public:
	CDialogImagexBackup(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogImagexBackup();

// �Ի�������
	enum { IDD = IDD_DIALOG_ONEKEY_IMAGEX };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	// ��Ҫ���ݵķ���
	CString m_strVolNameSrc;
	// Ӳ��-������ţ���1:1��ʾ��һ��Ӳ�̵ĵ�һ��������
	CString m_strPartionNumber;
	// ���ݵ���Ŀ���ļ�
	CString m_strNameDest;
	// �����Ҫ����ImageEx������
	CString m_strImageName;
	// ѡ��0��ѹ����1����ѹ����2���ѹ��
	CComboBox m_ComboCompressType;
	BOOL m_bAddtionBackup;
	CString m_strProgressPercent;
	// ʣ��ʱ��
	CString m_strTimeLeft;
	// �Ѿ�ʹ�õ�ʱ��
	CString m_strTimeUsed;
	// �Ƿ���Ҫ�ļ�У��
	BOOL m_bIsFileCheck;
	// �Ƿ���ɺ�����
	BOOL m_bRebootWhileSuccess;
	// �Ƿ���ɺ�ػ�
	BOOL m_bHaltWhileSuccess;
	// �Ƿ����ڽ���
	int m_step;
	afx_msg void OnBnClickedOk();
	LRESULT OnBackupEnd(WPARAM wParam, LPARAM lParam);
	// ȷ����ť
	CButton m_btnOK;
	CString m_state;
};
