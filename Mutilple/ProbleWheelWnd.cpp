#include "pch.h"

#include "Mutilple.h"
#include "ProbleWheelWnd.h"

void ProbleWheelWnd::InitWindow() {
    CDuiWindowBase::InitWindow();

    // 初始化显示Label、
    m_pEditWheelHub               = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditWheelHub")));
    m_pEditWheelRim               = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditWheelRim")));
    m_pEditWheelInnerSideOffset   = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditWheelInnerSideOffset")));
    m_pEditWheelInnerDiameter     = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditWheelInnerDiameter")));
    m_pEditWheelHubOuterDiameter  = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditWheelHubOuterDiameter")));
    m_pEditWheelRimlInnerDiameter = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditWheelRimlInnerDiameter")));
    m_pEditWheelRimOuterDiameter  = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditWheelRimOuterDiameter")));
    m_pEditWheelFlangeHeight      = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditfWheelFlangeHeight")));

    m_pEditWheelHub->OnNotify += MakeDelegate(this, &ProbleWheelWnd::OnEdit);
    m_pEditWheelRim->OnNotify += MakeDelegate(this, &ProbleWheelWnd::OnEdit);
    m_pEditWheelInnerSideOffset->OnNotify += MakeDelegate(this, &ProbleWheelWnd::OnEdit);
    m_pEditWheelInnerDiameter->OnNotify += MakeDelegate(this, &ProbleWheelWnd::OnEdit);
    m_pEditWheelHubOuterDiameter->OnNotify += MakeDelegate(this, &ProbleWheelWnd::OnEdit);
    m_pEditWheelRimlInnerDiameter->OnNotify += MakeDelegate(this, &ProbleWheelWnd::OnEdit);
    m_pEditWheelRimOuterDiameter->OnNotify += MakeDelegate(this, &ProbleWheelWnd::OnEdit);
    m_pEditWheelFlangeHeight->OnNotify += MakeDelegate(this, &ProbleWheelWnd::OnEdit);

    m_pEditTreadWidth = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditTreadWidth")));
    m_pEditSideWidth  = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditSideWidth")));
    m_pEditTreadWidth->OnNotify += MakeDelegate(this, &ProbleWheelWnd::OnEdit);
    m_pEditSideWidth->OnNotify += MakeDelegate(this, &ProbleWheelWnd::OnEdit);

    UpdateUI();
}
void ProbleWheelWnd::UpdateUI() {
    CString strText;
    strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHub);
    m_pEditWheelHub->SetText(strText);

    strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRim);
    m_pEditWheelRim->SetText(strText);

    strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelInnerSideOffset);
    m_pEditWheelInnerSideOffset->SetText(strText);

    strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHubInnerDiameter);
    m_pEditWheelInnerDiameter->SetText(strText);

    strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHubOuterDiameter);
    m_pEditWheelHubOuterDiameter->SetText(strText);

    strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimlInnerDiameter);
    m_pEditWheelRimlInnerDiameter->SetText(strText);

    strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimOuterDiameter);
    m_pEditWheelRimOuterDiameter->SetText(strText);

    strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelFlangeHeight);
    m_pEditWheelFlangeHeight->SetText(strText);

    strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fTreadWidth);
    m_pEditTreadWidth->SetText(strText);

    strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fSideWidth);
    m_pEditSideWidth->SetText(strText);
}
void ProbleWheelWnd::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        CDuiString strName = msg.pSender->GetName();

        if (strName == _T("IDCANCELProbleWheel")) {
            ShowWindow(SW_HIDE);
        } else if (strName == _T("BtnSpin")) {
            // UserData里面保存Spin按键需要控制的按键名称与步进
            CDuiString strData = msg.pSender->GetUserData();

            TCHAR pName[64]{0};
            float fValue = 0;
            _stscanf_s(strData.GetData(), _T("%[^:]:%f"), pName, 64, &fValue);

            OnBtnSpin(pName, fValue);
        }
    }

    CDuiWindowBase::Notify(msg);
}

