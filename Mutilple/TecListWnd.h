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
    void ShowListData();      // ��ʾ�����ļ���list
    void OnSelectelistItem(); // �������
    void OnBtnSaveTech();     // ���ձ���
    void UpdateTechList();    // �����ļ������б�

    void OnBtnLoadTech();   // ���ù���
    void OnBtnDelectTech(); // ɾ������
public:
    TECHLIST_TYPE     m_TechType;
    CArray<FILE_INFO> m_ITechList; // �����б�

    CListUI* m_pListTechFileList;
    CEditUI* m_pEditTechName;

    CVerticalLayoutUI *m_pLayoutTechSave, *m_pLayoutReadRead;
};
