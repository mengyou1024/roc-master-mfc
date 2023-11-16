#pragma once

class CDMessageBox : public CDuiWindowBase {
public:
    CDMessageBox(LPCTSTR lpStr, LPCTSTR lpTitle, UINT iType);
    ~CDMessageBox(void);

public:
    virtual LPCTSTR GetWindowClassName() const {
        return _T("MessageBox");
    }
    virtual CDuiString GetSkinFile() {
        return _T("Theme\\UI_MessageBox.xml");
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
