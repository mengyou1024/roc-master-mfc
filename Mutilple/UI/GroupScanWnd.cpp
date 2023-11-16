#include "pch.h"

#include "ChannelSettingWnd.h"
#include "GroupScanWnd.h"
#include "Mutilple.h"
#include "SettingWnd.h"
#include <MeshAscan.h>
#include <MeshGroupCScan.h>
#include <Model.h>
#include <Model/ScanRecord.h>
#include <ModelGroupAScan.h>
#include <ModelGroupCScan.h>
#include <RecordSelectWnd.h>
#include <TBusyWnd.hpp>
#include <algorithm>
#include <array>
#include <chrono>
#include <iomanip>
#include <regex>
#include <sstream>

#undef GATE_A
#undef GATE_B

enum TIMER_ENUM {
    CSCAN_UPDATE = 0,
    BUTTON,
    TIMER_SIZE,
};

constexpr int swapAScanIndex(int x) {
    int result = x / 4;
    int remain = x % 4;
    switch (remain) {
        case 0: remain = 2; break;
        case 1: remain = 3; break;
        case 2: remain = 0; break;
        case 3: remain = 1; break;
        default: break;
    }
    return result * 4 + remain;
}

constexpr std::wstring_view SCAN_CONFIG_NAME = _T("ɨ������");
using sqlite_orm::c;
using sqlite_orm::column;
using sqlite_orm::columns;
using sqlite_orm::where;

GroupScanWnd::GroupScanWnd() {
    try {
        auto config = TOFDUSBPort::storage().get_all<TOFDUSBPort>(where(c(&TOFDUSBPort::name) == std::wstring(SCAN_CONFIG_NAME)));
        if (config.size() == 1) {
            mUtils = std::make_unique<HD_Utils>(std::make_unique<TOFDUSBPort>(config[0]));
        } else {
            mUtils = std::make_unique<HD_Utils>(std::make_unique<TOFDUSBPort>());
        }
        mUtils->getBridge()->defaultInit();
    } catch (std::exception &e) { spdlog::error(e.what()); }
}

GroupScanWnd::~GroupScanWnd() {}

