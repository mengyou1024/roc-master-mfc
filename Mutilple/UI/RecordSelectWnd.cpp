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
            str              = StringFromWString(L"DB/" + std::wstring(pListYearMonth->GetText().GetData()) + L"/" +
                                                 std::wstring(pListDay->GetText().GetData()) + L"/" + std::wstring(pListTime->GetText().GetData()));
            Close();
        } else if (msg.pSender->GetName() == _T("BtnDEL")) {
            auto pListYearMonth = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboYearMonth"));
            auto pListDay       = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboDay"));
            auto pListTime      = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboTime"));
            if (pListTime->GetText().IsEmpty() || pListDay->GetText().IsEmpty() || pListYearMonth->GetText().IsEmpty()) {
                return;
            }
            auto path =
                StringFromWString(L"DB/" + std::wstring(pListYearMonth->GetText().GetData()) + L"/" +
                                  std::wstring(pListDay->GetText().GetData()) + L"/" + std::wstring(pListTime->GetText().GetData()));
            pListTime->RemoveAt(pListTime->GetCurSel());
            fs::remove(path);
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
        for (auto& v : directory_iterator("./DB")) {
            auto fileName = v.path().filename().string();
            if (v.status().type() == file_type::directory) {
                auto fileName = v.path().filename().string();
                auto pList    = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboYearMonth"));
                auto list     = new CListLabelElementUI;
                list->SetText(WStringFromString(fileName).data());
                pList->Add(list);
            }
        }
        auto pListYearMonth = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboYearMonth"));
        if (pListYearMonth->GetCount() > 0) {
            auto it = static_cast<CListLabelElementUI*>(pListYearMonth->GetItemAt(0));
            it->Select();
        }
    } catch (std::exception& e) {
        spdlog::error("file:{} line:{}", __FILE__, __LINE__);
        spdlog::error(e.what());
    }
}

void RecordSelectWnd::ListDay() const {
    try {
        auto         pList  = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboYearMonth"));
        std::wstring parent = pList->GetText();
        if (parent == L"") {
            return;
        }
        for (auto& v : directory_iterator(string("./DB/") + StringFromWString(parent))) {
            auto fileName = v.path().filename().string();
            if (v.status().type() == file_type::directory) {
                auto fileName = v.path().filename().string();
                auto pList    = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboDay"));
                auto list     = new CListLabelElementUI;
                list->SetText(WStringFromString(fileName).data());
                pList->Add(list);
            }
        }
        auto pListDay = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboDay"));
        if (pListDay->GetCount() > 0) {
            auto it = static_cast<CListLabelElementUI*>(pListDay->GetItemAt(0));
            it->Select();
        }
    } catch (std::exception& e) {
        spdlog::error("file:{} line:{}", __FILE__, __LINE__);
        spdlog::error(e.what());
    }
}

void RecordSelectWnd::ListTime() const {
    try {
        auto pListYearMonth = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboYearMonth"));
        auto pListDay       = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboDay"));
        if (pListYearMonth->GetText() == L"" || pListDay->GetText() == L"") {
            return;
        }
        std::wstring parent = pListYearMonth->GetText() + L"/" + pListDay->GetText();
        for (auto& v : directory_iterator(string("./DB/") + StringFromWString(parent))) {
            auto fileName = v.path().filename().string();
            if (v.status().type() == file_type::regular) {
                auto fileName = v.path().filename().string();
                auto pList    = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboTime"));
                auto list     = new CListLabelElementUI;
                list->SetText(WStringFromString(fileName).data());
                pList->Add(list);
            }
        }
        auto pListTime = static_cast<CComboUI*>(m_PaintManager.FindControl(L"ComboTime"));
        if (pListTime->GetCount() > 0) {
            auto it = static_cast<CListLabelElementUI*>(pListTime->GetItemAt(0));
            it->Select();
        }
    } catch (std::exception& e) {
        spdlog::error("file:{} line:{}", __FILE__, __LINE__);
        spdlog::error(e.what());
    }
}
