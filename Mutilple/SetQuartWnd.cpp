#include "pch.h"

#include "CheckWnd.h"
#include "Mutilple.h"
#include "SetQuartWnd.h"
#include "Techniques.h"
#include <cmath>
#include <functional>

constexpr auto TIMEOUT_TIME = 20000ULL;

float    SetQuartWnd::m_halfAmpGain = 0.0f;
float    SetQuartWnd::m_fullAmpGain = 0.0f;
uint64_t SetQuartWnd::m_timeoutTime = TIMEOUT_TIME;

class AbsPerformance {
public:
    /**
     * @brief 前处理
     *
     * @return CString 返回帮助提示
     */
    virtual CString pretreatment() {
        return _T("");
    }

    /**
     * @brief 测试
     *
     * @return true 通过测试
     * @return false 未通过测试
     */
    virtual bool test() {
        if (m_verifyType == LessEqual) {
            if (getResult() <= m_verifyValue) {
                return true;
            }
        } else {
            if (getResult() >= m_verifyValue) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief 后处理
     *
     */
    virtual void postprocessing() {}

    /**
     * @brief 获取结果
     *
     * @return 测试结果
     */
    virtual float getResult() {
        return m_result;
    }

    /**
     * @brief 获取错误字符串
     *
     * @return CString
     */
    virtual CString getErrString() {
        CString str;
        if (AbsPerformance::test() == false) {
            str.Format(_T("%s测量值为:%.1f, 标准为: %s %.1f, 点击确定重新测量, 点击取消使用测量值"), m_testName, m_result,
                       m_verifyType ? _T("≥") : _T("≤"), m_verifyValue);
        } else {
            str.Format(_T("确认探伤机是否连接，点击确定重新测量, 点击取消使用测量值"));
        }

        return str;
    }

    enum VerifyType {
        LessEqual    = 0,
        GreaterEqual = 1,
    } m_verifyType{LessEqual};

    wchar_t* m_testName    = nullptr;
    float    m_result      = 0.0f;
    float    m_verifyValue = 0.0f;
};

class ZeroPerformance : public AbsPerformance {
public:
    virtual CString pretreatment() override {
        auto channel              = g_MainProcess.GetCurChannel();
        channel->m_fGain          = 20.0f;
        channel->m_fRange         = 125.0f;
        channel->m_iVelocity      = 5920;
        channel->m_fOffset        = 0.0f;
        channel->m_fDelay         = 0.0f;
        channel->m_iDemodu        = 1;
        channel->m_pGatePos[0]    = 0.70f;
        channel->m_pGateWidth[0]  = 0.16f;
        channel->m_pGateHeight[0] = 0.5f;
        channel->m_pGateWidth[1]  = -0.0f;
        g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);

        return _T("测零点:\n探头放置CSK-IA试块距底面100mm处，然后点击确定");
    }

    virtual bool test() override {
        auto channel       = g_MainProcess.GetCurChannel();
        m_result           = (channel->m_pGateData[0].fPos - 100.0f) / 2.4f * 0.82f;
        channel->m_fOffset = m_result;
        g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
        return true;
    }
};

class HorLinearityPerformance : public AbsPerformance {
public:
    HorLinearityPerformance() {
        m_testName    = _T("水平线性");
        m_verifyValue = 2.0f;
        m_verifyType  = LessEqual;
    }
    virtual CString pretreatment() override {
        auto channel      = g_MainProcess.GetCurChannel();
        channel->m_fRange = 125.0f;
        channel->m_fDelay = 0;
        g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);

        return _T("测水平线性:\n探头放置CSK-IA试块距底面25mm处，然后点击确定");
    }

