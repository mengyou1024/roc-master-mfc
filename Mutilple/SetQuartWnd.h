#pragma once

#include "OpenGL.h"

#include "Define.h"

class SetQuartWnd : public CDuiWindowBase {
public:
    ~SetQuartWnd();

    virtual LPCTSTR GetWindowClassName() const {
        return _T("SetQuartWnd");
    }
    virtual CDuiString GetSkinFile() {
        return _T("Theme\\UI_SetQuartWnd.xml");
    }

    void InitOpenGL();
    void InitWindow();
    void Notify(TNotifyUI& msg);

    CWindowUI*      m_pWndOpenGL;
    OpenGL          m_OpenGL;
    Thread          m_thread;
    Thread          m_uiThread;
    DB_QUARTER_DATA m_dbQuarterData = {}; ///< �������ܲ��Խ��

    enum PerformoranceStatus {
        PS_PRE,   ///< ǰ����
        PS_RUN,   ///< ����
        PS_POST,  ///< ����
        PS_ERROR, ///< ��������
        PS_IDLE,  ///< ����
    };
    PerformoranceStatus m_performanceStatus{PS_PRE}, m_nextPerformanceStatus{PS_IDLE};

    static float    m_halfAmpGain;
    static float    m_fullAmpGain;
    static uint64_t m_timeoutTime;

private:
    /**
     * @brief �������ܲ����߳�
     *
     * @param param
     */
    static void QuarterlyPerformanceTestThread(SetQuartWnd* param);

    /**
     * @brief ��������UI�߳�
     *
     * @param param
     */
    static void QuarterlyUiThread(SetQuartWnd* param);
};
