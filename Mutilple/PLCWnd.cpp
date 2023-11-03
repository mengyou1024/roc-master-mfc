#include "pch.h"

#include "Mutilple.h"
#include "PLCWnd.h"

constexpr ULONGLONG UISYNC_TICK = 200;

DWORD PLCWnd::SyncUIThread(void* param) {
    PLCWnd*          wnd     = static_cast<PLCWnd*>(param);
    CPaintManagerUI* manager = static_cast<CPaintManagerUI*>(&wnd->m_PaintManager);
    ULONGLONG        tick    = 0;
    wnd->m_SyncUiExit        = false;
    while (!wnd->m_SyncUiExit) {
        if (!wnd->m_IpAddr.empty()) {
            RuitiePLC::disconnect();
            Sleep(500);
            RuitiePLC::connectTo(wnd->m_IpAddr.c_str());
            wnd->m_IpAddr.clear();
        }
        if (GetTickCount64() > tick + UISYNC_TICK) {
            auto opt = static_cast<COptionUI*>(manager->FindControl(_T("OptIsConnected")));
            if (opt) {
                opt->Selected(RuitiePLC::isConnected());
            }

            auto _lSyncOption = [&manager](string name, uint8_t val) {
                CString indexStr = L"Opt";
                indexStr += name.c_str();
                auto opt = static_cast<COptionUI*>(manager->FindControl(indexStr));
                if (opt) {
                    opt->Selected(val);
                }
                indexStr = L"OptQuick";
                indexStr += name.c_str();
                opt = static_cast<COptionUI*>(manager->FindControl(indexStr));
                if (opt) {
                    opt->Selected(val);
                }
            };

            auto _lSyncLabel = [&manager](string name, float val) {
                CString indexStr = L"Label";
                indexStr += name.c_str();
                auto opt = static_cast<CLabelUI*>(manager->FindControl(indexStr));
                if (opt) {
                    CString value = L"";
                    value.Format(_T("%.5f"), val);
                    opt->SetText(value);
                }
                indexStr = L"LabelQuick";
                indexStr += name.c_str();
                opt = static_cast<CLabelUI*>(manager->FindControl(indexStr));
                if (opt) {
                    CString value = L"";
                    value.Format(_T("%.4f"), val);
                    opt->SetText(value);
                }
            };

            RuitiePLC::getVariable(_lSyncLabel);
            RuitiePLC::getMRegion(_lSyncOption);
            RuitiePLC::getInput(_lSyncOption);
            RuitiePLC::getOutput(_lSyncOption);

            tick = GetTickCount64();
        }
        Sleep(0);
    }
    return 0;
}

PLCWnd::~PLCWnd() {
    auto tick    = GetTickCount64();
    m_SyncUiExit = true;
    // 恢复同步线程
    ResumeThread(m_SyncThreadHandle);
    // 等待线程退出
    WaitForSingleObject(m_SyncThreadHandle, 5000);
    m_SyncThreadHandle = NULL;
}

void PLCWnd::ResumeSyncThread(void) {
    if (m_SyncThreadHandle) {
        ResumeThread(m_SyncThreadHandle);
    }
}

void PLCWnd::InitWindow() {
    CDuiWindowBase::InitWindow();
    auto   ui = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditIpAddr")));
    string ip = "";
    RuitiePLC::getConnectedInfo(&ip);
    ui->SetText(CString(ip.c_str()));
    if (!m_SyncThreadHandle) {
        m_SyncThreadHandle = CreateThread(NULL, 0, SyncUIThread, this, 0, NULL);
        for (auto& it : m_SelfLockKeyMap) {
            CString findStr = _T("Btn");
            CString findBtn = findStr + it.first.c_str();
            auto    ui      = static_cast<CButtonUI*>(m_PaintManager.FindControl(findBtn));
            if (ui) {
                ui->SetClickType(!it.second);
            }
            CString findQuickBtn = findStr + "Quick" + it.first.c_str();
            ui                   = static_cast<CButtonUI*>(m_PaintManager.FindControl(findQuickBtn));
            if (ui) {
                ui->SetClickType(!it.second);
            }
        }
    }
}

void PLCWnd::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK_DOWN || msg.sType == DUI_MSGTYPE_CLICK_UP) {
        auto   cStrName   = msg.pSender->GetName();
        string stdStrName = StringFromLPCTSTR(cStrName.GetData());
        string vName      = stdStrName.substr(3);
        if (vName.rfind("Quick", 0) == 0) {
            vName = vName.substr(5);
        }
        CString optStr = L"Opt";
        optStr += vName.c_str();
        if (msg.sType == DUI_MSGTYPE_CLICK_DOWN) {
            RuitiePLC::setVariable(vName, true);
        } else if (msg.sType == DUI_MSGTYPE_CLICK_UP) {
            RuitiePLC::setVariable(vName, false);
        }
    } else if (msg.sType == DUI_MSGTYPE_CLICK) {
        auto   cStrName   = msg.pSender->GetName();
        string stdStrName = StringFromLPCTSTR(cStrName.GetData());
        string vName      = stdStrName.substr(3);
        if (vName.rfind("Quick", 0) == 0) {
            vName = vName.substr(5);
        }
        CString optStr = L"Opt";
        optStr += vName.c_str();
        if (m_SelfLockKeyMap[vName]) {
            auto optionUi = static_cast<COptionUI*>(m_PaintManager.FindControl(optStr));
            if (optionUi) {
                bool opt = optionUi->IsSelected();
                RuitiePLC::setVariable(vName, !opt);
            } else {
                RuitiePLC::setVariable(vName, true);
            }
        }

        if (cStrName == _T("hidenBtn")) {
            // 隐藏窗口
            ShowWindow(SW_HIDE);
            // 挂起同步线程
            SuspendThread(m_SyncThreadHandle);
        } else if (cStrName == _T("BtnSetIpAddr")) {
            // 重新链接的IP
            auto ui  = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditIpAddr")));
            m_IpAddr = StringFromLPCTSTR(ui->GetText().GetData());
        } else if (cStrName == _T("BtnChangeLayout")) {
            // 切换布局
            auto ui = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("LayoutTable")));
            if (ui) {
                ui->SetVisible(!ui->IsVisible());
            }
            ui = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("LayoutQuick")));
            if (ui) {
                ui->SetVisible(!ui->IsVisible());
            }
        }
    }

    if (msg.sType == DUI_MSGTYPE_RETURN) {
        auto   cStrName   = msg.pSender->GetName();
        string stdStrName = StringFromLPCTSTR(cStrName.GetData());
        string vName      = stdStrName.substr(4);
        if (vName.rfind("Quick", 0) == 0) {
            vName = vName.substr(5);
        }
        float fValue = 0;
        if (_stscanf_s(msg.pSender->GetText().GetData(), _T("%f"), &fValue) > 0) {
            RuitiePLC::setVariable(vName, fValue);
        }
    }
    CDuiWindowBase::Notify(msg);
}
