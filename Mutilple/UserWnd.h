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
     * @brief ���CEdit�ؼ��Ƿ�Ϊ��
     *
     * @param ctrlName �ؼ�ID
     * @param msg ���Ϊ����ʾ���ַ���
     * @return true: ��Ϊ�� false: Ϊ��
     */
    bool CheckInputEmpty(CString &&ctrlName, CString &&msg);

    /**
     * @brief ���CEdit�ؼ��Ƿ�������
     *
     * @param ctrlName �ؼ�ID
     * @param msg �������������ʾ���ַ���
     * @return true: �������� false: ��������
     */
    bool CheckInputChinese(CString &&ctrlName, CString &&msg);

    /**
     * @brief ���CEdit�ؼ��Ƿ��������ַ�
     *
     * @param ctrlName �ؼ�ID
     * @param msg ����������ַ����ַ���
     * @param checkList ����б�
     * @return true: ���������ַ� false: �������ַ�
     */
    bool CheckInputSpecial(CString &&ctrlName, CString &&msg, std::vector<wchar_t> checkList = {'\'', ' ', '\"'});
};
