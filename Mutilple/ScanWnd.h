#pragma once

#include "OpenGL.h"

const int MESROW = 7; // ��ʾ��Ϣ����
const int MESCOL = 3; // ��
class DefectListWnd;

class ScanWnd : public CDuiWindowBase {
public:
    virtual ~ScanWnd();

    virtual LPCTSTR GetWindowClassName() const {
        return _T("ScanWnd");
    }
    virtual CDuiString GetSkinFile() {
        return _T("Theme\\UI_ScanWnd.xml");
    }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void    InitOpenGL();
    void    InitWindow();
    void    Init();
    void    OnBtnSpin(LPCTSTR lpName, float fValue);
    void    Notify(TNotifyUI& msg);
    void    OnTimer(int iIdEvent);

    void OnLButtonDown(UINT nFlags, ::CPoint pt);
    void OnRButtonDown(UINT nFlags, ::CPoint pt);
    void OnStartDailyScan(); // ��ʼɨ��
    void OnStartScan();      // ��ʼɨ��
    void OnStopScan();       // ��ʼɨ��

    void OnDefectList(); // ȱ���б�
    void UpdateLabelMes();

    void UpdateBtnEnable();
    // ɨ��״̬�ط�
    void SetScanReplay(bool bReplay);

    void ScanWnd::UpdateAscanPoint();
    void OpenWheelUPDownWnd(int type);

public:
    CWindowUI*     m_pWndOpenGL;
    DefectListWnd* m_pDefectListWnd; // ȱ���б���
    CLabelUI *     m_pLabelData, *m_pLabelTime;

    CEditUI *m_pEditGainSur[HD_CHANNEL_NUM], *m_pEditCHGain[HD_CHANNEL_NUM];
    CEditUI *m_pEditGainSoundSur[HD_CHANNEL_NUM], *m_pEditCHGainSound[HD_CHANNEL_NUM];

    CLabelUI* m_pCHName[HD_CHANNEL_NUM];

    CLabelUI *m_pLabelMesName[MESROW][MESCOL], *m_pLabelScanRes;
    CLabelUI* m_pLablelWheelValue[4];

    CButtonUI *m_pBtnWheelUp, *m_pBtnWheelDown, *m_pBtnDailyCheck, *m_pBtnScanStart, *m_pBtnScanStop, *m_pBtnDefectPos, *m_pBtnDefectScan, *m_pIDCANCEL;
    CButtonUI* m_pBtnChangeSequencer;

public:
    OpenGL m_OpenGL;

    int m_iBuffer;
};
