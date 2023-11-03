#include "pch.h"

#include "AutoScanInfoWnd.h"
#include "DefectListWnd.h"
#include "DetectionStd_TBT2995_200.h"
#include "GroupScanWnd.h"
#include "Mutilple.h"
#include "PLCWnd.h"
#include "ReViewScanWnd.h"
#include "SetWnd.h"
#include "WheelUpDownWnd.h"
#include <Model.h>
#include <regex>

#undef GATE_A
#undef GATE_B

GroupScanWnd::~GroupScanWnd() {
    g_MainProcess.m_ConnectPLC.SetPLCAuto(false);
    // �˳�ʱ���رղ���
    g_MainProcess.m_HDBridge.StartSequencer(FALSE);
}

void GroupScanWnd::InitOpenGL() {
    // ��ʼ��OpenGL����
    // Aɨ����
    m_pWndOpenGL_ASCAN = static_cast<CWindowUI*>(m_PaintManager.FindControl(_T("WndOpenGL_ASCAN")));
    m_OpenGL_ASCAN.Create(m_hWnd);
    m_OpenGL_ASCAN.Attach(m_pWndOpenGL_ASCAN);
    // Cɨ����
    m_pWndOpenGL_CSCAN = static_cast<CWindowUI*>(m_PaintManager.FindControl(_T("WndOpenGL_CSCAN")));
    m_OpenGL_CSCAN.Create(m_hWnd);
    m_OpenGL_CSCAN.Attach(m_pWndOpenGL_CSCAN);
}

void GroupScanWnd::InitWindow() {
    CDuiWindowBase::InitWindow();

    InitOpenGL();
    // ��ʼ��
    std::thread Init(&GroupScanWnd::InitOnThread, this);
    Init.detach(); // �̷߳���
    g_MainProcess.m_ConnectPLC.GetAllFloatValue();
    g_MainProcess.m_ConnectPLC.SetPLCAuto(true);

    // InitBtnSelectGroup();
}

void GroupScanWnd::InitOnThread() {
    // �ӳ���󻯴���
    // Sleep(100);
    SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

    Sleep(100);

    g_MainProcess.m_Techniques.Compute();
    m_OpenGL_ASCAN.AddGroupAScanModel(&g_MainProcess.m_Techniques);
    m_OpenGL_CSCAN.AddGroupCScanModel(&g_MainProcess.m_Techniques);
    // ���ð忨����
    g_MainProcess.m_HDBridge.SetCoder(&g_MainProcess.m_ConnectPLC);
    g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
    Sleep(100);
    g_MainProcess.m_HDBridge.StartSequencer(TRUE);
    for (int i = 0; i < 10; i++) {
        g_MainProcess.m_Techniques.GetDetectionStd()->CountDAC(
            &(g_MainProcess.m_Techniques), i, g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[i].fScanGain, 0);
    }
}

