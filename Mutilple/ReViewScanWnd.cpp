#include "pch.h"

#include "AutoScanInfoWnd.h"
#include "DefectListWnd.h"
#include "DetectionStd_TBT2995_200.h"
#include "Mutilple.h"
#include "PLCWnd.h"
#include "ReViewScanWnd.h"
#include "SetWnd.h"
#include "WheelUpDownWnd.h"

// TODO: 暂时屏蔽警告
#pragma warning(disable :4267 4244 4552 4305 4101)

ReViewScanWnd::~ReViewScanWnd() {
    KillTimer(TIMER_TIME);
    CString strCfgFile;
    strCfgFile.Format(_T("%sLastParam.ini"), theApp.m_pExePath);
    g_MainProcess.m_Techniques.LoadTec(strCfgFile); // 退出时打开上次保存参数 防止篡改工艺

    g_MainProcess.m_Techniques.m_ScanType = SCAN_NORMAL;
    // 退出时，关闭采样
    // g_MainProcess.m_HDBridge.StartSequencer(FALSE);
    if (m_pDefectListWnd != NULL) {
        delete m_pDefectListWnd;
    }
    // 缺陷数据
    for (int i = 0; i < HD_CHANNEL_NUM; i++) {
        std::vector<DB_DEFECT_DATA*>::iterator it = g_MainProcess.m_Techniques.m_pDefect[i].begin();
        while (it != g_MainProcess.m_Techniques.m_pDefect[i].end()) {
            delete *it;
            *it = NULL;
            it++;
        }
        g_MainProcess.m_Techniques.m_pDefect[i].clear();
    }
}

void ReViewScanWnd::InitOpenGL() {
    m_pWndOpenGL = static_cast<CWindowUI*>(m_PaintManager.FindControl(_T("WndOpenGL")));

    // 初始化OpenGL窗口
    m_OpenGL.Create(m_hWnd);
    m_OpenGL.Attach(m_pWndOpenGL);
}

