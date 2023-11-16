#include "RecordSelectWnd.h"
#include "ModelAScan.h"
#include <Model/ScanRecord.h>
#include <TBusyWnd.hpp>
#include <regex>
#include <HDBridge/Utils.h>

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
    LoadRecord();
}

void RecordSelectWnd::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_ITEMCLICK) {
        auto list = dynamic_cast<CListTextElementUI*>(msg.pSender);
        if (list) {
            auto& [_, str] = mResult;
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::wstring                                     time = list->GetText(1);
            std::replace(time.begin(), time.end(), L':', L'-');
            str = converter.to_bytes(std::wstring(list->GetText(0)) + L"__" + time);
        }
    } else if (msg.sType == DUI_MSGTYPE_CLICK) {
        if (msg.pSender->GetName() == _T("closebtn")) {
            auto& [ret, str] = mResult;
            ret              = false;
            str              = "";
        } else if (msg.pSender->GetName() == _T("BtnOK")) {
            auto pList       = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl(_T("ListRecordName")));
            if (pList->GetCurSel() < 0) {
                auto& [ret, str] = mResult;
                ret              = false;
            } else {
                auto& [ret, str] = mResult;
                ret              = true;
            }
            Close();
        } else if (msg.pSender->GetName() == _T("BtnDEL")) {
            auto pList = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl(_T("ListRecordName")));
            if (pList->GetCurSel() < 0) {
                goto __exit;
            }
            auto elm = static_cast<CListTextElementUI*>(pList->GetItemAt(pList->GetCurSel()));
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::string time = converter.to_bytes(std::wstring(elm->GetText(0)) + L"__" + std::wstring(elm->GetText(1)));
            std::replace(time.begin(), time.end(), ':', '-');
            try {
                TBusyWnd wnd([&pList, &time]() {
                    ORM_Model::ScanRecord::storage().remove_all<ORM_Model::ScanRecord>(where(c(&ORM_Model::ScanRecord::time) == time));
                    HD_Utils::storage().remove_all<HD_Utils>(where(c(&HD_Utils::time) == time));
                    pList->RemoveAt(pList->GetCurSel());    
                }); 
                wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
                wnd.ShowModal();

            } catch (std::exception& e) { spdlog::error(e.what()); }
        }

    }
__exit:
    CDuiWindowBase::Notify(msg);
}

RecordSelectWnd::TYPE_RES RecordSelectWnd::GetResult() {
    return mResult;
}

void RecordSelectWnd::LoadRecord() const {
    TBusyWnd wnd([this]() {
        auto list  = ORM_Model::ScanRecord::storage().get_all<ORM_Model::ScanRecord>();
        auto pList = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl(_T("ListRecordName")));
        pList->RemoveAll();
        for (auto& it : list) {
            std::wregex                                      matchReg(_T(R"((.+)__(.+))"));
            std::wsmatch                                     match;
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::wstring                                     str(converter.from_bytes(it.time));
            if (std::regex_match(str, match, matchReg)) {
                auto pLine = new CListTextElementUI;
                pLine->SetTag(it.id);
                pList->Add(pLine);
                std::wstring time = match[2].str().data();
                std::replace(time.begin(), time.end(), L'-', L':');
                pLine->SetText(0, match[1].str().data());
                pLine->SetText(1, match[2].str().data());
            }
        }
    });
    wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
    wnd.CenterWindow();
    wnd.ShowModal();
}