void GroupScanWnd::OnBtnModelClicked(std::wstring name) {
    auto btnScanMode   = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnScanMode")));
    auto btnReviewMode = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnReviewMode")));
    if (name == _T("BtnScanMode")) {
        if (btnScanMode->GetBkColor() != 0xFF666666) {
            btnScanMode->SetBkColor(0xFF666666);
            btnReviewMode->SetBkColor(0xFFEEEEEE);
            ExitReviewMode();
            // �˳������¿�ʼɨ��
            StartScan(false);
        }

    } else {
        RecordSelectWnd selectWnd;
        selectWnd.Create(m_hWnd, selectWnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        selectWnd.CenterWindow();
        selectWnd.ShowModal();
        auto &[res, name] = selectWnd.GetResult();
        if (!res) {
            return;
        }
        btnScanMode->SetBkColor(0xFFEEEEEE);
        btnReviewMode->SetBkColor(0xFF666666);
        // ��ѡ�񴰿�
        auto    &selName = name;
        TBusyWnd wnd([this, &selName]() {
            // ����ǰ����ͣɨ��
            StopScan(false);
            EnterReviewMode(selName);
        });
        wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        wnd.CenterWindow();
        wnd.ShowModal();
    }
}

void GroupScanWnd::InitOpenGL() {
    // ��ʼ��OpenGL����
    // Aɨ����
    m_pWndOpenGL_ASCAN = static_cast<CWindowUI *>(m_PaintManager.FindControl(_T("WndOpenGL_ASCAN")));
    m_OpenGL_ASCAN.Create(m_hWnd);
    m_OpenGL_ASCAN.Attach(m_pWndOpenGL_ASCAN);
    // Cɨ����
    m_pWndOpenGL_CSCAN = static_cast<CWindowUI *>(m_PaintManager.FindControl(_T("WndOpenGL_CSCAN")));
    m_OpenGL_CSCAN.Create(m_hWnd);
    m_OpenGL_CSCAN.Attach(m_pWndOpenGL_CSCAN);
}

LPCTSTR GroupScanWnd::GetWindowClassName() const {
    return _T("GroupScanWnd");
}

CDuiString GroupScanWnd::GetSkinFile() noexcept {
    return _T("Theme\\UI_GroupScanWnd.xml");
}

void GroupScanWnd::InitWindow() {
    CDuiWindowBase::InitWindow();

    InitOpenGL();
    // ��ʼ��
    std::thread Init(&GroupScanWnd::InitOnThread, this);
    Init.detach(); // �̷߳���

    UpdateSliderAndEditValue(mCurrentGroup, mConfigType, mGateType, mChannelSel, true);
}

void GroupScanWnd::InitOnThread() {
    // �ӳ���󻯴���
    // Sleep(100);
    SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

    Sleep(100);

    m_OpenGL_ASCAN.AddGroupAScanModel();
    m_OpenGL_CSCAN.AddGroupCScanModel();
    // ���ð忨����
    Sleep(100);
    mUtils->start();
    auto model = static_cast<ModelGroupAScan *>(m_OpenGL_ASCAN.m_pModel[0]);
    mUtils->addReadCallback(std::bind(&GroupScanWnd::UpdateAScanCallback, this, std::placeholders::_1, std::placeholders::_2));
    ScanButtonInit();
}

void GroupScanWnd::UpdateSliderAndEditValue(long newGroup, ConfigType newConfig, GateType newGate, ChannelSel newChannelSel,
                                            bool bypassCheck) {
    if (!bypassCheck && (mCurrentGroup == newGroup && mConfigType == newConfig && mGateType == newGate && mChannelSel == newChannelSel)) {
        return;
    }

    spdlog::debug("Click Button, group:{}, config: {}, gate: {}, channelsel: {}", newGroup, (int)newConfig, (int)newGate,
                 (int)newChannelSel);

    OnBtnSelectGroupClicked(newGroup);
    mConfigType = newConfig;
    mGateType   = newGate;
    mChannelSel = newChannelSel;

    // ����Edit��λ
    auto edit = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditConfig")));
    if (edit) {
        edit->SetEnabled(true);
        edit->SetTextExt(mConfigTextext.at(mConfigType));
    }
    // ����Slider��min��max
    auto slider = static_cast<CSliderUI *>(m_PaintManager.FindControl(_T("SliderConfig")));
    if (slider) {
        slider->SetEnabled(true);
        slider->SetCanSendMove(true);
        // ���¼��㲨�����Ͳ��ſ��ȵ����ֵ
        int    gate    = static_cast<int>(mGateType);
        size_t chIndex = static_cast<size_t>(mChannelSel) + static_cast<size_t>(mCurrentGroup) * 4ull;
        switch (mConfigType) {
            case GroupScanWnd::ConfigType::GateStart: {
                if (gate == static_cast<int>(GateType::GATE_SCAN)) {
                    auto &[_, maxLimits] = mConfigLimits[mConfigType];
                    maxLimits            = static_cast<float>((1.0 - mGateScan[chIndex].width) * 100.0);
                    if (maxLimits <= 2) {
                        slider->SetEnabled(false);
                        slider->SetCanSendMove(false);
                        if (edit) {
                            edit->SetEnabled(false);
                        }
                    }
                    break;
                }
                auto &[_, maxLimits] = mConfigLimits[mConfigType];
                auto gateInfo        = mUtils->getBridge()->getGateInfo(gate);
                maxLimits            = static_cast<float>((1.0 - gateInfo[chIndex].width) * 100.0);
                if (maxLimits <= 2) {
                    slider->SetEnabled(false);
                    slider->SetCanSendMove(false);
                    if (edit) {
                        edit->SetEnabled(false);
                    }
                }
                break;
            }
            case GroupScanWnd::ConfigType::GateWidth: {
                if (gate == static_cast<int>(GateType::GATE_SCAN)) {
                    auto &[_, maxLimits] = mConfigLimits[mConfigType];
                    maxLimits            = static_cast<float>((1.0 - mGateScan[chIndex].pos) * 100.0);
                    if (maxLimits <= 2) {
                        slider->SetEnabled(false);
                        slider->SetCanSendMove(false);
                        if (edit) {
                            edit->SetEnabled(false);
                        }
                    }
                    break;
                }
                auto &[_, maxLimits] = mConfigLimits[mConfigType];
                auto gateInfo        = mUtils->getBridge()->getGateInfo(gate);
                maxLimits            = static_cast<float>((1.0 - gateInfo[chIndex].pos) * 100.0);
                if (maxLimits <= 2) {
                    slider->SetEnabled(false);
                    slider->SetCanSendMove(false);
                    if (edit) {
                        edit->SetEnabled(false);
                    }
                }
                break;
            }
            case GroupScanWnd::ConfigType::DetectRange: {
                auto &[minLimits, maxLimits] = mConfigLimits[mConfigType];
                minLimits                    = 50.0f;
                maxLimits                    = 1000.f;
                break;
            }
            default: {
                break;
            }
        }

        slider->SetMinValue(static_cast<int>(mConfigLimits.at(mConfigType).first));
        slider->SetMaxValue(static_cast<int>(mConfigLimits.at(mConfigType).second));
    }
    // DONE: ���¶�ȡ��ֵ
    double reloadValue = 0.0;
    int    _channelSel = static_cast<int>(mChannelSel) + mCurrentGroup * 4;
    int    gate        = static_cast<int>(mGateType);
    auto   bridge      = mUtils->getBridge();
    switch (mConfigType) {
        case GroupScanWnd::ConfigType::DetectRange: {
            reloadValue = bridge->time2distance(bridge->getSampleDepth()[_channelSel]);
            break;
        }
        case GroupScanWnd::ConfigType::Gain: {
            reloadValue = bridge->getGain()[_channelSel];
            break;
        }
        case GroupScanWnd::ConfigType::GateStart: {
            if (gate == static_cast<int>(GateType::GATE_SCAN)) {
                reloadValue = mGateScan[_channelSel].pos * 100.0;
                break;
            }
            reloadValue = bridge->getGateInfo(gate)[_channelSel].pos * 100.0;
            break;
        }
        case GroupScanWnd::ConfigType::GateWidth: {
            if (gate == static_cast<int>(GateType::GATE_SCAN)) {
                reloadValue = mGateScan[_channelSel].width * 100.0;
                break;
            }
            reloadValue = bridge->getGateInfo(gate)[_channelSel].width * 100.0;
            break;
        }
        case GroupScanWnd::ConfigType::GateHeight: {
            if (gate == static_cast<int>(GateType::GATE_SCAN)) {
                reloadValue = mGateScan[_channelSel].height * 100.0;
                break;
            }
            reloadValue = reloadValue = bridge->getGateInfo(gate)[_channelSel].height * 100.0;
            break;
        }
        default: {
            break;
        }
    }
    if (slider) {
        slider->SetValue(static_cast<int>(std::round(reloadValue)));
    }

    if (edit) {
        std::wstring limit = std::to_wstring(reloadValue);
        std::wregex  reg(mConfigRegex.at(mConfigType));
        std::wsmatch match;
        if ((std::regex_search(limit, match, reg))) {
            edit->SetText(match[0].str().data());
        }
    }
}

void GroupScanWnd::SetConfigValue(float val) {
    auto tick = GetTickCount64();
    spdlog::debug("set config value {}", val);
    int  _channelSel = static_cast<int>(mChannelSel) + mCurrentGroup * 4;
    int  gate        = static_cast<int>(mGateType);
    auto bridge      = mUtils->getBridge();
    switch (mConfigType) {
        case GroupScanWnd::ConfigType::DetectRange: {
            bridge->setSampleDepth(_channelSel, (float)(bridge->distance2time((double)val)));
            break;
        }
        case GroupScanWnd::ConfigType::Gain: {
            bridge->setGain(_channelSel, val);
            break;
        }
        case GroupScanWnd::ConfigType::GateStart: {
            if (gate == static_cast<int>(GateType::GATE_SCAN)) {
                mGateScan[_channelSel].pos = static_cast<float>(val / 100.0);
                auto m                     = static_cast<MeshAscan *>(m_OpenGL_ASCAN.getModel<ModelGroupAScan *>()->m_pMesh[_channelSel]);
                m->UpdateGate(gate, true, mGateScan[_channelSel].pos, mGateScan[_channelSel].width, mGateScan[_channelSel].height);
                break;
            }
            HDBridge::HB_GateInfo g = bridge->getGateInfo(gate)[_channelSel];
            g.gate                  = gate;
            g.active                = 1;
            g.pos                   = static_cast<float>(val / 100.0);
            spdlog::debug("set gate info {}, gate: {}", bridge->setGateInfo(_channelSel, g), gate);
            break;
        }
        case GroupScanWnd::ConfigType::GateWidth: {
            if (gate == static_cast<int>(GateType::GATE_SCAN)) {
                mGateScan[_channelSel].width = static_cast<float>(val / 100.0);
                auto m                       = static_cast<MeshAscan *>(m_OpenGL_ASCAN.getModel<ModelGroupAScan *>()->m_pMesh[_channelSel]);
                m->UpdateGate(gate, true, mGateScan[_channelSel].pos, mGateScan[_channelSel].width, mGateScan[_channelSel].height);
                break;
            }
            HDBridge::HB_GateInfo g = bridge->getGateInfo(gate)[_channelSel];
            g.gate                  = gate;
            g.active                = 1;
            g.width                 = static_cast<float>(val / 100.0);
            spdlog::debug("set gate info {}, gate: {}", bridge->setGateInfo(_channelSel, g), gate);
            break;
        }
        case GroupScanWnd::ConfigType::GateHeight: {
            if (gate == static_cast<int>(GateType::GATE_SCAN)) {
                mGateScan[_channelSel].height = static_cast<float>(val / 100.0);
                auto m = static_cast<MeshAscan *>(m_OpenGL_ASCAN.getModel<ModelGroupAScan *>()->m_pMesh[_channelSel]);
                m->UpdateGate(gate, true, mGateScan[_channelSel].pos, mGateScan[_channelSel].width, mGateScan[_channelSel].height);
                break;
            }
            HDBridge::HB_GateInfo g = bridge->getGateInfo(gate)[_channelSel];
            g.gate                  = gate;
            g.active                = 1;
            g.height                = static_cast<float>(val / 100.0);
            spdlog::debug("set gate info {}, gate: {}", bridge->setGateInfo(_channelSel, g), gate);
            break;
        }
        default: {
            break;
        }
    }

    std::thread t([bridge]() { bridge->flushSetting(); });
    t.detach();
    spdlog::debug("config takes times:{}", GetTickCount64() - tick);
}

void GroupScanWnd::UpdateAScanCallback(const HDBridge::NM_DATA &data, const HD_Utils &caller) {
    auto model = static_cast<ModelGroupAScan *>(m_OpenGL_ASCAN.m_pModel[0]);
    if (model->m_pMesh.at(data.iChannel) == nullptr) {
        return;
    }
    auto                                  bridge = caller.getBridge();
    auto                                  mesh   = static_cast<MeshAscan *>(model->m_pMesh[data.iChannel]);
    std::shared_ptr<std::vector<uint8_t>> hdata  = std::make_shared<std::vector<uint8_t>>(data.pAscan);
    if (model == nullptr || bridge == nullptr || mesh == nullptr || hdata == nullptr) {
        return;
    }
    mesh->hookAScanData(hdata);
    float delay  = bridge->getDelay()[data.iChannel];
    float deepth = bridge->getSampleDepth()[data.iChannel];
    mesh->SetLimits((float)(bridge->time2distance(delay)), (float)(bridge->time2distance(deepth)));
    for (int i = 0; i < 2; i++) {
        HDBridge::HB_GateInfo g = bridge->getGateInfo(i)[data.iChannel];
        mesh->UpdateGate(g.gate, g.active, g.pos, g.width, g.height);
    }
}

void GroupScanWnd::UpdateCScanOnTimer() {
    auto scanData = mUtils->mScanOrm.mScanData;

    for (auto &it : scanData) {
        if (it != nullptr) {
            auto mesh = static_cast<MeshGroupCScan *>(((ModelGroupCScan *)m_OpenGL_CSCAN.m_pModel[0])->m_pMesh[it->iChannel]);
            if (mGateScan[it->iChannel].width != 0.0f) {
                auto l = static_cast<size_t>(std::round(mGateScan[it->iChannel].pos * it->pAscan.size()));
                auto r = static_cast<size_t>(std::round((mGateScan[it->iChannel].pos + mGateScan[it->iChannel].width) * it->pAscan.size()));
                auto max        = std::max_element(std::begin(it->pAscan) + l, std::begin(it->pAscan) + r);
                glm::vec4 color = {};
                if (*max > 255 / 4 * 3) {
                    color = {1.0f, 0.0f, 0.0f, 1.0f};
                } else if (*max > 255 / 2) {
                    color = {0.0f, 0.0f, 1.0f, 1.0f};
                } else if (*max > 255 / 4) {
                    color = {0.0f, 1.0f, 0.0f, 1.0f};
                } else {
                    color = {1.0f, 1.0f, 1.0f, 1.0f};
                }
                if (*max > 255 / 4) {
                    GroupScanWnd::mScanButtonValue[it->iChannel] = 1;
                } else {
                    GroupScanWnd::mScanButtonValue[it->iChannel] = 0;
                }
                mesh->AppendDot(*max, color);
            }
        }
    }

    // if (saveScanDataFlag != 0) {
    SaveScanDefect();
    //}
}

void GroupScanWnd::OnBtnUIClicked(std::wstring &name) {
    if (name == _T("Setting")) {
        SettingWnd wnd;
        wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        wnd.CenterWindow();
        wnd.ShowModal();
    } else if (name == _T("AutoScan")) {
        if (mScanningFlag == true) {
            StopScan();
            auto btn = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnUIAutoScan")));
            btn->SetBkColor(0xFFEEEEEE);
        } else {
            if (!mUtils->getBridge()->isOpen()) {
                DMessageBox(L"������δ�򿪣���ȷ���Ƿ����ӣ�");
                return;
            }
            StartScan();
            auto btn = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnUIAutoScan")));
            btn->SetBkColor(0xFF339933);
        }
    }
}

void GroupScanWnd::KillUITimer(void) {
    ::KillTimer(m_OpenGL_ASCAN.m_hWnd, 0);
    ::KillTimer(m_OpenGL_CSCAN.m_hWnd, 0);
}

void GroupScanWnd::ResumeUITimer(void) {
    ::SetTimer(m_OpenGL_ASCAN.m_hWnd, 0, 15, NULL);
    ::SetTimer(m_OpenGL_CSCAN.m_hWnd, 0, 15, NULL);
}

void GroupScanWnd::Notify(TNotifyUI &msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        CDuiString   strName = msg.pSender->GetName();
        std::wregex  matchReg(_T(R"(BtnSelectGroup(\d))"));
        std::wsmatch match;
        std::wstring str(strName.GetData());

        long       _currentGroup = mCurrentGroup;
        ConfigType _configType   = mConfigType;
        GateType   _gateType     = mGateType;
        ChannelSel _channelSel   = mChannelSel;

        if (std::regex_match(str, match, matchReg)) {
            _currentGroup = _wtol(match[1].str().data());
        }

        matchReg = _T(R"(OptConfigType)");
        auto opt = static_cast<COptionUI *>(msg.pSender);
        if (std::regex_match(str, matchReg)) {
            ConfigType type = static_cast<ConfigType>(_wtol(opt->GetUserData().GetData()));
            _configType     = type;
        }

        matchReg = _T(R"(OptGateType)");
        if (std::regex_match(str, matchReg)) {
            GateType type = static_cast<GateType>(_wtol(opt->GetUserData().GetData()));
            _gateType     = type;
        }
        matchReg = _T(R"(OptChannel\d)");
        if (std::regex_match(str, matchReg)) {
            ChannelSel type = static_cast<ChannelSel>(_wtol(opt->GetUserData().GetData()));
            _channelSel     = type;
        }

        matchReg = _T(R"(BtnUI(.+))");
        if (std::regex_match(str, match, matchReg)) {
            OnBtnUIClicked(match[1].str());
        }

        matchReg = _T(R"((BtnScanMode)|(BtnReviewMode))");
        if (std::regex_match(str, match, matchReg)) {
            OnBtnModelClicked(match[1].str());
        }

        UpdateSliderAndEditValue(_currentGroup, _configType, _gateType, _channelSel);

    } else if (msg.sType == DUI_MSGTYPE_VALUECHANGED) {
        if (msg.pSender->GetName() == _T("SliderConfig")) {
            auto slider = static_cast<CSliderUI *>(msg.pSender);
            auto edit   = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditConfig")));
            if (edit) {
                int     sliderValue = slider->GetValue();
                CString val;
                val.Format(_T("%.2f"), static_cast<float>(sliderValue));
                edit->SetText(val);
                spdlog::debug(_T("setValue: {}"), val);

                // ����Edit��ֵ
                auto edit = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditConfig")));
                if (edit) {
                    edit->SetText(std::to_wstring(sliderValue).data());
                }
                // ���ó�������ֵ
                if (msg.pSender->IsEnabled()) {
                    SetConfigValue(static_cast<float>(sliderValue));
                }
            }
        }
    } else if (msg.sType == DUI_MSGTYPE_VALUECHANGED_MOVE) {
        if (msg.pSender->GetName() == _T("SliderConfig")) {
            auto slider = static_cast<CSliderUI *>(msg.pSender);
            auto edit   = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditConfig")));
            if (edit) {
                int sliderValue = slider->GetValue();
                edit->SetText(std::to_wstring(sliderValue).data());
            }
        }
    } else if (msg.sType == DUI_MSGTYPE_TEXTCHANGED) {
        if (msg.pSender->GetName() == _T("EditConfig")) {
            // ����������ַ�
            auto         edit = static_cast<CEditUI *>(msg.pSender);
            std::wstring text = edit->GetText();
            if (text.length() > 0) {
                auto [sel, _] = edit->GetSel();
                std::wregex  reg(mConfigRegex.at(mConfigType));
                std::wsmatch match;
                if ((!std::regex_match(text, match, reg)) && sel > 0) {
                    text.erase(sel - 1ull, 1);
                    edit->SetText(text.data());
                    edit->SetSel(sel, sel);
                }
            }
        }
    } else if (msg.sType == DUI_MSGTYPE_RETURN) {
        if (msg.pSender->GetName() == _T("EditConfig")) {
            // ����Edit�����뷶Χ
            auto         edit         = static_cast<CEditUI *>(msg.pSender);
            std::wstring text         = edit->GetText();
            auto         currentValue = _wtof(text.data());
            if (currentValue < mConfigLimits.at(mConfigType).first) {
                currentValue       = mConfigLimits.at(mConfigType).first;
                std::wstring limit = std::to_wstring(currentValue);
                std::wregex  reg(mConfigRegex.at(mConfigType));
                std::wsmatch match;
                if ((std::regex_search(limit, match, reg))) {
                    edit->SetText(match[0].str().data());
                    edit->SetSel(static_cast<long>(match[0].str().length()), static_cast<long>(match[1].str().length()));
                }
            } else if (currentValue > mConfigLimits.at(mConfigType).second) {
                currentValue       = mConfigLimits.at(mConfigType).second;
                std::wstring limit = std::to_wstring(currentValue);
                std::wregex  reg(mConfigRegex.at(mConfigType));
                std::wsmatch match;
                if ((std::regex_search(limit, match, reg))) {
                    edit->SetText(match[0].str().data());
                    edit->SetSel(static_cast<long>(match[0].str().length()), static_cast<long>(match[1].str().length()));
                }
            }
            // ���»�ȡֵ
            text         = edit->GetText();
            currentValue = _wtof(text.data());
            spdlog::debug("EditConfigSetValue: {}", currentValue);

            // ����slider ֵ
            auto slider = static_cast<CSliderUI *>(m_PaintManager.FindControl(_T("SliderConfig")));
            if (slider) {
                slider->SetValue(static_cast<int>(std::round(currentValue)));
            }

            // ���ó�������ֵ
            SetConfigValue(static_cast<float>(currentValue));
        }
    } else if (msg.sType == DUI_MSGTYPE_MOUSEWHELL) {
        if (msg.pSender->GetName() == _T("EditConfig")) {
            auto         edit         = static_cast<CEditUI *>(msg.pSender);
            auto         currentValue = _wtof(edit->GetText());
            std::wstring text         = edit->GetText();
            if (LOWORD(msg.wParam)) {
                currentValue -= mConfigStep.at(mConfigType);
            } else {
                currentValue += mConfigStep.at(mConfigType);
            }
            if (currentValue < mConfigLimits.at(mConfigType).first) {
                currentValue = mConfigLimits.at(mConfigType).first;
            } else if (currentValue > mConfigLimits.at(mConfigType).second) {
                currentValue = mConfigLimits.at(mConfigType).second;
            }
            text = std::to_wstring(currentValue);
            std::wregex  reg(mConfigRegex.at(mConfigType));
            std::wsmatch match;
            if (std::regex_search(text, match, reg)) {
                edit->SetText(match[0].str().data());
            }

            // ����slider ֵ
            auto slider = static_cast<CSliderUI *>(m_PaintManager.FindControl(_T("SliderConfig")));
            if (slider) {
                slider->SetValue(static_cast<int>(std::round(currentValue)));
            }

            spdlog::debug("Mouse Wheel config value: {}", currentValue);

            // ���ó�������ֵ
            SetConfigValue(static_cast<float>(currentValue));
        }
    }

    CDuiWindowBase::Notify(msg);
}

