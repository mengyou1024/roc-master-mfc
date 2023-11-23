#include "DetectionInformationEntryWnd.h"
#include "ModelAScan.h"
#include <regex>
#include <rttr/type.h>

using ORM_Model::DetectInfo;
using ORM_Model::JobGroup;
using ORM_Model::User;
using sqlite_orm::c;
using sqlite_orm::where;
using namespace rttr;

DetectionInformationEntryWnd::DetectionInformationEntryWnd(bool reviewView) : mReviewView(reviewView) {}

DetectionInformationEntryWnd::~DetectionInformationEntryWnd() {}

LPCTSTR DetectionInformationEntryWnd::GetWindowClassName() const {
    return _T("DetectionInformationEntryWnd");
}

CDuiString DetectionInformationEntryWnd::GetSkinFile() {
    return _T(R"(Theme\UI_DetectionInformationEntryWnd.xml)");
}

void DetectionInformationEntryWnd::InitWindow() {
    CDuiWindowBase::InitWindow();
    auto list     = static_cast<CListUI*>(m_PaintManager.FindControl(L"ListUserList"));
    auto userList = ORM_Model::User::storage().get_all<ORM_Model::User>();
    if (!mReviewView) {
        LoadUserGroupInfo();
    }
}

void DetectionInformationEntryWnd::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK && !mReviewView) {
        if (msg.pSender->GetName() == L"BtnUserAdd") {
            try {
                auto Edit = static_cast<CEditUI*>(m_PaintManager.FindControl(L"EditUserAdd"));
                if (!Edit->GetText().IsEmpty()) {
                    ORM_Model::User user = {};
                    user.name            = Edit->GetText();
                    ORM_Model::User::storage().insert(user);
                }
                LoadUserGroupInfo();
            } catch (std::exception& e) { spdlog::warn(e.what()); }
        } else if (msg.pSender->GetName() == L"BtnUserDel") {
            try {
                auto list = static_cast<CListUI*>(m_PaintManager.FindControl(L"ListUserList"));
                if (list->GetCurSel() >= 0) {
                    auto item = static_cast<CListTextElementUI*>(list->GetItemAt(list->GetCurSel()));
                    User::storage().remove_all<User>(where(c(&::User::name) == std::wstring(item->GetText(0))));
                }
                LoadUserGroupInfo();
            } catch (std::exception& e) { spdlog::warn(e.what()); }
        } else if (msg.pSender->GetName() == L"BtnJobGroupAdd") {
            try {
                auto Edit = static_cast<CEditUI*>(m_PaintManager.FindControl(L"EditJobGroupAdd"));
                if (!Edit->GetText().IsEmpty()) {
                    JobGroup jobgroup  = {};
                    jobgroup.groupName = Edit->GetText();
                    JobGroup::storage().insert(jobgroup);
                }
                LoadUserGroupInfo();
            } catch (std::exception& e) { spdlog::warn(e.what()); }
        } else if (msg.pSender->GetName() == L"BtnJobGroupDel") {
            try {
                auto list = static_cast<CListUI*>(m_PaintManager.FindControl(L"ListJobGroupList"));
                if (list->GetCurSel() >= 0) {
                    auto item = static_cast<CListTextElementUI*>(list->GetItemAt(list->GetCurSel()));
                    JobGroup::storage().remove_all<JobGroup>(where(c(&::JobGroup::groupName) == std::wstring(item->GetText(0))));
                }
                LoadUserGroupInfo();
            } catch (std::exception& e) { spdlog::warn(e.what()); }
        } else if (msg.pSender->GetName() == L"BtnOK") {
            auto list = static_cast<CListUI*>(m_PaintManager.FindControl(L"ListJobGroupList"));
            if (list->GetCurSel() >= 0) {
                auto item           = static_cast<CListTextElementUI*>(list->GetItemAt(list->GetCurSel()));
                mJobGroup.groupName = item->GetText(0);
            } else if (list->GetCount() > 0) {
                auto item           = static_cast<CListTextElementUI*>(list->GetItemAt(0));
                mJobGroup.groupName = item->GetText(0);
            } else {
                mJobGroup.groupName = _T(DB_DIRECTORIES_PREFIX);
            }
            list = static_cast<CListUI*>(m_PaintManager.FindControl(L"ListUserList"));
            if (list->GetCurSel() >= 0) {
                auto item  = static_cast<CListTextElementUI*>(list->GetItemAt(list->GetCurSel()));
                mUser.name = item->GetText(0);
            } else if (list->GetCount() > 0) {
                auto item  = static_cast<CListTextElementUI*>(list->GetItemAt(0));
                mUser.name = item->GetText(0);
            } else {
                mUser.name = _T("Î´ÃüÃû");
            }
            DetectInfo::storage().insert(mDetectinfo);
            mResult = true;
            Close();
        }
    } else if (msg.sType == DUI_MSGTYPE_TEXTCHANGED) {
        std::wregex  reg(L"Edit(.+)");
        std::wsmatch match;
        std::wstring str = msg.pSender->GetName();
        if (std::regex_match(str, match, reg)) {
            property prop = type::get(mDetectinfo).get_property(StringFromWString(match[1].str()));
            auto     edit = static_cast<CEditUI*>(msg.pSender);
            prop.set_value(mDetectinfo, std::wstring(edit->GetText()));
        }
    } else if (msg.sType == DUI_MSGTYPE_CLICK && msg.pSender->GetName() == L"BtnOK" && mReviewView) {
        Close();
    }
    CDuiWindowBase::Notify(msg);
}

