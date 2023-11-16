#include "ChannelSettingWnd.h"
#include "ModelAScan.h"
#include "MeshAScan.h"

ChannelSettingWnd::ChannelSettingWnd(std::unique_ptr<HD_Utils> utils, int channel) : mUtils(std::move(utils)), mChannel(channel) {}

ChannelSettingWnd::~ChannelSettingWnd() {
    if (m_OpenGL) {
        delete m_OpenGL;
        m_OpenGL = nullptr;
    }
}

LPCTSTR ChannelSettingWnd::GetWindowClassName() const {
    return _T("ChannelSettingWnd");
}

CDuiString ChannelSettingWnd::GetSkinFile() {
    return _T(R"(Theme\UI_ChannelSettingWnd.xml)");
}

void ChannelSettingWnd::InitWindow() {
    CDuiWindowBase::InitWindow();
    m_pWndOpenGL = static_cast<CWindowUI*>(m_PaintManager.FindControl(_T("WndOpenGL")));
    m_OpenGL     = new OpenGL;
    m_OpenGL->Create(m_hWnd);
    m_OpenGL->Attach(m_pWndOpenGL);
    m_OpenGL->AddModel<ModelAScan>();
    m_OpenGL->getModel<ModelAScan*>()->SetViewChanel(mChannel);
    mUtils->addReadCallback(std::bind(&ChannelSettingWnd::UpdateAScanCallback, this, std::placeholders::_1, std::placeholders::_2));
    ReadValue2UI();
}

void ChannelSettingWnd::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_RETURN) {
        if (msg.pSender->GetName() == _T("EditSoundVelocity")) {
            mUtils->getBridge()->setSoundVelocity((float)_wtof(msg.pSender->GetText().GetData()));
        } else if (msg.pSender->GetName() == _T("EditFrequency")) {
            mUtils->getBridge()->setFrequency(_wtol(msg.pSender->GetText().GetData()));
        } else if (msg.pSender->GetName() == _T("EditChannelFlag")) {
            mUtils->getBridge()->setChannelFlag(_wtol(msg.pSender->GetText().GetData()));
        } else if (msg.pSender->GetName() == _T("EditScanIncrement")) {
            mUtils->getBridge()->setScanIncrement(_wtol(msg.pSender->GetText().GetData()));
        } else if (msg.pSender->GetName() == _T("EditZeroBias")) {
            mUtils->getBridge()->setZeroBias(mChannel, (float)_wtof(msg.pSender->GetText().GetData()));
        } else if (msg.pSender->GetName() == _T("EditPulseWidth")) {
            mUtils->getBridge()->setPulseWidth(mChannel, (float)_wtof(msg.pSender->GetText().GetData()));
        } else if (msg.pSender->GetName() == _T("EditDelay")) {
            mUtils->getBridge()->setDelay(mChannel, (float)_wtof(msg.pSender->GetText().GetData()));
        } else if (msg.pSender->GetName() == _T("EditSampleDepth")) {
            mUtils->getBridge()->setSampleDepth(mChannel, (float)_wtof(msg.pSender->GetText().GetData()));
        } else if (msg.pSender->GetName() == _T("EditSampleFactor")) {
            mUtils->getBridge()->setSampleFactor(mChannel, _wtol(msg.pSender->GetText().GetData()));
        } else if (msg.pSender->GetName() == _T("EditGain")) {
            mUtils->getBridge()->setGain(mChannel, (float)_wtof(msg.pSender->GetText().GetData()));
        } 
    } else if (msg.sType == DUI_MSGTYPE_TEXTCHANGED) {
        
    } else if (msg.sType == DUI_MSGTYPE_ITEMSELECT) {
        if (msg.pSender->GetName() == _T("ComboVolatage")) {
            auto voltage = static_cast<CComboUI*>(msg.pSender);
            auto index = voltage->GetCurSel();
            mUtils->getBridge()->setVoltage(static_cast<HDBridge::HB_Voltage>(index));
        } else if (msg.pSender->GetName() == _T("ComboFilter")) {
            auto p = static_cast<CComboUI*>(msg.pSender);
            auto index   = p->GetCurSel();
            mUtils->getBridge()->setFilter(mChannel, static_cast<HDBridge::HB_Filter>(index));
        } else if (msg.pSender->GetName() == _T("CombotDemodu")) {
            auto p     = static_cast<CComboUI*>(msg.pSender);
            auto index = p->GetCurSel();
            mUtils->getBridge()->setDemodu(mChannel, static_cast<HDBridge::HB_Demodu>(index));
        } else if (msg.pSender->GetName() == _T("CombotGateBType")) {
            auto p     = static_cast<CComboUI*>(msg.pSender);
            auto index = p->GetCurSel();
            mUtils->getBridge()->setGate2Type(mChannel, static_cast<HDBridge::HB_Gate2Type>(index));
        }
    }
    mUtils->getBridge()->flushSetting();
    CDuiWindowBase::Notify(msg);
}

std::unique_ptr<HD_Utils>&& ChannelSettingWnd::returnHDUtils() {
    spdlog::debug("channel: {}", mChannel);
    mUtils->removeReadCallback();
    return std::move(mUtils);
}

void ChannelSettingWnd::ReadValue2UI() {
    auto edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditSoundVelocity")));
    if (edit) {
        CString str;
        str.Format(_T("%.2f"), mUtils->getBridge()->getSoundVelocity());
        edit->SetText(str);
    }
    edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditFrequency")));
    if (edit) {
        CString str;
        str.Format(_T("%d"), mUtils->getBridge()->getFrequency());
        edit->SetText(str);
    }
    edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditChannelFlag")));
    if (edit) {
        CString str;
        str.Format(_T("%08X"), mUtils->getBridge()->getChannelFlag());
        edit->SetText(str);
    }
    edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditScanIncrement")));
    if (edit) {
        CString str;
        str.Format(_T("%d"), mUtils->getBridge()->getScanIncrement());
        edit->SetText(str);
    }
    edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditZeroBias")));
    if (edit) {
        CString str;
        str.Format(_T("%.2f"), mUtils->getBridge()->getZeroBias(mChannel));
        edit->SetText(str);
    }
    edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditPulseWidth")));
    if (edit) {
        CString str;
        str.Format(_T("%.2f"), mUtils->getBridge()->getPulseWidth()[mChannel]);
        edit->SetText(str);
    }
    edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditDelay")));
    if (edit) {
        CString str;
        str.Format(_T("%.2f"), mUtils->getBridge()->getDelay()[mChannel]);
        edit->SetText(str);
    }
    edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditSampleDepth")));
    if (edit) {
        CString str;
        str.Format(_T("%.2f"), mUtils->getBridge()->getSampleDepth()[mChannel]);
        edit->SetText(str);
    }
    edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditSampleFactor")));
    if (edit) {
        CString str;
        str.Format(_T("%d"), mUtils->getBridge()->getSampleFactor()[mChannel]);
        edit->SetText(str);
    }
    edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditGain")));
    if (edit) {
        CString str;
        str.Format(_T("%.2f"), mUtils->getBridge()->getGain()[mChannel]);
        edit->SetText(str);
    }

}

void ChannelSettingWnd::UpdateAScanCallback(const HDBridge::NM_DATA& data, const HD_Utils& caller) {
    if (data.iChannel == mChannel) {
        auto                                  model  = static_cast<ModelAScan*>(m_OpenGL->m_pModel[0]);
        auto                                  bridge = caller.getBridge();
        auto                                  mesh   = static_cast<MeshAscan*>(model->m_pMesh[0]);
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
}
