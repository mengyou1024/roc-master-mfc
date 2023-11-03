#include "pch.h"

#include "DACSetWnd.h"
#include "DetectionStd_TBT2995_200.h"
#include "Mutilple.h"
#include "SetWnd.h"

void DACSetWnd::InitWindow() {
    CDuiWindowBase::InitWindow();

    m_pComDACAperture     = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("ComDACAperture")));
    m_pComUseHole         = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("ComUseHole")));
    m_pEditLineRL         = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditLineRL")));
    m_pEditLineRD         = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditLineRD")));
    m_pEditDACGain        = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditDACGain")));
    m_pEditDACGateAAmpPos = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditDACGateAAmpPos")));
    // m_pComDACAperture->SetEnabled(false);

    m_pComDACAperture->OnNotify += MakeDelegate(this, &DACSetWnd::OnEdit);
    m_pComUseHole->OnNotify += MakeDelegate(this, &DACSetWnd::OnEdit);
    m_pEditLineRL->OnNotify += MakeDelegate(this, &DACSetWnd::OnEdit);
    m_pEditLineRD->OnNotify += MakeDelegate(this, &DACSetWnd::OnEdit);
    m_pEditDACGain->OnNotify += MakeDelegate(this, &DACSetWnd::OnEdit);
    m_pEditDACGateAAmpPos->OnNotify += MakeDelegate(this, &DACSetWnd::OnEdit);

    CString strTxt;
    for (int i = 0; i < MAX_UIHOLENUM; i++) {
        strTxt.Format(_T("EditHoleDepth%d"), i + 1);
        m_pEditHoleDepth[i] = static_cast<CEditUI*>(m_PaintManager.FindControl(strTxt));
        m_pEditHoleDepth[i]->OnNotify += MakeDelegate(this, &DACSetWnd::OnEdit);
        strTxt.Format(_T("EditHoleAmp%d"), i + 1);
        m_pEditHoleAmp[i] = static_cast<CEditUI*>(m_PaintManager.FindControl(strTxt));
        m_pEditHoleAmp[i]->OnNotify += MakeDelegate(this, &DACSetWnd::OnEdit);
    }

    if (g_MainProcess.m_Techniques.m_iDetectionStd == DETECTIONSTD_TBT2995_200) {
        m_pSTD = (DetectionStd_TBT2995_200*)g_MainProcess.m_Techniques.GetDetectionStd();
    }

    UpdateUI();
}
void DACSetWnd::UpdateUI() {
    int ch = g_MainProcess.m_Techniques.m_iChannel;
    m_pComDACAperture->SelectItem(((int)m_pSTD->mDetetionParam2995_200[ch].fAperture) == 2 ? 0 : 1); // 孔径2 选0 ，孔径3 选1
    m_pComUseHole->SelectItem(m_pSTD->mDetetionParam2995_200[ch].nUseHole - 3);                      // 3,4,5  对应 0.12.03
    CString strText;
    strText.Format(_T("%.1f"), m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRD);
    m_pEditLineRL->SetText(strText);

    strText.Format(_T("%.1f"), m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRL);
    m_pEditLineRD->SetText(strText);

    strText.Format(_T("%.1f"), m_pSTD->mDetetionParam2995_200[ch].fDAC_BaseGain);
    m_pEditDACGain->SetText(strText);

    strText.Format(_T("%.1f"), m_pSTD->mDetetionParam2995_200[ch].fGateAAmpPos);
    m_pEditDACGateAAmpPos->SetText(strText);
    for (int i = 0; i < MAX_UIHOLENUM; i++) {
        strText.Format(_T("%.1f"), m_pSTD->mDetetionParam2995_200[ch].fDAC_HoleDepth[i]);
        m_pEditHoleDepth[i]->SetText(strText);

        strText.Format(_T("%.1f"), m_pSTD->mDetetionParam2995_200[ch].fDAC_HolebAmp[i] * 100.0f); // 计算0-0.1 显示1-100
        m_pEditHoleAmp[i]->SetText(strText);
    }

    for (int i = 0; i < MAX_UIHOLENUM; i++) {
        if (i < m_pSTD->mDetetionParam2995_200[ch].nUseHole) {
            m_pEditHoleDepth[i]->SetEnabled(true);
            m_pEditHoleAmp[i]->SetEnabled(true);
        } else {
            m_pEditHoleDepth[i]->SetEnabled(false);
            m_pEditHoleAmp[i]->SetEnabled(false);
        }
    }
}
void DACSetWnd::Notify(TNotifyUI& msg) {
    int ch = g_MainProcess.m_Techniques.m_iChannel;
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        CDuiString strName = msg.pSender->GetName();

        if (strName == _T("IDCANCELDACSET")) { // 退出
            ShowWindow(SW_HIDE);
        } else if (strName == _T("IDCONFIRMDACSET")) { // 确认

            ShowWindow(SW_HIDE);
        } else if (strName == _T("BtnSpin")) {
            // UserData里面保存Spin按键需要控制的按键名称与步进
            CDuiString strData = msg.pSender->GetUserData();

            TCHAR pName[64]{0};
            float fValue = 0;
            _stscanf_s(strData.GetData(), _T("%[^:]:%f"), pName, 64, &fValue);

            OnBtnSpin(pName, fValue);
        }
    } else if (msg.sType == DUI_MSGTYPE_ITEMSELECT) {
        CDuiString strName = msg.pSender->GetName();

        if (strName == _T("ComDACAperture")) {
            INT iIndex = m_pComDACAperture->GetCurSel();

            if (iIndex >= 0) {
                m_pSTD->mDetetionParam2995_200[ch].fAperture = 2.0f + iIndex; // 0-对应2孔径  1对应3孔径
            }
        }
        if (strName == _T("ComUseHole")) {
            INT iIndex = m_pComUseHole->GetCurSel();

            if (iIndex >= 0) {
                m_pSTD->mDetetionParam2995_200[ch].nUseHole = 3 + iIndex; // 0-对应3孔数 2对应5孔数
                for (int i = 0; i < MAX_UIHOLENUM; i++) {
                    if (i < m_pSTD->mDetetionParam2995_200[ch].nUseHole) {
                        m_pEditHoleDepth[i]->SetEnabled(true);
                        m_pEditHoleAmp[i]->SetEnabled(true);
                    } else {
                        m_pEditHoleDepth[i]->SetEnabled(false);
                        m_pEditHoleAmp[i]->SetEnabled(false);
                    }
                }

                m_pSetWnd->UpdateDAC();
            }
        }
    }
    CDuiWindowBase::Notify(msg);
}

