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
    // ��ʼ����ʾLabel
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

            if (strName == _T("BtnMaterialTrayUp")) // ����������
            {
                RuitiePLC::setVariable("M26", true); // �����̽���

            } else if (strName == _T("BtnMaterialTrayDown")) // �����̽���
            {
                RuitiePLC::setVariable("M27", true);  // �����̽���
            } else if (strName == _T("BtnChuckOpen")) // �����ſ�
            {
                RuitiePLC::setVariable("M20", true);   // �����ſ�
            } else if (strName == _T("BtnChuckClose")) // ���̺�£
            {
                RuitiePLC::setVariable("M21", true); // ���̺�£
            }

        } else if (msg.sType == DUI_MSGTYPE_CLICK_UP) {
            if (strName == _T("BtnMaterialTrayUp")) // ����������
            {
                RuitiePLC::setVariable("M26", false); // ����������

            } else if (strName == _T("BtnMaterialTrayDown")) // �����̽���
            {
                RuitiePLC::setVariable("M27", false); // �����̽���
            } else if (strName == _T("BtnChuckOpen")) // �����ſ�
            {
                RuitiePLC::setVariable("M20", false);  // �����ſ�
            } else if (strName == _T("BtnChuckClose")) // ���̺�£
            {
                RuitiePLC::setVariable("M21", false); // ���̺�£
            }
        }

    }

    else if (msg.sType == DUI_MSGTYPE_CLICK) {
        if (strName == _T("IDCONFIRMUPDOWM")) // ȷ��
        {
            g_MainProcess.m_ConnectPLC.SetPLCAuto(true);
            Close(IDOK);
        } else if (strName == _T("IDCANCELUPDOWM")) // �˳�
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