void GroupScanWnd::OnLButtonDown(UINT nFlags, ::CPoint pt) {
    POINT point{pt.x, pt.y};
    auto  wnd = dynamic_cast<CWindowUI *>(m_PaintManager.FindControl(point));
    if (wnd) {
        if (wnd->GetName() == _T("WndOpenGL_ASCAN")) {
            m_OpenGL_ASCAN.OnLButtonDown(nFlags, pt);
        } else if (wnd->GetName() == _T("WndOpenGL_CSCAN")) {
            m_OpenGL_CSCAN.OnLButtonDown(nFlags, pt);
        }
    }

    if (mWidgetMode == WidgetMode::MODE_REVIEW && mReviewData.size() > 0 && pointInRect(m_pWndOpenGL_CSCAN->GetPos(), pt)) {
        auto temp = pt;
        temp.x -= m_pWndOpenGL_CSCAN->GetX();
        temp.y -= m_pWndOpenGL_CSCAN->GetY();
        size_t index = (size_t)((float)mReviewData.size() * (float)temp.x / (float)m_pWndOpenGL_CSCAN->GetWidth());
        if (index >= mReviewData.size()) {
            index = mReviewData.size() - 1;
        }
        auto &bridge = mReviewData[index];

        for (int i = 0; i < HDBridge::CHANNEL_NUMBER; i++) {
            auto model = static_cast<ModelGroupAScan *>(m_OpenGL_ASCAN.m_pModel[0]);
            auto mesh  = static_cast<MeshAscan *>(model->m_pMesh[i]);
            auto cMesh = static_cast<MeshGroupCScan *>(((ModelGroupCScan *)m_OpenGL_CSCAN.m_pModel[0])->m_pMesh[i]);
            cMesh->AppendLine(temp.x);
            mesh->hookAScanData(std::make_shared<std::vector<uint8_t>>(bridge.mScanOrm.mScanData[i]->pAscan));
            mesh->UpdateGate(2, 1, bridge.mScanOrm.mScanData[i]->scanGateInfo.pos, bridge.mScanOrm.mScanData[i]->scanGateInfo.width,
                             bridge.mScanOrm.mScanData[i]->scanGateInfo.height);
        }
    }
}