bool DACSetWnd::OnEdit(void* pv) {
    int        ch  = g_MainProcess.m_Techniques.m_iChannel;
    TNotifyUI* msg = (TNotifyUI*)pv;

    if (msg->sType == DUI_MSGTYPE_RETURN || msg->sType == DUI_MSGTYPE_KILLFOCUS) {
        CDuiString strName = msg->pSender->GetName();
        CDuiString strText = msg->pSender->GetText();

        if (strName == _T("EditLineRL")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_DAC_RL || fValue > MAX_DAC_RL) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

            m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRL = fValue;
            m_pSetWnd->UpdateDAC();
        } else if (strName == _T("EditLineRD")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_DAC_RL || fValue > m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRL) {
                DMessageBox(_Tr("非法的参数值,且不能大于判废线！"));
                return false;
            }

            m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRD = fValue;
            m_pSetWnd->UpdateDAC();
        } else if (strName == _T("EditDACGain")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_GAIN || fValue > MAX_GAIN) {
                DMessageBox(_Tr("非法的参数值,且不能大于判废线！"));
                return false;
            }

            m_pSTD->mDetetionParam2995_200[ch].fDAC_BaseGain = fValue;

            m_pSetWnd->UpdateDAC();

        } // EditDACGateAAmpPos
        else if (strName == _T("EditDACGateAAmpPos")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < 0 || fValue > MAX_SAMPLEDEPTH) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

            m_pSTD->mDetetionParam2995_200[ch].fGateAAmpPos = fValue;

            m_pSetWnd->UpdateDAC();

        } else {
            CString strTxt;
            TCHAR   pName[64]{0};
            float   fValue = 0;
            _stscanf_s(strName.GetData(), _T("%[^:]:%f"), pName, 64, &fValue);
            for (int i = 0; i < MAX_UIHOLENUM; i++) {
                strTxt.Format(_T("EditHoleDepth%d"), i + 1);
                if (!_tcsicmp(pName, strTxt)) {
                    FLOAT fValue(0.0f);
                    _stscanf_s(strText.GetData(), _T("%f"), &fValue);

                    if (i == 0) {
                        if (fValue < MIN_DAC_Hole || fValue > MAX_DAC_Hole) {
                            DMessageBox(_Tr("非法的参数值！"));
                            return false;
                        }
                    } else {
                        if (fValue < m_pSTD->mDetetionParam2995_200[ch].fDAC_HoleDepth[i - 1] || fValue > MAX_DAC_Hole) {
                            DMessageBox(_Tr("非法的参数值,孔距要大于前一个孔距！"));
                            return false;
                        }
                    }

                    m_pSTD->mDetetionParam2995_200[ch].fDAC_HoleDepth[i] = fValue;

                    m_pSetWnd->UpdateDAC();
                }
                strTxt.Format(_T("EditHoleAmp%d"), i + 1);
                if (!_tcsicmp(pName, strTxt)) {
                    FLOAT fValue(0.0f);
                    _stscanf_s(strText.GetData(), _T("%f"), &fValue);
                    if (i == 0) {
                        if (fValue < MIN_DAC_AMP || fValue > MAX_DAC_AMP) {
                            DMessageBox(_Tr("非法的参数值"));
                            return false;
                        }
                    }
                    /*else
                    {
                        if (fValue <MIN_DAC_AMP || fValue >m_pSTD->mDetetionParam2995_200[ch].fDAC_HolebAmp[i - 1] * 100)
                        {
                            DMessageBox(_Tr("非法的参数值，波幅小于前一个点的波幅"));
                            return false;
                        }
                    }*/

                    m_pSTD->mDetetionParam2995_200[ch].fDAC_HolebAmp[i] = fValue / 100;

                    m_pSetWnd->UpdateDAC();
                }
            }
        }
    }

    return true;
}