const ORM_Model::JobGroup& DetectionInformationEntryWnd::GetJobGroup() const noexcept {
    return mJobGroup;
}

const ORM_Model::User& DetectionInformationEntryWnd::GetUser() const noexcept {
    return mUser;
}

const ORM_Model::DetectInfo& DetectionInformationEntryWnd::GetDetectInfo() const noexcept {
    return mDetectinfo;
}

bool DetectionInformationEntryWnd::GetResult() const noexcept {
    return mResult;
}

void DetectionInformationEntryWnd::LoadDetectInfo(const ORM_Model::DetectInfo& info, std::wstring userName, std::wstring groupName) {
    mDetectinfo = info;
    for (const auto& prot : type::get(mDetectinfo).get_properties()) {
        auto edit = dynamic_cast<CEditUI*>(m_PaintManager.FindControl(WStringFromString(string("Edit") + prot.get_name()).data()));
        if (edit) {
            edit->SetText(prot.get_value(mDetectinfo).convert<std::wstring>().data());
            edit->SetEnabled(false);
        }
    }
    auto list = static_cast<CListUI*>(m_PaintManager.FindControl(L"ListUserList"));
    list->RemoveAll();
    auto item = new CListTextElementUI;
    list->Add(item);
    item->SetText(0, userName.data());
    item->SetAttribute(L"textalign", L"center");
    list           = static_cast<CListUI*>(m_PaintManager.FindControl(L"ListJobGroupList"));
    auto jobGroups = ORM_Model::JobGroup::storage().get_all<ORM_Model::JobGroup>();
    list->RemoveAll();
    item = new CListTextElementUI;
    list->Add(item);
    item->SetText(0, groupName.data());
}

void DetectionInformationEntryWnd::LoadUserGroupInfo() {
    auto list = static_cast<CListUI*>(m_PaintManager.FindControl(L"ListUserList"));
    list->RemoveAll();
    auto userList = ORM_Model::User::storage().get_all<ORM_Model::User>();
    for (auto& user : userList) {
        auto item = new CListTextElementUI;
        list->Add(item);
        item->SetText(0, user.name.data());
        item->SetAttribute(L"textalign", L"center");
    }
    list           = static_cast<CListUI*>(m_PaintManager.FindControl(L"ListJobGroupList"));
    auto jobGroups = ORM_Model::JobGroup::storage().get_all<ORM_Model::JobGroup>();
    list->RemoveAll();
    for (auto& jobGroups : jobGroups) {
        auto item = new CListTextElementUI;
        list->Add(item);
        item->SetText(0, jobGroups.groupName.data());
    }
}
