#include "pch.h"

#include "AutoScanInfoWnd.h"
#include "DefectListWnd.h"
#include "DetectionStd_TBT2995_200.h"
#include "Mutilple.h"
#include "PLCWnd.h"
#include "ReViewScanWnd.h"
#include "ScanWnd.h"
#include "SetWnd.h"
#include "WheelUpDownWnd.h"

ScanWnd::~ScanWnd() {
    g_MainProcess.m_ConnectPLC.SetPLCAuto(false);
    KillTimer(TIMER_TIME);
    KillTimer(TIMER_SCAN);
    // 退出时，关闭采样
    g_MainProcess.m_HDBridge.StartSequencer(FALSE);
    if (m_pDefectListWnd != NULL) {
        delete m_pDefectListWnd;
    }
}

void ScanWnd::InitOpenGL() {
    m_pWndOpenGL = static_cast<CWindowUI*>(m_PaintManager.FindControl(_T("WndOpenGL")));

    // 初始化OpenGL窗口
    m_OpenGL.Create(m_hWnd);
    m_OpenGL.Attach(m_pWndOpenGL);
}

void ScanWnd::InitWindow() {
    CDuiWindowBase::InitWindow();

    m_pDefectListWnd = NULL;
    m_pLabelData     = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("LabelData")));
    m_pLabelTime     = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("LabelTime")));
    CString strGainName, strGainSurName, strGainSoundName, strGainSoundSurName, strCHName; // 一个增益 一个透声
    CString strText;
    for (int i = 0; i < HD_CHANNEL_NUM; i++) {
        strGainSurName.Format(_T("EditGainSur%d"), i + 1);
        m_pEditGainSur[i] = static_cast<CEditUI*>(m_PaintManager.FindControl(strGainSurName));
        strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[i].fScanCompensate);
        m_pEditGainSur[i]->SetText(strText);
        m_pEditGainSur[i]->SetReadOnly(true);

        strGainSoundSurName.Format(_T("EditGainSoundSur%d"), i + 1);
        m_pEditGainSoundSur[i] = static_cast<CEditUI*>(m_PaintManager.FindControl(strGainSoundSurName));
        strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[i].fScanTrUTCompensate);
        m_pEditGainSoundSur[i]->SetText(strText);
        m_pEditGainSoundSur[i]->SetReadOnly(true);

        strGainName.Format(_T("CHGain%d"), i + 1);
        m_pEditCHGain[i] = static_cast<CEditUI*>(m_PaintManager.FindControl(strGainName));
        strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[i].fScanGain);
        m_pEditCHGain[i]->SetText(strText);

        strGainSoundName.Format(_T("CHGainSound%d"), i + 1);
        m_pEditCHGainSound[i] = static_cast<CEditUI*>(m_PaintManager.FindControl(strGainSoundName));
        strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[i].fScanTrUTGain);
        m_pEditCHGainSound[i]->SetText(strText);

        strCHName.Format(_T("CHName%d"), i + 1);
        m_pCHName[i] = static_cast<CLabelUI*>(m_PaintManager.FindControl(strCHName));
        m_pCHName[i]->SetBkColor(COLOR_TAB_CH[i + 1]);
    }

    // m_pLabelMesName[MESROW][MESCOL];
    for (int row = 0; row < MESROW; row++) {
        for (int col = 0; col < MESCOL; col++) {
            strText.Format(_T("LabelMesName%d%d"), (row + 1), (col + 1));
            m_pLabelMesName[row][col] = static_cast<CLabelUI*>(m_PaintManager.FindControl(strText));
        }
    }
    for (int i = 0; i < 4; i++) {
        strText.Format(_T("LabelWheelValue%d"), i + 1);
        m_pLablelWheelValue[i] = static_cast<CLabelUI*>(m_PaintManager.FindControl(strText));
    }

    m_pLabelScanRes = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("LabelScanRes")));
    m_pLabelScanRes->SetText(_T(""));
    m_pBtnWheelUp    = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnWheelUp")));
    m_pBtnWheelDown  = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnWheelDown")));
    m_pBtnDailyCheck = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnDailyCheck")));
    m_pBtnScanStart  = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnScanStart")));
    m_pBtnScanStop   = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnScanStop")));
    m_pBtnDefectPos  = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnDefectPos")));
    m_pBtnDefectScan = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnDefectScan")));
    m_pIDCANCEL      = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("IDCANCEL")));

    m_pBtnChangeSequencer = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnChangeSequencer")));
    m_pLabelData->SetTextColor(0xFF0000FF);
    m_pLabelTime->SetTextColor(0xFF0000FF);
    InitOpenGL();
    // 初始化
    std::thread Init(&ScanWnd::Init, this);
    Init.detach(); // 线程分离
    g_MainProcess.m_ConnectPLC.GetAllFloatValue();
    g_MainProcess.m_ConnectPLC.SetPLCAuto(true);
    UpdateBtnEnable();
}
void ScanWnd::UpdateLabelMes() {
    CString strText;

    m_pLablelWheelValue[0]->SetText(g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.szWheelModel);
    m_pLablelWheelValue[1]->SetText(g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.szWheelNumber);
    m_pLablelWheelValue[2]->SetText(g_MainProcess.m_User.mLogionUser.strName);
    strText.Format(_T("%.1f转每分钟"), g_MainProcess.m_ConnectPLC.mPLCSpeed.fRotateSpeed1);
    m_pLablelWheelValue[3]->SetText(strText);

    if (g_MainProcess.m_Techniques.m_ScanType != SCAN_SCAN) {
        g_MainProcess.m_ConnectPLC.GetAllFloatValue();
    }
    if (g_MainProcess.m_HDBridge.IsOpened()) {
        m_pLabelMesName[0][0]->SetText(_T("超声已连接！"));
    } else {
        m_pLabelMesName[0][0]->SetTextColor(0xFFFF0000);
        m_pLabelMesName[0][0]->SetText(_T("超声未连接！"));
    }

    if (g_MainProcess.m_ConnectPLC.isConnected()) {
        m_pLabelMesName[0][1]->SetText(_T("PLC卡已连接！"));
    } else {
        m_pLabelMesName[0][1]->SetTextColor(0xFFFF0000);
        m_pLabelMesName[0][1]->SetText(_T("PLC未已连接！"));
    }

    m_pLabelMesName[0][2]->SetText(g_MainProcess.m_Techniques.m_pCurTechName);
    ;
    // 工艺步进
    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep);
    m_pLabelMesName[2][1]->SetText(strText);
    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep);
    m_pLabelMesName[2][2]->SetText(strText);
    // 工艺设定
    strText.Format(_T("X:%.1f;Y:(%.1f-%0.1f)"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadXPos,
                   g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart, g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYEnd);
    m_pLabelMesName[3][1]->SetText(strText);
    strText.Format(_T("Y:%.1f;X:(%.1f-%0.1f)"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideYPos,
                   g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart, g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXEnd);
    m_pLabelMesName[3][2]->SetText(strText);

    // PLC当前状态
    strText.Format(_T("X:%.1f;Y:%.1f"), g_MainProcess.m_ConnectPLC.GetPLCTreadX(), g_MainProcess.m_ConnectPLC.GetPLCTreadY());
    m_pLabelMesName[4][1]->SetText(strText);
    strText.Format(_T("Y:%.1f;X:%.1f"), g_MainProcess.m_ConnectPLC.GetPLCSideY(), g_MainProcess.m_ConnectPLC.GetPLCSideX());
    m_pLabelMesName[4][2]->SetText(strText);

    //
    strText.Format(_T("PLC模式:%s"), g_MainProcess.m_ConnectPLC.GetPLCAuto() == 0 ? _T("手动") : _T("自动"));
    m_pLabelMesName[5][0]->SetText(strText);
    strText.Format(_T("卡盘角度:%.3f"), g_MainProcess.m_ConnectPLC.GetCoder());
    m_pLabelMesName[5][1]->SetText(strText);
    strText.Format(_T("检测圈数:%d / %d"), g_MainProcess.m_Techniques.m_iSideCirIndex,
                   g_MainProcess.m_ConnectPLC.m_PlcDownParam.nBestCirSize);
    m_pLabelMesName[5][2]->SetText(strText);
}
void ScanWnd::Init() {
    // 延迟最大化窗口
    Sleep(100);
    SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

    Sleep(100);
    //	g_MainProcess.m_Techniques.LoadTec();

    g_MainProcess.m_Techniques.Compute();
    m_OpenGL.AddScanModel(&g_MainProcess.m_Techniques);

    // 设置板卡参数
    g_MainProcess.m_HDBridge.SetCoder(&g_MainProcess.m_ConnectPLC);
    g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
    Sleep(100);
    g_MainProcess.m_HDBridge.StartSequencer(TRUE);
    for (int i = 0; i < 10; i++) {
        g_MainProcess.m_Techniques.GetDetectionStd()->CountDAC(
            &(g_MainProcess.m_Techniques), i, g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[i].fScanGain, 0);
    }

    SetTimer(TIMER_TIME, 500);
    SetTimer(TIMER_SCAN, 50);
}

void ScanWnd::OnBtnSpin(LPCTSTR lpName, float fValue) {
    CEditUI* pEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(lpName));
    if (pEdit == nullptr)
        return;

    CString BtnSpinGainSurName, BtnSpinGainSoundSurName; // 一个增益 一个透声
    int     nCurSpinGainSur = 0, nCurSpinGainSoundSur = 0;
    for (int i = 0; i < HD_CHANNEL_NUM; i++) {
        BtnSpinGainSurName.Format(_T("EditGainSur%d"), i + 1);
        if (!_tcsicmp(lpName, BtnSpinGainSurName)) {
            nCurSpinGainSur = i; // 获取当前第几个BtnSpin
            break;
        }

        BtnSpinGainSoundSurName.Format(_T("EditGainSoundSur%d"), i + 1);
        if (!_tcsicmp(lpName, BtnSpinGainSoundSurName)) {
            nCurSpinGainSoundSur = i; // 获取当前第几个BtnSpin
            break;
        }
    }

    CString strText;
    if (!_tcsicmp(lpName, BtnSpinGainSurName)) {
        g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[nCurSpinGainSur].fScanCompensate += fValue;
        g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
        strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[nCurSpinGainSur].fScanCompensate);
    } else if (!_tcsicmp(lpName, BtnSpinGainSoundSurName)) {
        g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[nCurSpinGainSoundSur].fScanTrUTCompensate += fValue;
        strText.Format(_T("%.1f"),
                       g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[nCurSpinGainSoundSur].fScanTrUTCompensate);
    }

    pEdit->SetText(strText);
}
void ScanWnd::OnStartDailyScan() {
    g_MainProcess.m_Techniques.m_bDailyScan = true;
    AutoScanInfoWnd autoScanInfoWnd;
    autoScanInfoWnd.Create(m_hWnd, _T("AutoScanInfoWnd"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
    autoScanInfoWnd.CenterWindow();
    int res = autoScanInfoWnd.ShowModal();
    if (res == IDOK) {
        SetScanReplay(false);
        m_pBtnChangeSequencer->SetEnabled(false);

        m_pBtnWheelUp->SetEnabled(false);
        m_pBtnWheelDown->SetEnabled(false);
        m_pBtnScanStart->SetEnabled(false);
        m_pBtnDailyCheck->SetEnabled(false);
        m_pBtnDailyCheck->SetText(_T("日常校验中..."));
        m_pIDCANCEL->SetEnabled(false);
        m_pBtnDefectPos->SetEnabled(false);
        m_pBtnDefectScan->SetEnabled(false);
        g_MainProcess.StartScan();
    }
}
// 开始扫差
void ScanWnd::OnStartScan() {
    g_MainProcess.m_Techniques.m_bDailyScan = false;
    m_pLabelScanRes->SetText(_T(""));
    AutoScanInfoWnd autoScanInfoWnd;
    autoScanInfoWnd.Create(m_hWnd, _T("AutoScanInfoWnd"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
    autoScanInfoWnd.CenterWindow();
    int res = autoScanInfoWnd.ShowModal();
    if (res == IDOK) {
        SetScanReplay(false);
        m_pBtnChangeSequencer->SetEnabled(false);

        m_pBtnWheelUp->SetEnabled(false);
        m_pBtnWheelDown->SetEnabled(false);
        m_pBtnScanStart->SetEnabled(false);
        m_pBtnDailyCheck->SetEnabled(false);
        m_pBtnScanStart->SetText(_T("检测中..."));
        m_pIDCANCEL->SetEnabled(false);
        m_pBtnDefectPos->SetEnabled(false);
        m_pBtnDefectScan->SetEnabled(false);
        g_MainProcess.StartScan();
    }
}
// 结束扫差
void ScanWnd::OnStopScan() {
    g_MainProcess.StopScan();
    m_pBtnWheelUp->SetEnabled(true);
    m_pBtnWheelDown->SetEnabled(true);
    m_pBtnScanStart->SetEnabled(true);
    m_pBtnDailyCheck->SetEnabled(true);
    m_pIDCANCEL->SetEnabled(true);
    m_pBtnScanStart->SetText(_T("开始检测"));
    m_pBtnDailyCheck->SetText(_T("日常校验"));
    m_pBtnDefectPos->SetEnabled(true);
    m_pBtnDefectScan->SetEnabled(true);
    g_MainProcess.m_Techniques.m_bDailyScan = false;
    m_pBtnChangeSequencer->SetEnabled(true);
    bool bOK = true;
    for (int i = 0; i < HD_CHANNEL_NUM; i++) {
        if (g_MainProcess.m_Techniques.m_pDefect[i].size() > 0) {
            bOK = false;
        }
    }

    if (!bOK) {
        m_pLabelScanRes->SetTextColor(0Xff0000);

        // m_pLabelScanRes->SetText(_T("不合格"));
    } else {
        m_pLabelScanRes->SetTextColor(0X0000ff);
        // m_pLabelScanRes->SetText(_T("合格"));
    }

    // ReViewScanWnd Mydlg;

    //	Mydlg.Create(m_hWnd, _T("ReViewScanWnd"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_FRAME);
    // Mydlg.Create(m_hWnd, _T("ReViewScanWnd"), UI_WNDSTYLE_FRAME, UI_WNDSTYLE_EX_FRAME);
    // Mydlg.CenterWindow();
    // Mydlg.ShowModal();
}

void ScanWnd::OnDefectList() {
    if (m_pDefectListWnd == NULL) {
        if (g_MainProcess.m_Techniques.m_ScanType == SCAN_REPLAY)
            g_MainProcess.m_Techniques.m_ScanType = SCAN_NORMAL;
        m_pDefectListWnd             = new DefectListWnd();
        m_pDefectListWnd->m_pScanWnd = this;
        m_pDefectListWnd->Create(m_hWnd, _T("ProbleWheelWnd"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        m_pDefectListWnd->CenterWindow();
        m_pDefectListWnd->ShowWindow(SW_NORMAL);
        g_MainProcess.m_Techniques.m_ScanType = SCAN_REPLAY;
        //	m_pDefectListWnd->ShowListData();
    } else {
        m_pDefectListWnd->ShowWindow(SW_NORMAL);
        m_pDefectListWnd->ShowListData();
        // m_pDefectListWnd->UpdateUI();
    }

    m_pBtnWheelUp->SetEnabled(false);
    m_pBtnWheelDown->SetEnabled(false);
    m_pBtnScanStart->SetEnabled(false);
    m_pBtnScanStop->SetEnabled(false);
    m_pBtnDailyCheck->SetEnabled(false);
    m_pIDCANCEL->SetEnabled(false);
    m_pBtnDefectPos->SetEnabled(false);
    m_pBtnDefectScan->SetEnabled(false);
}
void ScanWnd::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        CDuiString strName = msg.pSender->GetName();
        if (strName == _T("BtnSpin")) {
            // UserData里面保存Spin按键需要控制的按键名称与步进
            CDuiString strData = msg.pSender->GetUserData();

            TCHAR pName[64]{0};
            float fValue = 0;
            _stscanf_s(strData.GetData(), _T("%[^:]:%f"), pName, 64, &fValue);

            OnBtnSpin(pName, fValue);
        }

        else if (strName == _T("BtnDailyCheck")) {
            OnStartDailyScan();
        } else if (strName == _T("BtnScanStart")) {
            OnStartScan();
        } else if (strName == _T("BtnScanStop")) {
            if (g_MainProcess.m_ConnectPLC.isConnected()) { // PLC连接 停止时 急停
                g_MainProcess.m_ConnectPLC.SetPLCStop();
            }
            OnStopScan();
        } else if (strName == _T("BtnDefectPos")) {
            OnDefectList();
        } else if (strName == _T("BtnDefectScan")) {
        } else if (strName == _T("BtnChangeSequencer")) {
            //	g_MainProcess.m_Techniques.Test();

            if (g_MainProcess.m_Techniques.m_ScanType == SCAN_REPLAY)
                SetScanReplay(false);
            else if (g_MainProcess.m_Techniques.m_ScanType == SCAN_NORMAL)
                SetScanReplay(true);
        } else if (strName == _T("BtnWheelUp")) {
            OpenWheelUPDownWnd(0);
            UpdateBtnEnable();
        } else if (strName == _T("BtnWheelDown")) {
            // OpenWheelUPDownWnd(1);
            if (DMessageBox(_T("是否确认一键回原点?"), _T("原点操作！"), MB_YESNO) == IDYES) {
                g_MainProcess.m_ConnectPLC.SetAllZero(); //
            }
        }
    }

    CDuiWindowBase::Notify(msg);
}
void ScanWnd::OpenWheelUPDownWnd(int type) {
    WheelUpDownWnd wnd;
    wnd.m_enumType = (UPDOWN_TYPE)type;
    wnd.Create(m_hWnd, _T("WheelUpDownWnd"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
    wnd.CenterWindow();
    wnd.ShowModal();
}
void ScanWnd::OnTimer(int iIdEvent) {
    switch (iIdEvent) {
        case TIMER_TIME: // 500ms
        {
            CString    strText;
            SYSTEMTIME st;
            GetLocalTime(&st);

            strText.Format(_T("%4d-%02d-%02d "), st.wYear, st.wMonth, st.wDay);
            m_pLabelData->SetText(strText);
            strText.Format(_T("%02d:%02d:%02d"), st.wHour, st.wMinute, st.wSecond);
            m_pLabelTime->SetText(strText);
            UpdateLabelMes();

        } break;
        case TIMER_SCAN: // 100ms
        {
            if (g_MainProcess.m_Techniques.m_ScanType == SCAN_SCAN) { // 自动扫差状态下 判断停止

                if (g_MainProcess.m_Techniques.m_iTreadCirIndex >= 0 &&
                    g_MainProcess.m_Techniques.m_iTreadCirIndex >= g_MainProcess.m_Techniques.m_Scan.m_iSideSize) {
                    if (g_MainProcess.m_ConnectPLC.isConnected()) {
                        if (!g_MainProcess.m_ConnectPLC.GetPLCAutoState()) { // PLC连接状态停止需要判断是否在自动状态
                            OnStopScan();
                        }
                    } else {
                        OnStopScan();
                    }
                }
            }
        }

            bool bData = false;
            for (int i = 0; i < 2; i++) {
                m_iBuffer = (m_iBuffer + 1) % 2;
                if (WaitForSingleObject(g_MainProcess.m_Techniques.m_pDraw[m_iBuffer].hReady, 0) != WAIT_TIMEOUT) {
                    bData = true;
                    break;
                }
            }
            P_DRAW_BUFFER pDraw = &g_MainProcess.m_Techniques.m_pDraw[m_iBuffer];
            for (int i = 0; i < 6; i++) { // 侧面6个动态DAC
                Channel pChannel = g_MainProcess.m_Techniques.m_pChannel[i];
                g_MainProcess.m_Techniques.GetDetectionStd()->CountDAC(
                    &(g_MainProcess.m_Techniques), i, g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[i].fScanGain,
                    (float)pDraw->pGatePos[i][GATE_A] / 100.0f * (float)pChannel.m_iVelocity / 2000.0f + pChannel.m_fDelay);
            }
            break;
    }
}

void ScanWnd::UpdateBtnEnable() {
    if (g_MainProcess.m_ConnectPLC.GetPLCAuto()) {
        m_pBtnDailyCheck->SetEnabled(true);
        m_pBtnScanStart->SetEnabled(true);
        m_pBtnScanStop->SetEnabled(true);
        m_pBtnDefectPos->SetEnabled(true);
        m_pBtnDefectScan->SetEnabled(true);
    } else {
        m_pBtnWheelUp->SetEnabled(true);
        m_pBtnWheelDown->SetEnabled(true);
        m_pBtnDailyCheck->SetEnabled(false);
        m_pBtnScanStart->SetEnabled(false);
        m_pBtnScanStop->SetEnabled(false);
        m_pBtnDefectPos->SetEnabled(false);
        m_pBtnDefectScan->SetEnabled(false);
    }
}

LRESULT ScanWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg > WM_LBUTTONDOWN && uMsg < WM_MBUTTONDBLCLK) {
        // TRACE("%d-%d", WM_RBUTTONDOWN, uMsg);
    }
    switch (uMsg) {
        case WM_TIMER: {
            OnTimer((int)wParam);
        } break;
        case WM_LBUTTONDOWN: {
            if (g_MainProcess.m_Techniques.m_ScanType == SCAN_REPLAY) {
                OnLButtonDown(LOWORD(wParam), (::CPoint)lParam);
            }

        } break;
        case WM_LBUTTONUP: {
        } break;
        case WM_RBUTTONDOWN: {
            OnRButtonDown(LOWORD(wParam), (::CPoint)lParam);
        } break;
        case WM_RBUTTONUP: {
        } break;
        case WM_LBUTTONDBLCLK: {
            //	OnLButtonDClick(LOWORD(wParam), (::CPoint)lParam);
        } break;
        case WM_MOUSEMOVE: {
            //	OnMouseMove(LOWORD(wParam), (::CPoint)lParam);
        } break;
        case WM_MOUSEWHEEL: {
            ::CPoint pt;
            pt.x = (short)LOWORD(lParam);
            pt.y = (short)HIWORD(lParam);

            ScreenToClient(m_hWnd, &pt);
            OnMouseWheel(LOWORD(wParam), (short)HIWORD(wParam), pt);
        }
        case WM_KEYDOWN: // 其它按键响应
        {
            if (g_MainProcess.m_Techniques.m_ScanType == SCAN_REPLAY) {
                switch (wParam) {
                    case VK_UP: // 上一圈
                    {
                        int test = 0;
                        m_OpenGL.OnKeyUp();
                        //	UpdateAscanPoint();
                    } break;
                    case VK_DOWN: // 下一圈
                    {
                        m_OpenGL.OnKeyDown();
                        UpdateAscanPoint();
                    } break;
                    case VK_LEFT: // 前一个点
                    {
                        m_OpenGL.OnKeyLeft();
                        UpdateAscanPoint();
                    } break;

                    case VK_RIGHT: // 后一个点
                    {
                        m_OpenGL.OnKeyRight();
                        UpdateAscanPoint();

                    } break;
                }
            }
        }

        break;
        case WM_DEFECTLISTITEMSELECT: {
            int a    = (int)LOWORD(lParam);
            int b    = (int)LOWORD(lParam);
            int test = 0;
        } break;
    }

    return CDuiWindowBase::HandleMessage(uMsg, wParam, lParam);
}
void ScanWnd::OnLButtonDown(UINT nFlags, ::CPoint pt) {
    int x = pt.x;
    int y = pt.y;

    RECT rt;
    GetWindowRect(m_OpenGL.m_hWnd, &rt);
    ::CPoint ptGL;
    ::GetCursorPos(&ptGL);
    ptGL.x -= rt.left;
    ptGL.y -= rt.top;
    m_OpenGL.OnLButtonDown(nFlags, ptGL);

    UpdateAscanPoint();
}
void ScanWnd::OnRButtonDown(UINT nFlags, ::CPoint pt) {
    int x = pt.x;
    int y = pt.y;
    //
    m_OpenGL.OnRButtonDown(nFlags, pt);
}
void ScanWnd::SetScanReplay(bool bReplay) {
    if (bReplay) {
        g_MainProcess.m_Techniques.m_ScanType = SCAN_REPLAY;
        m_pBtnChangeSequencer->SetText(_T("实时采样"));
    } else {
        g_MainProcess.m_Techniques.m_ScanType = SCAN_NORMAL;
        m_pBtnChangeSequencer->SetText(_T("数据分析"));
    }
}

void ScanWnd::UpdateAscanPoint() {
    float fRecordindex = (m_OpenGL.m_fPt_Cir + m_OpenGL.m_fPt_Xscale) * g_MainProcess.m_Techniques.m_iSidePoints;
    int   nRecordindex = fRecordindex;
    //	nRecordindex=g_MainProcess.m_Techniques.m_Scan.m_pBscanIndex[(int)nRecordindex/10];
    printf("%d\n", nRecordindex);
    for (int iChannel = 0; iChannel < HD_CHANNEL_NUM; iChannel++) {
        if (nRecordindex < g_MainProcess.m_Techniques.m_pRecord.size()) {
            size_t iSize = g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pAscan[iChannel].size();
            if (iSize != 0) {
                if (iChannel < 6) {
                    g_MainProcess.m_Techniques.GetDetectionStd()->CountDAC(
                        &(g_MainProcess.m_Techniques), iChannel,
                        g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[iChannel].fScanGain,
                        (float)g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pGatePos[iChannel][GATE_A] / 100.0f *
                                (float)g_MainProcess.m_Techniques.m_pChannel[iChannel].m_iVelocity / 2000.0f +
                            g_MainProcess.m_Techniques.m_pChannel[iChannel].m_fDelay);
                }
                g_MainProcess.m_Techniques.m_pDraw[0].pData[iChannel].resize(iSize);
                g_MainProcess.m_Techniques.m_pDraw[1].pData[iChannel].resize(iSize);
                for (size_t i = 0; i < iSize; i++) {
                    g_MainProcess.m_Techniques.m_pDraw[0].pData[iChannel][i] =
                        float(g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pAscan[iChannel][i]) / MAX_AMP;
                    g_MainProcess.m_Techniques.m_pDraw[1].pData[iChannel][i] =
                        float(g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pAscan[iChannel][i]) / MAX_AMP;
                }
            }
        }
    }
}
