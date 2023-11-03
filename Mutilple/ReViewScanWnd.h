#pragma once

#include "OpenGL.h"

const int MESROW_RW = 7; // 提示信息的行
const int MESCOL_RW = 3; // 列

class DefectListWnd;

class ReViewScanWnd : public CDuiWindowBase {
public:
    virtual ~ReViewScanWnd();
    LRESULT         HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LPCTSTR GetWindowClassName() const {
        return _T("ReViewScanWnd");
    }
    virtual CDuiString GetSkinFile() {
        return _T("Theme\\UI_ReViewScanWnd.xml");
    }

    void InitOpenGL();
    void InitWindow();
    void Init();

    void Notify(TNotifyUI& msg);
    void OnTimer(int iIdEvent);

    void OnLButtonDown(UINT nFlags, ::CPoint pt);
    void OnRButtonDown(UINT nFlags, ::CPoint pt);

    void OnDefectList(); // 缺陷列表
    void UpdateLabelMes();
    void OnReport();
    void OnDeletReport(int type); // 0 上次选中 1.删除整月，2删除全部
    void DeleteDir(CString str);
    BOOL IsDirExist(const CString csDir);
    void OpenWheelUPDownWnd(int type);

    void FindFolderFile_YM(CString strFoldername); // 查找指定目录下的文件夹 年月
    void FindFolderFile_D(CString strFoldername);  // 查找指定目录下的文件夹 天
    void FindFolderFiles(CString strFoldername);   // 查找指定目录下的文件
    void ShowListData();                           // 显示工艺文件到list
    void OnSelectelistItem_DB();                   // 双击
    void OnSelectelistItem_LB();                   // 点击

    // void OnBtnSaveTech();  //工艺保存
    void UpdateList(); // 更新文件工艺列表

    void UpdateAscanPoint(); // 跟新点击A扫数据
public:
    CWindowUI*     m_pWndOpenGL;
    DefectListWnd* m_pDefectListWnd; // 缺陷列表窗口
    CLabelUI *     m_pLabelData, *m_pLabelTime;

    CLabelUI* m_pCHName[HD_CHANNEL_NUM];

    CLabelUI *m_pLabelMesName[MESROW_RW][MESCOL_RW], *m_pLabelScanRes;

    CButtonUI *m_pBtnDefectList, *m_pBtnDeletSelect, *m_pBtnDeleteSelectMonth, *m_pBtnDeleteAll, *m_pBtnReport, *m_pIDCANCEL;

    CComboUI *m_pComboViewType, *m_pComboYearMonth, *m_pComboDay;

    CListUI*             m_pListDetectFileList;
    std::vector<CString> m_DetectFileList;

    CString m_strSelectFileName;

public:
    OpenGL m_OpenGL;
};
