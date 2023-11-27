#include "HardWareWnd.h"

HardWareWnd::HardWareWnd() {}

HardWareWnd::~HardWareWnd() {}

LPCTSTR HardWareWnd::GetWindowClassName() const {
    return _T("SettingWnd");
}

CDuiString HardWareWnd::GetSkinFile() noexcept{
    return _T(R"(Theme\UI_HardWareWnd.xml)");
}

void HardWareWnd::InitWindow() {
    CDuiWindowBase::InitWindow();
}

void HardWareWnd::Notify(TNotifyUI& msg) {
    CDuiWindowBase::Notify(msg);
}
