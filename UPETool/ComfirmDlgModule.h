#pragma once
// ������ṩһ���ӿ����ڱ�׼����ʱ����
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
	// ǿ���޶��Ӷ�������������OK��ť������
	virtual void DoWhileOk() = 0;
	// ��ǿ���޶��Ӷ�������������Cancel��ť����
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
	// ���öԻ���ı���
	CString m_strDialogCaption;
	// ���öԻ�����˵��LABLE������
	CString m_strLable;
	// ���öԻ����С�ȷ������ť�ı���
	CString m_ButtonText;
	// ���ö������ִ��ָ���ĺ�����
	UINT m_iTimeOut;
};

