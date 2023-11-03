#include "pch.h"

#include "AboutWnd.h"
#include "CheckWnd.h"
#include "DetectionStd.h"
#include "MainFrame.h"
#include "Mutilple.h"
#include "ReViewScanWnd.h"
#include "ScanWnd.h"
#include "SetQuartWnd.h"
#include "SetWnd.h"
#include "UserWnd.h"
#include "GroupScanWnd.h"

CMainFrame::CMainFrame(void) {
}

CMainFrame::~CMainFrame(void) {
}

LRESULT CMainFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_KEYDOWN: {
            /*switch ((TCHAR)wParam)
            {
                case VK_UP: g_MainProcess.OnUpkey(); break;
                case VK_DOWN: g_MainProcess.OnDownKey(); break;
                case VK_LEFT: g_MainProcess.OnLeftkey(); break;
                case VK_RIGHT: g_MainProcess.OnRightKey(); break;
            }*/
        } break;
    }

    return CDuiWindowBase::HandleMessage(uMsg, wParam, lParam);
}

void CMainFrame::InitWindow() {
    CDuiWindowBase::InitWindow();

    // ��ʼ��
    std::thread Init(&CMainFrame::Init, this);
    Init.detach();
}

void CMainFrame::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        CDuiString strName = msg.pSender->GetName();
        if (strName == _T("BtnSet")) {
            OnBtnSet();
        } else if (strName == _T("BtnScan")) {
            OnBtnScan();
        } else if (strName == _T("BtnUser")) {
            OnBtnUser();
        } else if (strName == _T("BtnGroupScan")) {
            OnBtnGroupScan();
        }
        else if (strName == _T("BtnQuart")) {
            OnBtnQuart();
        } else if (strName == _T("BtnAnalysis")) {
            OnBtnAnalysis();
        } else if (strName == _T("BtnAbout")) {
            OnBtnAbout();
        }
    }

    WindowImplBase::Notify(msg);
}

void CMainFrame::Init() {
    // �ӳ���󻯴���
    Sleep(100);
    SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
    g_MainProcess.m_Techniques.SetDetectionStd(DETECTIONSTD_TBT2995_200);
    // �Լ�
    CheckWnd Mydlg;
    Mydlg.Create(m_hWnd, _T("CheckWnd"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
    Mydlg.CenterWindow();
    if (Mydlg.ShowModal() == IDNO) {
        Close(IDOK);
    }
}

void CMainFrame::OnBtnSet() {
    CString strCfgFile;
    strCfgFile.Format(_T("%sLastParam.ini"), theApp.m_pExePath);
    g_MainProcess.m_Techniques.LoadTec(strCfgFile); // ��ɨ��
    // ���ô���
    g_MainProcess.m_enumMainProType = ROCESS_PARAMSET;
    SetWnd Mydlg;
    Mydlg.Create(m_hWnd, _T("SetWnd"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
    Mydlg.CenterWindow();
    Mydlg.ShowModal();

    CString strTechName = g_MainProcess.m_Techniques.m_pCurTechName;

    // CString strCfgFile;
    //	strCfgFile.Format(_T("%sTechINI\\%s.ini"), theApp.m_pExePath, strTechName.GetBuffer(0));
    // g_MainProcess.m_Techniques.SaveTec(strCfgFile); // ���浽���� ini����

    strCfgFile.Format(_T("%sLastParam.ini"), theApp.m_pExePath);
    g_MainProcess.m_Techniques.SaveTec(strCfgFile); // ���浽��һ��
    strCfgFile.Format(_T("%sTechINI\\%s.tech"), theApp.m_pExePath, strTechName.GetBuffer(0));

    long lStartTime = GetTickCount64();
    g_MainProcess.m_Techniques.SaveTecBinary(strCfgFile);
    long lTime = GetTickCount64() - lStartTime;
    int  test  = 0;
}

void CMainFrame::OnBtnScan() {
    CString strCfgFile;
    strCfgFile.Format(_T("%sLastParam.ini"), theApp.m_pExePath);
    g_MainProcess.m_Techniques.LoadTec(strCfgFile); // ��ɨ��

    g_MainProcess.m_enumMainProType = ROCESS_SCAN;
    // ɨ�鴰��
    ScanWnd Mydlg;
    Mydlg.Create(m_hWnd, _T("ScanWnd"), UI_WNDSTYLE_FRAME, UI_WNDSTYLE_EX_FRAME);
    Mydlg.CenterWindow();
    Mydlg.ShowModal();
}

void CMainFrame::OnBtnGroupScan() {
    // ɨ�鴰��
    GroupScanWnd Mydlg;
    Mydlg.Create(m_hWnd, _T("GroupScanWnd"), UI_WNDSTYLE_FRAME, UI_WNDSTYLE_EX_FRAME);
    Mydlg.CenterWindow();
    Mydlg.ShowModal();
}

void CMainFrame::OnBtnUser() {
    //
    UserWnd userWnd;
    userWnd.Create(m_hWnd, userWnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
    userWnd.CenterWindow();
    userWnd.ShowModal();
}

void CMainFrame::OnBtnQuart() {
    // ���ü��ȴ���

    SetQuartWnd Mydlg;
    Mydlg.Create(m_hWnd, _T("SetQuartWnd"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
    Mydlg.CenterWindow();
    Mydlg.ShowModal();
}
void CMainFrame::OnBtnAbout() {
    CString strAbout = _T("\n����汾:V1.1.231008\n\n������λ:��ͨ�������뼼���������޹�˾");

    AboutWnd Msgbox(strAbout, _T("�������"), MB_OK);
    Msgbox.Create(m_hWnd, _T("AboutWnd"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
    Msgbox.CenterWindow();
    UINT iRet = Msgbox.ShowModal();
}
void CMainFrame::OnBtnAnalysis() {
    ReViewScanWnd Mydlg;

    //	Mydlg.Create(m_hWnd, _T("ReViewScanWnd"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_FRAME);
    Mydlg.Create(m_hWnd, _T("ReViewScanWnd"), UI_WNDSTYLE_FRAME, UI_WNDSTYLE_EX_FRAME);
    Mydlg.CenterWindow();
    Mydlg.ShowModal();
}