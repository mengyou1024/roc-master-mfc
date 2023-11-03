#include "pch.h"

#include "Mutilple.h"
#include "RuitiePLC.h"
#include "WheelUpDownWnd.h"

const int TIME_PLCSTATE = 120;
const int TIME_Long     = 121;

WheelUpDownWnd::~WheelUpDownWnd() {
    KillTimer(TIME_PLCSTATE);
}
void WheelUpDownWnd::InitWindow() {
    CDuiWindowBase::InitWindow();
    g_MainProcess.m_ConnectPLC.SetPLCAuto(false);
    // 初始化显示Label
    CButtonUI* BtnMaterialTrayUp   = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnMaterialTrayUp")));
    CButtonUI* BtnChuckClose       = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnChuckClose")));
    CButtonUI* BtnMaterialTrayDown = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnMaterialTrayDown")));
    CButtonUI* BtnChuckOpen        = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnChuckOpen")));
    BtnMaterialTrayUp->SetClickType(1);
    BtnChuckOpen->SetClickType(1);
    BtnMaterialTrayDown->SetClickType(1);
    BtnChuckClose->SetClickType(1);
    m_PLCOrderRunTime = 0;
    m_nType           = 0;
}

void WheelUpDownWnd::Notify(TNotifyUI& msg) {
    CDuiString strName = msg.pSender->GetName();
    if (msg.sType == DUI_MSGTYPE_CLICK_DOWN || msg.sType == DUI_MSGTYPE_CLICK_UP) {
        if (msg.sType == DUI_MSGTYPE_CLICK_DOWN) { //

            if (strName == _T("BtnMaterialTrayUp")) // 上料盘升起
            {
                RuitiePLC::setVariable("M26", true); // 上料盘降下

            } else if (strName == _T("BtnMaterialTrayDown")) // 上料盘降下
            {
                RuitiePLC::setVariable("M27", true);  // 上料盘降下
            } else if (strName == _T("BtnChuckOpen")) // 卡盘张开
            {
                RuitiePLC::setVariable("M20", true);   // 卡盘张开
            } else if (strName == _T("BtnChuckClose")) // 卡盘合拢
            {
                RuitiePLC::setVariable("M21", true); // 卡盘合拢
            }

        } else if (msg.sType == DUI_MSGTYPE_CLICK_UP) {
            if (strName == _T("BtnMaterialTrayUp")) // 上料盘升起
            {
                RuitiePLC::setVariable("M26", false); // 上料盘升起

            } else if (strName == _T("BtnMaterialTrayDown")) // 上料盘降下
            {
                RuitiePLC::setVariable("M27", false); // 上料盘降下
            } else if (strName == _T("BtnChuckOpen")) // 卡盘张开
            {
                RuitiePLC::setVariable("M20", false);  // 卡盘张开
            } else if (strName == _T("BtnChuckClose")) // 卡盘合拢
            {
                RuitiePLC::setVariable("M21", false); // 卡盘合拢
            }
        }

    }

    else if (msg.sType == DUI_MSGTYPE_CLICK) {
        if (strName == _T("IDCONFIRMUPDOWM")) // 确认
        {
            g_MainProcess.m_ConnectPLC.SetPLCAuto(true);
            Close(IDOK);
        } else if (strName == _T("IDCANCELUPDOWM")) // 退出
        {
            g_MainProcess.m_ConnectPLC.SetPLCAuto(false);
            Close(IDCANCEL);
        }
    }
    CDuiWindowBase::Notify(msg);
}

void WheelUpDownWnd::OnTimer(int iIdEvent) {
    switch (iIdEvent) {
        case TIME_PLCSTATE: {
            DWORD RunTime = (GetTickCount64() - m_PLCOrderRunTime);
            bool  ok;
            if (m_nType == 1) {
            }
            if (RunTime > 5000) {
                KillTimer(TIME_PLCSTATE);
            }
        }

        break;
        case TIME_Long: {
            TRACE(_T("%d\n"), GetTickCount64());
        } break;
    }
}
