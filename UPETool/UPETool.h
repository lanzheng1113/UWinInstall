
// UPETool.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CUPEToolApp:
// �йش����ʵ�֣������ UPETool.cpp
//

class CUPEToolApp : public CWinApp
{
public:
	CUPEToolApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CUPEToolApp theApp;