    virtual bool test() override {
        auto  channel            = g_MainProcess.GetCurChannel();
        float theoreticalValue[] = {25.0f, 50.0f, 75.0f, 100.0f, 125.0f};
        float gatePosValue[]     = {0.12f, 0.32f, 0.52f, 0.72f, 0.72f};
        float errMax             = 0.0f;
        for (int i = 0; i < 5; i++) {
            Sleep(500);
            if (i == 4) {
                channel->m_fDelay = 25.0f;
            }
            channel->m_pGatePos[0] = gatePosValue[i];
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
            Sleep(500);
            auto tempErr = std::abs(channel->m_pGateData[0].fPos + channel->m_fDelay - theoreticalValue[i]);
            if (errMax < tempErr) {
                errMax = tempErr;
            }
        }
        m_result = errMax / (0.8f * 1.5f);

        return AbsPerformance::test();
    }
};

class ResolutionPerformance : public AbsPerformance {
private:
    CString errors;

public:
    ResolutionPerformance() {
        m_testName    = _T("分辨力");
        m_verifyValue = 26.0f;
        m_verifyType  = GreaterEqual;
    }

    virtual CString getErrString() override {
        return errors;
    }

    virtual CString pretreatment() override {
        auto channel             = g_MainProcess.GetCurChannel();
        channel->m_fDelay        = 0.0f;
        channel->m_fRange        = 100.0f;
        channel->m_fGain         = 26.0f;
        channel->m_pGatePos[0]   = 0.82f;
        channel->m_pGateWidth[0] = 0.12f;
        g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
        return _T("测分辨力:\n在CSK-IA试块上移动探头，当85mm和91mm两处的回波波峰等高，然后点击确定");
    }

    virtual bool test() override {
        auto channel = g_MainProcess.GetCurChannel();

        float i       = 0.0f;
        float gainMin = 0.0f, gainMax = 0.0f;
        if (channel->m_pGateData[0].fAmp >= 0.2f) {
            i = -0.1f;
        } else {
            i = 0.1f;
        }
        auto _startTime = GetTickCount64();
        while (std::abs(channel->m_pGateData[0].fAmp - 0.2f) >= 0.005f) {
            channel->m_fGain += i;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
            Sleep(50);
            if (i > 0) {
                if (channel->m_pGateData[0].fAmp > 0.21f) {
                    break;
                }
            } else {
                if (channel->m_pGateData[0].fAmp < 0.19f) {
                    break;
                }
            }
            if (GetTickCount64() - _startTime > SetQuartWnd::m_timeoutTime) {
                return false;
            }
        }
        gainMin = channel->m_fGain;

        _startTime = GetTickCount64();

        while (1) {
            auto& readData = g_MainProcess.m_HDBridge.m_ReadData;
            auto& pAscan   = readData.pAscan[g_MainProcess.m_Techniques.m_iChannel];
            channel->m_fGain += 0.2f;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
            Sleep(50);
            // DONE: 找波谷
            int  negativeIndex = 0;
            int  left          = (int)(pAscan.size() * 0.85 /*channel->m_pGatePos[0]*/);
            int  right         = left + (int)(pAscan.size() * 0.06 /*channel->m_pGateWidth[0]*/);
            auto vIndex        = AMPD(pAscan.data() + left, right - left /*(int)(pAscan.size() * 0.15)*/);

            printf("vIndex.size():%llu\r\n", vIndex.size());
            if (vIndex.size() >= 1) {
                negativeIndex = vIndex[0] + left;
            }
            if (vIndex.size() == 1 && pAscan[negativeIndex] >= 0.2f * 255.0f) {
                break;
            }
            if (vIndex.size() == 0 && channel->m_fGain > 40.0f) {
                errors = _T("未找到波谷！点击确定重新测量，点击取消继续下一步");
                return false;
            }
            if (GetTickCount64() - _startTime > SetQuartWnd::m_timeoutTime) {
                errors = _T("超时！点击确定重新测量，点击取消继续下一步");
                return false;
            }
        }
        gainMax  = channel->m_fGain;
        m_result = gainMax - gainMin;

        return AbsPerformance::test();
    }

private:
    static int argmin(vector<int32_t>& data) {
        int min_index = 0;
        int min       = data[0];
        for (int i = 0; i < data.size(); i++) {
            if (data[i] < min) {
                min       = data[i];
                min_index = i;
            }
        }
        return min_index;
    }

