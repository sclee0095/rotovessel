
// LiveVessel.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CLiveVesselApp:
// �� Ŭ������ ������ ���ؼ��� LiveVessel.cpp�� �����Ͻʽÿ�.
//

class CLiveVesselApp : public CWinApp
{
public:
	CLiveVesselApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CLiveVesselApp theApp;