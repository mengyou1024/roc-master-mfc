#include "pch.h"

#include "ChannelSettingWnd.h"
#include "DefectsListWnd.h"
#include "GroupScanWnd.h"
#include "Mutilple.h"
#include "ParamManagementWnd.h"
#include "SettingWnd.h"
#include "Version.h"
#include <BusyWnd.h>
#include <HardWareWnd.h>
#include <MeshAscan.h>
#include <MeshGroupCScan.h>
#include <Model.h>
#include <ModelGroupAScan.h>
#include <ModelGroupCScan.h>
#include <RecordSelectWnd.h>
#include <UI/DetectionInformationEntryWnd.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <iomanip>
#include <regex>
#include <rttr/type.h>
#include <sstream>

using rttr::array_range;
using rttr::property;
using rttr::type;

#undef GATE_A
#undef GATE_B

enum TIMER_ENUM {
    CSCAN_UPDATE = 0,
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

constexpr std::wstring_view SCAN_CONFIG_LAST = _T("上一次配置");
using sqlite_orm::c;
using sqlite_orm::column;
using sqlite_orm::columns;
using sqlite_orm::where;

GroupScanWnd::GroupScanWnd() {
    try {
        mSystemConfig = GetSystemConfig();
        auto config = TOFDUSBPort::storage().get_all<TOFDUSBPort>(where(c(&TOFDUSBPort::name) == std::wstring(SCAN_CONFIG_LAST)));
        if (config.size() == 1) {
            if (config[0].isValid) {
                mUtils = std::make_unique<HD_Utils>(std::make_unique<TOFDUSBPort>(config[0]));
                mUtils->getBridge()->syncCache2Board();
                config[0].isValid = false;
                TOFDUSBPort::storage().update(config[0]);
            } else {
                mUtils = std::make_unique<HD_Utils>(std::make_unique<TOFDUSBPort>());
                mUtils->getBridge()->defaultInit();
            }
        } else {
            mUtils = std::make_unique<HD_Utils>(std::make_unique<TOFDUSBPort>());
            mUtils->getBridge()->defaultInit();
        }
        auto detectInfos = ORM_Model::DetectInfo::storage().get_all<ORM_Model::DetectInfo>();
        if (detectInfos.size() == 1) {
            mDetectInfo = detectInfos[0];
        }
    } catch (std::exception &e) { spdlog::error(GB2312ToUtf8(e.what())); }
}

GroupScanWnd::~GroupScanWnd() {
    try {
        // 退出前停止扫查并且退出回放模式
        StopScan(true);
        if (mWidgetMode == WidgetMode::MODE_REVIEW) {
            ExitReviewMode();
        }
        auto tick    = GetTickCount64();
        auto bridges = TOFDUSBPort::storage().get_all<TOFDUSBPort>(where(c(&TOFDUSBPort::name) == std::wstring(SCAN_CONFIG_LAST)));
        if (bridges.size() == 1) {
            bridges[0].isValid = true;
            bridges[0].mCache  = mUtils->getBridge<TOFDUSBPort *>()->mCache;
            TOFDUSBPort::storage().update(bridges[0]);
        } else {
            mUtils->getBridge<TOFDUSBPort *>()->name    = std::wstring(SCAN_CONFIG_LAST);
            mUtils->getBridge<TOFDUSBPort *>()->isValid = true;
            TOFDUSBPort::storage().insert(*(mUtils->getBridge<TOFDUSBPort *>()));
        }
        auto detectInfos = ORM_Model::DetectInfo::storage().get_all<ORM_Model::DetectInfo>();
        if (detectInfos.size() == 1) {
            mDetectInfo.id = 1;
            ORM_Model::DetectInfo::storage().update(mDetectInfo);
        } else {
            ORM_Model::DetectInfo::storage().insert(mDetectInfo);
        }
        spdlog::debug("take time: {}", GetTickCount64() - tick);
        UpdateSystemConfig(mSystemConfig);
    } catch (std::exception &e) { spdlog::error(GB2312ToUtf8(e.what())); }
}

void GroupScanWnd::OnBtnModelClicked(std::wstring name) {
    auto btnScanMode   = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnScanMode")));
    auto btnReviewMode = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnReviewMode")));
    if (name == _T("BtnScanMode")) {
        if (btnScanMode->GetBkColor() != 0xFF339933) {
            btnScanMode->SetBkColor(0xFF339933);
            btnReviewMode->SetBkColor(0xFFEEEEEE);
            BusyWnd wnd([this]() { ExitReviewMode(); });
            wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
            wnd.ShowModal();
            // 退出后重新开始扫查
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
        btnReviewMode->SetBkColor(0xFF339933);
        // 打开选择窗口
        auto   &selName = name;
        BusyWnd wnd([this, &selName]() {
            // 进入前先暂停扫查
            StopScan(false);
            EnterReviewMode(selName);
        });
        wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        wnd.ShowModal();
    }
}

void GroupScanWnd::InitOpenGL() {
    // 初始化OpenGL窗口
    // A扫窗口
    m_pWndOpenGL_ASCAN = static_cast<CWindowUI *>(m_PaintManager.FindControl(_T("WndOpenGL_ASCAN")));
    m_OpenGL_ASCAN.Create(m_hWnd);
    m_OpenGL_ASCAN.Attach(m_pWndOpenGL_ASCAN);
    // C扫窗口
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
    // 初始化
    std::thread Init(&GroupScanWnd::InitOnThread, this);
    Init.detach(); // 线程分离

    UpdateSliderAndEditValue(mCurrentGroup, mConfigType, mGateType, mChannelSel, true);
}

void GroupScanWnd::InitOnThread() {
    // 延迟最大化窗口
    // Sleep(100);
    SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

    Sleep(100);

    m_OpenGL_ASCAN.AddGroupAScanModel();
    m_OpenGL_CSCAN.AddGroupCScanModel();
    // 设置板卡参数
    Sleep(100);
    mUtils->start();
    auto model = static_cast<ModelGroupAScan *>(m_OpenGL_ASCAN.m_pModel[0]);
    mUtils->addReadCallback(std::bind(&GroupScanWnd::UpdateAScanCallback, this, std::placeholders::_1, std::placeholders::_2));
    // #ifndef _DEBUG
    //     auto [tag, body, url] = GetLatestReleaseNote("https://api.github.com/repos/mengyou1024/roc-master-mfc/releases/latest");
    //     if (Check4Update("v0.0", tag)) {
    //         std::wstring wBody  = WStringFromString(body);
    //         std::wstring wTitle = std::wstring(L"更新可用:") + WStringFromString(tag);
    //         auto         ret    = DMessageBox(wBody.data(), wTitle.data(), MB_YESNO);
    //         spdlog::debug("ret = {}", ret);
    //         spdlog::info("tag: {}\n body: {} \n url: {}", tag, body, url);
    //         FILE *fp   = fopen("./upgrade.exe", "wb");
    //         CURL *curl = curl_easy_init();
    //         curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    //         curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    //         curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    //         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
    //         CURLcode result = curl_easy_perform(curl);
    //         fclose(fp);
    //         curl_easy_cleanup(curl);
    //         auto aret = system(".\\upgrade.exe /verysilent /suppressmsgboxes");
    //         spdlog::info("ret={}", aret);
    //     }
    // #endif // !_DEBUG
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

    // 设置Edit单位
    auto edit = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditConfig")));
    if (edit) {
        edit->SetEnabled(true);
        edit->SetTextExt(mConfigTextext.at(mConfigType));
    }
    // 设置Slider的min、max
    auto slider = static_cast<CSliderUI *>(m_PaintManager.FindControl(_T("SliderConfig")));
    if (slider) {
        slider->SetEnabled(true);
        slider->SetCanSendMove(true);
        // 重新计算波门起点和波门宽度的最大值
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
    // DONE: 重新读取数值
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
    reloadValue = std::round(reloadValue * 100.f) / 100.f;

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

void GroupScanWnd::SetConfigValue(float val, bool sync) {
    auto tick        = GetTickCount64();
    int  _channelSel = static_cast<int>(mChannelSel) + mCurrentGroup * 4;
    int  gate        = static_cast<int>(mGateType);
    auto bridge      = mUtils->getBridge();
    switch (mConfigType) {
        case GroupScanWnd::ConfigType::DetectRange: {
            bridge->setSampleDepth(_channelSel, (float)(bridge->distance2time((double)val)));
            // 重新计算采样因子
            auto depth        = bridge->getSampleDepth()[_channelSel];
            auto delay        = bridge->getDelay()[_channelSel] + bridge->getZeroBias()[_channelSel];
            auto sampleFactor = static_cast<int>(std::round((depth - delay) * 100.0 / 1024.0));
            bridge->setSampleFactor(_channelSel, sampleFactor);
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
    if (sync) {
        std::thread t([bridge]() { bridge->flushSetting(); });
        t.detach();
    }
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

    if (!mEnableAmpMemory) {
        for (int i = 0; i < 3; i++) {
            mesh->hootAmpMemoryData(i, nullptr);
            mesh->ClearAmpMemoryData(i);
        }
        return;
    }
    for (int i = 0; i < 2; i++) {
        const auto ampData = mesh->getAmpMemoryData(i);
        auto       g       = bridge->getGateInfo(i, data.iChannel);
        // 获取波门内的数据
        auto l = data.pAscan.begin() + static_cast<int64_t>(std::round(static_cast<float>(data.pAscan.size()) * g.pos));
        auto r = data.pAscan.begin() + static_cast<int64_t>(std::round(static_cast<float>(data.pAscan.size()) * (g.pos + g.width)));
        std::vector<uint8_t> newAmpData(l, r);
        if (ampData.size() == newAmpData.size()) {
            for (auto i = 0; i < ampData.size(); i++) {
                if (newAmpData[i] < ampData[i]) {
                    newAmpData[i] = ampData[i];
                }
            }
        }
        mesh->hootAmpMemoryData(i, std::make_shared<std::vector<uint8_t>>(newAmpData));
    }
    {
        const auto ampData = mesh->getAmpMemoryData(2);
        auto       l =
            data.pAscan.begin() + static_cast<int64_t>(std::round(static_cast<float>(data.pAscan.size()) * mGateScan[data.iChannel].pos));
        auto                 r = data.pAscan.begin() + static_cast<int64_t>(std::round(static_cast<float>(data.pAscan.size()) *
                                                                                       (mGateScan[data.iChannel].pos + mGateScan[data.iChannel].width)));
        std::vector<uint8_t> newAmpData(l, r);
        if (ampData.size() == newAmpData.size()) {
            for (auto i = 0; i < ampData.size(); i++) {
                if (newAmpData[i] < ampData[i]) {
                    newAmpData[i] = ampData[i];
                }
            }
        }
        mesh->hootAmpMemoryData(2, std::make_shared<std::vector<uint8_t>>(newAmpData));
    }
}

void GroupScanWnd::UpdateCScanOnTimer() {
    std::array<std::shared_ptr<HDBridge::NM_DATA>, HDBridge::CHANNEL_NUMBER> scanData = mUtils->mScanOrm.mScanData;

    for (auto &it : scanData) {
        if (it != nullptr) {
            auto mesh = static_cast<MeshGroupCScan *>(((ModelGroupCScan *)m_OpenGL_CSCAN.m_pModel[0])->m_pMesh[it->iChannel]);
            if (mGateScan[it->iChannel].width != 0.0f) {
                auto l = static_cast<size_t>(std::round(mGateScan[it->iChannel].pos * it->pAscan.size()));
                auto r = static_cast<size_t>(std::round((mGateScan[it->iChannel].pos + mGateScan[it->iChannel].width) * it->pAscan.size()));
                auto max        = std::max_element(std::begin(it->pAscan) + l, std::begin(it->pAscan) + r);
                glm::vec4 color = {};
                if (*max > it->pGateAmp[1]) {
                    color = {1.0f, 0.f, 0.f, 1.0f};
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
    SaveScanData();
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
                DMessageBox(L"超声板未打开，请确认是否连接！");
                return;
            }
            StartScan();
            auto btn = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnUIAutoScan")));
            btn->SetBkColor(0xFF339933);
        }
    } else if (name == _T("ParamManagement")) {
        ParamManagementWnd wnd(mUtils->getBridge());
        wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        wnd.CenterWindow();
        wnd.ShowModal();
        UpdateSliderAndEditValue(mCurrentGroup, mConfigType, mGateType, mChannelSel, true);
    } else if (name == _T("About")) {
        DMessageBox(APP_VERSIONW, L"软件版本");
    } else if (name == _T("Freeze")) {
        auto btn = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnUIFreeze")));
        if (btn->GetBkColor() == 0xFFEEEEEE) {
            mUtils->pushCallback();
            StopScan(false);
            btn->SetBkColor(0xFF339933);
        } else {
            mUtils->popCallback();
            StartScan(false);
            btn->SetBkColor(0xFFEEEEEE);
        }
    } else if (name == _T("AmpMemory")) {
        auto btn = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnUIAmpMemory")));
        if (btn->GetBkColor() == 0xFFEEEEEE) {
            mEnableAmpMemory = true;
            btn->SetBkColor(0xFF339933);
        } else {
            mEnableAmpMemory = false;
            btn->SetBkColor(0xFFEEEEEE);
        }
    } else if (name == _T("AutoGain")) {
        if (mGateType == GateType::GATE_SCAN) {
            return;
        }
        BusyWnd wnd([this]() {
            mUtils->autoGain(static_cast<int>(mChannelSel) + 4 * static_cast<int>(mCurrentGroup), static_cast<int>(mGateType));
        });
        wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        wnd.ShowModal();
    } else if (name == _T("InformationEntry")) {
        DetectionInformationEntryWnd wnd;
        wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        wnd.CenterWindow();
        wnd.LoadDetectInfo(mDetectInfo, mSystemConfig.userName, mSystemConfig.groupName);
        wnd.ShowModal();
        if (wnd.GetResult()) {
            mDetectInfo             = wnd.GetDetectInfo();
            mSystemConfig.groupName = wnd.GetJobGroup().groupName;
            mSystemConfig.userName  = wnd.GetUser().name;
        }
    } else if (name == _T("HardPort")) {
        HardWareWnd wnd;
        wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        wnd.CenterWindow();
        wnd.ShowModal();
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

        if (msg.pSender->GetName() == _T("BtnExportReport")) {
            std::map<string, string> valueMap = {};
            valueMap["jobGroup"]              = GetJobGroup();
            valueMap["user"]                  = StringFromWString(mSystemConfig.userName);
            for (const auto &prot : type::get<ORM_Model::DetectInfo>().get_properties()) {
                valueMap[string(prot.get_name())] = StringFromWString(prot.get_value(mDetectInfo).convert<std::wstring>());
            }
            CFileDialog dlg(false, L"docx", L"Report.docx", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Word Document (*.docx)| *.docx||");
            if (dlg.DoModal() == IDOK) {
                if (WordTemplateRender(L"./template/template.docx", dlg.GetPathName().GetString(), valueMap) == false) {
                    spdlog::error("export report document error!");
                    DMessageBox(L"导出失败!");
                } else {
                    DMessageBox(L"导出成功!");
                }
            }

        } else if (msg.pSender->GetName() == _T("BtnDetectInformation")) {
            DetectionInformationEntryWnd wnd;
            wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
            wnd.LoadDetectInfo(mDetectInfo, mSystemConfig.userName, mSystemConfig.groupName);
            wnd.CenterWindow();
            wnd.ShowModal();
            if (wnd.GetResult()) {
                mDetectInfo             = wnd.GetDetectInfo();
                mSystemConfig.groupName = wnd.GetJobGroup().groupName;
                mSystemConfig.userName  = wnd.GetUser().name;
            }
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

                // 设置Edit数值
                auto edit = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EditConfig")));
                if (edit) {
                    edit->SetText(std::to_wstring(sliderValue).data());
                }
                // 设置超声板数值
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
                SetConfigValue(static_cast<float>(sliderValue), false);
            }
        }
    } else if (msg.sType == DUI_MSGTYPE_TEXTCHANGED) {
        if (msg.pSender->GetName() == _T("EditConfig")) {
            // 限制输入的字符
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
            // 限制Edit的输入范围
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
            // 重新获取值
            text         = edit->GetText();
            currentValue = _wtof(text.data());
            spdlog::debug("EditConfigSetValue: {}", currentValue);

            // 设置slider 值
            auto slider = static_cast<CSliderUI *>(m_PaintManager.FindControl(_T("SliderConfig")));
            if (slider) {
                slider->SetValue(static_cast<int>(std::round(currentValue)));
            }

            // 设置超声板数值
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

            // 设置slider 值
            auto slider = static_cast<CSliderUI *>(m_PaintManager.FindControl(_T("SliderConfig")));
            if (slider) {
                slider->SetValue(static_cast<int>(std::round(currentValue)));
            }

            spdlog::debug("Mouse Wheel config value: {}", currentValue);

            // 设置超声板数值
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

    if (mWidgetMode == WidgetMode::MODE_REVIEW && mReviewData.size() > 0 && PointInRect(m_pWndOpenGL_CSCAN->GetPos(), pt)) {
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
    if (PointInRect(m_pWndOpenGL_ASCAN->GetPos(), pt)) {
        auto temp = pt;
        temp.x -= m_pWndOpenGL_ASCAN->GetX();
        temp.y -= m_pWndOpenGL_ASCAN->GetY();
        if (mWidgetMode == WidgetMode::MODE_SCAN) {
            for (const auto &[index, ptr] : m_OpenGL_ASCAN.getModel<ModelGroupAScan *>()->m_pMesh) {
                if (index >= static_cast<size_t>(mCurrentGroup * 4) && index < static_cast<size_t>((mCurrentGroup + 1) * 4) &&
                    ptr->IsInArea(temp)) {
                    KillUITimer();
                    // 如果正在扫查则停止扫查
                    if (mScanningFlag == true) {
                        StopScan(false);
                    }
                    spdlog::debug("double click: {}", swapAScanIndex(static_cast<int>(index)));
                    mUtils->pushCallback();
                    // 移交所有权
                    ChannelSettingWnd *wnd = new ChannelSettingWnd(std::move(mUtils), swapAScanIndex(static_cast<int>(index)));
                    wnd->Create(m_hWnd, wnd->GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
                    wnd->CenterWindow();
                    wnd->ShowModal();
                    // 移回所有权
                    mUtils = std::move(wnd->returnHDUtils());
                    delete wnd;
                    mUtils->popCallback();
                    ResumeUITimer();
                    // 如果正在扫查则重新开始扫查
                    if (mScanningFlag == true) {
                        StartScan(false);
                    }
                }
            }
        } else {
            // 列出缺陷列表
            DefectsListWnd wnd;
            wnd.Create(m_hWnd, wnd.GetWindowClassName(), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
            wnd.LoadDefectsList(mDetectInfo.time);
            wnd.ShowModal();
            auto &[res, index, channel] = wnd.getResult();
            if (res) {
                OnBtnSelectGroupClicked(channel / 4);
                auto     width  = m_pWndOpenGL_CSCAN->GetWidth();
                auto     height = m_pWndOpenGL_CSCAN->GetHeight();
                ::CPoint pt;
                pt.x = (long)(m_pWndOpenGL_CSCAN->GetX() + (float)width * ((float)index / (float)mReviewData.size()));
                pt.y = (long)(m_pWndOpenGL_CSCAN->GetY() + height / 2);
                OnLButtonDown(1, pt);
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
        default: break;
    }
}

void GroupScanWnd::OnBtnSelectGroupClicked(long index) {
    if (index == mCurrentGroup) {
        return;
    }
    mCurrentGroup = index;

    // 设置通道选择的Text
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

    // 设置选项按钮的颜色
    for (long i = 0; i < BTN_SELECT_GROUP_MAX; i++) {
        CString str;
        str.Format(_T("BtnSelectGroup%d"), i);
        auto btn = static_cast<CButtonUI *>(m_PaintManager.FindControl(str));
        if (btn) {
            if (i != index) {
                btn->SetBkColor(0xFFEEEEEE);
            } else {
                btn->SetBkColor(0xFF339933);
            }
        }
    }
    m_OpenGL_ASCAN.getModel<ModelGroupAScan *>()->SetViewGroup(mCurrentGroup);
    m_OpenGL_CSCAN.getModel<ModelGroupCScan *>()->SetViewGroup(mCurrentGroup);
}

void GroupScanWnd::SaveDefectStartID(int channel) {
    ORM_Model::ScanRecord scanRecord = {};
    scanRecord.startID               = mRecordCount + (int)mReviewData.size();
    scanRecord.channel               = channel;
    mScanRecordCache.push_back(scanRecord);
    mIDDefectRecord[channel] = (int)mScanRecordCache.size() - 1;
}

void GroupScanWnd::SaveDefectEndID(int channel) {
    if (mScanRecordCache.size() == 0) {
        return;
    }
    auto &scanRecord = mScanRecordCache.at(mIDDefectRecord[channel]);
    if (mRecordCount + (int)mReviewData.size() < scanRecord.startID) {
        return;
    }
    scanRecord.endID = mRecordCount + (int)mReviewData.size();
}

void GroupScanWnd::SaveScanData() {
    // 保存当前扫查波门的位置信息
    for (int i = 0; i < HDBridge::CHANNEL_NUMBER; i++) {
        mUtils->mScanOrm.mScanData[i]->scanGateInfo.pos    = mGateScan[i].pos;
        mUtils->mScanOrm.mScanData[i]->scanGateInfo.width  = mGateScan[i].width;
        mUtils->mScanOrm.mScanData[i]->scanGateInfo.height = mGateScan[i].height;
    }
    // 保存C扫的坐标信息
    auto [minLimit, maxLimit]        = m_OpenGL_CSCAN.getModel<ModelGroupCScan *>()->GetAxisRange();
    mUtils->mScanOrm.mCScanLimits[0] = minLimit;
    mUtils->mScanOrm.mCScanLimits[1] = maxLimit;
    // 保存扫查数据
    if (mReviewData.size() >= SCAN_RECORD_CACHE_MAX_ITEMS) {
        std::vector<HD_Utils> copyData = mReviewData;
        // 线程中将扫查数据保存
        std::thread t([this, copyData]() { HD_Utils::storage(mSavePath).insert_range(copyData.begin(), copyData.end()); });
        t.detach();
        mRecordCount += (int)mReviewData.size();
        mReviewData.clear();
    } else {
        mReviewData.push_back(*mUtils);
    }
    auto &res = mDetectionSM.UpdateData(mScanButtonValue);
    for (int i = 0; i < res.size(); i++) {
        if (res[i] == DetectionStateMachine::DetectionStatus::Rasing) {
            SaveDefectStartID(i);
        } else if (res[i] == DetectionStateMachine::DetectionStatus::Falling) {
            SaveDefectEndID(i);
        }
    }
}

void GroupScanWnd::EnterReviewMode(std::string name) {
    auto tick = GetTickCount64();
    // 存放回调函数
    mUtils->pushCallback();
    // 保存配置信息备份
    mDetectInfoBak   = mDetectInfo;
    mSystemConfigBak = mSystemConfig;
    // 读取并加载数据
    mDetectInfo            = ORM_Model::DetectInfo::storage(name).get<ORM_Model::DetectInfo>(1);
    mSystemConfig.groupName = ORM_Model::JobGroup::storage(name).get<ORM_Model::JobGroup>(1).groupName;
    mReviewData            = HD_Utils::storage(name).get_all<HD_Utils>();
    spdlog::info("load:{}, frame:{}", name, mReviewData.size());
    // 删除所有通道的C扫数据
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
                if (*max > data.mScanOrm.mScanData[index]->pGateAmp[1]) {
                    color = {1.0f, 0.f, 0.f, 1.0f};
                } else {
                    color = {1.0f, 1.0f, 1.0f, 1.0f};
                }
                mesh->AppendDot(*max, color, MAXSIZE_T);
            } else {
                mesh->AppendDot(0, {0.0f, 1.0f, 0.0f, 1.0f}, MAXSIZE_T);
            }
        }
    }

    // 回放的C扫范围为第一幅图的最小值到最后一幅图的最大值
    if (mReviewData.size() > 0) {
        float cScanMinLimits = (*std::begin(mReviewData)).mScanOrm.mCScanLimits[0];
        float cScanMaxLimits = (*std::rbegin(mReviewData)).mScanOrm.mCScanLimits[1];
        m_OpenGL_CSCAN.getModel<ModelGroupCScan *>()->SetAxisRange(cScanMinLimits, cScanMaxLimits);
    }

    // 切换界面布局
    auto layout = static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(_T("LayoutParamSetting")));
    layout->SetVisible(false);
    layout = static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(_T("LayoutFunctionButton")));
    layout->SetVisible(false);
    layout = static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(_T("LayoutReviewExt")));
    layout->SetVisible(true);
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
    layout = static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(_T("LayoutReviewExt")));
    layout->SetVisible(false);
    for (int i = 0; i < HDBridge::CHANNEL_NUMBER; i++) {
        auto model = static_cast<ModelGroupAScan *>(m_OpenGL_ASCAN.m_pModel[0]);
        auto mesh  = static_cast<MeshAscan *>(model->m_pMesh[i]);
        auto cMesh = static_cast<MeshGroupCScan *>(((ModelGroupCScan *)m_OpenGL_CSCAN.m_pModel[0])->m_pMesh[i]);
        cMesh->RemoveLine();
        cMesh->RemoveDot();
        mesh->UpdateGate(2, 1, mGateScan[i].pos, mGateScan[i].width, mGateScan[i].height);
    }
    mDetectInfo = mDetectInfoBak;
    UpdateSystemConfig(mSystemConfigBak);
    mWidgetMode = WidgetMode::MODE_SCAN;
}

void GroupScanWnd::StartScan(bool changeFlag) {
    if (mWidgetMode != WidgetMode::MODE_SCAN) {
        return;
    }

    if (!changeFlag) {
        if (mScanningFlag == true) {
            SetTimer(CSCAN_UPDATE, 1000 / mSamplesPerSecond);
        }
        return;
    }
    if (mScanningFlag == false) {
        // 保存当前时间
        std::stringstream                     buffer = {};
        std::chrono::system_clock::time_point t      = std::chrono::system_clock::now();
        time_t                                tm     = std::chrono::system_clock::to_time_t(t);
        buffer << std::put_time(localtime(&tm), "%Y-%m-%d__%H-%M-%S");
        mDetectInfo.time = buffer.str();

        mScanButtonValue.fill(0);
        std::regex  reg(R"((\d+)-(\d+)-(\d+)__(.+))");
        std::smatch match;
        if (std::regex_match(mDetectInfo.time, match, reg)) {
            auto year           = match[1].str();
            auto month          = match[2].str();
            auto day            = match[3].str();
            auto tm             = match[4].str();
            mScanTime.yearMonth = year + month;
            mScanTime.day       = day;
            mScanTime.time      = tm;
            auto path           = string(SCAN_DATA_DIR_NAME + GetJobGroup() + "/") + mScanTime.yearMonth + "/" + day;
            std::replace(path.begin(), path.end(), '/', '\\');
            CreateMultipleDirectory(WStringFromString(path).data());
            path += "\\" + tm + ".db";
            mSavePath = path;
            // 创建表
            try {
                HD_Utils::storage(path).sync_schema();
                ORM_Model::DetectInfo::storage(path).sync_schema();
                ORM_Model::DetectInfo::storage(path).insert(mDetectInfo);
                ORM_Model::User::storage(path).sync_schema();
                ORM_Model::User user;
                user.name = mSystemConfig.userName;
                ORM_Model::User::storage(path).insert(user);
                ORM_Model::ScanRecord::storage(path).sync_schema();
                ORM_Model::JobGroup::storage(path).sync_schema();
                ORM_Model::JobGroup jobgroup = {};
                jobgroup.groupName           = mSystemConfig.groupName;
                ORM_Model::JobGroup::storage(path).insert(jobgroup);
                mReviewData.clear();
                mRecordCount = 0;
                mScanRecordCache.clear();
                mScanningFlag = true;
                SetTimer(CSCAN_UPDATE, 1000 / mSamplesPerSecond);
            } catch (std::exception &e) {
                spdlog::warn(GB2312ToUtf8(e.what()));
                DMessageBox(L"请勿快速点击扫查按钮");
            }
        }
    }
}

void GroupScanWnd::StopScan(bool changeFlag) {
    if (mWidgetMode != WidgetMode::MODE_SCAN) {
        return;
    }
    if (!changeFlag) {
        if (mScanningFlag == true) {
            KillTimer(CSCAN_UPDATE);
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
        // 保存缺陷记录
        ORM_Model::ScanRecord::storage(mSavePath).insert_range(mScanRecordCache.begin(), mScanRecordCache.end());
        // 保存扫查数据
        HD_Utils::storage(mSavePath).insert_range(mReviewData.begin(), mReviewData.end());
        // 清除扫查数据
        mReviewData.clear();
        mRecordCount = 0;
        mScanRecordCache.clear();
        KillTimer(CSCAN_UPDATE);
    }
}