    static vector<int> AMPD(uint8_t* data, int data_len) {
        vector<int32_t> p_data(data_len, 0);
        auto            count      = data_len;
        vector<int32_t> arr_rowsum = {};
        for (int k = 1; k <= count / 2; k++) {
            int32_t row_sum = 0;
            for (int i = k; i < count - k; i++) {
                if (((255 - data[i]) > (255 - data[i - k])) && ((255 - data[i]) > (255 - data[i + k]))) {
                    row_sum -= 1;
                }
            }
            arr_rowsum.push_back(row_sum);
        }
        auto min_index         = argmin(arr_rowsum);
        auto max_window_length = min_index;
        for (int k = 1; k <= max_window_length; k++) {
            for (int i = k; i < count - k; i++) {
                if (((255 - data[i]) > (255 - data[i - k])) && ((255 - data[i]) > (255 - data[i + k]))) {
                    p_data[i] += 1;
                }
            }
        }

        vector<int> ret = {};

        for (int i = 0; i < data_len; i++) {
            if (p_data[i] == max_window_length) {
                ret.push_back(i);
            }
        }
        return ret;
    }
};

class VerLinearityPerformance : public AbsPerformance {
public:
    VerLinearityPerformance() {
        m_testName    = _T("垂直线性");
        m_verifyValue = 6.0f;
        m_verifyType  = LessEqual;
    }

    virtual CString pretreatment() override {
        auto channel              = g_MainProcess.GetCurChannel();
        channel->m_fGain          = 40.0f;
        channel->m_fRange         = 250.f;
        channel->m_fOffset        = 0.0f;
        channel->m_iDemodu        = 1;
        channel->m_pGatePos[0]    = 0.75f;
        channel->m_pGateWidth[0]  = 0.1f;
        channel->m_pGateHeight[0] = 0.5f;
        channel->m_pGateWidth[1]  = -0.0f;
        g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);

        return _T("测垂直线性:\n将探头放置在DB-P Z20-2试块上，使200mm深Φ2平底孔的回波位于波门内，然后点击确定");
    }

    virtual bool test() override {
        auto  channel = g_MainProcess.GetCurChannel();
        float i       = 0.1f;
        float minErr = INFINITY, maxErr = -INFINITY;

        auto _startTime = GetTickCount64();
        while (std::abs(channel->m_pGateData[0].fAmp - 1.0f) >= 0.005f) {
            channel->m_fGain += i;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
            Sleep(50);
            if (std::abs(channel->m_pGateData[0].fAmp - 0.5f) <= 0.005f) {
                SetQuartWnd::m_halfAmpGain = channel->m_fGain;
            }
            if (std::abs(channel->m_pGateData[0].fAmp >= 1.0f)) {
                break;
            }
            if (GetTickCount64() - _startTime > SetQuartWnd::m_timeoutTime) {
                return false;
            }
        }
        SetQuartWnd::m_fullAmpGain = channel->m_fGain;

        i                                  = 2.0f;
        constexpr float theoreticalValue[] = {
            79.4f,
            63.1f,
            50.0f,
            39.8f,
            31.6f,
            25.1f,
            20.0f,
            15.8f,
            12.5f,
            10.0f,
            7.9f,
            6.3f,
            5.0f,
        };
        for (auto& _err : theoreticalValue) {
            channel->m_fGain -= i;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
            Sleep(1000);
            auto tempErr = channel->m_pGateData->fAmp * 100.f - _err;
            if (tempErr > maxErr) {
                maxErr = tempErr;
            }
            if (tempErr < minErr) {
                minErr = tempErr;
            }
        }
        m_result = maxErr - minErr;
        return AbsPerformance::test();
    }
};

