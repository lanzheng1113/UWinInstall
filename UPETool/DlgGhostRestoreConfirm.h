#pragma once

// GHOST��װ��ҵ����󣬽�����ֳ�������ͽ��漷��һ�š���CDlgGhostRestoreConfirmֻ�ܽ���Ľ���
class CBussGhostRestore
{
public:
	CBussGhostRestore();
	~CBussGhostRestore();
public:
	// ��ʼ��װ
	void Start(const CString& strPartionName,const CString& strPartionIndex,const CString& strImageFilePath,HWND hWnd);
private:
	// ��Ҫ��װ���ĸ��̣���C:
	CString m_strPartionName;
	// ��װ�̵Ĵ��̼ӷ�����ID����1:1
	CString m_strPartionIndex;
	// ��װԴ����Z:\GHOST\WIN7_1.GHO
	CString m_strImageFilePath;
	// ����֪ͨ���Ⱥͽ���Ĵ��ھ��
	HWND m_hWnd;
	// ��װ�߳�
	static UINT ThreadFunGhostRestore(LPVOID lpThreadParam);
};

// CDlgGhostRestoreConfirm �Ի���

class CDlgGhostRestoreConfirm : 
	public CDialogEx
{
	DECLARE_DYNAMIC(CDlgGhostRestoreConfirm)

public:
	CDlgGhostRestoreConfirm(const CString& strPartionName,const CString& strPartionIndex,const CString& strImageFilePath,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgGhostRestoreConfirm();

// �Ի�������
	enum { IDD = IDD_DIALOG_GHOST_RESTORE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// ��װ�߼�
	CBussGhostRestore* m_buss;
	// ��װ��ɺ��Ƿ���Ҫ����
	BOOL m_bReboot;
	// ��װ��ɺ��Ƿ���Ҫ�ػ�
	BOOL m_bHalt;
	// ��Ҫ��װ���ĸ��̣���C:
	CString m_strPartionName;
	// ��װ�̵Ĵ��̼ӷ�����ID����1:1
	CString m_strPartionIndex;
	// ��װԴ����Z:\GHOST\WIN7_1.GHO
	CString m_strImageFilePath;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedCheckRebootComplete();
	afx_msg void OnBnClickedCheckHaltComplete();
	LRESULT OnRestoreDataEnd(WPARAM wParam,LPARAM lParam);
};
