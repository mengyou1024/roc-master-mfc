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
    opt = static_cast<COptionUI*>(m_PaintManager.FindControl(L"OptMeasureThickness"));
    opt->Selected(systemConfig.enableMeasureThickness);
    auto edit = static_cast<CEditUI*>(m_PaintManager.FindControl(L"EditSystemProxy"));
    edit->SetText(systemConfig.httpProxy.c_str());
    opt = static_cast<COptionUI*>(m_PaintManager.FindControl(L"OptUseNetwork"));
    opt->Selected(systemConfig.enableNetwork);
    edit = static_cast<CEditUI*>(m_PaintManager.FindControl(L"EditUseNetwork"));
    edit->SetText(WStringFromString(systemConfig.ipFPGA).c_str());
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
        } else if (msg.pSender->GetName() == L"OptMeasureThickness") {
            auto systemConfig                   = GetSystemConfig();
            systemConfig.enableMeasureThickness = static_cast<COptionUI*>(msg.pSender)->IsSelected();
            UpdateSystemConfig(systemConfig);
        } else if (msg.pSender->GetName() == L"OptUseNetwork") {
            auto systemConfig          = GetSystemConfig();
            systemConfig.enableNetwork = static_cast<COptionUI*>(msg.pSender)->IsSelected();
            UpdateSystemConfig(systemConfig);
        }
    } else if (msg.sType == DUI_MSGTYPE_TEXTCHANGED) {
        if (msg.pSender->GetName() == L"EditSystemProxy") {
            auto systemConfig      = GetSystemConfig();
            systemConfig.httpProxy = std::wstring(static_cast<CEditUI*>(msg.pSender)->GetText());
            UpdateSystemConfig(systemConfig);
        }
        if (msg.pSender->GetName() == L"EditUseNetwork") {
            auto systemConfig   = GetSystemConfig();
            systemConfig.ipFPGA = StringFromWString(std::wstring(static_cast<CEditUI*>(msg.pSender)->GetText()));
            UpdateSystemConfig(systemConfig);
        }
    }
    CDuiWindowBase::Notify(msg);
}