class DynamicRangePerformance : public AbsPerformance {
private:
    float                  m_zeroAmpGain = 0.0f;
    constexpr static float zeroAmp       = 0.01f;

public:
    DynamicRangePerformance() {
        m_testName    = _T("动态范围");
        m_verifyValue = 26.0f;
        m_verifyType  = GreaterEqual;
    }

    virtual CString pretreatment() override {
        auto _startTime = GetTickCount64();
        auto channel    = g_MainProcess.GetCurChannel();
        while (channel->m_pGateData[0].fAmp >= zeroAmp) {
            channel->m_fGain -= 0.5f;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
            Sleep(100);
            if (GetTickCount64() - _startTime > SetQuartWnd::m_timeoutTime) {
                return false;
            }
        }
        m_zeroAmpGain = channel->m_fGain;
        return _T("测动态范围:\n直接点击确定");
    }

    virtual bool test() override {
        m_result = SetQuartWnd::m_fullAmpGain - m_zeroAmpGain;
        return AbsPerformance::test();
    }
};

class SensitivityPerformance : public AbsPerformance {
private:
    constexpr static float zeroAmp = 0.01f;

public:
    SensitivityPerformance() {
        m_testName    = _T("灵敏度余量");
        m_verifyValue = 42.0f;
        m_verifyType  = GreaterEqual;
    }

    virtual CString pretreatment() override {
        auto channel = g_MainProcess.GetCurChannel();

        channel->m_fGain = 110.0f;
        g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
        return _T("测灵敏度余量:\n拔掉探头，然后点击确定");
    }

    virtual bool test() override {
        auto channel = g_MainProcess.GetCurChannel();

        auto& readData = g_MainProcess.m_HDBridge.m_ReadData;
        auto& pAscan   = readData.pAscan[g_MainProcess.m_Techniques.m_iChannel];
        int   left     = (int)(pAscan.size() * 0.2);
        int   right    = left + (int)(pAscan.size() * 0.6);

        int sum = 0;
        for (int i = left; i < right; i++) {
            sum += pAscan[i];
        }
        float result_noise_v = (float)sum / (float)(right - left) / 255.f;
        // TODO: 记录噪声电平
        (void)result_noise_v;

        auto _startTime = GetTickCount64();

        struct {
            float x_last;
            float p_last;
            float Q;
            float R;
        } filter{0.0f, 0.0f, 0.02f, 0.01f};

        while (1) {
            channel->m_fGain -= 2.0f;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
            Sleep(500);
            auto& readData = g_MainProcess.m_HDBridge.m_ReadData;
            auto& pAscan   = readData.pAscan[g_MainProcess.m_Techniques.m_iChannel];
            int   left     = (int)(pAscan.size() * channel->m_pGatePos[0]);
            int   right    = left + (int)(pAscan.size() * channel->m_pGateWidth[0]);

            sum = 0;
            for (int i = left; i < right; i++) {
                sum += pAscan[i];
            }
            float result_noise_v = (float)sum / (float)(right - left) / 255.f;

            float real = (float)sum / (float)(right - left) / 255.f;

            float p_temp  = filter.p_last + filter.Q;
            float k       = p_temp / (p_temp + filter.R);
            filter.x_last = filter.x_last + k * (real - filter.x_last);
            filter.p_last = (1 - k) * p_temp;

            if (filter.x_last <= 0.1f) {
                break;
            }
            if (GetTickCount64() - _startTime > SetQuartWnd::m_timeoutTime) {
                return false;
            }
        }

        m_result = channel->m_fGain - SetQuartWnd::m_halfAmpGain;
        return AbsPerformance::test();
    }

    virtual void postprocessing() override {
        auto channel              = g_MainProcess.GetCurChannel();
        channel->m_fGain          = 15.0f;
        channel->m_fRange         = 125.0f;
        channel->m_fOffset        = 0.0f;
        channel->m_iDemodu        = 1;
        channel->m_pGatePos[0]    = 0.75f;
        channel->m_pGateWidth[0]  = 0.1f;
        channel->m_pGateHeight[0] = 0.5f;
        channel->m_pGateWidth[1]  = -0.0f;
        g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
    }
};

