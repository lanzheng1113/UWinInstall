
// UPETool.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "UPETool.h"
#include "UPEToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "util/Logger.h"
#include "util/DateTime.h"
#include <sstream>
using std::stringstream;

// CUPEToolApp

BEGIN_MESSAGE_MAP(CUPEToolApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CUPEToolApp ����

CUPEToolApp::CUPEToolApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CUPEToolApp ����

CUPEToolApp theApp;


// CUPEToolApp ��ʼ��

BOOL CUPEToolApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CUPEToolDlg dlg;
	m_pMainWnd = &dlg;

	//��ʼ����־
	DateTime dt;
	stringstream ss;
	ss << dt.getYear() << dt.getMonth() << dt.getDay() << dt.getHour() << dt.getMinute() << dt.getSecond() << ".txt";
	if (PathFileExistsW(L"X:\\temp"))
	{
		Logger::getInstance()->setLogFileName(std::string("X:\\temp\\") + ss.str());
	}
	else
		Logger::getInstance()->setLogFileName(ss.str());
	
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

