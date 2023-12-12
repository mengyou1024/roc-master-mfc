#include "HardWareWnd.h"
#include "AbsPLCIntf.h"
using namespace AbsPLCIntf;

HardWareWnd::HardWareWnd() {}

HardWareWnd::~HardWareWnd() {
    disconnect();
}

LPCTSTR HardWareWnd::GetWindowClassName() const {
    return _T("SettingWnd");
}

CDuiString HardWareWnd::GetSkinFile() noexcept {
    return _T(R"(Theme\UI_HardWareWnd.xml)");
}

void HardWareWnd::InitWindow() {
    CDuiWindowBase::InitWindow();
}

void HardWareWnd::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        if (msg.pSender->GetName() == _T("BtnConnectToPLC")) {
            auto edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditIpAddr")));
            auto opt  = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("OptIsConnected")));
            auto ret  = connectTo(StringFromWString(std::wstring(edit->GetText())).c_str());
            opt->Selected(ret);
        } else if (msg.pSender->GetName() == _T("BtnBitRead")) {
            auto edit  = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditBit")));
            auto  [res,ret]   = getVariable<bool>(StringFromWString(std::wstring(edit->GetText())).c_str());
            if (res) {
                auto value = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditBitValue")));
                value->SetText(std::to_wstring(ret ? 1 : 0).c_str());
            }
        } else if (msg.pSender->GetName() == _T("BtnFloatRead")) {
            auto edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditFloat")));
            auto    [res, ret] = getVariable<float>(StringFromWString(std::wstring(edit->GetText())).c_str());
            if (res) {
                auto    value = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditFloatValue")));
                CString str;
                str.Format(L"%.2f", ret);
                value->SetText(str);
            }
        } else if (msg.pSender->GetName() == _T("BtnBitWrite")) {
            auto edit  = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditBit")));
            auto value = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditBitValue")));
            setVariable(StringFromWString(std::wstring(edit->GetText())).c_str(), _wtol(value->GetText()) ? true : false);
        } else if (msg.pSender->GetName() == _T("BtnFloatWrite")) {
            auto edit  = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditFloat")));
            auto value = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditFloatValue")));
            setVariable(StringFromWString(std::wstring(edit->GetText())).c_str(), (float)_wtof(value->GetText()));
        }
    }
    CDuiWindowBase::Notify(msg);
}