SetQuartWnd::~SetQuartWnd() {
    // 退出时，关闭采样
    g_MainProcess.m_Techniques.m_System.m_iChMode = 0x0fff0fff;
    g_MainProcess.m_HDBridge.StartSequencer(FALSE);
    m_timeoutTime = 0ULL;
    m_thread.Close();
    m_uiThread.Close();
}

void SetQuartWnd::InitOpenGL() {
    m_pWndOpenGL = static_cast<CWindowUI*>(m_PaintManager.FindControl(_T("WndOpenGL")));

    // 初始化OpenGL窗口
    m_OpenGL.Create(m_hWnd);
    m_OpenGL.Attach(m_pWndOpenGL);
    m_OpenGL.AddSetModel(&g_MainProcess.m_Techniques);
}

void SetQuartWnd::InitWindow() {
    CDuiWindowBase::InitWindow();
    InitOpenGL();
    g_MainProcess.m_Techniques.m_System.m_iChMode = 0x0fff0fff;
    // 设置板卡参数
    g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
    Sleep(100);
    g_MainProcess.m_HDBridge.StartSequencer(TRUE);
    if (m_thread.IsNull()) {
        m_timeoutTime = TIMEOUT_TIME;
        m_thread.Create(&QuarterlyPerformanceTestThread, this);
    }
    if (m_uiThread.IsNull()) {
        m_uiThread.Create(QuarterlyUiThread, this);
    }

    Sleep(100);
}

void SetQuartWnd::Notify(TNotifyUI& msg) {
    auto channel = g_MainProcess.GetCurChannel();
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        CDuiString strName = msg.pSender->GetName();
        if (strName == _T("BtnConfirm")) {
            if (m_performanceStatus == SetQuartWnd::PS_ERROR) {
                m_performanceStatus = SetQuartWnd::PS_PRE;
                return CDuiWindowBase::Notify(msg);
            }
            if (m_performanceStatus != SetQuartWnd::PS_IDLE) {
                return CDuiWindowBase::Notify(msg);
            }
            m_performanceStatus = m_nextPerformanceStatus;
        } else if (strName == _T("BtnCancel")) {
            if (m_performanceStatus == SetQuartWnd::PS_ERROR) {
                m_performanceStatus = SetQuartWnd::PS_POST;
                return CDuiWindowBase::Notify(msg);
            }
            if (m_performanceStatus != SetQuartWnd::PS_IDLE) {
                return CDuiWindowBase::Notify(msg);
            }
            m_performanceStatus = SetQuartWnd::PS_PRE;
        }
    } else if (msg.sType == DUI_MSGTYPE_ITEMSELECT) {
        CDuiString strName = msg.pSender->GetName();

        if (strName == _T("ComboChannel")) {
            auto comboChannel = dynamic_cast<CComboUI*>(msg.pSender);
            if (comboChannel) {
                g_MainProcess.m_Techniques.m_iChannel = comboChannel->GetCurSel();
            }
        }
    }
    g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
    CDuiWindowBase::Notify(msg);
}

static void AutoGetResult(SetQuartWnd* param, AbsPerformance* perf, float (AbsPerformance::*func)()) {
    if (dynamic_cast<HorLinearityPerformance*>(perf)) {
        param->m_dbQuarterData.m_nHorLinearity[g_MainProcess.m_Techniques.m_iChannel] = invoke(func, perf);
    } else if (dynamic_cast<ResolutionPerformance*>(perf)) {
        param->m_dbQuarterData.m_nDistinguishValuel[g_MainProcess.m_Techniques.m_iChannel] = invoke(func, perf);
    } else if (dynamic_cast<VerLinearityPerformance*>(perf)) {
        param->m_dbQuarterData.m_nVerLinearity[g_MainProcess.m_Techniques.m_iChannel] = invoke(func, perf);
    } else if (dynamic_cast<DynamicRangePerformance*>(perf)) {
        param->m_dbQuarterData.m_nDynamicRange[g_MainProcess.m_Techniques.m_iChannel] = invoke(func, perf);
    } else if (dynamic_cast<SensitivityPerformance*>(perf)) {
        param->m_dbQuarterData.m_nSensitivityMargin[g_MainProcess.m_Techniques.m_iChannel] = invoke(func, perf);
    } else {
    }
}

