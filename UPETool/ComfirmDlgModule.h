#pragma once
// 这个类提供一个接口用于标准倒计时窗口
// IComfirmDlgModule* pObj = new CComfirmDlgModuleInhert(DialogCaption,Lable,ButtonText,TimeOut)
// CDlgConfirmWithDefault dlg(this,pObj)
// dlg.doModule();
class IComfirmDlgModule
{
public:
	IComfirmDlgModule(CString DialogCaption,CString Lable,CString ButtonText,UINT TimeOut)
	{ 
		m_strDialogCaption = DialogCaption;
		m_strLable = Lable;
		m_ButtonText = ButtonText;
		m_iTimeOut = TimeOut;
	}
	virtual ~IComfirmDlgModule(void){;}
public:
	// 强制限定子对象必须重载这个OK按钮函数。
	virtual void DoWhileOk() = 0;
	// 不强制限定子对象必须重载这个Cancel按钮对象。
	virtual void DoWhileCancel()
	{
		return;
	}
public:
	CString DialogCaption() const { return m_strDialogCaption; }
	CString Lable() const { return m_strLable; }
	CString ButtonText() const { return m_ButtonText; }
	UINT TimeOut() const { return m_iTimeOut; }
private:
	// 设置对话框的标题
	CString m_strDialogCaption;
	// 设置对话框中说明LABLE的内容
	CString m_strLable;
	// 设置对话框中“确定”按钮的标题
	CString m_ButtonText;
	// 设置多少秒后执行指定的函数。
	UINT m_iTimeOut;
};

