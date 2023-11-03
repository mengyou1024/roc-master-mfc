#include "pch.h"

#include "AutoDefectPosInfoWnd.h"
#include "DefectListWnd.h"
#include "DetectionStd_TBT2995_200.h"
#include "Mutilple.h"
#include "ReViewScanWnd.h"
#include "RuitiePLC.h"
#include "ScanWnd.h"

const int TIME_PLCSTATE = 120;

DefectListWnd::DefectListWnd(void) {
}

DefectListWnd::~DefectListWnd(void) {
}

void DefectListWnd::InitWindow() {
    CDuiWindowBase::InitWindow();

    m_pListDefectFileList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("DefectFileList")));
    m_pLabel11            = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("Label11")));

    m_pBtnDefectReport  = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnDefectReport")));
    m_pBtnDefectPos     = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnDefectPos")));
    m_pBtnDefectFinish  = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnDefectFinish")));
    m_pBtnDefectPrePare = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnDefectPrePare")));
    if (g_MainProcess.m_Techniques.m_ScanType == SCAN_REPLAY) {
        m_pBtnDefectReport->SetVisible(false);
        m_pBtnDefectPos->SetVisible(false);
        m_pBtnDefectFinish->SetVisible(false);
        m_pBtnDefectPrePare->SetVisible(false);

    } else {
        m_pBtnDefectReport->SetVisible(false);
        m_pBtnDefectPos->SetVisible(true);
        m_pBtnDefectFinish->SetVisible(true);
        m_pBtnDefectPrePare->SetVisible(true);
        // m_pBtnDefectPos->SetEnabled(false);
        //  m_pBtnDefectPos->SetEnabled(false);
        // 设置开始追踪状态
        // 缺陷开始前发就位数据
    }
    ShowListData();
}
void DefectListWnd::InitDefectPos() { // 缺陷定位准备
    g_MainProcess.m_ConnectPLC.SetPLCAuto(true);
    g_MainProcess.m_ConnectPLC.SetPLC_Speed();
    g_MainProcess.m_ConnectPLC.SetPLC_TreadMove(
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadLyXPos,
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadXPos,
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart,
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYEnd,
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep);

    g_MainProcess.m_ConnectPLC.SetPLC_SideMove(
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart,
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXEnd,
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideYPos,
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep);
    RuitiePLC::setVariable("M64", true); // 开始追踪命令
    SetTimer(TIME_PLCSTATE, 200);
    m_lStart = GetTickCount();
    m_pBtnDefectPos->SetEnabled(false);
    m_pBtnDefectFinish->SetEnabled(false);
}
void DefectListWnd::OnTimer(int iIdEvent) {
    switch (iIdEvent) {
        case TIME_PLCSTATE: {
            int  res   = g_MainProcess.m_ConnectPLC.GetDetectState(); // 运行状态
            long lEnd  = GetTickCount();                              // 程序段结束后取得系统运行时间(ms)
            int  nTime = (lEnd - m_lStart) / 1000;
            if (res == 1) { // 缺陷移动就位 可以定位
                m_pBtnDefectPos->SetEnabled(true);
                m_pBtnDefectFinish->SetEnabled(true);
                KillTimer(TIME_PLCSTATE);
            }
            if (res == -1) {
                // KillTimer(TIME_PLCSTATE);
            } else {
                if (nTime > 10) {
                    //	KillTimer(TIME_PLCSTATE);
                }
            }
        } break;
    }
}
void DefectListWnd::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        CDuiString strName = msg.pSender->GetName();

        if (strName == _T("IDOK")) {
            Close(IDOK);
        } else if (strName == _T("IDCANCEL1")) // 退出
        {
            ShowWindow(SW_HIDE);
            if (m_pScanWnd) {
                m_pScanWnd->m_pBtnWheelUp->SetEnabled(true);
                m_pScanWnd->m_pBtnWheelDown->SetEnabled(true);
                m_pScanWnd->m_pBtnScanStart->SetEnabled(true);
                m_pScanWnd->m_pBtnScanStop->SetEnabled(true);
                m_pScanWnd->m_pBtnDailyCheck->SetEnabled(true);
                m_pScanWnd->m_pIDCANCEL->SetEnabled(true);
                m_pScanWnd->m_pBtnDefectPos->SetEnabled(true);
                m_pScanWnd->m_pBtnDefectScan->SetEnabled(true);
            }
            KillTimer(TIME_PLCSTATE);
        } else if (strName == _T("BtnDefectPos")) // 缺陷追踪
        {
            AutoDefectPosInfoWnd Mydlg;
            Mydlg.Create(m_hWnd, _T("AutoDefectPosInfoWnd"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
            Mydlg.CenterWindow();
            Mydlg.ShowModal();

        }

        else if (strName == _T("BtnDefectFinish")) // 结束追踪
        {
            RuitiePLC::setVariable("M64", false); // 开始追踪命令
            ShowWindow(SW_HIDE);
            if (m_pScanWnd) {
                m_pScanWnd->m_pBtnWheelUp->SetEnabled(true);
                m_pScanWnd->m_pBtnWheelDown->SetEnabled(true);
                m_pScanWnd->m_pBtnScanStart->SetEnabled(true);
                m_pScanWnd->m_pBtnScanStop->SetEnabled(true);
                m_pScanWnd->m_pBtnDailyCheck->SetEnabled(true);
                m_pScanWnd->m_pIDCANCEL->SetEnabled(true);
                m_pScanWnd->m_pBtnDefectPos->SetEnabled(true);
                m_pScanWnd->m_pBtnDefectScan->SetEnabled(true);
            }
        } else if (strName == _T("BtnDefectPrePare")) //
        {
            InitDefectPos();
        }
    } else if (msg.sType == DUI_MSGTYPE_ITEMSELECT) {
        CDuiString strName = msg.pSender->GetName();
        if (strName == _T("DefectFileList")) {
            OnSelectelistItem();
        }
    }
    CDuiWindowBase::Notify(msg);
}