void ReViewScanWnd::InitWindow() {
    CDuiWindowBase::InitWindow();

    g_MainProcess.m_Techniques.m_ScanType = SCAN_REPLAY;
    m_pDefectListWnd                      = NULL;
    m_pLabelData                          = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("LabelData")));
    m_pLabelTime                          = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("LabelTime")));

    CString strText;

    m_pComboViewType  = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("ComboViewType")));
    m_pComboYearMonth = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("ComboYearMonth")));
    m_pComboDay       = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("ComboDay")));

    m_pListDetectFileList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("DetectFileList")));
    SYSTEMTIME stm;
    GetLocalTime(&stm);
    CString strFolderPathYM, strFolderPathD, strFolderPath;
    if (m_pComboViewType->GetCurSel() == 1) {
        strFolderPathYM.Format(_T("%sData\\Daily\\"), theApp.m_pExePath);
        strFolderPathD.Format(_T("%sData\\Daily\\%4d%02d\\"), theApp.m_pExePath, stm.wYear, stm.wMonth);
        strFolderPath.Format(_T("%sData\\Daily\\%4d%02d\\%02d"), theApp.m_pExePath, stm.wYear, stm.wMonth, stm.wDay);
    } else {
        strFolderPathYM.Format(_T("%sData\\scan\\"), theApp.m_pExePath);
        strFolderPathD.Format(_T("%sData\\Scan\\%4d%02d\\"), theApp.m_pExePath, stm.wYear, stm.wMonth);
        strFolderPath.Format(_T("%sData\\Scan\\%4d%02d\\%02d"), theApp.m_pExePath, stm.wYear, stm.wMonth, stm.wDay);
    }
    FindFolderFile_YM(strFolderPathYM);

    // FindFolderFile_D(strFolderPathD);

    // FindFolderFiles(strFolderPath);
    // ShowListData();
    //  m_pLabelMesName[MESROW][MESCOL];
    for (int row = 0; row < MESROW_RW; row++) {
        for (int col = 0; col < MESCOL_RW; col++) {
            strText.Format(_T("LabelMesName%d%d"), (row + 1), (col + 1));
            m_pLabelMesName[row][col] = static_cast<CLabelUI*>(m_PaintManager.FindControl(strText));
        }
    }

    m_pLabelScanRes         = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("LabelScanRes")));
    m_pBtnDefectList        = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnDefectList")));
    m_pBtnDeletSelect       = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnDeletSelect")));
    m_pBtnDeleteSelectMonth = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BBtnDeleteSelectMonth")));
    m_pBtnDeleteAll         = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnDeleteAll")));
    m_pBtnReport            = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnReport")));

    m_pIDCANCEL = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("IDCANCEL")));

    m_pLabelData->SetTextColor(0xFF0000FF);
    m_pLabelTime->SetTextColor(0xFF0000FF);
    m_pLabelScanRes->SetText(_T(""));
    InitOpenGL();
    // 初始化
    std::thread Init(&ReViewScanWnd::Init, this);
    Init.detach(); // 线程分离
}
void ReViewScanWnd::UpdateLabelMes() {
    CString strText;

    m_pLabelMesName[0][1]->SetText(strText);
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

    //

    m_pLabelMesName[4][0]->SetText(strText);
    strText.Format(_T("X:%.1f;Y:%.1f"), g_MainProcess.m_ConnectPLC.GetPLCTreadX(),
                   g_MainProcess.m_ConnectPLC.GetPLCTreadY());
    m_pLabelMesName[4][1]->SetText(strText);
    strText.Format(_T("Y:%.1f;X:%.1f"), g_MainProcess.m_ConnectPLC.GetPLCSideY(),
                   g_MainProcess.m_ConnectPLC.GetPLCSideX());
    m_pLabelMesName[4][2]->SetText(strText);

    //
    //	strText.Format(_T("PLC模式:%s"), g_MainProcess.m_ConnectPLC.GetPLCAuto()==0?_T("手动"): _T("自动"));
    if (m_OpenGL.m_nDefectCh > 0 && m_OpenGL.m_nDefectCh < 11) {
        if (m_OpenGL.m_nDefectCh <= 6) {
            strText.Format(_T("缺陷通道：踏面%d"), m_OpenGL.m_nDefectCh);

        } else {
            strText.Format(_T("缺陷通道：侧面%d"), m_OpenGL.m_nDefectCh);
        }
        m_pLabelMesName[5][0]->SetBkColor(COLOR_TAB_CH[m_OpenGL.m_nDefectCh]);
    }

    else {
        strText.Format(_T("缺陷通道：无缺陷"));
        m_pLabelMesName[5][0]->SetBkColor(0xfffffff);
    }
    m_pLabelMesName[5][0]->SetText(strText);
    strText.Format(_T("点击角度:%.3f"), m_OpenGL.m_fPt_Xscale * g_MainProcess.m_Techniques.m_iSidePoints);
    m_pLabelMesName[5][1]->SetText(strText);
    strText.Format(_T("点击圈数:%d / %d"), m_OpenGL.m_fPt_Cir, g_MainProcess.m_ConnectPLC.m_PlcDownParam.nBestCirSize);
    m_pLabelMesName[5][2]->SetText(strText);
}
void ReViewScanWnd::Init() {
    // 延迟最大化窗口
    Sleep(100);
    SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

    Sleep(100);
    g_MainProcess.m_Techniques.Compute();

    m_OpenGL.AddScanModel(&g_MainProcess.m_Techniques);

    // 设置板卡参数

    // g_MainProcess.m_HDBridge.SetCoder(&g_MainProcess.m_ConnectPLC);
    // g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
    // Sleep(100);
    // g_MainProcess.m_HDBridge.StartSequencer(TRUE);
    // for (int i = 0; i < 10;i++) {
    //	g_MainProcess.m_Techniques.GetDetectionStd()->CountDAC(&(g_MainProcess.m_Techniques), i, g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[i].fScanGain);
    // }
    for (int i = 0; i < 10; i++) {
        //	g_MainProcess.m_Techniques.GetDetectionStd()->CountDAC(&(g_MainProcess.m_Techniques), i, g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[i].fScanGain);
    }
    SetTimer(TIMER_TIME, 500);
}

