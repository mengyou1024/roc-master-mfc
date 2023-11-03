#pragma once

class AboutWnd : public CDuiWindowBase {
public:
    AboutWnd(LPCTSTR lpStr, LPCTSTR lpTitle, UINT iType);
    ~AboutWnd(void);

public:
    virtual LPCTSTR GetWindowClassName() const {
        return _T("AboutWnd");
    }
    virtual CDuiString GetSkinFile() {
        return _T("Theme\\UI_AboutWnd.xml");
    }
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void            Notify(TNotifyUI &msg);
    virtual void    InitWindow();

public:
    CTabLayoutUI *m_pTabBtn;
    CTextUI      *m_pTextTitle;
    CTextUI      *m_pTextString;
    UINT          m_iType;
    LPCTSTR       m_pString, m_pTitle;
};
