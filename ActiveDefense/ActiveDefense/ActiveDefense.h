
// ActiveDefense.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CActiveDefenseApp: 
// �йش����ʵ�֣������ ActiveDefense.cpp
//

class CActiveDefenseApp : public CWinApp
{
public:
	CActiveDefenseApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CActiveDefenseApp theApp;