#pragma once

#include "DuiWindowBase.h"
#include "OpenGL.h"
#include <map>

#pragma push_macro("GATE_A")
#pragma push_macro("GATE_B")

#undef GATE_A
#undef GATE_B

class GroupScanWnd : public CDuiWindowBase {
public:
    virtual ~GroupScanWnd();

    virtual LPCTSTR GetWindowClassName() const override {
        return _T("GroupScanWnd");
    }
    virtual CDuiString GetSkinFile() noexcept override {
        return _T("Theme\\UI_GroupScanWnd.xml");
    }

    virtual void InitWindow() override;
    virtual void Notify(TNotifyUI& msg) override;

    virtual void OnLButtonDown(UINT nFlags, ::CPoint pt) override;

private:
    /// 配置类型
    enum class ConfigType {
        DetectRange = 0,
        Gain,
        GateStart,
        GateWidth,
        GateHeight,
    };

    /// 波门类型
    enum class GateType {
        GATE_A = 0,
        GATE_B,
    };

    /// 通道选择
    enum class ChannelSel {
        CHANNEL_1 = 0,
        CHANNEL_2,
        CHANNEL_3,
        CHANNEL_4,
    };

    /// Edit控件显示的单位文本
    const static inline std::map<ConfigType, CString> mConfigTextext = {
        {ConfigType::DetectRange, _T("")  },
        {ConfigType::Gain,        _T("dB")},
        {ConfigType::GateStart,   _T("mm")},
        {ConfigType::GateWidth,   _T("mm")},
        {ConfigType::GateHeight,  _T("mm")},
    };

    const static inline std::map<ConfigType, std::wstring> mConfigRegex = {
        {ConfigType::DetectRange, _T(R"((-?[1-9](\d+)?\.?(\d{0,2})|-?0\.(\d{0,2}))|-0|-?\.\d{0,2}|-|0)")},
        {ConfigType::Gain,        _T(R"((-?[1-9](\d+)?\.?(\d{0,2})|-?0\.(\d{0,2}))|-0|-?\.\d{0,2}|-|0)")},
        {ConfigType::GateStart,   _T(R"((-?[1-9](\d+)?\.?(\d{0,2})|-?0\.(\d{0,2}))|-0|-?\.\d{0,2}|-|0)")},
        {ConfigType::GateWidth,   _T(R"((-?[1-9](\d+)?\.?(\d{0,2})|-?0\.(\d{0,2}))|-0|-?\.\d{0,2}|-|0)")},
        {ConfigType::GateHeight,  _T(R"((-?[1-9](\d+)?\.?(\d{0,2})|-?0\.(\d{0,2}))|-0|-?\.\d{0,2}|-|0)")},
    };

    /// Slider的极限范围
    static inline std::map<ConfigType, std::pair<float, float>> mConfigLimits = {
        {ConfigType::DetectRange, {0.f, 100.f}},
        {ConfigType::Gain,        {0.f, 110.f}},
        {ConfigType::GateStart,   {0.f, 100.f}},
        {ConfigType::GateWidth,   {0.f, 100.f}},
        {ConfigType::GateHeight,  {0.f, 100.f}},
    };

    /// Edit控件鼠标滚轮事件的步进
    const static inline std::map<ConfigType, double> mConfigStep = {
        {ConfigType::DetectRange, 0.1},
        {ConfigType::Gain,        0.1},
        {ConfigType::GateStart,   0.1},
        {ConfigType::GateWidth,   0.1},
        {ConfigType::GateHeight,  0.1},
    };

    CWindowUI* m_pWndOpenGL_ASCAN = nullptr; ///< A扫Duilib的窗口指针
    CWindowUI* m_pWndOpenGL_CSCAN = nullptr; ///< C扫Duilib的窗口指针
    OpenGL     m_OpenGL_ASCAN     = {};      ///< A扫OpenGL窗口
    OpenGL     m_OpenGL_CSCAN     = {};      ///< C扫OpenGL窗口

    long       mCurrentGroup = 0;                       ///< 当前分组
    ConfigType mConfigType   = ConfigType::DetectRange; ///< 当前选中设置类型
    GateType   mGateType     = GateType::GATE_A;        ///< 当前选中的波门类型

    /**
     * @brief 当前选中的通道
     * @note 并不是表示实际的通道值, 而是表示第几个选项，例如`CHANNEL_1`表示第一个通道选项
     * 实际的通道号可能是 1,5,9 中的一个
     */
    ChannelSel            mChannelSel          = ChannelSel::CHANNEL_1;
    constexpr static auto BTN_SELECT_GROUP_MAX = 3;

    /**
     * @brief 选组按钮单击回调函数
     * @param index 索引
     */
    void OnBtnSelectGroupClicked(long index);

    /**
     * @brief 初始化OpenGL窗口
     */
    void InitOpenGL();

    /**
     * @brief 线程中初始化
     */
    void InitOnThread();

    /**
     * @brief 更新Slider和Edit控件的值
     * @param newGroup
     * @param newConfig
     * @param newGate
     * @param newChannelSel
     */
    void UpdateSliderAndEditValue(long newGroup, ConfigType newConfig, GateType newGate, ChannelSel newChannelSel);

    /**
     * @brief 设置Config值
     * @param val value
     */
    void SetConfigValue(float val);
};

#pragma pop_macro("GATE_A")
#pragma pop_macro("GATE_B")
