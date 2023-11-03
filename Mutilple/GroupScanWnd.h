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
    /// ��������
    enum class ConfigType {
        DetectRange = 0,
        Gain,
        GateStart,
        GateWidth,
        GateHeight,
    };

    /// ��������
    enum class GateType {
        GATE_A = 0,
        GATE_B,
    };

    /// ͨ��ѡ��
    enum class ChannelSel {
        CHANNEL_1 = 0,
        CHANNEL_2,
        CHANNEL_3,
        CHANNEL_4,
    };

    /// Edit�ؼ���ʾ�ĵ�λ�ı�
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

    /// Slider�ļ��޷�Χ
    static inline std::map<ConfigType, std::pair<float, float>> mConfigLimits = {
        {ConfigType::DetectRange, {0.f, 100.f}},
        {ConfigType::Gain,        {0.f, 110.f}},
        {ConfigType::GateStart,   {0.f, 100.f}},
        {ConfigType::GateWidth,   {0.f, 100.f}},
        {ConfigType::GateHeight,  {0.f, 100.f}},
    };

    /// Edit�ؼ��������¼��Ĳ���
    const static inline std::map<ConfigType, double> mConfigStep = {
        {ConfigType::DetectRange, 0.1},
        {ConfigType::Gain,        0.1},
        {ConfigType::GateStart,   0.1},
        {ConfigType::GateWidth,   0.1},
        {ConfigType::GateHeight,  0.1},
    };

    CWindowUI* m_pWndOpenGL_ASCAN = nullptr; ///< AɨDuilib�Ĵ���ָ��
    CWindowUI* m_pWndOpenGL_CSCAN = nullptr; ///< CɨDuilib�Ĵ���ָ��
    OpenGL     m_OpenGL_ASCAN     = {};      ///< AɨOpenGL����
    OpenGL     m_OpenGL_CSCAN     = {};      ///< CɨOpenGL����

    long       mCurrentGroup = 0;                       ///< ��ǰ����
    ConfigType mConfigType   = ConfigType::DetectRange; ///< ��ǰѡ����������
    GateType   mGateType     = GateType::GATE_A;        ///< ��ǰѡ�еĲ�������

    /**
     * @brief ��ǰѡ�е�ͨ��
     * @note �����Ǳ�ʾʵ�ʵ�ͨ��ֵ, ���Ǳ�ʾ�ڼ���ѡ�����`CHANNEL_1`��ʾ��һ��ͨ��ѡ��
     * ʵ�ʵ�ͨ���ſ����� 1,5,9 �е�һ��
     */
    ChannelSel            mChannelSel          = ChannelSel::CHANNEL_1;
    constexpr static auto BTN_SELECT_GROUP_MAX = 3;

    /**
     * @brief ѡ�鰴ť�����ص�����
     * @param index ����
     */
    void OnBtnSelectGroupClicked(long index);

    /**
     * @brief ��ʼ��OpenGL����
     */
    void InitOpenGL();

    /**
     * @brief �߳��г�ʼ��
     */
    void InitOnThread();

    /**
     * @brief ����Slider��Edit�ؼ���ֵ
     * @param newGroup
     * @param newConfig
     * @param newGate
     * @param newChannelSel
     */
    void UpdateSliderAndEditValue(long newGroup, ConfigType newConfig, GateType newGate, ChannelSel newChannelSel);

    /**
     * @brief ����Configֵ
     * @param val value
     */
    void SetConfigValue(float val);
};

#pragma pop_macro("GATE_A")
#pragma pop_macro("GATE_B")
