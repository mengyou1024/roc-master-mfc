#pragma once

#include "ICheckCarrier.h"

const int CHECK_NUM = 5;

class CheckWnd : public CDuiWindowBase, public ICheckCarrier {
public:
    virtual LPCTSTR GetWindowClassName() const {
        return _T("CheckWnd");
    }
    virtual CDuiString GetSkinFile() {
        return _T("Theme\\UI_CheckWnd.xml");
    }

    virtual void InitWindow();
    virtual void Notify(TNotifyUI& msg);

    virtual void Begin();
    virtual void SetResult(CHECK check, bool bResult);
    virtual void End();

    void OnBtnRecheck();

public:
    CLabelUI *m_pLabelBegin, *m_pLabelEnd;
    CLabelUI* m_pLabelCheck[CHECK_NUM];

public:
    bool m_bResult;
};
