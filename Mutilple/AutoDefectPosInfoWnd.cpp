#include "pch.h"

#include "AutoDefectPosInfoWnd.h"
#include "Mutilple.h"
#include "RuitiePLC.h"

const int TIME_PLCSTATE = 120;

AutoDefectPosInfoWnd::~AutoDefectPosInfoWnd() {
    KillTimer(TIME_PLCSTATE);
}
void AutoDefectPosInfoWnd::InitWindow() {
    CDuiWindowBase::InitWindow();

    m_lStart = 0;
    // ��ʼ����ʾLabel
    m_pThreadText    = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("TextThreadData")));
    m_pSideText      = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("TextSideData")));
    m_pTextMessage   = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("TextMessage")));
    m_pIDCONFIRMDPOS = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("IDCONFIRMDPOS")));
    m_pIDCANCELDPOS  = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("IDCANCELDPOS")));
    m_pOptAuto       = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("OptAutoM01"))); // �Զ��ֶ�״̬
    m_pBtnQuickM22   = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnQuickM22")));
    m_pBtnQuickM23   = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnQuickM23"))); // ����ת��
    m_pBtnQuickM33   = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnQuickM33")));
    m_pBtnQuickM34   = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnQuickM34"))); // �����ƶ� ����
    m_pBtnQuickM55   = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnQuickM55")));
    m_pBtnQuickM56   = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BtnQuickM56"))); // ̤���ƶ� ����

    m_pBtnQuickM22->SetClickType(1);
    m_pBtnQuickM23->SetClickType(1);
    m_pBtnQuickM33->SetClickType(1);

    m_pBtnQuickM34->SetClickType(1);
    m_pBtnQuickM55->SetClickType(1);
    m_pBtnQuickM56->SetClickType(1);

    m_pBtnQuickM33->SetVisible(false); // ̽ͷ�ƶ��ر�
    m_pBtnQuickM34->SetVisible(false);
    m_pBtnQuickM55->SetVisible(false);
    m_pBtnQuickM56->SetVisible(false);
    m_pOptAuto->Selected(g_MainProcess.m_ConnectPLC.GetPLCAuto());

    // SetCtrlEnable(false);
    CString strTxt;
    if (g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nCH > 5) { // ����
        strTxt.Format(_T("����ȱ�ݲ�����\nx��λλ�ã�%.1f,Y��λλ�ã�%.1f,�Ƕȶ�λ��%.1f"),

                      g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.fProbleXPos,
                      g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart,

                      g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nDefectAngle);
        m_pSideText->SetText(strTxt);
        m_pThreadText->SetVisible(false);
        m_pBtnQuickM55->SetEnabled(false);
        m_pBtnQuickM56->SetEnabled(false);
    } else {
        strTxt.Format(_T("̤��ȱ�ݲ�����\nX��λλ�ã�%.1f��Y��λλ�ã�%.1f���Ƕȶ�λ��%.1f"),
                      g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart,
                      g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.fProbleYPos,

                      g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nDefectAngle);
        m_pThreadText->SetText(strTxt);
        m_pSideText->SetVisible(false);
        m_pBtnQuickM34->SetEnabled(false);
        m_pBtnQuickM33->SetEnabled(false);
    }
}
void AutoDefectPosInfoWnd ::SetCtrlEnable(bool bEnable) {
    m_pOptAuto->SetEnabled(bEnable);
    m_pBtnQuickM22->SetEnabled(bEnable);
    m_pBtnQuickM23->SetEnabled(bEnable);
    if (g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nCH > 5 && bEnable) {
        m_pBtnQuickM33->SetEnabled(bEnable);
        m_pBtnQuickM34->SetEnabled(bEnable);
    }
    if (g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nCH <= 5 && bEnable) {
        m_pBtnQuickM55->SetEnabled(bEnable);
        m_pBtnQuickM56->SetEnabled(bEnable);
    }
}
void AutoDefectPosInfoWnd::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK_DOWN || msg.sType == DUI_MSGTYPE_CLICK_UP) {
        CDuiString strName = msg.pSender->GetName();
        bool       bDown   = false;
        if (msg.sType == DUI_MSGTYPE_CLICK_DOWN) { //

            bDown = true;
            // SetTimer(TIME_Long,30);
        } else if (msg.sType == DUI_MSGTYPE_CLICK_UP) {
            bDown = false;
            //  KillTimer(TIME_Long);
        }
        if (strName == _T("BtnQuickM22")) // ��������
        {
            RuitiePLC::setVariable("M22", bDown);
        } else if (strName == _T("BtnQuickM23")) // ���̷���
        {
            RuitiePLC::setVariable("M23", bDown);
        } else if (strName == _T("BtnQuickM55")) // ̤���ϵ㶯
        {
            RuitiePLC::setVariable("M55", bDown);
        } else if (strName == _T("BtnQuickM56")) // ̤���µ㶯
        {
            RuitiePLC::setVariable("M56", bDown);
        } else if (strName == _T("BtnQuickM33")) // ������㶯
        {
            RuitiePLC::setVariable("M33", bDown);
        } else if (strName == _T("BtnQuickM34")) // �����ҵ㶯
        {
            RuitiePLC::setVariable("M34", bDown);
        }
    }
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        CDuiString strName = msg.pSender->GetName();
        if (strName == _T("IDCONFIRMDPOS")) // ȷ��
        {
            // g_MainProcess.m_ConnectPLC.SetPLC_DetectMove(100.0f, 100.0f, 90.0f);

            if (g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nCH > 5) { // ����
                g_MainProcess.m_ConnectPLC.SetPLC_DetectMove(
                    g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.fProbleXPos,
                    g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart,

                    g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nDefectAngle);

            } else { // ̤��
                g_MainProcess.m_ConnectPLC.SetPLC_DetectMove(
                    g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart,
                    g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.fProbleYPos,

                    g_MainProcess.m_Techniques.mSelect_DB_DEFECT_DATA.nDefectAngle);
            }
            RuitiePLC::setVariable("M13", true); // ������ԭ
            m_pTextMessage->SetText(_T("���ڶ�λȱ�ݽǶȣ�̽ͷλ��,�������..."));
            m_pIDCONFIRMDPOS->SetEnabled(false);
            m_pIDCANCELDPOS->SetEnabled(false);

            // ��ȡ��������ʱ��
            m_lStart = GetTickCount(); // ����ο�ʼǰȡ��ϵͳ����ʱ��(ms)

            SetTimer(TIME_PLCSTATE, 100); // ������ʱ��
            /*
                      void ConnectPLC::SetPLC_SideMove(float fPosX1, float fPosX2, float fPosY1, float fPosY2, float fPosStep) {
                          RuitiePLC::setVariable("V1060", fPosX1);  //��ˮƽ��һ�ζ�λλ��
                          RuitiePLC::setVariable("V1064", fPosX2);  //��ˮƽ����ζ�λλ��
                          RuitiePLC::setVariable("V1080", fPosY1);  //��������һ�ζ�λλ��
                          RuitiePLC::setVariable("V1084", fPosY2);  //����������ζ�λλ��
                          RuitiePLC::setVariable("V1056", fPosStep);  //��ˮƽ������λ��SV
                      }
                      */
            // Close(IDOK);
        } else if (strName == _T("IDCANCELDPOS")) // �˳�
        {
            Close(IDCANCEL);
        } else if (strName == _T("OptAutoM01")) // �ֶ��Զ�
        {
            bool bSelect = m_pOptAuto->IsSelected();
            g_MainProcess.m_ConnectPLC.SetPLCAuto(bSelect);
        }
    }
    CDuiWindowBase::Notify(msg);
}

