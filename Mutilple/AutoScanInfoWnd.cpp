#include "pch.h"

#include "AutoScanInfoWnd.h"
#include "Mutilple.h"
#include "RuitiePLC.h"

const int TIME_PLCSTATE = 120;

AutoScanInfoWnd::~AutoScanInfoWnd() {
    KillTimer(TIME_PLCSTATE);
}
void AutoScanInfoWnd::InitWindow() {
    CDuiWindowBase::InitWindow();

    // 初始化显示Label
    m_pThreadText      = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("TextThreadData")));
    m_pSideText        = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("TextSideData")));
    m_pEditWheelModel  = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditWheelModel")));
    m_pEditHeatNumber  = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditHeatNumber")));
    m_pEditWheelNumber = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditWheelNumber")));

    m_pEditWheelModel->SetText(g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.szWheelModel);
    m_pEditHeatNumber->SetText(g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.szHeatNumber);
    m_pEditWheelNumber->SetText(g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.szWheelNumber);

    m_pEditWheelModel->OnNotify += MakeDelegate(this, &AutoScanInfoWnd::OnEdit);
    m_pEditHeatNumber->OnNotify += MakeDelegate(this, &AutoScanInfoWnd::OnEdit);
    m_pEditWheelNumber->OnNotify += MakeDelegate(this, &AutoScanInfoWnd::OnEdit);
    m_pLayDailyType = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("LayDailyType")));
    m_pComboDemodu  = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("ComboDemodu")));
    if (g_MainProcess.m_Techniques.m_bDailyScan) {
        m_pLayDailyType->SetVisible(true);
    } else {
        m_pLayDailyType->SetVisible(false);
    }

    CString strTxt;
    strTxt.Format(_T("踏面右轴：\nX一次：%.1f，X二次：%.1f,\nY一次：%.1f，Y二次：%.1f,\n步进：%.1f"),
                  g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadLyXPos, g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadXPos,
                  g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart, g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYEnd,
                  g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep);
    m_pThreadText->SetText(strTxt);

    strTxt.Format(_T("侧面左轴：\nX一次：%.1f，X二次：%.1f,\nY一次：%.1f，\n步进：%.1f"),
                  g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart, g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXEnd,
                  g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideYPos,
                  g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep);
    m_pSideText->SetText(strTxt);
}

void AutoScanInfoWnd::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        CDuiString strName = msg.pSender->GetName();
        if (strName == _T("IDCONFIRMUPDOWM")) // 确认
        {
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

            g_MainProcess.m_ConnectPLC.SetPLCAutoStart(); // 开始自动检测

            /*
                      void ConnectPLC::SetPLC_SideMove(float fPosX1, float fPosX2, float fPosY1, float fPosY2, float fPosStep) {
                          RuitiePLC::setVariable("V1060", fPosX1);  //右水平轴一次定位位置
                          RuitiePLC::setVariable("V1064", fPosX2);  //右水平轴二次定位位置
                          RuitiePLC::setVariable("V1080", fPosY1);  //右上下轴一次定位位置
                          RuitiePLC::setVariable("V1084", fPosY2);  //右上下轴二次定位位置
                          RuitiePLC::setVariable("V1056", fPosStep);  //右水平轴增量位置SV
                      }
                      */
            Close(IDOK);
        } else if (strName == _T("IDCANCELUPDOWM")) // 退出
        {
            Close(IDCANCEL);
        }

    } else if (msg.sType == DUI_MSGTYPE_ITEMSELECT) {
        CDuiString strName = msg.pSender->GetName();

        if (strName == _T("ComboDemodu")) {
            INT iIndex                              = m_pComboDemodu->GetCurSel();
            g_MainProcess.m_Techniques.m_nDailyType = iIndex;
        }
    }
    CDuiWindowBase::Notify(msg);
}

void AutoScanInfoWnd::OnTimer(int iIdEvent) {
    switch (iIdEvent) {
        case TIME_PLCSTATE: {
        } break;
    }
}
bool AutoScanInfoWnd::OnEdit(void* pv) {
    TNotifyUI* msg = (TNotifyUI*)pv;

    if (msg->sType == DUI_MSGTYPE_RETURN || msg->sType == DUI_MSGTYPE_KILLFOCUS) {
        CDuiString strName = msg->pSender->GetName();
        CDuiString strText = msg->pSender->GetText();

        if (strName == _T("EditWheelModel")) {
            StrCpy(g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.szWheelModel, strText.GetData());
        }
        if (strName == _T("EditHeatNumber")) {
            StrCpy(g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.szHeatNumber, strText.GetData());
        }
        if (strName == _T("EditWheelNumber")) {
            StrCpy(g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.szWheelNumber, strText.GetData());
        }
    }

    return true;
}