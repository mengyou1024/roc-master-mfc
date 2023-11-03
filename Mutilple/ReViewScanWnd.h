#pragma once

#include "OpenGL.h"

const int MESROW_RW = 7; // ��ʾ��Ϣ����
const int MESCOL_RW = 3; // ��

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

    void OnDefectList(); // ȱ���б�
    void UpdateLabelMes();
    void OnReport();
    void OnDeletReport(int type); // 0 �ϴ�ѡ�� 1.ɾ�����£�2ɾ��ȫ��
    void DeleteDir(CString str);
    BOOL IsDirExist(const CString csDir);
    void OpenWheelUPDownWnd(int type);

    void FindFolderFile_YM(CString strFoldername); // ����ָ��Ŀ¼�µ��ļ��� ����
    void FindFolderFile_D(CString strFoldername);  // ����ָ��Ŀ¼�µ��ļ��� ��
    void FindFolderFiles(CString strFoldername);   // ����ָ��Ŀ¼�µ��ļ�
    void ShowListData();                           // ��ʾ�����ļ���list
    void OnSelectelistItem_DB();                   // ˫��
    void OnSelectelistItem_LB();                   // ���

    // void OnBtnSaveTech();  //���ձ���
    void UpdateList(); // �����ļ������б�

    void UpdateAscanPoint(); // ���µ��Aɨ����
public:
    CWindowUI*     m_pWndOpenGL;
    DefectListWnd* m_pDefectListWnd; // ȱ���б���
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
