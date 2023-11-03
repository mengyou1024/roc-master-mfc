#include "pch.h"

#include "UserWnd.h"

#define CHECK_EMPTY(x, m)                     \
    do {                                      \
        if (CheckInputEmpty(x, m) == false) { \
            return false;                     \
        }                                     \
    } while (0)

#define CHECK_CHINESE(x, m)                     \
    do {                                        \
        if (CheckInputChinese(x, m) == false) { \
            return false;                       \
        }                                       \
    } while (0)

#define CHECK_SPECIAL(x, m)                     \
    do {                                        \
        if (CheckInputSpecial(x, m) == false) { \
            return false;                       \
        }                                       \
    } while (0)

UserWnd::UserWnd() {
    m_user.loadUser();
}

void sFmtData(DB_USER_DATA &data, const WCHAR *name, const WCHAR *number, const WCHAR *pwd, int level, const WCHAR *remark) {
    wsprintf(data.strName, name);
    wsprintf(data.strNumber, number);
    wsprintf(data.strPwd, pwd);
    data.lLevel = level;
    wsprintf(data.strRemark, remark);
}

void UserWnd::InitWindow() {
    auto pList = static_cast<DuiLib::CListUI *>(m_PaintManager.FindControl(_T("ListUser")));
    for (auto &item : m_user.m_pUserList) {
        auto pLine = new CListTextElementUI;
        pLine->SetTag(item.lSerial);
        pList->Add(pLine);
        CString id;
        id.Format(_T("%d"), item.lSerial);
        pLine->SetText(0, id.GetString());
        pLine->SetText(1, item.strName);
        pLine->SetText(2, item.strNumber);
        pLine->SetText(3, item.strRemark);
    }
    if (m_user.m_isLogin == true) {
        ChangeLayout(_T("LayoutManagement"));
    } else {
        ChangeLayout(_T("LayoutSignIn"));
        static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditSignInNameOrNumber")))->SetText(m_user.mLogionUser.strName);
        static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditSignInPwd")))->SetText(m_user.mLogionUser.strPwd);
    }
    CDuiWindowBase::InitWindow();
}

void UserWnd::Notify(TNotifyUI &msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        auto strName = msg.pSender->GetName();
        if (strName == _T("BtnLayoutBack")) {
            LayoutRollback();
        } else if (strName == _T("BtnSignInSignUp")) {
            ChangeLayout(_T("LayoutSignUp"));
            static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditSignUpName")))->SetText(_T(""));
            static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditSignUpNumber")))->SetText(_T(""));
            static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditSignUpPwd")))->SetText(_T(""));
            static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditSignUpPwdVerify")))->SetText(_T(""));
            static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditSignUpRemark")))->SetText(_T(""));
        } else if (strName == _T("BtnSignUpSignUp")) {
            if (SignUpUser()) {
                LayoutRollback();
            }
        } else if (strName == _T("BtnSignInLogin")) {
            if (SignIn()) {
                m_user.m_isLogin = true;
                ChangeLayout(_T("LayoutManagement"), true);
                while (!m_LayoutStack.empty()) {
                    m_LayoutStack.pop();
                }
            }
        } else if (strName == _T("BtnAddUser")) {
            ChangeLayout(_T("LayoutSignUp"));
        } else if (strName == _T("BtnDelUser")) {
            auto listUi = static_cast<CListUI *>(m_PaintManager.FindControl(_T("ListUser")));
            if (listUi->GetCurSel() >= 0) {
                auto item = static_cast<CListTextElementUI *>(listUi->GetItemAt(listUi->GetCurSel()));
                if (m_user.del(_wtol(item->GetText(0)))) {
                    listUi->RemoveAt(listUi->GetCurSel());
                }
            }
        } else if (strName == _T("BtnModUser")) {
            ToModifyUserLayout();
        } else if (strName == _T("BtnSignOut")) {
            m_user.m_isLogin = false;
            ChangeLayout(_T("LayoutSignIn"), true);
        } else if (strName == _T("BtnModifyConfirm")) {
            if (ModifyUser()) {
                LayoutRollback();
            }
        }
    }
    CDuiWindowBase::Notify(msg);
}

void UserWnd::ChangeLayout(int val, bool noPush) {
    if (val >= m_LayoutListName.size()) {
        return;
    }
    ChangeLayout(m_LayoutListName[val], noPush);
}

void UserWnd::ChangeLayout(CString &&layoutName, bool noPush) {
    auto layout = static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(layoutName));
    if (!layout || layout->IsVisible()) {
        return;
    }
    for (auto &item : m_LayoutListName) {
        auto layout = static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(item));
        if (layout && layout->IsVisible()) {
            if (!noPush) {
                m_LayoutStack.push(layout);
            }
            layout->SetVisible(false);
        }
    }
    layout = static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(layoutName));
    if (layout) {
        layout->SetVisible(true);
    }
    if (layoutName == _T("LayoutManagement")) {
        UpdateUserList();
    }
}

