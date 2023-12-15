#include "RecordSelectWnd.h"
#include "ModelAScan.h"
#include <BusyWnd.h>
#include <HDBridge/Utils.h>
#include <Model/ScanRecord.h>
#include <filesystem>
#include <iostream>
#include <regex>

using std::filesystem::directory_iterator;
using std::filesystem::file_type;
using std::filesystem::path;
namespace fs = std::filesystem;

using sqlite_orm::c;
using sqlite_orm::column;
using sqlite_orm::columns;
using sqlite_orm::where;

RecordSelectWnd::~RecordSelectWnd() {}

LPCTSTR RecordSelectWnd::GetWindowClassName() const {
    return _T("RecordSelectWnd");
}

CDuiString RecordSelectWnd::GetSkinFile() {
    return _T(R"(Theme\UI_RecordSelectWnd.xml)");
}

void RecordSelectWnd::InitWindow() {
    CDuiWindowBase::InitWindow();
    CenterWindow();
    LoadRecordUnique();
}

void RecordSelectWnd::Notify(TNotifyUI& msg) {
    OnNotifyUnique(msg);
    CDuiWindowBase::Notify(msg);
}

void RecordSelectWnd::OnNotifyUnique(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_ITEMSELECT) {
        if (msg.pSender->GetName() == L"ComboYearMonth") {
            auto pListDay = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboDay"));
            pListDay->RemoveAll();
            ListDay();
        } else if (msg.pSender->GetName() == L"ComboDay") {
            auto pListTime = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboTime"));
            pListTime->RemoveAll();
            ListTime();
        } else if (msg.pSender->GetName() == L"ComboTime") {
        }
    } else if (msg.sType == DUI_MSGTYPE_CLICK) {
        if (msg.pSender->GetName() == _T("closebtn")) {
            auto& [ret, str] = mResult;
            ret              = false;
            str              = "";
        } else if (msg.pSender->GetName() == _T("BtnOK")) {
            auto pListYearMonth = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboYearMonth"));
            auto pListDay       = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboDay"));
            auto pListTime      = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboTime"));
            if (pListTime->GetText().IsEmpty() || pListDay->GetText().IsEmpty() || pListYearMonth->GetText().IsEmpty()) {
                return;
            }
            auto& [ret, str] = mResult;
            ret              = true;
            str              = StringFromWString(_T(SCAN_DATA_DIR_NAME) + GetSystemConfig().groupName + L"/" +
                                                 std::wstring(pListYearMonth->GetText().GetData()) + L"/" + std::wstring(pListDay->GetText().GetData()) +
                                                 L"/" + std::wstring(pListTime->GetText().GetData()));
            Close();
        } else if (msg.pSender->GetName() == _T("BtnDEL")) {
            auto pListYearMonth = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboYearMonth"));
            auto pListDay       = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboDay"));
            auto pListTime      = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboTime"));
            if (pListDay->GetText().IsEmpty() || pListYearMonth->GetText().IsEmpty()) {
                return;
            }
            std::wstring yearMonthPath = std::wstring(L"./") + _T(SCAN_DATA_DIR_NAME) + GetSystemConfig().groupName + L"/" +
                                         std::wstring(pListYearMonth->GetText().GetData());
            std::wstring dayPath  = yearMonthPath + L"/" + std::wstring(pListDay->GetText().GetData());
            std::wstring timePath = dayPath + L"/" + std::wstring(pListTime->GetText().GetData());
            try {
                fs::remove(timePath);
                pListTime->RemoveAt(pListTime->GetCurSel());
                if (pListTime->GetCount() == 0) {
                    fs::remove(dayPath);
                    pListDay->RemoveAt(pListDay->GetCurSel());
                    if (pListDay->GetCount() == 0) {
                        fs::remove(yearMonthPath);
                        pListYearMonth->RemoveAt(pListYearMonth->GetCurSel());
                    }
                }
            } catch (std::exception& e) {
                try {
                    spdlog::warn(GB2312ToUtf8(e.what()));
                    DMessageBox(WStringFromString(string(e.what())).data());
                } catch (...) {}
            }

            auto YearMonth = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboYearMonth"));
            ListYearMonth();
        }
    }
}

