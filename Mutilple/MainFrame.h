#pragma once

#include "OpenGL.h"

class CMainFrame : public CDuiWindowBase {
public:
    CMainFrame(void);
    ~CMainFrame(void);

protected:
    LPCTSTR GetWindowClassName() const {
        return _T("MainFrame");
    }
    CDuiString GetSkinFile() {
        return _T("Theme\\UI_MainFrame.xml");
    }

public:
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void    InitWindow();
    virtual void    Notify(TNotifyUI& msg);

    void Init();

    void OnBtnSet();
    void OnBtnScan();
    void OnBtnGroupScan();
    void OnBtnUser();
    void OnBtnQuart();
    void OnBtnAnalysis();
    void OnBtnAbout();

public:
    vector<CButtonUI*> m_pDeleteBtn;
};
