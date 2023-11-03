#pragma once

#include "DuiWindowBase.h"
#include "User.h"
#include <stack>

class UserWnd : public CDuiWindowBase {
public:
    UserWnd();

    virtual LPCTSTR GetWindowClassName() const {
        return _T("UserWnd");
    }

    virtual CDuiString GetSkinFile() override {
        return _T("Theme\\UI_UserWnd.xml");
    }

    virtual void InitWindow() override;
    virtual void Notify(TNotifyUI &msg) override;

private:
    User                         m_user{};
    vector<wchar_t *>            m_LayoutListName = {_T("LayoutSignIn"), _T("LayoutSignUp"), _T("LayoutManagement"), _T("LayoutModify")};
    int                          m_LayoutCursor   = 0;
    stack<CHorizontalLayoutUI *> m_LayoutStack    = {};

    void ChangeLayout(int val, bool noPush = false);
    void ChangeLayout(CString &&layoutName, bool noPush = false);
    void LayoutRollback(void);
    bool SignUpUser();
    bool ModifyUser();
    void ToModifyUserLayout();
    bool SignIn();
    void UpdateUserList();
    /**
     * @brief 检查CEdit控件是否为空
     *
     * @param ctrlName 控件ID
     * @param msg 如果为空显示的字符串
     * @return true: 不为空 false: 为空
     */
    bool CheckInputEmpty(CString &&ctrlName, CString &&msg);

    /**
     * @brief 检查CEdit控件是否含有中文
     *
     * @param ctrlName 控件ID
     * @param msg 如果含有中文显示的字符串
     * @return true: 不含中文 false: 含有中文
     */
    bool CheckInputChinese(CString &&ctrlName, CString &&msg);

    /**
     * @brief 检查CEdit控件是否含有特殊字符
     *
     * @param ctrlName 控件ID
     * @param msg 如果含有殊字符的字符串
     * @param checkList 检查列表
     * @return true: 不含特殊字符 false: 含特殊字符
     */
    bool CheckInputSpecial(CString &&ctrlName, CString &&msg, std::vector<wchar_t> checkList = {'\'', ' ', '\"'});
};