RecordSelectWnd::TYPE_RES RecordSelectWnd::GetResult() {
    return mResult;
}

void RecordSelectWnd::LoadRecordUnique() const {
    ListYearMonth();
}

void RecordSelectWnd::ListYearMonth() const {
    try {
        auto pListYearMonth = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboYearMonth"));
        pListYearMonth->RemoveAll();
        std::wstring dirName = WStringFromString(string("./") + SCAN_DATA_DIR_NAME + GetJobGroup());
        for (auto& v : directory_iterator(dirName)) {
            auto fileName = v.path().filename().string();
            if (v.status().type() == file_type::directory) {
                auto fileName = v.path().filename().string();
                auto list     = new CListLabelElementUI;
                list->SetText(WStringFromString(fileName).data());
                pListYearMonth->Add(list);
            }
        }
        if (pListYearMonth->GetCount() > 0) {
            auto it = static_cast<CListLabelElementUI*>(pListYearMonth->GetItemAt(0));
            it->Select();
        }
    } catch (std::exception& e) {
        spdlog::error("file:{} line:{}", __FILE__, __LINE__);
        spdlog::error(GB2312ToUtf8(e.what()));
    }
}

void RecordSelectWnd::ListDay() const {
    try {
        auto         pList  = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboYearMonth"));
        std::wstring parent = pList->GetText();
        if (parent == L"") {
            return;
        }
        std::wstring dirName  = WStringFromString(string("./") + SCAN_DATA_DIR_NAME + GetJobGroup());
        auto         pListDay = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboDay"));
        pListDay->RemoveAll();
        for (auto& v : directory_iterator(dirName + L"/" + std::wstring(parent))) {
            auto fileName = v.path().filename().string();
            if (v.status().type() == file_type::directory) {
                auto fileName = v.path().filename().string();
                auto list     = new CListLabelElementUI;
                list->SetText(WStringFromString(fileName).data());
                pListDay->Add(list);
            }
        }
        if (pListDay->GetCount() > 0) {
            auto it = static_cast<CListLabelElementUI*>(pListDay->GetItemAt(0));
            it->Select();
        }
    } catch (std::exception& e) {
        spdlog::error("file:{} line:{}", __FILE__, __LINE__);
        spdlog::error(GB2312ToUtf8(e.what()));
    }
}

void RecordSelectWnd::ListTime() const {
    try {
        auto pListYearMonth = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboYearMonth"));
        auto pListDay       = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboDay"));
        if (pListYearMonth->GetText() == L"" || pListDay->GetText() == L"") {
            return;
        }
        std::wstring parent    = pListYearMonth->GetText() + L"/" + pListDay->GetText();
        std::wstring dirName   = WStringFromString(string("./") + SCAN_DATA_DIR_NAME + GetJobGroup());
        auto         pListTime = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboTime"));
        pListTime->RemoveAll();
        for (auto& v : directory_iterator(dirName + L"/" + std::wstring(parent))) {
            auto fileName = v.path().filename().string();
            if (v.status().type() == file_type::regular) {
                auto fileName = v.path().filename().string();
                auto list     = new CListLabelElementUI;
                list->SetText(WStringFromString(fileName).data());
                pListTime->Add(list);
            }
        }
        if (pListTime->GetCount() > 0) {
            auto it = static_cast<CListLabelElementUI*>(pListTime->GetItemAt(0));
            it->Select();
        }
    } catch (std::exception& e) {
        spdlog::error("file:{} line:{}", __FILE__, __LINE__);
        spdlog::error(GB2312ToUtf8(e.what()));
    }
}
