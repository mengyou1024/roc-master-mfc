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

    COptionUI* m_pOptAuto;                      // �Զ��ֶ�״̬
    CButtonUI *m_pBtnQuickM22, *m_pBtnQuickM23; // ����ת��
    CButtonUI *m_pBtnQuickM33, *m_pBtnQuickM34; // �����ƶ� ����
    CButtonUI *m_pBtnQuickM55, *m_pBtnQuickM56; // ̤���ƶ� ����

    CButtonUI *m_pIDCONFIRMDPOS, *m_pIDCANCELDPOS;
    int        m_nType; // ������������

    long m_lStart;

public:
};
