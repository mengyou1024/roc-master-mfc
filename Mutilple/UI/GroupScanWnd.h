#pragma once

#include "DuiWindowBase.h"
#include "OpenGL.h"
#include "multi_button.h"
#include <HDBridge.h>
#include <HDBridge/TOFDPort.h>
#include <HDBridge/Utils.h>
#include <map>
#include <memory>

#pragma push_macro("GATE_A")
#pragma push_macro("GATE_B")

#undef GATE_A
#undef GATE_B

class GroupScanWnd : public CDuiWindowBase {
public:
    GroupScanWnd();
    virtual ~GroupScanWnd();
    virtual LPCTSTR    GetWindowClassName() const override;
    virtual CDuiString GetSkinFile() noexcept override;
    virtual void       InitWindow() override;
    virtual void       Notify(TNotifyUI& msg) override;
    virtual void       OnLButtonDown(UINT nFlags, ::CPoint pt) override;
    virtual void       OnLButtonDClick(UINT nFlags, ::CPoint pt) override;
    virtual void       OnTimer(int iIdEvent) override;

    /**
     * @brief 开始保存扫查缺陷(创建当前日期、时间的索引)
     * @param channel 通道号
     */
    void StartSaveScanDefect(int channel);

    /**
     * @brief 终止保存扫查缺陷
     * @param channel 通道号
     */
    void EndSaveScanDefect(int channel);

private:
    struct {
        float pos    = {};
        float width  = {};
        float height = {};
    } mGateScan[HD_CHANNEL_NUM]; ///< 扫查波门

    /// 扫查按钮值
    inline static std::array<uint8_t, HDBridge::CHANNEL_NUMBER> mScanButtonValue = {};

    enum class WidgetMode { MODE_SCAN = 0, MODE_REVIEW };

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
        GATE_SCAN,
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
        {ConfigType::GateStart,   _T("%") },
        {ConfigType::GateWidth,   _T("%") },
        {ConfigType::GateHeight,  _T("%") },
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

    bool mScanningFlag = false;

    constexpr static auto BTN_SELECT_GROUP_MAX = 3;

    /**
     * @brief 当前选中的通道
     * @note 并不是表示实际的通道值, 而是表示第几个选项，例如`CHANNEL_1`表示第一个通道选项
     * 实际的通道号可能是 1,5,9 中的一个
     */
    ChannelSel                                   mChannelSel        = ChannelSel::CHANNEL_1;
    CWindowUI*                                   m_pWndOpenGL_ASCAN = nullptr;                 ///< A扫Duilib的窗口指针
    CWindowUI*                                   m_pWndOpenGL_CSCAN = nullptr;                 ///< C扫Duilib的窗口指针
    OpenGL                                       m_OpenGL_ASCAN     = {};                      ///< A扫OpenGL窗口
    OpenGL                                       m_OpenGL_CSCAN     = {};                      ///< C扫OpenGL窗口
    long                                         mCurrentGroup      = 0;                       ///< 当前分组
    ConfigType                                   mConfigType        = ConfigType::DetectRange; ///< 当前选中设置类型
    GateType                                     mGateType          = GateType::GATE_A;        ///< 当前选中的波门类型
    std::unique_ptr<HD_Utils>                    mUtils             = nullptr;                 ///< 硬件接口
    std::array<Button, HDBridge::CHANNEL_NUMBER> mScanButtons       = {};                      ///< 扫查模拟按钮
    WidgetMode                                   mWidgetMode        = {WidgetMode::MODE_SCAN}; ///< 当前窗口的模式
    std::vector<HD_Utils>                        mReviewData        = {};                      ///< 扫查缺陷数据
    int                                          mSamplesPerSecond  = 33;                      ///< C扫图每秒钟采点个数
    bool                                         mEnableAmpMemory   = false;                   ///< 峰值记忆
    std::array<int, HDBridge::CHANNEL_NUMBER>    mIDDefectRecord    = {};                      ///< 缺陷记录的索引ID
    std::mutex                                   mRecordMutex       = {};

    /**
     * @brief 选组按钮单击回调函数
     * @param index 索引
     */
    void OnBtnSelectGroupClicked(long index);

    /**
     * @brief 模式按钮单击回调函数
     * @param name 按钮ID
     */
    void OnBtnModelClicked(std::wstring name);

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
     * @praram bypassCheck 绕过重复检查
     */
    void UpdateSliderAndEditValue(long newGroup, ConfigType newConfig, GateType newGate, ChannelSel newChannelSel,
                                  bool bypassCheck = false);

    /**
     * @brief 设置Config值
     * @param val value
     * @param sync 控制是否立即同步
     */
    void SetConfigValue(float val, bool sync = true);

    /**
     * @brief 更新A扫回调函数
     * @param data A扫数据
     * @param caller 调用类
     */
    void UpdateAScanCallback(const HDBridge::NM_DATA& data, const HD_Utils& caller);

    /**
     * @brief 定时器中更新C扫图像
     */
    void UpdateCScanOnTimer();

    /**
     * @brief UI 按钮信号
     * @param name 按键名称
     */
    void OnBtnUIClicked(std::wstring& name);

    /**
     * @brief 停止所有定时器
     * @param
     */
    void KillUITimer(void);

    /**
     * @brief 重启所有定时器
     * @param
     */
    void ResumeUITimer(void);

    /**
     * @brief 保存缺陷数据
     */
    void ScanScanData();

    /**
     * @brief 初始化扫查按钮逻辑
     */
    void ScanButtonInit();

    /**
     * @brief 扫查按钮事件回调
     * @param _btn
     */
    static void ScanButtonEventCallback(void* _btn);

    /**
     * @brief 进入回放模式
     * @param name 缺陷记录的名称
     */
    void EnterReviewMode(std::string name);

    /**
     * @brief 退出回放模式
     */
    void ExitReviewMode();

    /**
     * @brief 开始扫查
     * @param changeFlag 是否改变标志位
     */
    void StartScan(bool changeFlag = true);

    /**
     * @brief 停止扫查
     * @param changeFlag 是否改变标志位
     */
    void StopScan(bool changeFlag = true);
};

#pragma pop_macro("GATE_A")
#pragma pop_macro("GATE_B")
