#include "SettingWnd.h"
#include "ModelAScan.h"

SettingWnd::~SettingWnd() {}

LPCTSTR SettingWnd::GetWindowClassName() const {
    return _T("SettingWnd");
}

CDuiString SettingWnd::GetSkinFile() {
    return _T(R"(Theme\UI_SettingWnd.xml)");
}

void SettingWnd::InitWindow() {
    CDuiWindowBase::InitWindow();
}

void SettingWnd::Notify(TNotifyUI& msg) {
    CDuiWindowBase::Notify(msg);
}