void ReViewScanWnd::OnDefectList() {
    if (m_pDefectListWnd == NULL) {
        m_pDefectListWnd                   = new DefectListWnd();
        m_pDefectListWnd->m_pReViewScanWnd = this;
        m_pDefectListWnd->Create(m_hWnd, _T("ProbleWheelWnd"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        m_pDefectListWnd->CenterWindow();
        m_pDefectListWnd->ShowWindow(SW_NORMAL);
        //	m_pDefectListWnd->ShowListData();
    } else {
        m_pDefectListWnd->ShowWindow(SW_NORMAL);
        m_pDefectListWnd->ShowListData();
        // m_pDefectListWnd->UpdateUI();
    }
}
void ReViewScanWnd::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        CDuiString strName = msg.pSender->GetName();
        if (strName == _T("BtnSpin")) {
            // UserData里面保存Spin按键需要控制的按键名称与步进
            CDuiString strData = msg.pSender->GetUserData();

            TCHAR pName[64]{0};
            float fValue = 0;
            _stscanf_s(strData.GetData(), _T("%[^:]:%f"), pName, 64, &fValue);

            //	OnBtnSpin(pName, fValue);
        }

        else if (strName == _T("BtnDefectList")) {
            OnDefectList();
        }

        else if (strName == _T("BtnTest")) {
            // g_MainProcess.m_Techniques.Test();

        } else if (strName == _T("BtnDeletSelect")) {
            CString strFile = _T("确认删除选中文件?");

            if (DMessageBox(strFile, _T("文件删除"), MB_YESNO) == IDYES) {
                OnDeletReport(0);
            }

        } else if (strName == _T("BtnDeleteSelectMonth")) {
            // OpenWheelUPDownWnd(1);
            OnDeletReport(1);
        } else if (strName == _T("BtnDeleteAll")) {
            OnDeletReport(2);

        } else if (strName == _T("BtnReport")) {
            OnReport();
        }
    } else if (msg.sType == DUI_MSGTYPE_ITEMSELECT) {
        CDuiString strName = msg.pSender->GetName();

        if (strName == _T("ComboViewType")) {
            INT iIndex = m_pComboViewType->GetCurSel();
            if (iIndex >= 0) {
                CDuiString strName = m_pComboViewType->GetText();
                CString    strFolderPathYM;
                if (m_pComboViewType->GetCurSel() == 1) {
                    strFolderPathYM.Format(_T("%sData\\Daily\\"), theApp.m_pExePath);
                } else {
                    strFolderPathYM.Format(_T("%sData\\scan\\"), theApp.m_pExePath);
                }
                FindFolderFile_YM(strFolderPathYM);
            }

        } else if (strName == _T("ComboYearMonth")) {
            INT iIndex = m_pComboYearMonth->GetCurSel();

            if (iIndex >= 0) {
                CDuiString strName = m_pComboYearMonth->GetText();
                CString    strFolderPathD;
                if (m_pComboViewType->GetCurSel() == 1) {
                    strFolderPathD.Format(_T("%sData\\Daily\\%s\\"), theApp.m_pExePath, strName.GetData());
                } else {
                    strFolderPathD.Format(_T("%sData\\Scan\\%s\\"), theApp.m_pExePath, strName.GetData());
                }
                FindFolderFile_D(strFolderPathD);
            }
        } else if (strName == _T("ComboDay")) {
            INT iIndex = m_pComboDay->GetCurSel();

            if (iIndex >= 0) {
                CDuiString strNameYM = m_pComboYearMonth->GetText();
                CDuiString strName   = m_pComboDay->GetText();
                CString    strFolderPath;
                if (m_pComboViewType->GetCurSel() == 1) {
                    strFolderPath.Format(_T("%sData\\Daily\\%s\\%s"), theApp.m_pExePath, strNameYM.GetData(), strName.GetData());
                } else {
                    strFolderPath.Format(_T("%sData\\Scan\\%s\\%s"), theApp.m_pExePath, strNameYM.GetData(), strName.GetData());
                }

                FindFolderFiles(strFolderPath);
                ShowListData();
            }
        } else if (strName == _T("DetectFileList")) {
            // OnSelectelistItem();
            OnSelectelistItem_LB();
        }
    } else if (msg.sType == DUI_MSGTYPE_ITEMACTIVATE) {
        CDuiString strName = msg.pSender->GetParent()->GetName();
        // if (strName == _T("DetectFileList"))
        {
            OnSelectelistItem_DB();
        }
    }

    CDuiWindowBase::Notify(msg);
}
void ReViewScanWnd::OpenWheelUPDownWnd(int type) {
    WheelUpDownWnd wnd;
    wnd.m_enumType = (UPDOWN_TYPE)type;
    wnd.Create(m_hWnd, _T("WheelUpDownWnd"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
    wnd.CenterWindow();
    wnd.ShowModal();
}
void ReViewScanWnd::OnTimer(int iIdEvent) {
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
    }
}

LRESULT ReViewScanWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg > WM_LBUTTONDOWN && uMsg < WM_MBUTTONDBLCLK) {
        // TRACE("%d-%d", WM_RBUTTONDOWN, uMsg);
    }
    switch (uMsg) {
        case WM_TIMER: {
            OnTimer((int)wParam);
        } break;
        case WM_LBUTTONDOWN: {
            OnLButtonDown(LOWORD(wParam), (::CPoint)lParam);
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
            /*
            * #define VK_LEFT           0x25
    #define VK_UP             0x26
    #define VK_RIGHT          0x27
    #define VK_DOWN           0x28
            */
            switch (wParam) {
                case VK_UP: // 上一圈
                {
                    int test = 0;
                    m_OpenGL.OnKeyUp();
                    UpdateAscanPoint();
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

        break;
        case WM_DEFECTLISTITEMSELECT: {
            int a    = (int)LOWORD(lParam);
            int b    = (int)LOWORD(lParam);
            int test = 0;
        } break;
    }

    return CDuiWindowBase::HandleMessage(uMsg, wParam, lParam);
}

void ReViewScanWnd::OnLButtonDown(UINT nFlags, ::CPoint pt) {
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

void ReViewScanWnd::OnRButtonDown(UINT nFlags, ::CPoint pt) {
    int x = pt.x;
    int y = pt.y;
    //
    m_OpenGL.OnRButtonDown(nFlags, pt);
}

void ReViewScanWnd::UpdateAscanPoint() {
    float fRecordindex = (m_OpenGL.m_fPt_Cir + m_OpenGL.m_fPt_Xscale) * g_MainProcess.m_Techniques.m_iSidePoints;
    int   nRecordindex = fRecordindex;
    //	nRecordindex=g_MainProcess.m_Techniques.m_Scan.m_pBscanIndex[(int)nRecordindex/10];
    printf("%d\n", nRecordindex);
    for (int iChannel = 0; iChannel < HD_CHANNEL_NUM; iChannel++) {
        if (nRecordindex < g_MainProcess.m_Techniques.m_pRecord.size()) {
            size_t iSize = g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pAscan[iChannel].size();
            if (iSize != 0) {
                if (iChannel < 6) {
                    g_MainProcess.m_Techniques.GetDetectionStd()->CountDAC(&(g_MainProcess.m_Techniques), iChannel, g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[iChannel].fScanGain,
                                                                           (float)g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pGatePos[iChannel][GATE_A] / 100.0f * (float)g_MainProcess.m_Techniques.m_pChannel[iChannel].m_iVelocity / 2000.0f + g_MainProcess.m_Techniques.m_pChannel[iChannel].m_fDelay);
                }
                g_MainProcess.m_Techniques.m_pDraw[0].pData[iChannel].resize(iSize);
                g_MainProcess.m_Techniques.m_pDraw[1].pData[iChannel].resize(iSize);
                for (size_t i = 0; i < iSize; i++) {
                    g_MainProcess.m_Techniques.m_pDraw[0].pData[iChannel][i] = float(g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pAscan[iChannel][i]) / MAX_AMP;
                    g_MainProcess.m_Techniques.m_pDraw[1].pData[iChannel][i] = float(g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pAscan[iChannel][i]) / MAX_AMP;
                }

                g_MainProcess.m_Techniques.m_pDraw[0].pGatePos[iChannel][0] = g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pGatePos[iChannel][0];
                g_MainProcess.m_Techniques.m_pDraw[0].pGatePos[iChannel][1] = g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pGatePos[iChannel][1];
                // 波门波幅
                g_MainProcess.m_Techniques.m_pDraw[0].pGateAmp[iChannel][0] = g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pGateAmp[iChannel][0];
                g_MainProcess.m_Techniques.m_pDraw[0].pGateAmp[iChannel][1] = g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pGateAmp[iChannel][1];

                g_MainProcess.m_Techniques.m_pDraw[1].pGatePos[iChannel][0] = g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pGatePos[iChannel][0];
                g_MainProcess.m_Techniques.m_pDraw[1].pGatePos[iChannel][1] = g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pGatePos[iChannel][1];
                // 波门波幅
                g_MainProcess.m_Techniques.m_pDraw[1].pGateAmp[iChannel][0] = g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pGateAmp[iChannel][0];
                g_MainProcess.m_Techniques.m_pDraw[1].pGateAmp[iChannel][1] = g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pGateAmp[iChannel][1];
            }
        }
    }
}

void ReViewScanWnd::FindFolderFile_YM(CString strFoldername) {
    if (m_pListDetectFileList != NULL) {
        m_pListDetectFileList->RemoveAll();
    }
    std::vector<CString> m_MonthList;
    CString              strDir = strFoldername;
    strDir += "\\*.*";

    // 遍历得到所有子文件夹名
    CFileFind finder;
    BOOL      bWorking = finder.FindFile(strDir);

    while (bWorking) {
        bWorking = finder.FindNextFile();
        if (finder.IsDirectory() && "." != finder.GetFileName() && ".." != finder.GetFileName()) // 注意该句需要排除“.”“..”
        {
            CString strTmp  = finder.GetFilePath();
            CString strname = finder.GetFileName();
            m_MonthList.push_back(strname);
            int test = 0;
            // 递归调用
            // if (finder.GetFileName() != L"源PE文件")
            //	FindFileInDir(finder.GetFilePath());
        }
    }
    finder.Close();

    m_pComboYearMonth->RemoveAll();
    for (int i = 0; i < m_MonthList.size(); i++) {
        CListLabelElementUI* pEle = NULL;
        pEle                      = new CListLabelElementUI;
        pEle->SetText(m_MonthList[i]);
        pEle->SetFixedHeight(35);
        pEle->SetTag(0);
        m_pComboYearMonth->Add(pEle);
    }
    m_pComboDay->RemoveAll();
    m_pComboYearMonth->SelectItem(m_MonthList.size() - 1);
    return;
}

void ReViewScanWnd::FindFolderFile_D(CString strFoldername) {
    if (m_pListDetectFileList != NULL) {
        m_pListDetectFileList->RemoveAll();
    }
    std::vector<CString> m_DayList;
    CString              strDir = strFoldername;
    strDir += "\\*.*";

    // 遍历得到所有子文件夹名
    CFileFind finder;
    BOOL      bWorking = finder.FindFile(strDir);

    while (bWorking) {
        bWorking = finder.FindNextFile();
        if (finder.IsDirectory() && "." != finder.GetFileName() && ".." != finder.GetFileName()) // 注意该句需要排除“.”“..”
        {
            CString strTmp  = finder.GetFilePath();
            CString strname = finder.GetFileName();
            m_DayList.push_back(strname);
            int test = 0;
            // 递归调用
            // if (finder.GetFileName() != L"源PE文件")
            //	FindFileInDir(finder.GetFilePath());
        }
    }
    finder.Close();

    m_pComboDay->RemoveAll();
    for (int i = 0; i < m_DayList.size(); i++) {
        CListLabelElementUI* pEle = NULL;
        pEle                      = new CListLabelElementUI;
        pEle->SetText(m_DayList[i]);
        pEle->SetFixedHeight(35);
        pEle->SetTag(0);
        m_pComboDay->Add(pEle);
    }
    m_pComboDay->SelectItem(m_DayList.size() - 1);
    return;
}
void ReViewScanWnd::FindFolderFiles(CString strFoldername) {
    CString strDir = strFoldername;
    strDir += "\\*.*";

    // 遍历得到所有子文件夹名
    CFileFind finder;
    BOOL      bWorking = finder.FindFile(strDir);
    if (m_pListDetectFileList != NULL) {
        m_DetectFileList.clear();
    }
    while (bWorking) {
        bWorking = finder.FindNextFile();
        if (finder.IsDirectory() && "." != finder.GetFileName() && ".." != finder.GetFileName()) // 注意该句需要排除“.”“..”
        {
            CString strTmp  = finder.GetFilePath();
            CString strname = finder.GetFileName();
            //	m_DayList.push_back(strname);
            int test = 0;
            // 递归调用
            // if (finder.GetFileName() != L"源PE文件")
            //	FindFileInDir(finder.GetFilePath());
        } else {
            if ("." != finder.GetFileName() && ".." != finder.GetFileName()) {
                CString strTmp  = finder.GetFilePath();
                CString strname = finder.GetFileName();
                m_DetectFileList.push_back(strname);
            }
        }
    }
    finder.Close();
}
void ReViewScanWnd::UpdateList() {
    // m_IDEFECTList.clear();
    // for (int i = 0; i < 10; i++) {
    //	DB_DEFECT_DATA data;
    //	data.nIndex = i;
    //	data.nCH = i % 10;
    //	m_IDEFECTList.push_back(data);
    // }
}
void ReViewScanWnd::ShowListData() {
    // UpdateTechList();
    CListContainerElementUI* pElement = NULL;

    if (m_pListDetectFileList != NULL) {
        m_pListDetectFileList->RemoveAll();

        for (int i = m_DetectFileList.size() - 1; i >= 0; i--) {
            pElement = new CListContainerElementUI();
            pElement->SetFixedHeight(28);
            m_pListDetectFileList->AddAt(pElement, 0);

            CString strXml;
            strXml.Format(_T("Theme\\DetectLineItemFile.xml"));
            CDialogBuilder       builder;
            CHorizontalLayoutUI* pLayout = static_cast<CHorizontalLayoutUI*>(builder.Create(strXml.GetBuffer(0), 0, NULL, &m_PaintManager));
            pElement->Add(pLayout);

            CLabelUI* pLabel1 = static_cast<CLabelUI*>(pElement->FindSubControl(_T("LabelFileName")));
            CLabelUI* pLabel2 = static_cast<CLabelUI*>(pElement->FindSubControl(_T("LabelFileInfo")));

            CString strText;
            //	strText.Format(_T("%d"), 10-i);
            pLabel1->SetText(m_DetectFileList[i]);

            pLabel2->SetText(strText);
        }
    }
}
void ReViewScanWnd::OnSelectelistItem_LB() {
    if (m_pDefectListWnd)
        m_pDefectListWnd->ShowWindow(SW_HIDE);
    CString    strFilePath;
    CDuiString strNameYM = m_pComboYearMonth->GetText();
    CDuiString strName   = m_pComboDay->GetText();
    if (m_pComboViewType->GetCurSel() == 1) {
        strFilePath.Format(_T("%sData\\Daily\\%s\\%s\\"), theApp.m_pExePath, strNameYM.GetData(), strName.GetData());
    } else {
        strFilePath.Format(_T("%sData\\Scan\\%s\\%s\\"), theApp.m_pExePath, strNameYM.GetData(), strName.GetData());
    }

    INT                      iIndex   = m_pListDetectFileList->GetCurSel();
    int                      iCount   = m_pListDetectFileList->GetCount();
    CListContainerElementUI* pElement = (CListContainerElementUI*)(m_pListDetectFileList->GetItemAt(iIndex));

    CLabelUI* pLabelFileName = static_cast<CLabelUI*>(pElement->FindSubControl(_T("LabelFileName"))); // 选中通道
    CString   strIFileName   = pLabelFileName->GetText();
    strFilePath += strIFileName;
    m_strSelectFileName = strFilePath;
    // g_MainProcess.m_Techniques.Compute();

    //	CString strIndex = pLabelCHIndex->GetText();
    //_stscanf_s(strIndex, _T("%d"), &nValue);
}
void ReViewScanWnd::OnSelectelistItem_DB() {
    if (m_pDefectListWnd)
        m_pDefectListWnd->ShowWindow(SW_HIDE);
    CString    strFilePath;
    CDuiString strNameYM = m_pComboYearMonth->GetText();
    CDuiString strName   = m_pComboDay->GetText();
    if (m_pComboViewType->GetCurSel() == 1) {
        strFilePath.Format(_T("%sData\\Daily\\%s\\%s\\"), theApp.m_pExePath, strNameYM.GetData(), strName.GetData());
    } else {
        strFilePath.Format(_T("%sData\\Scan\\%s\\%s\\"), theApp.m_pExePath, strNameYM.GetData(), strName.GetData());
    }

    INT                      iIndex   = m_pListDetectFileList->GetCurSel();
    int                      iCount   = m_pListDetectFileList->GetCount();
    CListContainerElementUI* pElement = (CListContainerElementUI*)(m_pListDetectFileList->GetItemAt(iIndex));

    CLabelUI* pLabelFileName = static_cast<CLabelUI*>(pElement->FindSubControl(_T("LabelFileName"))); // 选中通道
    CString   strIFileName   = pLabelFileName->GetText();
    strFilePath += strIFileName;
    m_strSelectFileName = strFilePath;
    // g_MainProcess.m_Techniques.Compute();

    g_MainProcess.m_Techniques.LoadRecData(strFilePath); // 读取数据
    // m_OpenGL.AddScanModel(&g_MainProcess.m_Techniques);  //初始化GDI

    for (int i = 0; i < 10; i++) {
        g_MainProcess.m_Techniques.GetDetectionStd()->CountDAC(&(g_MainProcess.m_Techniques), i, g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[i].fScanGain, 0);
    }

    int nSidePoint = g_MainProcess.m_Techniques.m_Scan.GetSidePoint(); // 每圈绘图点数
    int nSideSize  = g_MainProcess.m_Techniques.m_Scan.m_iSideSize;
    for (size_t i = 0; i < g_MainProcess.m_Techniques.m_pRecord.size(); i++) // 读取回放数据
    {
        g_MainProcess.m_Techniques.m_iTreadPointIndex = 1.0f * i * (nSidePoint * 1.0 / (g_MainProcess.m_Techniques.m_pRecord.size() / nSideSize)); //
        g_MainProcess.m_Techniques.m_iSidePointIndex  = g_MainProcess.m_Techniques.m_iTreadPointIndex;
        // printf("%d_", g_MainProcess.m_Techniques.m_iTreadPointIndex);
        if (g_MainProcess.m_Techniques.m_iTreadPointIndex % 360 == 0) {
            // printf("%\n");
        }
        g_MainProcess.m_Techniques.GetDetectionStd()->ReViewDetection(&g_MainProcess.m_Techniques, (g_MainProcess.m_Techniques.m_pRecord[i]), i);
    }

    // g_MainProcess.m_Techniques->m_pDraw[0].pData[0].data()

    SetEvent(g_MainProcess.m_Techniques.m_pDraw[0].hReady);
    OnDefectList();
    int test = 0;

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
        //	m_pLabelScanRes->SetText(_T("合格"));
    }
    //	CString strIndex = pLabelCHIndex->GetText();
    //_stscanf_s(strIndex, _T("%d"), &nValue);
}

void ReViewScanWnd::OnDeletReport(int type) {
    if (type == 0) { // 删除当前

        BOOL bRes = DeleteFile(m_strSelectFileName);
        if (!bRes) {
            DMessageBox(_T("删除失败"), _T("文件删除！"));
            return;
        }
        DMessageBox(_T("删除成功！"), _T("文件删除！"));

        CDuiString strNameYM = m_pComboYearMonth->GetText();
        CDuiString strName   = m_pComboDay->GetText();
        CString    strFolderPath;
        if (m_pComboViewType->GetCurSel() == 1) {
            strFolderPath.Format(_T("%sData\\Daily\\%s\\%s"), theApp.m_pExePath, strNameYM.GetData(), strName.GetData());
        } else {
            strFolderPath.Format(_T("%sData\\Scan\\%s\\%s"), theApp.m_pExePath, strNameYM.GetData(), strName.GetData());
        }
        FindFolderFiles(strFolderPath);
        ShowListData();
    } else if (type == 1) // 删除选中月
    {
        CString    strFilePath;
        CDuiString strNameYM = m_pComboYearMonth->GetText();
        if (m_pComboViewType->GetCurSel() == 1) {
            strFilePath.Format(_T("%sData\\Daily\\%s\\"), theApp.m_pExePath, strNameYM.GetData());
        } else {
            strFilePath.Format(_T("%sData\\Scan\\%s\\"), theApp.m_pExePath, strNameYM.GetData());
        }
        bool    res = IsDirExist(strFilePath);
        CString strFile;
        strFile.Format(_T("确认删除%s所有数据文件"), strNameYM.GetData());
        if (DMessageBox(strFile, _T("文件删除"), MB_YESNO) == IDYES) {
            DeleteDir(strFilePath);
            res = IsDirExist(strFilePath);
            if (!res) {
                DMessageBox(_T("删除成功！"), _T("文件删除！"));
            }
        } else {
            return;
        }
        CString strFolderPathYM;
        if (m_pComboViewType->GetCurSel() == 1) {
            strFolderPathYM.Format(_T("%sData\\Daily\\"), theApp.m_pExePath);
        } else {
            strFolderPathYM.Format(_T("%sData\\scan\\"), theApp.m_pExePath);
        }
        FindFolderFile_YM(strFolderPathYM);
    } else if (type == 2) // 全部删除
    {
        CString strFilePath;

        if (m_pComboViewType->GetCurSel() == 1) {
            strFilePath.Format(_T("%sData\\Daily\\"), theApp.m_pExePath);
        } else {
            strFilePath.Format(_T("%sData\\Scan\\"), theApp.m_pExePath);
        }
        bool    res = IsDirExist(strFilePath);
        CString strFile;

        if (DMessageBox(_T("确认删除所有数据文件"), _T("文件删除"), MB_YESNO) == IDYES) {
            DeleteDir(strFilePath);
            res = IsDirExist(strFilePath);
            if (!res) {
                DMessageBox(_T("删除成功！"), _T("文件删除！"));
            }
        } else {
            return;
        }

        CString strFolderPathYM;
        if (m_pComboViewType->GetCurSel() == 1) {
            strFolderPathYM.Format(_T("%sData\\Daily\\"), theApp.m_pExePath);
        } else {
            strFolderPathYM.Format(_T("%sData\\scan\\"), theApp.m_pExePath);
        }
        FindFolderFile_YM(strFolderPathYM);
    }
    // 更新列表
}

void ReViewScanWnd::DeleteDir(CString str) {
    CFileFind finder;                // 文件查找类
    CString   strdel, strdir;        // strdir:要删除的目录，strdel：要删除的文件
    strdir        = str + _T("\\*"); // 删除文件夹，先要清空文件夹,加上路径,注意加"\\"
    BOOL b_finded = (BOOL)finder.FindFile(strdir);
    while (b_finded) {
        b_finded = (BOOL)finder.FindNextFile();
        if (finder.IsDots())
            continue;                  // 找到的是当前目录或上级目录则跳过
        strdel = finder.GetFileName(); // 获取找到的文件名
        if (finder.IsDirectory())      // 如果是文件夹
        {
            strdel = str + "\\" + strdel; // 加上路径,注意加"\\"
            DeleteDir(strdel);            // 递归删除文件夹
        } else                            // 不是文件夹
        {
            strdel = str + "\\" + strdel;
            if (finder.IsReadOnly()) // 清除只读属性
            {
                SetFileAttributes(strdel, GetFileAttributes(strdel) & (~FILE_ATTRIBUTE_READONLY));
            }
            DeleteFile(strdel); // 删除文件(API)
        }
    }
    finder.Close();
    RemoveDirectory(str); // 删除文件夹(API)
}
BOOL ReViewScanWnd::IsDirExist(const CString csDir) {
    DWORD dwAttrib = GetFileAttributes(csDir);
    return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 != (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}
void ReViewScanWnd::OnReport() {
    TCHAR path[MAX_PATH];
    ZeroMemory(path, MAX_PATH);
    GetModuleFileName(NULL, path, MAX_PATH);
    CString strPath = path;
    int     pos     = strPath.ReverseFind('\\');
    strPath         = strPath.Left(pos);
    CString strDir  = strPath + L"\\DataReport";
    strPath += _T("\\DataReport\\DataReport.exe");
    ShellExecute(NULL, _T("open"), strPath, NULL, strDir, SW_SHOWNORMAL);
}
