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
    DB_QUARTER_DATA m_dbQuarterData = {}; ///< 季度性能测试结果

    enum PerformoranceStatus {
        PS_PRE,   ///< 前处理
        PS_RUN,   ///< 运行
        PS_POST,  ///< 后处理
        PS_ERROR, ///< 发生错误
        PS_IDLE,  ///< 空闲
    };
    PerformoranceStatus m_performanceStatus{PS_PRE}, m_nextPerformanceStatus{PS_IDLE};

    static float    m_halfAmpGain;
    static float    m_fullAmpGain;
    static uint64_t m_timeoutTime;

private:
    /**
     * @brief 季度性能测试线程
     *
     * @param param
     */
    static void QuarterlyPerformanceTestThread(SetQuartWnd* param);

    /**
     * @brief 季度性能UI线程
     *
     * @param param
     */
    static void QuarterlyUiThread(SetQuartWnd* param);
};