void GroupScanWnd::UpdateSliderAndEditValue(long newGroup, ConfigType newConfig, GateType newGate, ChannelSel newChannelSel) {
    if (mCurrentGroup == newGroup && mConfigType == newConfig && mGateType == newGate && mChannelSel == newChannelSel) {
        return;
    }

    spdlog::info("Click Button, group:{}, config: {}, gate: {}, channelsel: {}", newGroup, (int)newConfig, (int)newGate,
                 (int)newChannelSel);

    OnBtnSelectGroupClicked(newGroup);
    mConfigType = newConfig;
    mGateType   = newGate;
    mChannelSel = newChannelSel;

    // ����Edit��λ
    auto edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditConfig")));
    if (edit) {
        edit->SetEnabled(true);
        edit->SetTextExt(mConfigTextext.at(mConfigType));
    }
    // ����Slider��min��max
    auto slider = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("SliderConfig")));
    if (slider) {
        slider->SetEnabled(true);
        slider->SetCanSendMove(true);
        // ���¼��㲨�����Ͳ��ſ�ȵ����ֵ
        int gate = static_cast<int>(mGateType);
        auto channel = g_MainProcess.GetChannel(static_cast<int>(mChannelSel) + mCurrentGroup * 4);
        switch (mConfigType) {
            case GroupScanWnd::ConfigType::GateStart: {
                auto &[_, maxLimits]       = mConfigLimits[mConfigType];
                maxLimits            = (1.0 - channel->m_pGateWidth[gate]) * 100.0;
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
                auto &[_, maxLimits] = mConfigLimits[mConfigType];
                maxLimits            = (1.0 - channel->m_pGatePos[gate]) * 100.0;
                if (maxLimits <= 2) {
                    slider->SetEnabled(false);
                    slider->SetCanSendMove(false);
                    if (edit) {
                        edit->SetEnabled(false);
                    }
                }
                break;
            }
            default: {
                break;
            }
        }

        slider->SetMinValue(static_cast<int>(mConfigLimits.at(mConfigType).first));
        slider->SetMaxValue(static_cast<int>(mConfigLimits.at(mConfigType).second));
    }
    // TODO: ���¶�ȡ��ֵ
    double reloadValue = 0.0;
    int    _channelSel = static_cast<int>(mChannelSel) + mCurrentGroup * 4;
    int    gate        = static_cast<int>(mGateType);
    auto   channel     = g_MainProcess.GetChannel(_channelSel);
    switch (mConfigType) {
        case GroupScanWnd::ConfigType::DetectRange: {
            break;
        }
        case GroupScanWnd::ConfigType::Gain: {
            reloadValue = channel->m_fGain;
            break;
        }
        case GroupScanWnd::ConfigType::GateStart: {
            reloadValue = channel->m_pGatePos[gate]*100.0;
            break;
        }
        case GroupScanWnd::ConfigType::GateWidth: {
            reloadValue = channel->m_pGateWidth[gate]* 100.0;
            break;
        }
        case GroupScanWnd::ConfigType::GateHeight: {
            reloadValue = channel->m_pGateHeight[gate] * 100.0;
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
    spdlog::debug("set config value {}", val);
    int _channelSel = static_cast<int>(mChannelSel) + mCurrentGroup * 4;
    int  gate        = static_cast<int>(mGateType);
    auto channel     = g_MainProcess.GetChannel(_channelSel);
    switch (mConfigType) {
        case GroupScanWnd::ConfigType::DetectRange: {
            break;
        }
        case GroupScanWnd::ConfigType::Gain: {
            channel->m_fGain = val;
            break;
        }
        case GroupScanWnd::ConfigType::GateStart: {
            channel->m_pGatePos[gate] = val/100.0;
            break;
        }
        case GroupScanWnd::ConfigType::GateWidth: {
            channel->m_pGateWidth[gate] = val/100.0;
            break;
        }
        case GroupScanWnd::ConfigType::GateHeight: {
            channel->m_pGateHeight[gate] = val/100.0;
            break;
        }
        default: {
            break;
        }
    }
    g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
}

void GroupScanWnd::Notify(TNotifyUI& msg) {
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
        if (std::regex_match(str, match, matchReg)) {
            auto       opt  = static_cast<COptionUI*>(msg.pSender);
            ConfigType type = static_cast<ConfigType>(_wtol(opt->GetUserData().GetData()));
            _configType     = type;
        }

        matchReg = _T(R"(OptGateType)");
        if (std::regex_match(str, match, matchReg)) {
            auto     opt  = static_cast<COptionUI*>(msg.pSender);
            GateType type = static_cast<GateType>(_wtol(opt->GetUserData().GetData()));
            _gateType     = type;
        }
        matchReg = _T(R"(OptChannel\d)");
        if (std::regex_match(str, match, matchReg)) {
            auto       opt  = static_cast<COptionUI*>(msg.pSender);
            ChannelSel type = static_cast<ChannelSel>(_wtol(opt->GetUserData().GetData()));
            _channelSel     = type;
        }

        UpdateSliderAndEditValue(_currentGroup, _configType, _gateType, _channelSel);

    } else if (msg.sType == DUI_MSGTYPE_VALUECHANGED) {
        if (msg.pSender->GetName() == _T("SliderConfig")) {
            auto slider = dynamic_cast<CSliderUI*>(msg.pSender);
            if (slider) {
                auto edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditConfig")));
                if (edit) {
                    int     sliderValue = slider->GetValue();
                    CString val;
                    val.Format(_T("%.2f"), static_cast<float>(sliderValue));
                    edit->SetText(val);
                    spdlog::debug(_T("setValue: {}"), val);

                    // ����Edit��ֵ
                    auto edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditConfig")));
                    if (edit) {
                        edit->SetText(std::to_wstring(sliderValue).data());
                    }
                    // ���ó�������ֵ
                    if (msg.pSender->IsEnabled()) {
                        SetConfigValue(static_cast<float>(sliderValue));
                    }
                }
            }
        }
    } else if (msg.sType == DUI_MSGTYPE_VALUECHANGED_MOVE) {
        if (msg.pSender->GetName() == _T("SliderConfig")) {
            auto slider = dynamic_cast<CSliderUI*>(msg.pSender);
            if (slider) {
                auto edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditConfig")));
                if (edit) {
                    int sliderValue = slider->GetValue();
                    edit->SetText(std::to_wstring(sliderValue).data());
                }
            }
        }
    } else if (msg.sType == DUI_MSGTYPE_TEXTCHANGED) {
        if (msg.pSender->GetName() == _T("EditConfig")) {
            // ����������ַ�
            auto         edit = dynamic_cast<CEditUI*>(msg.pSender);
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
            auto         edit         = dynamic_cast<CEditUI*>(msg.pSender);
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
            auto slider = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("SliderConfig")));
            if (slider) {
                slider->SetValue(static_cast<int>(std::roundf(currentValue)));
            }

            // ���ó�������ֵ
            SetConfigValue(static_cast<float>(currentValue));
        }
    } else if (msg.sType == DUI_MSGTYPE_MOUSEWHELL) {
        if (msg.pSender->GetName() == _T("EditConfig")) {
            auto edit = dynamic_cast<CEditUI*>(msg.pSender);
            if (edit) {
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
                auto slider = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("SliderConfig")));
                if (slider) {
                    slider->SetValue(static_cast<int>(std::roundf(currentValue)));
                }

                spdlog::debug("Mouse Wheel config value: {}", currentValue);

                // ���ó�������ֵ
                SetConfigValue(static_cast<float>(currentValue));
            }
        }
    }

    CDuiWindowBase::Notify(msg);
}

void GroupScanWnd::OnLButtonDown(UINT nFlags, ::CPoint pt) {
    POINT point{pt.x, pt.y};
    auto  wnd = dynamic_cast<CWindowUI*>(m_PaintManager.FindControl(point));
    if (wnd) {
        if (wnd->GetName() == _T("WndOpenGL_ASCAN")) {
            m_OpenGL_ASCAN.OnLButtonDown(nFlags, pt);
        } else if (wnd->GetName() == _T("WndOpenGL_CSCAN")) {
            m_OpenGL_CSCAN.OnLButtonDown(nFlags, pt);
        }
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
        auto opt = static_cast<COptionUI*>(m_PaintManager.FindControl(name));
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
        auto btn = static_cast<CButtonUI*>(m_PaintManager.FindControl(str));
        if (btn) {
            if (i != index) {
                btn->SetBkColor(0xFFEEEEEE);
            } else {
                btn->SetBkColor(0xFF666666);
            }
        }
    }

    g_MainProcess.m_Techniques.m_GroupScanOffset = mCurrentGroup * 4;
}
