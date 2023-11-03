#pragma once
#include "DuiWindowBase.h"

enum TECHLIST_TYPE {
    TECH_SAVE,
    TECH_READ,
};

class TecListWnd :

public CDuiWindowBase {
public:
    TecListWnd(void);
    ~TecListWnd(void);

public:
    virtual LPCTSTR GetWindowClassName() const {
        return _T("TecListWnd");
    }
    virtual CDuiString GetSkinFile() {
        return _T("Theme\\UI_TecListWnd.xml");
    }

    virtual void InitWindow();
    void         Notify(TNotifyUI& msg);
    // void TecListWnd::OnEdit();
    bool OnEdit(void* pv);
    void ShowListData();      // 显示工艺文件到list
    void OnSelectelistItem(); // 点击工艺
    void OnBtnSaveTech();     // 工艺保存
    void UpdateTechList();    // 更新文件工艺列表

    void OnBtnLoadTech();   // 调用工艺
    void OnBtnDelectTech(); // 删除工艺
public:
    TECHLIST_TYPE     m_TechType;
    CArray<FILE_INFO> m_ITechList; // 工艺列表

    CListUI* m_pListTechFileList;
    CEditUI* m_pEditTechName;

    CVerticalLayoutUI *m_pLayoutTechSave, *m_pLayoutReadRead;
};