void AutoDefectPosInfoWnd::OnTimer(int iIdEvent) {
    switch (iIdEvent) {
        case TIME_PLCSTATE: {
            int  res   = RuitiePLC::getVariable("M13", true); // ������ԭ
            long lEnd  = GetTickCount();                      // ����ν�����ȡ��ϵͳ����ʱ��(ms)
            int  nTime = (lEnd - m_lStart) / 1000;
            if (res == 0) { // ��ԭ����
                m_pTextMessage->SetText(_T("ȱ�ݶ�λ��ɣ�������һ����"));
                SetCtrlEnable(true);
                m_pIDCONFIRMDPOS->SetEnabled(true);
                m_pIDCANCELDPOS->SetEnabled(true);
                KillTimer(TIME_PLCSTATE);
            }
            if (res == -1) {
                SetCtrlEnable(true);
                m_pTextMessage->SetText(_T("PLC����ʧ�ܣ�"));
                m_pIDCONFIRMDPOS->SetEnabled(true);
                m_pIDCANCELDPOS->SetEnabled(true);
                KillTimer(TIME_PLCSTATE);
            } else {
                if (nTime > 10) {
                    SetCtrlEnable(true);
                    m_pTextMessage->SetText(_T("ȱ�ݶ�λ��ʱ10sʧ�ܣ���������¶�λ�����˳�"));
                    m_pIDCONFIRMDPOS->SetEnabled(true);
                    m_pIDCANCELDPOS->SetEnabled(true);
                    KillTimer(TIME_PLCSTATE);
                }
            }
        } break;
    }
}
bool AutoDefectPosInfoWnd::OnEdit(void* pv) {
    TNotifyUI* msg = (TNotifyUI*)pv;

    if (msg->sType == DUI_MSGTYPE_RETURN || msg->sType == DUI_MSGTYPE_KILLFOCUS) {
        CDuiString strName = msg->pSender->GetName();
        CDuiString strText = msg->pSender->GetText();

        if (strName == _T("EditWheelModel")) {
        }
    }

    return true;
}