
// Copyǩ��.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCopyǩ��App: 
// �йش����ʵ�֣������ Copyǩ��.cpp
//

class CCopyǩ��App : public CWinApp
{
public:
	CCopyǩ��App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCopyǩ��App theApp;