void UserWnd::LayoutRollback(void) {
    if (m_LayoutStack.empty()) {
        return;
    }
    for (auto &item : m_LayoutListName) {
        auto layout = static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(item));
        if (layout && layout->IsVisible()) {
            layout->SetVisible(false);
        }
    }
    auto layout = m_LayoutStack.top();
    if (layout) {
        layout->SetVisible(true);
    }
    m_LayoutStack.pop();
    if (layout == static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(_T("LayoutManagement")))) {
        UpdateUserList();
    }
}

bool UserWnd::SignUpUser() {
    DB_USER_DATA newUser = {0};

    vector<pair<CString, CString>> check = {
        {_T("EditSignUpName"),      _T("姓名不能为空")      },
        {_T("EditSignUpNumber"),    _T("工号不能为空")      },
        {_T("EditSignUpPwd"),       _T("密码不能为空")      },
        {_T("EditSignUpPwdVerify"), _T("确认密码不能为空")},
    };

    for (auto &item : check) {
        CHECK_EMPTY(std::move(item.first), std::move(item.second));
    }

    check = {
        {_T("EditSignUpName"),   _T("姓名不能含有{空格, \' , \"}")},
        {_T("EditSignUpNumber"), _T("工号不能含有{空格, \' , \"}")},
        {_T("EditSignUpPwd"),    _T("密码不能含有{空格, \' , \"}")},
    };

    for (auto &item : check) {
        CHECK_SPECIAL(std::move(item.first), std::move(item.second));
    }

    check = {
        {_T("EditSignUpNumber"),    _T("工号不能含有中文")      },
        {_T("EditSignUpPwd"),       _T("密码不能含有中文")      },
        {_T("EditSignUpPwdVerify"), _T("确认密码不能含有中文")},
    };

    for (auto &item : check) {
        CHECK_CHINESE(std::move(item.first), std::move(item.second));
    }
    wsprintfW(newUser.strName, static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditSignUpName")))->GetText().GetData());
    wsprintfW(newUser.strNumber, static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditSignUpNumber")))->GetText().GetData());
    wsprintfW(newUser.strPwd, static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditSignUpPwd")))->GetText().GetData());
    wsprintfW(newUser.strRemark, static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditSignUpRemark")))->GetText().GetData());
    newUser.lLevel = 0;
    if (StrCmpW(static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditSignUpPwd")))->GetText().GetData(),
                static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditSignUpPwdVerify")))->GetText().GetData()) == 0) {
        if (!m_user.add(newUser)) {
            DMessageBox(_T("注册失败，工号或姓名已经被使用"));
            return false;
        }
        return true;
    } else {
        DMessageBox(_T("两次密码不一致"));
    }
    return false;
}

bool UserWnd::SignIn() {
    vector<pair<CString, CString>> check = {
        {_T("EditSignInNameOrNumber"), _T("姓名或工号不能为空")},
        {_T("EditSignInPwd"),          _T("密码不能为空")         },
    };

    for (auto &item : check) {
        CHECK_EMPTY(std::move(item.first), std::move(item.second));
    }

    check = {
        {_T("EditSignInNameOrNumber"), _T("姓名或工号不能含有{空格, \' , \"}")},
        {_T("EditSignInPwd"),          _T("密码不能不能含有{空格, \' , \"}")   },
    };

    for (auto &item : check) {
        CHECK_SPECIAL(std::move(item.first), std::move(item.second));
    }

    const wchar_t *input_number = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditSignInNameOrNumber")))->GetText().GetData();

    DB_USER_DATA user = {};
    user              = m_user.findUserByNumber(input_number);
    if (user.lSerial == 0) {
        wchar_t name[22] = {};
        wsprintfW(name, static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditSignInNameOrNumber")))->GetText().GetData());
        user = m_user.findUser(name);
    }

    if (user.lSerial == 0) {
        DMessageBox(_T("工号/姓名不存在"));
        return false;
    }

    if (StrCmpNW(user.strPwd, static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditSignInPwd")))->GetText().GetData(), sizeof(user.strPwd) / 2) == 0) {
        memcpy(&m_user.mLogionUser, &user, sizeof(DB_USER_DATA));
        return true;
    } else {
        wprintf_s(_T("%s != %s"), user.strPwd, static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditSignInPwd")))->GetText().GetData());
        DMessageBox(_T("工号/姓名或者密码不正确"));
    }
    return false;
}

bool UserWnd::ModifyUser() {
    auto listUi = static_cast<CListUI *>(m_PaintManager.FindControl(_T("ListUser")));
    if (listUi->GetCurSel() < 0) {
        return false;
    }
    auto         item = static_cast<CListTextElementUI *>(listUi->GetItemAt(listUi->GetCurSel()));
    DB_USER_DATA user = m_user.findUserByNumber(item->GetText(2));

    vector<pair<CString, CString>> check = {
        {_T("EditModifyName"),      _T("姓名不能为空")      },
        {_T("EditModifyPwd"),       _T("密码不能为空")      },
        {_T("EditModifyPwdVerify"), _T("确认密码不能为空")},
    };

    for (auto &item : check) {
        CHECK_EMPTY(std::move(item.first), std::move(item.second));
    }

    check = {
        {_T("EditModifyName"),      _T("姓名不能含有{空格, \' , \"}")      },
        {_T("EditModifyPwd"),       _T("密码不能含有{空格, \' , \"}")      },
        {_T("EditModifyPwdVerify"), _T("确认密码不能含有{空格, \' , \"}")},
    };

    for (auto &item : check) {
        CHECK_SPECIAL(std::move(item.first), std::move(item.second));
    }

    check = {
        {_T("EditModifyPwd"),       _T("密码不能含有中文")      },
        {_T("EditModifyPwdVerify"), _T("确认密码不能含有中文")},
    };

    for (auto &item : check) {
        CHECK_CHINESE(std::move(item.first), std::move(item.second));
    }

    if (StrCmpNW(
            static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditModifyOriPwd")))->GetText().GetData(),
            user.strPwd, sizeof(user.strPwd) / 2) != 0) {
        DMessageBox(_T("原密码不正确"));
        return false;
    }

    wsprintfW(user.strName, static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditModifyName")))->GetText().GetData());
    wsprintfW(user.strPwd, static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditModifyPwd")))->GetText().GetData());
    wsprintfW(user.strRemark, static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditModifyRemark")))->GetText().GetData());
    if (StrCmpW(static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditModifyPwd")))->GetText().GetData(),
                static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditModifyPwdVerify")))->GetText().GetData()) == 0) {
        if (StrCmpNW(static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditModifyPwd")))->GetText().GetData(), user.strPwd, sizeof(user.strPwd) / 2) == 0) {
            DMessageBox(_T("新密码不能与旧密码相同"));
            return false;
        }
        if (m_user.modify(user)) {
            return true;
        }
        DMessageBox(_T("修改失败"));
    } else {
        DMessageBox(_T("两次密码不一致"));
    }

    return false;
}

void UserWnd::ToModifyUserLayout() {
    auto listUi = static_cast<CListUI *>(m_PaintManager.FindControl(_T("ListUser")));
    auto item   = static_cast<CListTextElementUI *>(listUi->GetItemAt(listUi->GetCurSel()));
    if (listUi->GetCurSel() < 0) {
        return;
    }
    DB_USER_DATA user = m_user.findUserByNumber(item->GetText(2));
    auto         edit = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditModifyName")));
    edit->SetText(user.strName);
    ChangeLayout(_T("LayoutModify"));
}

void UserWnd::UpdateUserList() {
    m_user.loadUser();
    auto pList = static_cast<DuiLib::CListUI *>(m_PaintManager.FindControl(_T("ListUser")));
    pList->RemoveAll();
    for (auto &item : m_user.m_pUserList) {
        auto pLine = new CListTextElementUI;
        pLine->SetTag(item.lSerial);
        pList->Add(pLine);
        CString id;
        id.Format(_T("%d"), item.lSerial);
        pLine->SetText(0, id.GetString());
        pLine->SetText(1, item.strName);
        pLine->SetText(2, item.strNumber);
        pLine->SetText(3, item.strRemark);
    }
}

bool UserWnd::CheckInputEmpty(CString &&ctrlName, CString &&msg) {
    if (static_cast<CEditUI *>(m_PaintManager.FindControl(ctrlName.GetString()))->GetText().IsEmpty()) {
        DMessageBox(msg.GetString());
        return false;
    }
    return true;
}

bool UserWnd::CheckInputChinese(CString &&ctrlName, CString &&msg) {
    if (IncChinese(static_cast<CEditUI *>(m_PaintManager.FindControl(ctrlName.GetString()))->GetText().GetData())) {
        DMessageBox(msg.GetString());
        return false;
    }
    return true;
}

bool UserWnd::CheckInputSpecial(CString &&ctrlName, CString &&msg, std::vector<wchar_t> checkList) {
    auto str = static_cast<CEditUI *>(m_PaintManager.FindControl(ctrlName.GetString()))->GetText();
    int  len = str.GetLength();

    for (int i = 0; i != len; i++) {
        auto ch = str[i];
        for (auto &check : checkList) {
            if (check == ch) {
                DMessageBox(msg.GetString());
                return false;
            }
        }
    }

    return true;
}