void SetQuartWnd::QuarterlyPerformanceTestThread(SetQuartWnd* param) {
    CString   helpString   = {};
    CTextUI*  textUI       = static_cast<CTextUI*>(param->m_PaintManager.FindControl(_T("TextHelp")));
    CComboUI* comboChannel = static_cast<CComboUI*>(param->m_PaintManager.FindControl(_T("ComboChannel")));
    bool      testRet      = false;

    auto zeroPerformance         = make_shared<ZeroPerformance>();
    auto horLinearityPerformance = make_shared<HorLinearityPerformance>();
    auto resolutionPerformance   = make_shared<ResolutionPerformance>();
    auto verLinearityPerformance = make_shared<VerLinearityPerformance>();
    auto dynamicRangePerformance = make_shared<DynamicRangePerformance>();
    auto sensitivityPerformance  = make_shared<SensitivityPerformance>();

    AbsPerformance* performanceList[] = {
        zeroPerformance.get(),
        horLinearityPerformance.get(),
        resolutionPerformance.get(),
        verLinearityPerformance.get(),
        dynamicRangePerformance.get(),
        sensitivityPerformance.get(),
    };
    int             performanceCount = 0;
    AbsPerformance* performancePtr   = performanceList[performanceCount];

    while (param->m_thread.m_bWorking) {
        switch (param->m_performanceStatus) {
            case SetQuartWnd::PS_PRE: {
                // 前处理
                textUI->SetText(performancePtr->pretreatment());
                param->m_nextPerformanceStatus = SetQuartWnd::PS_RUN;
                param->m_performanceStatus     = SetQuartWnd::PS_IDLE;
                break;
            }
            case SetQuartWnd::PS_RUN: {
                // 运行检测
                if (performancePtr->test()) {
                    param->m_performanceStatus = SetQuartWnd::PS_POST;
                } else {
                    param->m_performanceStatus = SetQuartWnd::PS_ERROR;
                }
                break;
            }
            case SetQuartWnd::PS_POST: {
                // 后处理
                performancePtr->postprocessing();
                // 获取结果
                AutoGetResult(param, performancePtr, &AbsPerformance::getResult);

                if (++performanceCount >= sizeof(performanceList) / sizeof(performanceList[0])) {
                    performanceCount = 0;
                    performancePtr   = performanceList[performanceCount];

                    if (g_MainProcess.m_Techniques.m_iChannel == 9) {
                        SYSTEMTIME stm;
                        GetLocalTime(&stm);
                        sprintf_s(param->m_dbQuarterData.m_dtTime, 11, "%d-%d-%d", stm.wYear, stm.wMonth, stm.wDay);
                        CFile   file;
                        CString fileName;
                        CString sdir;
                        sdir.Format(_T("%sData\\QuartPerformance\\%4d%02d\\%02d"), theApp.m_pExePath, stm.wYear, stm.wMonth, stm.wDay);
                        if (!PathIsDirectory(sdir)) {
                            if (!CreateMultipleDirectory(sdir)) {
                            }
                        }
                        fileName.Format(_T("%s\\%02d-%02d-%02d.qpdat"), sdir.GetString(), stm.wHour, stm.wMinute, stm.wSecond);
                        if (file.Open(fileName, CFile::modeCreate | CFile::modeWrite)) {
                            file.Write(&param->m_dbQuarterData, sizeof(param->m_dbQuarterData));
                            file.Close();
                        }

                        textUI->SetText(_T("所有通道测量完成，已输出季度检查文件"));
                        g_MainProcess.m_Techniques.m_iChannel = 0;
                        g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
                    } else {
                        CString str;
                        str.Format(_T("通道 [%d] 测量完成"), ++g_MainProcess.m_Techniques.m_iChannel);
                        textUI->SetText(str);
                    }

                    comboChannel->SelectItem(g_MainProcess.m_Techniques.m_iChannel);

                    param->m_nextPerformanceStatus = SetQuartWnd::PS_PRE;
                    param->m_performanceStatus     = SetQuartWnd::PS_IDLE;

                } else {
                    performancePtr             = performanceList[performanceCount];
                    param->m_performanceStatus = SetQuartWnd::PS_PRE;
                }

                break;
            }
            case SetQuartWnd::PS_ERROR: {
                textUI->SetText(performancePtr->getErrString());
                break;
            }
            case SetQuartWnd::PS_IDLE: {
                break;
            }
            default: break;
        }
        Sleep(0);
    }
}