void DACSetWnd::OnBtnSpin(LPCTSTR lpName, float fValue) {
    int      ch    = g_MainProcess.m_Techniques.m_iChannel;
    CEditUI* pEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(lpName));
    if (pEdit == nullptr)
        return;

    CString strText;
    if (!_tcsicmp(lpName, _T("EditLineRL"))) {
        m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRL += fValue;

        if (m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRL < MIN_DAC_RL)
            m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRL = MIN_DAC_RL;
        if (m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRL > MAX_DAC_RL)
            m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRL = MAX_DAC_RL;

        strText.Format(_T("%.1f"), m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRL);
    } else if (!_tcsicmp(lpName, _T("EditLineRD"))) {
        m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRD += fValue;

        if (m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRD < MIN_DAC_RL)
            m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRD = MIN_DAC_RL;
        if (m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRD > m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRL)
            m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRD = m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRL;

        strText.Format(_T("%.1f"), m_pSTD->mDetetionParam2995_200[ch].fDAC_LineRD);
    } else if (!_tcsicmp(lpName, _T("EditDACGain"))) {
        m_pSTD->mDetetionParam2995_200[ch].fDAC_BaseGain += fValue;

        if (m_pSTD->mDetetionParam2995_200[ch].fDAC_BaseGain < MIN_GAIN)
            m_pSTD->mDetetionParam2995_200[ch].fDAC_BaseGain = MIN_GAIN;
        if (m_pSTD->mDetetionParam2995_200[ch].fDAC_BaseGain > MAX_GAIN)
            m_pSTD->mDetetionParam2995_200[ch].fDAC_BaseGain = MAX_GAIN;

        strText.Format(_T("%.1f"), m_pSTD->mDetetionParam2995_200[ch].fDAC_BaseGain);
    }
    m_pSetWnd->UpdateDAC();
    pEdit->SetText(strText);
}
