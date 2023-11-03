#include "pch.h"

#include "CheckWnd.h"
#include "Mutilple.h"

void CheckWnd::InitWindow() {
    CDuiWindowBase::InitWindow();

    // ��ʼ����ʾLabel
    m_pLabelBegin = static_cast<CLabelUI *>(m_PaintManager.FindControl(_T("LabelBegin")));
    m_pLabelEnd   = static_cast<CLabelUI *>(m_PaintManager.FindControl(_T("LabelEnd")));
    for (int i = 0; i < CHECK_NUM; i++) {
        CString strName;
        strName.Format(_T("LabelCheck%d"), i);
        m_pLabelCheck[i] = static_cast<CLabelUI *>(m_PaintManager.FindControl(strName));
    }

    // �Լ�
    std::thread check(&MainProcess::Check, &g_MainProcess, this);
    check.detach();
}

void CheckWnd::Notify(TNotifyUI &msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        CDuiString strName = msg.pSender->GetName();
        if (strName == _T("BtnRecheck")) {
            // �����Լ�
            OnBtnRecheck();
        } else if (strName == _T("BtnEnter")) {
            // Exited code == IDYES ����ϵͳ
            Close(IDYES);
            return;
        } else if (strName == _T("BtnExit")) {
            // Exited code == IDNO �˳�ϵͳ
            Close(IDNO);
            return;
        }
    }

    CDuiWindowBase::Notify(msg);
}

void CheckWnd::Begin() {
    m_PaintManager.FindControl(_T("BtnRecheck"))->SetEnabled(false);
    m_PaintManager.FindControl(_T("BtnEnter"))->SetEnabled(false);
    m_PaintManager.FindControl(_T("BtnExit"))->SetEnabled(false);

    m_pLabelBegin->SetText(_T("��ʼ�Լ�"));
    m_bResult = true;
}

void CheckWnd::SetResult(CHECK check, bool bResult) {
    m_bResult      = m_bResult && bResult;
    int     icheck = static_cast<int>(check);
    CString strText;
    switch (check) {
        case CHECK::DIR: strText = _T("�������Ŀ¼"); break;
        case CHECK::CALIB: strText = _T("����Уģ��"); break;
        case CHECK::SCAN: strText = _T("���̽��ģ��"); break;
        case CHECK::ANALYSIS: strText = _T("������ݴ���ģ��"); break;
        case CHECK::PLC: strText = _T("PLC����ģ��"); break;
    }

    if (icheck < CHECK_NUM) {
        strText.AppendFormat(_T(": %s"), bResult ? _T("�ɹ�") : _T("ʧ��"));
        m_pLabelCheck[icheck]->SetText(strText);
    }
}

void CheckWnd::End() {
    m_pLabelEnd->SetText(_T("�Լ����"));

    m_PaintManager.FindControl(_T("BtnRecheck"))->SetEnabled(true);
    m_PaintManager.FindControl(_T("BtnEnter"))->SetEnabled(true);
    m_PaintManager.FindControl(_T("BtnExit"))->SetEnabled(true);

#ifndef _DEBUG
    if (m_bResult) {
        Sleep(2000);
        //        Close(IDOK);
    }
#endif
}

void CheckWnd::OnBtnRecheck() {
    m_pLabelBegin->SetText(_T(""));
    m_pLabelEnd->SetText(_T(""));
    for (int i = 0; i < CHECK_NUM; i++) {
        m_pLabelCheck[i]->SetText(_T(""));
    }

    std::thread check(&MainProcess::Check, &g_MainProcess, this);
    check.detach();
}