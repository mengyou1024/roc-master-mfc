#pragma once

class AutoScanInfoWnd : public CDuiWindowBase {
public:
    ~AutoScanInfoWnd();
    virtual LPCTSTR GetWindowClassName() const {
        return _T("AutoScanInfoWnd");
    }
    virtual CDuiString GetSkinFile() {
        return _T("Theme\\UI_AutoScanInfoWnd.xml");
    }

    virtual void InitWindow();
    virtual void Notify(TNotifyUI& msg);

    virtual void OnTimer(int iIdEvent);

    bool OnEdit(void* pv);

public:
    CTextUI *            m_pThreadText, *m_pSideText;
    CEditUI *            m_pEditWheelModel, *m_pEditHeatNumber, *m_pEditWheelNumber;
    CHorizontalLayoutUI* m_pLayDailyType;
    CComboUI*            m_pComboDemodu;
    int                  m_nType; // ≤Ÿ◊˜√¸¡Ó¿‡–Õ
public:
};
