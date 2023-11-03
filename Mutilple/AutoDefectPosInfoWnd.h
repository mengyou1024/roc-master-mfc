#pragma once

class AutoDefectPosInfoWnd : public CDuiWindowBase {
public:
    ~AutoDefectPosInfoWnd();
    virtual LPCTSTR GetWindowClassName() const {
        return _T("AutoDefectPosInfoWnd");
    }
    virtual CDuiString GetSkinFile() {
        return _T("Theme\\UI_AutoDefectPosInfoWnd.xml");
    }

    virtual void InitWindow();
    virtual void Notify(TNotifyUI& msg);

    virtual void OnTimer(int iIdEvent);
    void         SetCtrlEnable(bool bEnable);
    bool         OnEdit(void* pv);

public:
    CTextUI *m_pThreadText, *m_pSideText, *m_pTextMessage;

    COptionUI* m_pOptAuto;                      // 自动手动状态
    CButtonUI *m_pBtnQuickM22, *m_pBtnQuickM23; // 卡盘转动
    CButtonUI *m_pBtnQuickM33, *m_pBtnQuickM34; // 侧面移动 左右
    CButtonUI *m_pBtnQuickM55, *m_pBtnQuickM56; // 踏面移动 上下

    CButtonUI *m_pIDCONFIRMDPOS, *m_pIDCANCELDPOS;
    int        m_nType; // 操作命令类型

    long m_lStart;

public:
};
