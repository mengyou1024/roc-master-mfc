#pragma once

const int MAX_UIHOLENUM = 5;
class DetectionStd_TBT2995_200;
class SetWnd;
class DACSetWnd : public CDuiWindowBase {
public:
    virtual LPCTSTR GetWindowClassName() const {
        return _T("DACSetWnd");
    }
    virtual CDuiString GetSkinFile() {
        return _T("Theme\\UI_DACSetWnd.xml");
    }

    virtual void InitWindow();
    virtual void Notify(TNotifyUI& msg);

    void OnBtnSpin(LPCTSTR lpName, float fValue);
    bool OnEdit(void* pv);
    void UpdateUI();

public:
    // DAC参数
    CComboUI *                m_pComDACAperture, *m_pComUseHole;
    CEditUI *                 m_pEditLineRL, *m_pEditLineRD, *m_pEditDACGain, *m_pEditDACGateAAmpPos;
    CEditUI *                 m_pEditHoleDepth[MAX_UIHOLENUM], *m_pEditHoleAmp[MAX_UIHOLENUM];
    DetectionStd_TBT2995_200* m_pSTD; // 工艺里面的 标准指针 便于调用读写参数

    SetWnd* m_pSetWnd;
};
