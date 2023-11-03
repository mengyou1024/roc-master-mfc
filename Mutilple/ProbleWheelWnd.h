#pragma once

class ProbleWheelWnd : public CDuiWindowBase {
public:
    virtual LPCTSTR GetWindowClassName() const {
        return _T("ProbleWheelWnd");
    }
    virtual CDuiString GetSkinFile() {
        return _T("Theme\\UI_ProbleWheelWnd.xml");
    }

    virtual void InitWindow();
    virtual void Notify(TNotifyUI& msg);

    void OnBtnSpin(LPCTSTR lpName, float fValue);
    bool OnEdit(void* pv);
    void UpdateUI();

public:
    CEditUI *m_pEditWheelHub, *m_pEditWheelRim, *m_pEditWheelInnerSideOffset, *m_pEditWheelInnerDiameter,
        *m_pEditWheelHubOuterDiameter, *m_pEditWheelRimlInnerDiameter, *m_pEditWheelRimOuterDiameter,
        *m_pEditTreadWidth, *m_pEditSideWidth, *m_pEditWheelFlangeHeight;

public:
};
