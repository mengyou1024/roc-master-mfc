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
     * @brief ��ʼ����ɨ��ȱ��(������ǰ���ڡ�ʱ�������)
     * @param channel ͨ����
     */
    void StartSaveScanDefect(int channel);

    /**
     * @brief ��ֹ����ɨ��ȱ��
     * @param channel ͨ����
     */
    void EndSaveScanDefect(int channel);

private:
    struct {
        float pos    = {};
        float width  = {};
        float height = {};
    } mGateScan[HD_CHANNEL_NUM]; ///< ɨ�鲨��

    /// ɨ�鰴ťֵ
    inline static std::array<uint8_t, HDBridge::CHANNEL_NUMBER> mScanButtonValue = {};

    enum class WidgetMode { MODE_SCAN = 0, MODE_REVIEW };

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
        GATE_SCAN,
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

    bool mScanningFlag = false;

    constexpr static auto BTN_SELECT_GROUP_MAX = 3;

    /**
     * @brief ��ǰѡ�е�ͨ��
     * @note �����Ǳ�ʾʵ�ʵ�ͨ��ֵ, ���Ǳ�ʾ�ڼ���ѡ�����`CHANNEL_1`��ʾ��һ��ͨ��ѡ��
     * ʵ�ʵ�ͨ���ſ����� 1,5,9 �е�һ��
     */
    ChannelSel                                   mChannelSel        = ChannelSel::CHANNEL_1;
    CWindowUI*                                   m_pWndOpenGL_ASCAN = nullptr;                 ///< AɨDuilib�Ĵ���ָ��
    CWindowUI*                                   m_pWndOpenGL_CSCAN = nullptr;                 ///< CɨDuilib�Ĵ���ָ��
    OpenGL                                       m_OpenGL_ASCAN     = {};                      ///< AɨOpenGL����
    OpenGL                                       m_OpenGL_CSCAN     = {};                      ///< CɨOpenGL����
    long                                         mCurrentGroup      = 0;                       ///< ��ǰ����
    ConfigType                                   mConfigType        = ConfigType::DetectRange; ///< ��ǰѡ����������
    GateType                                     mGateType          = GateType::GATE_A;        ///< ��ǰѡ�еĲ�������
    std::unique_ptr<HD_Utils>                    mUtils             = nullptr;                 ///< Ӳ���ӿ�
    std::array<Button, HDBridge::CHANNEL_NUMBER> mScanButtons       = {};                      ///< ɨ��ģ�ⰴť
    WidgetMode                                   mWidgetMode        = {WidgetMode::MODE_SCAN}; ///< ��ǰ���ڵ�ģʽ
    std::vector<HD_Utils>                        mReviewData        = {};                      ///< ɨ��ȱ������
    int                                          mSamplesPerSecond  = 33;                      ///< Cɨͼÿ���Ӳɵ����
    bool                                         mEnableAmpMemory   = false;                   ///< ��ֵ����
    std::array<int, HDBridge::CHANNEL_NUMBER>    mIDDefectRecord    = {};                      ///< ȱ�ݼ�¼������ID
    std::mutex                                   mRecordMutex       = {};

    /**
     * @brief ѡ�鰴ť�����ص�����
     * @param index ����
     */
    void OnBtnSelectGroupClicked(long index);

    /**
     * @brief ģʽ��ť�����ص�����
     * @param name ��ťID
     */
    void OnBtnModelClicked(std::wstring name);

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
     * @praram bypassCheck �ƹ��ظ����
     */
    void UpdateSliderAndEditValue(long newGroup, ConfigType newConfig, GateType newGate, ChannelSel newChannelSel,
                                  bool bypassCheck = false);

    /**
     * @brief ����Configֵ
     * @param val value
     * @param sync �����Ƿ�����ͬ��
     */
    void SetConfigValue(float val, bool sync = true);

    /**
     * @brief ����Aɨ�ص�����
     * @param data Aɨ����
     * @param caller ������
     */
    void UpdateAScanCallback(const HDBridge::NM_DATA& data, const HD_Utils& caller);

    /**
     * @brief ��ʱ���и���Cɨͼ��
     */
    void UpdateCScanOnTimer();

    /**
     * @brief UI ��ť�ź�
     * @param name ��������
     */
    void OnBtnUIClicked(std::wstring& name);

    /**
     * @brief ֹͣ���ж�ʱ��
     * @param
     */
    void KillUITimer(void);

    /**
     * @brief �������ж�ʱ��
     * @param
     */
    void ResumeUITimer(void);

    /**
     * @brief ����ȱ������
     */
    void ScanScanData();

    /**
     * @brief ��ʼ��ɨ�鰴ť�߼�
     */
    void ScanButtonInit();

    /**
     * @brief ɨ�鰴ť�¼��ص�
     * @param _btn
     */
    static void ScanButtonEventCallback(void* _btn);

    /**
     * @brief ����ط�ģʽ
     * @param name ȱ�ݼ�¼������
     */
    void EnterReviewMode(std::string name);

    /**
     * @brief �˳��ط�ģʽ
     */
    void ExitReviewMode();

    /**
     * @brief ��ʼɨ��
     * @param changeFlag �Ƿ�ı��־λ
     */
    void StartScan(bool changeFlag = true);

    /**
     * @brief ֹͣɨ��
     * @param changeFlag �Ƿ�ı��־λ
     */
    void StopScan(bool changeFlag = true);
};

#pragma pop_macro("GATE_A")
#pragma pop_macro("GATE_B")
