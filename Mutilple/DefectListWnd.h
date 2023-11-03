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
    void ShowListData();      // 显示工艺文件到list
    void OnSelectelistItem(); // 点击
    // void OnBtnSaveTech();  //工艺保存
    void UpdateTechList(); // 更新文件工艺列表

    // void OnBtnLoadTech(); //调用工艺

    void InitDefectPos();

public:
    //	TECHLIST_TYPE  m_TechType;
    // vector<DB_DEFECT_DATA>  m_IDEFECTList; //工艺列表

    CListUI* m_pListDefectFileList;

    CLabelUI* m_pLabel11; // 控制列表有滚动条时 和表头和列表对齐

    ScanWnd*       m_pScanWnd;
    ReViewScanWnd* m_pReViewScanWnd;
    CButtonUI *    m_pBtnDefectReport, *m_pBtnDefectPrePare, *m_pBtnDefectPos, *m_pBtnDefectFinish;

    long m_lStart;
};