void DefectListWnd::UpdateTechList() {
    // m_IDEFECTList.clear();
    // for (int i = 0; i < 10; i++) {
    //	DB_DEFECT_DATA data;
    //	data.nIndex = i;
    //	data.nCH = i % 10;
    //	m_IDEFECTList.push_back(data);
    // }
}
void DefectListWnd::ShowListData() {
    // UpdateTechList();
    CListContainerElementUI* pElement = NULL;

    if (m_pListDefectFileList != NULL) {
        m_pListDefectFileList->RemoveAll();

        int iCount = 0;
        for (int i = 0; i < HD_CHANNEL_NUM; i++) {
            iCount += g_MainProcess.m_Techniques.m_pDefect[i].size(); // 10个通道的缺陷
        }

        if (iCount > 25) {
            m_pLabel11->SetVisible(true);
        } else {
            m_pLabel11->SetVisible(false);
        }
        int index = iCount;
        for (int ch = HD_CHANNEL_NUM - 1; ch >= 0; ch--) {
            for (int i = g_MainProcess.m_Techniques.m_pDefect[ch].size() - 1; i >= 0; i--) {
                index--;
                pElement = new CListContainerElementUI();
                pElement->SetFixedHeight(24);
                m_pListDefectFileList->AddAt(pElement, 0);

                CString strXml;
                strXml.Format(_T("Theme\\DefectItem.xml"));
                CDialogBuilder       builder;
                CHorizontalLayoutUI* pLayout = static_cast<CHorizontalLayoutUI*>(builder.Create(strXml.GetBuffer(0), 0, NULL, &m_PaintManager));
                pElement->Add(pLayout);

                CLabelUI* pLabel1  = static_cast<CLabelUI*>(pElement->FindSubControl(_T("Label1")));
                CLabelUI* pLabel2  = static_cast<CLabelUI*>(pElement->FindSubControl(_T("Label2")));
                CLabelUI* pLabel3  = static_cast<CLabelUI*>(pElement->FindSubControl(_T("Label3")));
                CLabelUI* pLabel4  = static_cast<CLabelUI*>(pElement->FindSubControl(_T("Label4")));
                CLabelUI* pLabel5  = static_cast<CLabelUI*>(pElement->FindSubControl(_T("Label5")));
                CLabelUI* pLabel6  = static_cast<CLabelUI*>(pElement->FindSubControl(_T("Label6")));
                CLabelUI* pLabel7  = static_cast<CLabelUI*>(pElement->FindSubControl(_T("Label7")));
                CLabelUI* pLabel8  = static_cast<CLabelUI*>(pElement->FindSubControl(_T("Label8")));
                CLabelUI* pLabel9  = static_cast<CLabelUI*>(pElement->FindSubControl(_T("Label9")));
                CLabelUI* pLabel10 = static_cast<CLabelUI*>(pElement->FindSubControl(_T("Label10")));
                CLabelUI* pLabel11 = static_cast<CLabelUI*>(pElement->FindSubControl(_T("Label11")));
                CLabelUI* pLabel12 = static_cast<CLabelUI*>(pElement->FindSubControl(_T("Label12")));
                CLabelUI* pLabel13 = static_cast<CLabelUI*>(pElement->FindSubControl(_T("Label13")));

                CString strText;
                strText.Format(_T("%d"), index);
                pLabel1->SetText(strText);

                strText.Format(_T("%d"), g_MainProcess.m_Techniques.m_pDefect[ch][i]->nCH + 1);
                pLabel2->SetText(strText);

                //	strText.Format(_T("%s"), );
                pLabel3->SetText(g_MainProcess.m_Techniques.m_pDefect[ch][i]->szProbeMake);
                pLabel3->SetBkColor(COLOR_TAB_CH[ch + 1]);

                strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_pDefect[ch][i]->nRadialDistance);
                pLabel4->SetText(strText);

                strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_pDefect[ch][i]->nAxialDepth);
                pLabel5->SetText(strText);
                strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_pDefect[ch][i]->nDefectAngle);
                pLabel6->SetText(strText);

                strText.Format(_T("%d"), g_MainProcess.m_Techniques.m_pDefect[ch][i]->nWaveHeight);
                pLabel7->SetText(strText);
                strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_pDefect[ch][i]->nDBOffset);
                pLabel8->SetText(strText);

                pLabel9->SetText(g_MainProcess.m_Techniques.m_pDefect[ch][i]->bDefectType ? _T("透声不良") : _T("缺陷"));

                strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_pDefect[ch][i]->nSensitivity);
                pLabel10->SetText(strText);

                strText.Format(_T("%d"), g_MainProcess.m_Techniques.m_pDefect[ch][i]->nIndex);
                pLabel11->SetText(strText);

                strText.Format(_T("%d"), g_MainProcess.m_Techniques.m_pDefect[ch][i]->nParam1);
                pLabel12->SetText(strText);

                strText.Format(_T("%d"), g_MainProcess.m_Techniques.m_pDefect[ch][i]->nParam2);
                pLabel13->SetText(strText);
            }
        }
    }
}
void DefectListWnd::OnSelectelistItem() {
    INT                      iIndex   = m_pListDefectFileList->GetCurSel();
    int                      iCount   = m_pListDefectFileList->GetCount();
    CListContainerElementUI* pElement = (CListContainerElementUI*)(m_pListDefectFileList->GetItemAt(iIndex));
    memset(&g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA, 0, sizeof(DB_DEFECT_DATA));
    CLabelUI* pLabelCH      = static_cast<CLabelUI*>(pElement->FindSubControl(_T("Label2")));  // 选中通道
    CLabelUI* pLabelCHIndex = static_cast<CLabelUI*>(pElement->FindSubControl(_T("Label11"))); // 选中通道 中的缺陷索引
    // CString strName = pLabelName->GetText();
    // m_pListDefectFileList->SetText(strName.GetBuffer(0));
    CString strICH = pLabelCH->GetText();
    int     nValue(0);
    _stscanf_s(strICH, _T("%d"), &nValue);

    int     iSeletCh = nValue - 1; // 选中缺陷的通道
    CString strIndex = pLabelCHIndex->GetText();
    _stscanf_s(strIndex, _T("%d"), &nValue);
    int iSelectIndex = nValue;

    memcpy(&g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA,
           g_MainProcess.m_Techniques.m_pDefect[iSeletCh][iSelectIndex],
           sizeof(g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA));
    if (g_MainProcess.m_Techniques.m_ScanType == SCAN_REPLAY) { // 回放处理

        if (m_pReViewScanWnd != NULL) {
            m_pReViewScanWnd->m_OpenGL.m_fPt_Cir      = g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nCircleIndex;
            m_pReViewScanWnd->m_OpenGL.m_fPtC_DrawCir = m_pReViewScanWnd->m_OpenGL.m_fPt_Cir;
            m_pReViewScanWnd->m_OpenGL.m_nDefectCh    = g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nCH + 1;
            if (g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nCH < g_MainProcess.m_Techniques.m_Scan.m_pSideProbe[0]) {
                if (g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nCH == 2 || g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nCH == 3) {
                    m_pReViewScanWnd->m_OpenGL.m_fPtC_DrawCir = m_pReViewScanWnd->m_OpenGL.m_fPt_Cir + g_MainProcess.m_Techniques.m_Scan.m_iDrawProbleTreadSize;
                } else if (g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nCH == 4 || g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nCH == 5) {
                    m_pReViewScanWnd->m_OpenGL.m_fPtC_DrawCir = m_pReViewScanWnd->m_OpenGL.m_fPt_Cir + g_MainProcess.m_Techniques.m_Scan.m_iDrawProbleTreadSize * 2;
                }
            } else {
                if (g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nCH == 7 || g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nCH == 9) {
                    m_pReViewScanWnd->m_OpenGL.m_fPtC_DrawCir = m_pReViewScanWnd->m_OpenGL.m_fPt_Cir + g_MainProcess.m_Techniques.m_Scan.m_iDrawProbleTreadSize;
                }
            }
            m_pReViewScanWnd->m_OpenGL.m_fPt_Xscale = g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nDefectAngle / g_MainProcess.m_Techniques.m_iSidePoints;
            m_pReViewScanWnd->m_OpenGL.GetClickPt();
        }
        //	 SendMessage(WM_DEFECTLISTITEMSELECT, 1, 2);
        int nRecordindex = g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nScanIndex;
        for (int iChannel = 0; iChannel < HD_CHANNEL_NUM; iChannel++) {
            size_t iSize = g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pAscan[iChannel].size();
            if (iSize != 0) {
                if (iChannel < 6) {
                    g_MainProcess.m_Techniques.GetDetectionStd()->CountDAC(&(g_MainProcess.m_Techniques), iChannel, g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[iChannel].fScanGain,
                                                                           (float)g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pGatePos[iChannel][GATE_A] / 100.0f * (float)g_MainProcess.m_Techniques.m_pChannel[iChannel].m_iVelocity / 2000.0f + g_MainProcess.m_Techniques.m_pChannel[iChannel].m_fDelay);
                }
            }
            if (nRecordindex < g_MainProcess.m_Techniques.m_pRecord.size()) {
                size_t iSize = g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pAscan[iChannel].size();

                if (iSize != 0) {
                    g_MainProcess.m_Techniques.m_pDraw[0].pData[iChannel].resize(iSize);
                    g_MainProcess.m_Techniques.m_pDraw[1].pData[iChannel].resize(iSize);
                    for (size_t i = 0; i < iSize; i++) {
                        g_MainProcess.m_Techniques.m_pDraw[0].pData[iChannel][i] = float(g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pAscan[iChannel][i]) / MAX_AMP;
                        g_MainProcess.m_Techniques.m_pDraw[1].pData[iChannel][i] = float(g_MainProcess.m_Techniques.m_pRecord[nRecordindex].pAscan[iChannel][i]) / MAX_AMP;
                    }
                }
            }
        }
    }
    int test = 0;
}

bool DefectListWnd::OnEdit(void* pv) {
    TNotifyUI* msg = (TNotifyUI*)pv;

    if (msg->sType == DUI_MSGTYPE_RETURN || msg->sType == DUI_MSGTYPE_KILLFOCUS) {
        CDuiString strName = msg->pSender->GetName();
        CDuiString strText = msg->pSender->GetText();

        if (strName == _T("EditTech")) {
            CString strName = strText.GetData();

            if (strName.GetLength() <= 0) {
                DMessageBox(_T("请输入工艺名称！"), _T("工艺保存！"));
                return false;
            }
        }
    }

    return true;
}