bool ProbleWheelWnd::OnEdit(void* pv) {
    TNotifyUI* msg = (TNotifyUI*)pv;

    if (msg->sType == DUI_MSGTYPE_RETURN || msg->sType == DUI_MSGTYPE_KILLFOCUS) {
        CDuiString strName = msg->pSender->GetName();
        CDuiString strText = msg->pSender->GetText();

        if (strName == _T("EditWheelHub")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_WHEEL || fValue > MAX_WHEEL) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHub = fValue;

        } else if (strName == _T("EditWheelRim")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);
            if (fValue < MIN_WHEEL || fValue > MAX_WHEEL) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRim = fValue;
        } else if (strName == _T("EditWheelInnerSideOffset")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);
            if (fValue < MIN_WHEEL_OFFSER || fValue > MAX_WHEEL_OFFSER) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelInnerSideOffset = fValue;
        } else if (strName == _T("EditWheelInnerDiameter")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);
            if (fValue < MIN_WHEEL || fValue > MAX_WHEEL) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHubInnerDiameter = fValue;
        } else if (strName == _T("EditWheelHubOuterDiameter")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);
            if (fValue < MIN_WHEEL || fValue > MAX_WHEEL) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHubOuterDiameter = fValue;
        } else if (strName == _T("EditWheelRimlInnerDiameter")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);
            if (fValue < MIN_WHEEL || fValue > MAX_WHEEL) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimlInnerDiameter = fValue;
        } else if (strName == _T("EditWheelRimOuterDiameter")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);
            if (fValue < MIN_WHEEL || fValue > MAX_WHEEL) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimOuterDiameter = fValue;
        } else if (strName == _T("EditfWheelFlangeHeight")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);
            if (fValue < MIN_WHEEL || fValue > MAX_WHEEL) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelFlangeHeight = fValue;
        } else if (strName == _T("EditTreadWidth")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);
            if (fValue < MIN_WHEEL || fValue > MAX_WHEEL) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fTreadWidth = fValue;
        } else if (strName == _T("EditSideWidth")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);
            if (fValue < MIN_WHEEL || fValue > MAX_WHEEL) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fSideWidth = fValue;
        }
    }

    return true;
}

void ProbleWheelWnd::OnBtnSpin(LPCTSTR lpName, float fValue) {
    CEditUI* pEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(lpName));
    if (pEdit == nullptr)
        return;

    CString strText;
    if (!_tcsicmp(lpName, _T("EditWheelHub"))) {
        g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHub += fValue;

        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHub < MIN_WHEEL)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHub = MIN_WHEEL;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHub > MAX_WHEEL)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHub = MAX_WHEEL;

        strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHub);
    }

    else if (!_tcsicmp(lpName, _T("EditWheelRim"))) {
        g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRim += fValue;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRim < MIN_WHEEL)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRim = MIN_WHEEL;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRim > MAX_WHEEL)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRim = MAX_WHEEL;

        strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRim);
    } else if (!_tcsicmp(lpName, _T("EditWheelInnerSideOffset"))) {
        g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelInnerSideOffset += fValue;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelInnerSideOffset < MIN_WHEEL_OFFSER)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelInnerSideOffset = MIN_WHEEL_OFFSER;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelInnerSideOffset > MAX_WHEEL_OFFSER)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelInnerSideOffset = MAX_WHEEL_OFFSER;

        strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelInnerSideOffset);
    } else if (!_tcsicmp(lpName, _T("EditWheelInnerDiameter"))) {
        g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHubInnerDiameter += fValue;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHubInnerDiameter < MIN_WHEEL)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHubInnerDiameter = MIN_WHEEL;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHubInnerDiameter > MAX_WHEEL)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHubInnerDiameter = MAX_WHEEL;

        strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHubInnerDiameter);
    } else if (!_tcsicmp(lpName, _T("EditWheelHubOuterDiameter"))) {
        g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHubOuterDiameter += fValue;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHubOuterDiameter < MIN_WHEEL)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHubOuterDiameter = MIN_WHEEL;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHubOuterDiameter > MAX_WHEEL)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHubOuterDiameter = MAX_WHEEL;

        strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelHubOuterDiameter);
    } else if (!_tcsicmp(lpName, _T("EditWheelRimlInnerDiameter"))) {
        g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimlInnerDiameter += fValue;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimlInnerDiameter < MIN_WHEEL)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimlInnerDiameter = MIN_WHEEL;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimlInnerDiameter > MAX_WHEEL)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimlInnerDiameter = MAX_WHEEL;

        strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimlInnerDiameter);
    } else if (!_tcsicmp(lpName, _T("EditWheelRimOuterDiameter"))) {
        g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimOuterDiameter += fValue;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimOuterDiameter < MIN_WHEEL)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimOuterDiameter = MIN_WHEEL;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimOuterDiameter > MAX_WHEEL)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimOuterDiameter = MAX_WHEEL;

        strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimOuterDiameter);
    } else if (!_tcsicmp(lpName, _T("EditfWheelFlangeHeight"))) {
        g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelFlangeHeight += fValue;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelFlangeHeight < MIN_WHEEL)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelFlangeHeight = MIN_WHEEL;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelFlangeHeight > MAX_WHEEL)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelFlangeHeight = MAX_WHEEL;

        strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelFlangeHeight);
    } else if (!_tcsicmp(lpName, _T("EditTreadWidth"))) {
        g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fTreadWidth += fValue;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fTreadWidth < MIN_WHEEL)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fTreadWidth = MIN_WHEEL;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fTreadWidth > MAX_WHEEL)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fTreadWidth = MAX_WHEEL;

        strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fTreadWidth);
    } else if (!_tcsicmp(lpName, _T("EditSideWidth"))) {
        g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fSideWidth += fValue;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fSideWidth < MIN_WHEEL)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fSideWidth = MIN_WHEEL;
        if (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fSideWidth > MAX_WHEEL)
            g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fSideWidth = MAX_WHEEL;

        strText.Format(_T("%.1f"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fSideWidth);
    }

    pEdit->SetText(strText);
}
