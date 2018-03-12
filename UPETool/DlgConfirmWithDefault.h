#pragma once

// 向前声明
class IComfirmDlgModule;

// CDlgConfirmWithDefault 对话框

class CDlgConfirmWithDefault : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgConfirmWithDefault)

public:
	CDlgConfirmWithDefault(IComfirmDlgModule* pModule,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgConfirmWithDefault();

// 对话框数据
	enum { IDD = IDD_DIALOG_CONFIRM_DEFAULT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
private:
	IComfirmDlgModule* m_pModule;
	UINT m_TimeOut;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
