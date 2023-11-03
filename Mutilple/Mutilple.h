
// Mutilple.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "MainFrame.h"
#include "MainProcess.h"

// CMutilpleApp:
// See Mutilple.cpp for the implementation of this class
//

class CMutilpleApp : public CWinApp
{
public:
	CMutilpleApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

public:
    CMainFrame* m_pMainFrame;
    TCHAR m_pExePath[_MAX_FNAME];
    CShellManager* m_pShellManager;
};

extern CMutilpleApp theApp;
extern MainProcess g_MainProcess;
extern CString TITLE;
extern CString VERSION;