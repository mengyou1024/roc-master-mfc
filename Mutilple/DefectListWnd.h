#pragma once
#include "DuiWindowBase.h"
class ScanWnd;
class ReViewScanWnd;
class DefectListWnd :
public CDuiWindowBase {
public:
    DefectListWnd(void);
    ~DefectListWnd(void);

public:
    virtual LPCTSTR GetWindowClassName() const {
        return _T("DefectListWnd");
    }
    virtual CDuiString GetSkinFile() {
        return _T("Theme\\UI_DefectListWnd.xml");
    }
    virtual void OnTimer(int iIdEvent);
    virtual void InitWindow();
    void         Notify(TNotifyUI& msg);
    // void TecListWnd::OnEdit();
    bool OnEdit(void* pv);
    void ShowListData();      // ��ʾ�����ļ���list
    void OnSelectelistItem(); // ���
    // void OnBtnSaveTech();  //���ձ���
    void UpdateTechList(); // �����ļ������б�

    // void OnBtnLoadTech(); //���ù���

    void InitDefectPos();

public:
    //	TECHLIST_TYPE  m_TechType;
    // vector<DB_DEFECT_DATA>  m_IDEFECTList; //�����б�

    CListUI* m_pListDefectFileList;

    CLabelUI* m_pLabel11; // �����б��й�����ʱ �ͱ�ͷ���б����

    ScanWnd*       m_pScanWnd;
    ReViewScanWnd* m_pReViewScanWnd;
    CButtonUI *    m_pBtnDefectReport, *m_pBtnDefectPrePare, *m_pBtnDefectPos, *m_pBtnDefectFinish;

    long m_lStart;
};
