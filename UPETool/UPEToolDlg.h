
// UPEToolDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include <vector>
using std::vector;
#include "DriverInfo.h"
#include "VirtualCDDriver.h"
#include "afxwin.h"
#include "Common.h"

// ��������������Ͽ�����ʾ��ѡ��װ��ϵͳ�б�
// һ��ISO�еĶ����һ��ISO���ص�������������ISO�������һ�����߶����װ��
class CExtraItem
{
public:
	// ��ʾ���������е����֣�����������֮һ��
	// 1��Ŀ¼Z:\source\install.wim
	// 2��WIM����� "1 Windows 7 �콢��[10.52GB]"
	// 3��GHOST����
	CString m_strName;	
	// ������棬��ô��������硰Z:\source\install.wim������Ŀ¼
	// ����Ǽ٣���ô����ͬWindows 7 �콢�����������ѡ��Ŀ¼ʱ���Զ���ת����һ�����
	// GHOST��ISOû��Ŀ¼�
	BOOL m_bIsFolder;
	// ������ӽڵ㣬��ô���ֵָ�����ĸ�Ŀ¼.����ڵ���Ŀ¼��ָ��Ŀ¼�ĵ�һ���ӽڵ�.ֻ����VIMʱ����Ŀ¼��
	CString m_strParentOrFirstChild;
	// �����GHOSTϵͳ����ôֻ��һ������,�� Z:\SYSTEM\GWIN7.GHO(û��Ŀ¼��)
	BOOL m_bIsGhost;
	// ������Ӧ�������в���(�������ڷ�GHOSTϵͳ������m_bIsGhost==FALSEʱ����Ч��)
	CString m_strCmdLine;
	// ϵͳ��ID
	int m_iWimIndex;
	// ԴISO��·�����硰Z:\source\install.wim��
	CString m_strSourceMain;
	// ϵͳ��Ϣ����1 Windows 7 �콢��[10.52GB]
	CString m_strSourceSub;
protected:
private:
};

// CUPEToolDlg �Ի���
class CUPEToolDlg : public CDialogEx
{
// ����
public:
	CUPEToolDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_UPETOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	LRESULT OnMesgFindISOEnd(WPARAM,LPARAM);
	LRESULT OnGhostBackUpEnd(WPARAM wParam,LPARAM lParam);
	LRESULT OnRestoreDataEnd(WPARAM wParam,LPARAM lParam);
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonExpore();
	afx_msg void OnBnClickedOk();
	CListCtrl m_listPartions;
	std::vector<CDriverInfo> m_arr;
	std::vector<wstring> m_ghoAndIsoArr;
	CComboBox m_comboIsoGho;
	afx_msg void OnNMRClickListCols(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioBackupGhost();
	afx_msg void OnBnClickedRadioRestore();
	afx_msg void OnBnClickedRadioBackupImagex();
	afx_msg void OnBnSetfocusRadioRestore();
	afx_msg void OnBnSetfocusRadioBackupGhost();
	afx_msg void OnBnSetfocusRadioBackupImagex();
	//Ghost����ʱ��Ŀ�ı����ļ�
	CString m_strGhoBackupFilePath;
	//Ghost����ʱ����Դ(���̷����ţ�����1:1)
	CString m_strGhostBackupSrc;
	//Ghost��ԭʱ����Դ�ļ�
	CString m_strGhostRestoreSrc;
	//Ghost��ԭʱ��Ŀ�ķ���(���̷����ţ�����1:1)
	CString m_strGhostRestoreDest;
	//Imagex����ʱ��Ŀ���ļ�
	CString m_strImgFileDest;
	//Imagex����ʱ��Դ����
	CString m_strImgFileSrce;
	// ����ISO�����ɵ�����
	vector<CExtraItem> m_ExtraItems;
	// �Ƿ�ΪWim
	BOOL m_bIsWIMInstall;
	// �ǲ���XPϵͳ��ԭ��ϵͳ��װ
	BOOL m_bIsISOWinXPInstall;
	afx_msg void OnMenuOpenPartion();
	afx_msg void OnMenuFormatPartion();
	CVirtualCDDriver m_virualCDD_Z;
private:
	void DoGhostRestore(const CString& strFilePath);
	void DoGhostSetup();
	BOOL FindWIM(LPCWSTR WimFilePath ,vector<CExtraItem>& vec);
public:
	afx_msg void OnCbnSelchangeComboIsoghost();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