void SetQuartWnd::QuarterlyUiThread(SetQuartWnd* param) {
    while (param->m_uiThread.m_bWorking) {
        auto label = static_cast<CLabelUI*>(param->m_PaintManager.FindControl(_T("LabelHorLinearity")));
        if (label) {
            float   data = param->m_dbQuarterData.m_nHorLinearity[g_MainProcess.m_Techniques.m_iChannel];
            CString str;
            str.Format(_T("%.1f"), data);
            label->SetText(str);
        }
        label = static_cast<CLabelUI*>(param->m_PaintManager.FindControl(_T("LabelResolution")));
        if (label) {
            float   data = param->m_dbQuarterData.m_nDistinguishValuel[g_MainProcess.m_Techniques.m_iChannel];
            CString str;
            str.Format(_T("%.1f"), data);
            label->SetText(str);
        }
        label = static_cast<CLabelUI*>(param->m_PaintManager.FindControl(_T("LabelVerLinearity")));
        if (label) {
            float   data = param->m_dbQuarterData.m_nVerLinearity[g_MainProcess.m_Techniques.m_iChannel];
            CString str;
            str.Format(_T("%.1f"), data);
            label->SetText(str);
        }
        label = static_cast<CLabelUI*>(param->m_PaintManager.FindControl(_T("LabelDynamicRange")));
        if (label) {
            float   data = param->m_dbQuarterData.m_nDynamicRange[g_MainProcess.m_Techniques.m_iChannel];
            CString str;
            str.Format(_T("%.1f"), data);
            label->SetText(str);
        }
        label = static_cast<CLabelUI*>(param->m_PaintManager.FindControl(_T("LabelSensitivity")));
        if (label) {
            float   data = param->m_dbQuarterData.m_nSensitivityMargin[g_MainProcess.m_Techniques.m_iChannel];
            CString str;
            str.Format(_T("%.1f"), data);
            label->SetText(str);
        }
        label = static_cast<CLabelUI*>(param->m_PaintManager.FindControl(_T("LabelLocation")));
        if (label) {
            float   data = g_MainProcess.GetCurChannel()->m_pGateData[GATE_A].fPos;
            CString str;
            str.Format(_T("%.1f"), data);
            label->SetText(str);
        }
        label = static_cast<CLabelUI*>(param->m_PaintManager.FindControl(_T("LabelHeight")));
        if (label) {
            float   data = g_MainProcess.GetCurChannel()->m_pGateData[GATE_A].fAmp * 100.f;
            CString str;
            str.Format(_T("%.1f"), data);
            label->SetText(str);
        }
        label = static_cast<CLabelUI*>(param->m_PaintManager.FindControl(_T("LabelGain")));
        if (label) {
            float   data = g_MainProcess.GetCurChannel()->m_fGain;
            CString str;
            str.Format(_T("%.1f"), data);
            label->SetText(str);
        }
        Sleep(200);
    }
}