void GroupScanWnd::OnLButtonDClick(UINT nFlags, ::CPoint pt) {
    if (mWidgetMode == WidgetMode::MODE_SCAN && pointInRect(m_pWndOpenGL_ASCAN->GetPos(), pt)) {
        auto temp = pt;
        temp.x -= m_pWndOpenGL_ASCAN->GetX();
        temp.y -= m_pWndOpenGL_ASCAN->GetY();
        for (const auto &[index, ptr] : m_OpenGL_ASCAN.getModel<ModelGroupAScan *>()->m_pMesh) {
            if (index >= static_cast<size_t>(mCurrentGroup * 4) && index < static_cast<size_t>((mCurrentGroup + 1) * 4) &&
                ptr->IsInArea(temp)) {
                KillUITimer();
                // �������ɨ����ֹͣɨ��
                if (mScanningFlag == true) {
                    StopScan(false);
                }
                spdlog::debug("double click: {}", swapAScanIndex(static_cast<int>(index)));
                mUtils->pushCallback();
                // �ƽ�����Ȩ
                ChannelSettingWnd *wnd = new ChannelSettingWnd(std::move(mUtils), swapAScanIndex(static_cast<int>(index)));
                wnd->Create(m_hWnd, wnd->GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
                wnd->CenterWindow();
                wnd->ShowModal();
                // �ƻ�����Ȩ
                mUtils = std::move(wnd->returnHDUtils());
                delete wnd;
                mUtils->popCallback();
                ResumeUITimer();
                // �������ɨ�������¿�ʼɨ��
                if (mScanningFlag == true) {
                    StartScan(false);
                }
            }
        }
    }
}

void GroupScanWnd::OnTimer(int iIdEvent) {
    switch (iIdEvent) {
        case CSCAN_UPDATE: {
            UpdateCScanOnTimer();
            break;
        }
        case BUTTON: {
            button_ticks();
            break;
        }

        default: break;
    }
}

void GroupScanWnd::OnBtnSelectGroupClicked(long index) {
    if (index == mCurrentGroup) {
        return;
    }
    mCurrentGroup = index;

    // ����ͨ��ѡ���Text
    for (int i = 0; i < 4; i++) {
        CString name;
        name.Format(_T("OptChannel%d"), i);
        auto opt = static_cast<COptionUI *>(m_PaintManager.FindControl(name));
        if (opt) {
            CString index;
            index.Format(_T("%d"), mCurrentGroup * 4 + i + 1);
            opt->SetText(index.GetString());
        }
    }

    // ����ѡ�ť����ɫ
    for (long i = 0; i < BTN_SELECT_GROUP_MAX; i++) {
        CString str;
        str.Format(_T("BtnSelectGroup%d"), i);
        auto btn = static_cast<CButtonUI *>(m_PaintManager.FindControl(str));
        if (btn) {
            if (i != index) {
                btn->SetBkColor(0xFFEEEEEE);
            } else {
                btn->SetBkColor(0xFF666666);
            }
        }
    }
    m_OpenGL_ASCAN.getModel<ModelGroupAScan *>()->SetViewGroup(mCurrentGroup);
    m_OpenGL_CSCAN.getModel<ModelGroupCScan *>()->SetViewGroup(mCurrentGroup);
}

void GroupScanWnd::StartSaveScanDefect() {
    if (saveScanDataFlag == 0) {
        std::stringstream                     buffer = {};
        std::chrono::system_clock::time_point t      = std::chrono::system_clock::now();
        time_t                                tm     = std::chrono::system_clock::to_time_t(t);
        buffer << std::put_time(localtime(&tm), "%Y-%m-%d__%H-%M-%S");
        mUtils->time                     = buffer.str();
        ORM_Model::ScanRecord scanRecord = {};
        scanRecord.time                  = buffer.str();
        try {
            ORM_Model::ScanRecord::storage().insert(scanRecord);
        } catch (std::exception &e) { spdlog::error(e.what()); }
    }
}

void GroupScanWnd::SaveScanDefect() {
    // ���浱ǰɨ�鲨�ŵ�λ����Ϣ
    for (int i = 0; i < HDBridge::CHANNEL_NUMBER; i++) {
        mUtils->mScanOrm.mScanData[i]->scanGateInfo.pos    = mGateScan[i].pos;
        mUtils->mScanOrm.mScanData[i]->scanGateInfo.width  = mGateScan[i].width;
        mUtils->mScanOrm.mScanData[i]->scanGateInfo.height = mGateScan[i].height;
    }
    // ����Cɨ��������Ϣ
    auto [minLimit, maxLimit]        = m_OpenGL_CSCAN.getModel<ModelGroupCScan *>()->GetAxisRange();
    mUtils->mScanOrm.mCScanLimits[0] = minLimit;
    mUtils->mScanOrm.mCScanLimits[1] = maxLimit;
    // ����ɨ������
    HD_Utils::storage().insert(*mUtils);
}

void GroupScanWnd::ScanButtonInit() {
    uint8_t index = 0;
    for (auto &btn : mScanButtons) {
        button_init(
            &btn, [](uint8_t id) -> uint8_t { return GroupScanWnd::mScanButtonValue[id]; }, 1, index++, this);
        button_attach(&btn, PRESS_DOWN, ScanButtonEventCallback);
        button_attach(&btn, PRESS_UP, ScanButtonEventCallback);
        button_start(&btn);
    }
}

void GroupScanWnd::ScanButtonEventCallback(void *_btn) {
    auto btn = static_cast<Button *>(_btn);
    auto wnd = static_cast<GroupScanWnd *>(btn->userData);
    switch (btn->event) {
        case PRESS_DOWN: {
            spdlog::debug("btn down: {}", btn->button_id);
            // ��ʼ����ȱ������
            // wnd->StartSaveScanDefect();
            // wnd->saveScanDataFlag |= 1 << btn->button_id;
            break;
        }
        case PRESS_UP: {
            spdlog::debug("btn up: {}", btn->button_id);
            // wnd->saveScanDataFlag &= ~(1 << btn->button_id);
            break;
        }
        default: break;
    }
}

void GroupScanWnd::EnterReviewMode(std::string name) {
    auto tick = GetTickCount64();
    // ��Żص�����
    mUtils->pushCallback();
    // ��ȡ����������
    mReviewData = HD_Utils::storage().get_all<HD_Utils>(where(c(&HD_Utils::time) == name));
    spdlog::info("load:{}, frame:{}", name, mReviewData.size());
    // ɾ������ͨ����Cɨ����
    for (int index = 0; index < HDBridge::CHANNEL_NUMBER; index++) {
        auto mesh = static_cast<MeshGroupCScan *>(((ModelGroupCScan *)m_OpenGL_CSCAN.m_pModel[0])->m_pMesh[index]);
        mesh->RemoveDot();
        mesh->RemoveLine();
    }
    for (const auto &data : mReviewData) {
        for (int index = 0; index < HDBridge::CHANNEL_NUMBER; index++) {
            auto mesh = static_cast<MeshGroupCScan *>(((ModelGroupCScan *)m_OpenGL_CSCAN.m_pModel[0])->m_pMesh[index]);
            if (data.mScanOrm.mScanData[index]->scanGateInfo.width != 0.0f) {
                auto l = static_cast<size_t>(
                    std::round(data.mScanOrm.mScanData[index]->scanGateInfo.pos * data.mScanOrm.mScanData[index]->pAscan.size()));
                auto r = static_cast<size_t>(
                    std::round((data.mScanOrm.mScanData[index]->scanGateInfo.pos + data.mScanOrm.mScanData[index]->scanGateInfo.width) *
                               data.mScanOrm.mScanData[index]->pAscan.size()));
                auto      max   = std::max_element(std::begin(data.mScanOrm.mScanData[index]->pAscan) + l,
                                                   std::begin(data.mScanOrm.mScanData[index]->pAscan) + r);
                glm::vec4 color = {};
                if (*max > 255 / 4 * 3) {
                    color = {1.0f, 0.0f, 0.0f, 1.0f};
                } else if (*max > 255 / 2) {
                    color = {0.0f, 0.0f, 1.0f, 1.0f};
                } else if (*max > 255 / 4) {
                    color = {0.0f, 1.0f, 0.0f, 1.0f};
                } else {
                    color = {1.0f, 1.0f, 1.0f, 1.0f};
                }
                mesh->AppendDot(*max, color, MAXSIZE_T);
            }
        }
    }

    // �طŵ�Cɨ��ΧΪ��һ��ͼ����Сֵ�����һ��ͼ�����ֵ
    if (mReviewData.size() > 0) {
        float cScanMinLimits = (*std::begin(mReviewData)).mScanOrm.mCScanLimits[0];
        float cScanMaxLimits = (*std::rbegin(mReviewData)).mScanOrm.mCScanLimits[1];
        m_OpenGL_CSCAN.getModel<ModelGroupCScan *>()->SetAxisRange(cScanMinLimits, cScanMaxLimits);
    }

    // �л����沼��
    auto layout = static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(_T("LayoutParamSetting")));
    layout->SetVisible(false);
    layout = static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(_T("LayoutFunctionButton")));
    layout->SetVisible(false);

    mWidgetMode = WidgetMode::MODE_REVIEW;
    spdlog::info("takes time: {} ms", GetTickCount64() - tick);
}

