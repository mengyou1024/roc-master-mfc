#pragma once

enum UPDOWN_TYPE {
    UP   = 0,
    DOWN = 1,
};

class WheelUpDownWnd : public CDuiWindowBase {
public:
    ~WheelUpDownWnd();
    virtual LPCTSTR GetWindowClassName() const {
        return _T("WheelUpDownWnd");
    }
    virtual CDuiString GetSkinFile() {
        return _T("Theme\\UI_WheelUpDownWnd.xml");
    }

    virtual void InitWindow();
    virtual void Notify(TNotifyUI& msg);

    virtual void OnTimer(int iIdEvent);

public:
    UPDOWN_TYPE m_enumType;
    DWORD       m_PLCOrderRunTime;

    int m_nType; // ≤Ÿ◊˜√¸¡Ó¿‡–Õ
};
