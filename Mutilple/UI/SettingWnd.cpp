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
    auto opt          = static_cast<COptionUI*>(m_PaintManager.FindControl(L"OptAutoUpgrade"));
    auto systemConfig = GetSystemConfig();
    opt->Selected(systemConfig.checkUpdate);
    opt = static_cast<COptionUI*>(m_PaintManager.FindControl(L"OptSystemProxy"));
    opt->Selected(systemConfig.enableProxy);
    auto edit = static_cast<CEditUI*>(m_PaintManager.FindControl(L"EditSystemProxy"));
    edit->SetText(systemConfig.httpProxy.c_str());
}

void SettingWnd::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_SELECTCHANGED) {
        if (msg.pSender->GetName() == L"OptAutoUpgrade") {
            auto systemConfig        = GetSystemConfig();
            systemConfig.checkUpdate = static_cast<COptionUI*>(msg.pSender)->IsSelected();
            UpdateSystemConfig(systemConfig);
        } else if (msg.pSender->GetName() == L"OptSystemProxy") {
            auto systemConfig        = GetSystemConfig();
            systemConfig.enableProxy = static_cast<COptionUI*>(msg.pSender)->IsSelected();
            UpdateSystemConfig(systemConfig);
        }
    } else if (msg.sType == DUI_MSGTYPE_TEXTCHANGED) {
        if (msg.pSender->GetName() == L"EditSystemProxy") {
            auto systemConfig      = GetSystemConfig();
            systemConfig.httpProxy = std::wstring(static_cast<CEditUI*>(msg.pSender)->GetText());
            UpdateSystemConfig(systemConfig);
        }
    }
    CDuiWindowBase::Notify(msg);
}