#pragma once
#include "afxwin.h"


// CDialogImagexBackup 对话框
enum{
	EImagexBackupInited,
	EImagexBackupRuning,
	EImagexBackupFinish
};
class CDialogImagexBackup : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogImagexBackup)

public:
	CDialogImagexBackup(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogImagexBackup();

// 对话框数据
	enum { IDD = IDD_DIALOG_ONEKEY_IMAGEX };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	// 需要备份的分区
	CString m_strVolNameSrc;
	// 硬盘-分区编号（如1:1表示第一个硬盘的第一个分区）
	CString m_strPartionNumber;
	// 备份到的目标文件
	CString m_strNameDest;
	// 这个是要传给ImageEx的名字
	CString m_strImageName;
	// 选择0不压缩，1快速压缩，2最大压缩
	CComboBox m_ComboCompressType;
	BOOL m_bAddtionBackup;
	CString m_strProgressPercent;
	// 剩余时间
	CString m_strTimeLeft;
	// 已经使用的时间
	CString m_strTimeUsed;
	// 是否需要文件校验
	BOOL m_bIsFileCheck;
	// 是否完成后重启
	BOOL m_bRebootWhileSuccess;
	// 是否完成后关机
	BOOL m_bHaltWhileSuccess;
	// 是否正在进行
	int m_step;
	afx_msg void OnBnClickedOk();
	LRESULT OnBackupEnd(WPARAM wParam, LPARAM lParam);
	// 确定按钮
	CButton m_btnOK;
	CString m_state;
};