void GroupScanWnd::ExitReviewMode() {
    mUtils->popCallback();
    mReviewData.clear();
    auto layout = static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(_T("LayoutParamSetting")));
    layout->SetVisible(true);
    layout = static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(_T("LayoutFunctionButton")));
    layout->SetVisible(true);
    for (int i = 0; i < HDBridge::CHANNEL_NUMBER; i++) {
        auto model = static_cast<ModelGroupAScan *>(m_OpenGL_ASCAN.m_pModel[0]);
        auto mesh  = static_cast<MeshAscan *>(model->m_pMesh[i]);
        auto cMesh = static_cast<MeshGroupCScan *>(((ModelGroupCScan *)m_OpenGL_CSCAN.m_pModel[0])->m_pMesh[i]);
        cMesh->RemoveLine();
        cMesh->RemoveDot();
        mesh->UpdateGate(2, 1, mGateScan[i].pos, mGateScan[i].width, mGateScan[i].height);
    }
    mWidgetMode = WidgetMode::MODE_SCAN;
}

void GroupScanWnd::StartScan(bool changeFlag) {
    if (mWidgetMode != WidgetMode::MODE_SCAN) {
        return;
    }

    if (!changeFlag) {
        if (mScanningFlag == true) {
            SetTimer(CSCAN_UPDATE, 10);
            SetTimer(BUTTON, 5);
        }
        return;
    }
    if (mScanningFlag == false) {
        // ���浱ǰʱ��
        std::stringstream                     buffer = {};
        std::chrono::system_clock::time_point t      = std::chrono::system_clock::now();
        time_t                                tm     = std::chrono::system_clock::to_time_t(t);
        buffer << std::put_time(localtime(&tm), "%Y-%m-%d__%H-%M-%S");
        mUtils->time                     = buffer.str();
        ORM_Model::ScanRecord scanRecord = {};
        scanRecord.time                  = buffer.str();
        try {
            ORM_Model::ScanRecord::storage().insert(scanRecord);
        } catch (std::exception &e) { spdlog::error(e.what()); }

        mScanningFlag = true;
        SetTimer(CSCAN_UPDATE, 10);
        SetTimer(BUTTON, 5);
        mScanButtonValue.fill(0);
    }
}

void GroupScanWnd::StopScan(bool changeFlag) {
    if (mWidgetMode != WidgetMode::MODE_SCAN) {
        return;
    }
    if (!changeFlag) {
        if (mScanningFlag == true) {
            KillTimer(CSCAN_UPDATE);
            KillTimer(BUTTON);
        }
        return;
    }
    if (mScanningFlag == true) {
        mScanningFlag = false;
        for (int i = 0; i < HDBridge::CHANNEL_NUMBER; i++) {
            auto meshAScan = static_cast<MeshAscan *>(((ModelGroupAScan *)m_OpenGL_ASCAN.m_pModel[0])->m_pMesh[i]);
            auto meshCScan = static_cast<MeshGroupCScan *>(((ModelGroupCScan *)m_OpenGL_CSCAN.m_pModel[0])->m_pMesh[i]);
            meshCScan->RemoveLine();
            meshCScan->RemoveDot();
            meshAScan->UpdateGate(2, 1, mGateScan[i].pos, mGateScan[i].width, mGateScan[i].height);
            mScanButtonValue.fill(0);
        }
        KillTimer(CSCAN_UPDATE);
        KillTimer(BUTTON);
    }
}