#pragma once
#include "DuiWindowBase.h"
#include <functional>

class TBusyWnd : public CDuiWindowBase {
public:
    TBusyWnd(std::function<void(void)> _func) {
        func = _func;
    }

    ~TBusyWnd() {}

public:
    virtual LPCTSTR GetWindowClassName() const override {
        return _T("TBusyWnd");
    }

    virtual CDuiString GetSkinFile() override {
        return _T("Theme\\UI_BusyWnd.xml");
    }

    virtual void InitWindow() override {
        CDuiWindowBase::InitWindow();
        CenterWindow();
        std::thread t([this]() {
            std::invoke(func);
            Close();
        });
        t.detach();
    }

private:
    std::function<void(void)> func;
};
