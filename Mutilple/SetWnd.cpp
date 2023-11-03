#include "pch.h"

#include "AutoScanInfoWnd.h"
#include "CheckWnd.h"
#include "ControlIP.h"
#include "DACSetWnd.h"
#include "DetectionStd_TBT2995_200.h"
#include "Mutilple.h"
#include "PLCWnd.h"
#include "ProbleWheelWnd.h"
#include "SetQuartWnd.h"
#include "SetWnd.h"
#include "TecListWnd.h"
#include "Techniques.h"
#include <bitset>

// TODO: 暂时屏蔽警告
#pragma warning(disable :4267 4244 4552 4305 4101)

constexpr auto TIME_STATUS = 0;

SetWnd::~SetWnd() {
    // 退出时，关闭采样
    g_MainProcess.m_HDBridge.StartSequencer(FALSE);
    KillTimer(TIME_STATUS);

    if (m_pDACSetWnd != NULL) {
        delete m_pDACSetWnd;
    }
    if (m_PLCWnd != NULL) {
        delete m_PLCWnd;
    }
    if (m_ProbleWheelWnd != NULL) {
        delete m_ProbleWheelWnd;
    }
}

void SetWnd::InitLabel() {
    m_pLabelStatus     = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("LabelStatus")));
    m_pLabelSelectTech = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("LabelSelectTech")));
    m_pLabelSelectTech->SetText(g_MainProcess.m_Techniques.m_pCurTechName);
    // 测量显示
    for (int i = 0; i < MESSURE_NUM; i++) {
        CString strName;
        strName.Format(_T("LabelChannel%d"), i);
        m_pLabelChannel[i] = static_cast<CLabelUI*>(m_PaintManager.FindControl(strName));
    }
    for (int iGate = 0; iGate < MAX_GATE_NUM; iGate++) {
        CString strName;
        strName.Format(_T("LabelGatePos%d"), iGate);
        m_pLabelGatePos[iGate] = static_cast<CEditUI*>(m_PaintManager.FindControl(strName));
        strName.Format(_T("LabelGateAmp%d"), iGate);
        m_pLabelGateAmp[iGate] = static_cast<CEditUI*>(m_PaintManager.FindControl(strName));
    }
    m_pLabelGatePosDistance = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("LabelGatePosDistance")));
    m_pLabelGateBDepth      = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("LabelGateBDepth")));

    m_pLabelBestCir = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("LabelBestCir")));
}

void SetWnd::InitEdit() {
    // 通道参数
    m_pEditRange    = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditRange")));
    m_pEditGain     = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditGain")));
    m_pEditVelocity = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditVelocity")));
    m_pEditDelay    = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditDelay")));
    m_pEditOffset   = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditOffset")));

    m_pEditRange->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditGain->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditVelocity->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditDelay->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditOffset->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);

    // 系统参数
    m_pEditFrequency  = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditFrequency")));
    m_pEditIP_FPGA    = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditIP_FPGA")));
    m_pEditPORT_FPGA  = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditPORT_FPGA")));
    m_pEditIP_PC      = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditIP_PC")));
    m_pEditPORT_PC    = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditPORT_PC")));
    m_pEditPulseWidth = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditPulseWidth")));
    m_pEditFrequency->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditIP_FPGA->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditPORT_FPGA->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditIP_PC->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditPORT_PC->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditPulseWidth->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    // 波门参数
    for (int iGate = 0; iGate < MAX_GATE_NUM; iGate++) {
        CString strName;
        strName.Format(_T("EditGatePos%d"), iGate);
        m_pEditGatePos[iGate] = static_cast<CEditUI*>(m_PaintManager.FindControl(strName));
        strName.Format(_T("EditGateWidth%d"), iGate);
        m_pEditGateWidth[iGate] = static_cast<CEditUI*>(m_PaintManager.FindControl(strName));
        strName.Format(_T("EditGateHeight%d"), iGate);
        m_pEditGateHeight[iGate] = static_cast<CEditUI*>(m_PaintManager.FindControl(strName));

        m_pEditGatePos[iGate]->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
        m_pEditGateWidth[iGate]->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
        m_pEditGateHeight[iGate]->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    }

    // 探伤参数

    m_pEditScanGain       = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditScanGain")));
    m_pEditScanCompensate = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditScanCompensate")));
    m_pEditTrUTGain       = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditTrUTGain")));
    m_pEditTrUtcompensate = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditTrUtcompensate")));

    m_pEditScanGain->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditScanCompensate->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditTrUTGain->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditTrUtcompensate->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);

    // 工件参数
    m_pEditWheelName      = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditWheelName")));
    m_pEditEditWheelModel = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditWheelModel")));
    m_pEditDetectionStd   = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditDetectionStd")));
    m_pEditWheelName->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditEditWheelModel->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditDetectionStd->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);

    m_pEditSideYPos   = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditSideYPos")));
    m_pEditSideXStart = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditSideXStart")));
    m_pEditSideXEnd   = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditSideXEnd")));
    m_pEditSideXStep  = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditSideXStep")));
    m_pEditSideYPos->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditSideXStart->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditSideXEnd->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditSideXStep->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);

    m_pEditTreadLyXPos = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditTreadLyXPos")));
    m_pEditTreadXPos   = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditTreadXPos")));
    m_pEditTreadYStart = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditTreadYStart")));
    m_pEditTreadYEnd   = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditTreadYEnd")));
    m_pEditTreadYStep  = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditTreadYStep")));
    m_pEditTreadLyXPos->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditTreadXPos->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditTreadYStart->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditTreadYEnd->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditTreadYStep->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);

    m_pEditSideXSpeed1 = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditSideXSpeed1")));
    m_pEditSideXSpeed2 = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditSideXSpeed2")));
    m_pEditSideYSpeed1 = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditSideYSpeed1")));
    m_pEditSideYSpeed2 = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditSideYSpeed2")));

    m_pEditSideXSpeed1->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditSideXSpeed2->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditSideYSpeed1->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditSideYSpeed2->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);

    m_pEditTreadXSpeed1 = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditTreadXSpeed1")));
    m_pEditTreadXSpeed2 = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditTreadXSpeed2")));
    m_pEditTreadYSpeed1 = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditTreadYSpeed1")));
    m_pEditTreadYSpeed2 = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditTreadYSpeed2")));
    m_pEditRotateSpeed1 = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditRotateSpeed1")));

    m_pEditTreadXSpeed1->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditTreadXSpeed2->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditTreadYSpeed1->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditTreadYSpeed2->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);
    m_pEditRotateSpeed1->OnNotify += MakeDelegate(this, &SetWnd::OnEdit);

    m_pLayTrUTGain        = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("LayTrUTGain")));
    m_pLayTrUTtcompensate = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("LayTrUTtcompensate")));

    CString strName;
    for (int i = 0; i < HD_CHANNEL_NUM; i++) {
        strName.Format(_T("OptCH%d"), i + 1);
        m_pOptCh[i] = static_cast<COptionUI*>(m_PaintManager.FindControl(strName));
    }
    m_pLayTrUTGain->SetVisible(false);
    m_pLayTrUTtcompensate->SetVisible(false);
}

void SetWnd::InitCombo() {
    // 系统参数
    m_pComboPulseWidth = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("ComboPulseWidth")));
    m_pComboVoltage    = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("ComboVoltage")));

    // 通道参数
    m_pComboChannel = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("ComboChannel")));
    m_pComboFilter  = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("ComboFilter")));
    m_pComboDemodu  = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("ComboDemodu")));
    m_pComboChFlag  = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("ComboChFlag")));
}

void SetWnd::InitOpenGL() {
    m_pWndOpenGL = static_cast<CWindowUI*>(m_PaintManager.FindControl(_T("WndOpenGL")));

    // 初始化OpenGL窗口
    m_OpenGL.Create(m_hWnd);
    m_OpenGL.Attach(m_pWndOpenGL);
    m_OpenGL.AddSetModel(&g_MainProcess.m_Techniques);
}

void SetWnd::InitWindow() {
    CDuiWindowBase::InitWindow();
    m_PLCWnd         = NULL;
    m_ProbleWheelWnd = NULL;
    m_pDACSetWnd     = NULL;
    InitLabel();
    InitEdit();
    InitCombo();
    InitOpenGL();

    // 设置板卡参数
    g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
    Sleep(100);
    g_MainProcess.m_HDBridge.StartSequencer(TRUE);
    if (g_MainProcess.m_Techniques.m_iDetectionStd == DETECTIONSTD_TBT2995_200) {
        m_pSTD = (DetectionStd_TBT2995_200*)g_MainProcess.m_Techniques.GetDetectionStd();
    }
    UpdateUI();
    UpdateDAC();
    SetTimer(TIME_STATUS, 500);
}

void SetWnd::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        CDuiString strName = msg.pSender->GetName();
        if (strName == _T("BtnSpin")) {
            // UserData里面保存Spin按键需要控制的按键名称与步进
            CDuiString strData = msg.pSender->GetUserData();

            TCHAR pName[64]{0};
            float fValue = 0;
            _stscanf_s(strData.GetData(), _T("%[^:]:%f"), pName, 64, &fValue);

            OnBtnSpin(pName, fValue);
        }

        else if (strName == _T("BtnCopy")) {
            OnBtnCopy();
        } else if (strName == _T("BtnLoad")) {
            OnBtnLoad();
        } else if (strName == _T("BtnSave")) {
            OnBtnSave();
        } else if (strName == _T("BtnLocalIp")) {
            OnSetLocalIp();
        } else if (strName == _T("BtnFPGA")) {
            // OnSetFPGAIp();
        } else if (strName == _T("hidenBtn")) {
            OnReturn();
        } else if (strName == _T("IDCANCELSetWnd")) { // 退出计算最佳步进和圈数
            OnReturn();
        } else if (strName == _T("BtnBestStep")) {
            OnSetBestStep();
        }

        else if (strName == _T("BtnDACSet")) {
            if (m_pDACSetWnd == NULL) {
                m_pDACSetWnd            = new DACSetWnd();
                m_pDACSetWnd->m_pSetWnd = this;
                m_pDACSetWnd->Create(m_hWnd, _T("PLCSetWnd"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
                m_pDACSetWnd->CenterWindow();
            } else {
                m_pDACSetWnd->ShowWindow(SW_NORMAL);
                m_pDACSetWnd->UpdateUI();
            }

        } else if (strName == _T("BtnSetSpeed")) {
            bool res = g_MainProcess.m_ConnectPLC.SetPLC_Speed();
            if (!res) {
                DMessageBox(_Tr("PLC未连接！"));
            }
        } else if (strName == _T("BtnMorePLC")) {
            if (m_PLCWnd == NULL) {
                m_PLCWnd = new PLCWnd();
                m_PLCWnd->Create(m_hWnd, _T("PLCWnd"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
                m_PLCWnd->CenterWindow();
            }
            m_PLCWnd->ShowWindow(SW_NORMAL);
            m_PLCWnd->ResumeSyncThread();
        } else if (strName == _T("BtnProbleWheel")) {
            if (m_ProbleWheelWnd == NULL) {
                m_ProbleWheelWnd = new ProbleWheelWnd();
                m_ProbleWheelWnd->Create(m_hWnd, _T("ProbleWheelWnd"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
                m_ProbleWheelWnd->CenterWindow();
                m_ProbleWheelWnd->ShowWindow(SW_NORMAL);
            } else {
                m_ProbleWheelWnd->ShowWindow(SW_NORMAL);
                m_ProbleWheelWnd->UpdateUI();
            }
        }
    } else if (msg.sType == DUI_MSGTYPE_ITEMSELECT) {
        CDuiString strName = msg.pSender->GetName();

        if (strName == _T("ComboChannel")) {
            INT iIndex = m_pComboChannel->GetCurSel();

            if (iIndex >= 0) {
                g_MainProcess.m_Techniques.m_iChannel = iIndex;

                UpdateChannelUI(&g_MainProcess.m_Techniques.m_pChannel[g_MainProcess.m_Techniques.m_iChannel]);
                UpdateUI();
                if (m_pDACSetWnd != NULL) {
                    m_pDACSetWnd->UpdateUI();
                }

                if (g_MainProcess.m_Techniques.m_iChannel == g_MainProcess.m_Techniques.m_Scan.m_pSideTrUT[0] ||
                    g_MainProcess.m_Techniques.m_iChannel == g_MainProcess.m_Techniques.m_Scan.m_pSideTrUT[1]) {
                    m_pLayTrUTGain->SetVisible(true);
                    m_pLayTrUTtcompensate->SetVisible(true);

                } else {
                    m_pLayTrUTGain->SetVisible(false);
                    m_pLayTrUTtcompensate->SetVisible(false);
                }
            }
        } else if (strName == _T("ComboChFlag")) // 通道设置单晶双晶
        {
            INT iIndex = m_pComboChFlag->GetCurSel();
            int iChMode[HD_CHANNEL_NUM];
            for (int i = 0; i < HD_CHANNEL_NUM; i++) {
                iChMode[i] = int((g_MainProcess.m_Techniques.m_System.m_iChMode >> i) & 0x01);
            }
            iChMode[g_MainProcess.m_Techniques.m_iChannel] = iIndex;
            bitset<32> ch                                  = g_MainProcess.m_Techniques.m_System.m_iChMode;
            ch.set(g_MainProcess.m_Techniques.m_iChannel, iChMode[g_MainProcess.m_Techniques.m_iChannel]);
            g_MainProcess.m_Techniques.m_System.m_iChMode = ch.to_ulong();

            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
        } else if (strName == _T("ComboFilter")) {
            INT iIndex = m_pComboFilter->GetCurSel();

            if (iIndex >= 0) {
                Channel* pChannel   = &g_MainProcess.m_Techniques.m_pChannel[g_MainProcess.m_Techniques.m_iChannel];
                pChannel->m_iFilter = iIndex;

                g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
            }
        } else if (strName == _T("ComboDemodu")) {
            INT iIndex = m_pComboDemodu->GetCurSel();

            if (iIndex >= 0) {
                Channel* pChannel   = &g_MainProcess.m_Techniques.m_pChannel[g_MainProcess.m_Techniques.m_iChannel];
                pChannel->m_iDemodu = iIndex;

                g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
            }
        } else if (strName == _T("ComboPulseWidth")) {
            INT iIndex = m_pComboPulseWidth->GetCurSel();

            if (iIndex >= 0) {
                g_MainProcess.m_Techniques.m_System.m_iPulseWidth = iIndex;
                g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
            }
        } else if (strName == _T("ComboVoltage")) {
            INT iIndex = m_pComboVoltage->GetCurSel();

            if (iIndex >= 0) {
                g_MainProcess.m_Techniques.m_System.m_iVoltage = iIndex;
                g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
            }
        }
    }

    CDuiWindowBase::Notify(msg);
}

bool SetWnd::OnEdit(void* pv) {
    TNotifyUI* msg = (TNotifyUI*)pv;

    if (msg->sType == DUI_MSGTYPE_RETURN || msg->sType == DUI_MSGTYPE_KILLFOCUS) {
        CDuiString strName = msg->pSender->GetName();
        CDuiString strText = msg->pSender->GetText();

        if (strName == _T("EditGatePos0")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < 0 || fValue > 100) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

            Channel* pChannel            = g_MainProcess.GetCurChannel();
            pChannel->m_pGatePos[GATE_A] = fValue / 100.0f;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
        } else if (strName == _T("EditGatePos1")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < 0 || fValue > 100) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

            Channel* pChannel            = g_MainProcess.GetCurChannel();
            pChannel->m_pGatePos[GATE_B] = fValue / 100.0f;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
        } else if (strName == _T("EditGateWidth0")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < 0 || fValue > 100) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

            Channel* pChannel              = g_MainProcess.GetCurChannel();
            pChannel->m_pGateWidth[GATE_A] = fValue / 100.0f;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
        } else if (strName == _T("EditGateWidth1")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < 0 || fValue > 100) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

            Channel* pChannel              = g_MainProcess.GetCurChannel();
            pChannel->m_pGateWidth[GATE_B] = fValue / 100.0f;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
        } else if (strName == _T("EditGateHeight0")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < 0 || fValue > 100) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

            Channel* pChannel               = g_MainProcess.GetCurChannel();
            pChannel->m_pGateHeight[GATE_A] = fValue / 100.0f;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
        } else if (strName == _T("EditGateHeight1")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < 0 || fValue > 100) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

            Channel* pChannel               = g_MainProcess.GetCurChannel();
            pChannel->m_pGateHeight[GATE_B] = fValue / 100.0f;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
        } else if (strName == _T("EditRange")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_RANGE) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

            Channel* pChannel = g_MainProcess.GetCurChannel();

            FLOAT fSampleDepth = fValue * 2000.0f / (float)pChannel->m_iVelocity;

            if (fSampleDepth > MAX_SAMPLEDEPTH) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

            pChannel->m_fRange = fValue;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
        } else if (strName == _T("EditGain")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_GAIN || fValue > MAX_GAIN) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

            Channel* pChannel = g_MainProcess.GetCurChannel();
            pChannel->m_fGain = fValue;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
            UpdateDAC();
        } else if (strName == _T("EditVelocity")) {
            int iValue(0);
            _stscanf_s(strText.GetData(), _T("%d"), &iValue);

            if (iValue < MIN_VELOCITY || iValue > MAX_VELOCITY) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

            Channel* pChannel     = g_MainProcess.GetCurChannel();
            pChannel->m_iVelocity = iValue;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
        } else if (strName == _T("EditDelay")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            Channel* pChannel = g_MainProcess.GetCurChannel();

            FLOAT fDelay = fValue * 2000.0f / (float)pChannel->m_iVelocity + pChannel->m_fOffset;

            if (fDelay < MIN_DELAY || fDelay > MAX_DELAY) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

            pChannel->m_fDelay = fValue;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
        } else if (strName == _T("EditOffset")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            Channel* pChannel = g_MainProcess.GetCurChannel();

            FLOAT fDelay = pChannel->m_fDelay * 2000.0f / (float)pChannel->m_iVelocity + fValue;

            if (fDelay < MIN_DELAY || fDelay > MAX_DELAY) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

            pChannel->m_fOffset = fValue;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
        } else if (strName == _T("EditFrequency")) {
            int iValue(0);
            _stscanf_s(strText.GetData(), _T("%d"), &iValue);

            if (iValue < MIN_FREQUENCY || iValue > MAX_FREQUENCY) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

            g_MainProcess.m_Techniques.m_System.m_iFrequency = iValue;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
        } else if (strName == _T("EditPulseWidth")) {
            int iValue(0);
            _stscanf_s(strText.GetData(), _T("%d"), &iValue);

            if (iValue < MIN_PULSEWIDTH || iValue > MAX_PULSEWIDTH) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

            g_MainProcess.m_Techniques.m_System.m_iPulseWidth = iValue;
            g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
        } else if (strName == _T("EditIP_FPGA")) {
            int pValue[4]{0};
            int iGet = _stscanf_s(strText.GetData(), _T("%d.%d.%d.%d"), &pValue[0], &pValue[1], &pValue[2], &pValue[3]);

            if (iGet == 4) {
            } else {
                DMessageBox(_Tr("非法的IP地址"));
            }
        } else if (strName == _T("EditPORT_FPGA")) {
            int iValue(0);
            _stscanf_s(strText.GetData(), _T("%d"), &iValue);

            if (iValue < 0 || iValue > 65535) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

        } else if (strName == _T("EditIP_PC")) {
            int pValue[4]{0};
            int iGet = _stscanf_s(strText.GetData(), _T("%d.%d.%d.%d"), &pValue[0], &pValue[1], &pValue[2], &pValue[3]);

            if (iGet == 4) {
            } else {
                DMessageBox(_Tr("非法的IP地址"));
            }
        } else if (strName == _T("EditPORT_PC")) {
            int iValue(0);
            _stscanf_s(strText.GetData(), _T("%d"), &iValue);

            if (iValue < 0 || iValue > 65535) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }

        }

        else if (strName == _T("EditScanGain")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_GAIN || fValue > MAX_GAIN) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            int ch                                       = g_MainProcess.m_Techniques.m_iChannel;
            m_pSTD->mDetetionParam2995_200[ch].fScanGain = fValue;
        }

        else if (strName == _T("EditScanCompensate")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_GAIN || fValue > MAX_GAIN) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            int ch                                             = g_MainProcess.m_Techniques.m_iChannel;
            m_pSTD->mDetetionParam2995_200[ch].fScanCompensate = fValue;
        }

        else if (strName == _T("EditTrUTGain")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_GAIN || fValue > MAX_GAIN) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            int ch                                           = g_MainProcess.m_Techniques.m_iChannel;
            m_pSTD->mDetetionParam2995_200[ch].fScanTrUTGain = fValue;
        }

        else if (strName == _T("EditTrUtcompensate")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_GAIN || fValue > MAX_GAIN) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            int ch                                                 = g_MainProcess.m_Techniques.m_iChannel;
            m_pSTD->mDetetionParam2995_200[ch].fScanTrUTCompensate = fValue;
        } else if (strName == _T("EditWheelModel")) {
            StrCpy(g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.szWheelModel, strText.GetData());

        } else if (strName == _T("EditWheelName")) {
            StrCpy(g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.szWheelName, strText.GetData());

        } else if (strName == _T("EditDetectionStd")) {
            StrCpy(g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.szDetectionStd, strText.GetData());

        } else if (strName == _T("EditSideYPos")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_Y_POS || fValue > MAX_Y_POS) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideYPos = fValue;
        } else if (strName == _T("EditSideXStart")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXEnd || fValue > MAX_X_POS) {
                DMessageBox(_Tr("非法的参数值，侧面起始X不能小于结束X"));
                return false;
            }
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart = fValue;
        } else if (strName == _T("EditSideXEnd")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_X_POS || fValue > g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart) {
                DMessageBox(_Tr("非法的参数值，侧面结束X要小于开始X"));
                return false;
            }
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXEnd = fValue;
        } else if (strName == _T("EditSideXStep")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep = fValue;
        } else if (strName == _T("EditTreadLyXPos")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_X_POS || fValue > MAX_X_POS) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadLyXPos = fValue;
        } else if (strName == _T("EditTreadXPos")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_X_POS || fValue > MAX_X_POS) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadXPos = fValue;
        }

        else if (strName == _T("EditTreadYStart")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYEnd || fValue > MAX_Y_POS) {
                DMessageBox(_Tr("非法的参数值,踏面起始Y要大于结束Y"));
                return false;
            }
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart = fValue;
        } else if (strName == _T("EditTreadYEnd")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_Y_POS || fValue > g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart) {
                DMessageBox(_Tr("非法的参数值，踏面结束Y要小于起始Y"));
                return false;
            }
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYEnd = fValue;
        } else if (strName == _T("EditTreadYStep")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep = fValue;
        }

        else if (strName == _T("EditSideXSpeed1")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_Y_Speed || fValue > MAX_Y_Speed) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_ConnectPLC.mPLCSpeed.fSideXSpeed1 = fValue;
        } else if (strName == _T("EditSideXSpeed2")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_Y_Speed || fValue > MAX_Y_Speed) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_ConnectPLC.mPLCSpeed.fSideXSpeed2 = fValue;
        } else if (strName == _T("EditSideYSpeed1")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_Y_Speed || fValue > MAX_Y_Speed) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_ConnectPLC.mPLCSpeed.fSideYSpeed1 = fValue;
        } else if (strName == _T("EditSideYSpeed2")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_Y_Speed || fValue > MAX_Y_Speed) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_ConnectPLC.mPLCSpeed.fSideYSpeed2 = fValue;
        }

        else if (strName == _T("EditTreadXSpeed1")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_Y_Speed || fValue > MAX_Y_Speed) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_ConnectPLC.mPLCSpeed.fTreadXSpeed1 = fValue;
        } else if (strName == _T("EditTreadXSpeed2")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_Y_Speed || fValue > MAX_Y_Speed) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_ConnectPLC.mPLCSpeed.fTreadXSpeed2 = fValue;
        } else if (strName == _T("EditTreadYSpeed1")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_Y_Speed || fValue > MAX_Y_Speed) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_ConnectPLC.mPLCSpeed.fTreadYSpeed1 = fValue;
        } else if (strName == _T("EditTreadYSpeed2")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_Y_Speed || fValue > MAX_Y_Speed) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_ConnectPLC.mPLCSpeed.fTreadYSpeed2 = fValue;
        }

        else if (strName == _T("EditRotateSpeed1")) {
            FLOAT fValue(0.0f);
            _stscanf_s(strText.GetData(), _T("%f"), &fValue);

            if (fValue < MIN_Y_Speed || fValue > MAX_Y_Speed) {
                DMessageBox(_Tr("非法的参数值"));
                return false;
            }
            g_MainProcess.m_ConnectPLC.mPLCSpeed.fRotateSpeed1 = fValue;
        }
    }

    return true;
}

void SetWnd::UpdateUI() {
    CString strText;
    // 设置IP

    // 系统参数
    strText.Format(_T("%d"), g_MainProcess.m_Techniques.m_System.m_iFrequency);
    m_pEditFrequency->SetText(strText);
    strText.Format(_T("%d"), g_MainProcess.m_Techniques.m_System.m_iPulseWidth);
    m_pEditPulseWidth->SetText(strText);
    // m_pComboPulseWidth->SelectItem(g_MainProcess.m_Techniques.m_System.m_iPulseWidth);
    m_pComboVoltage->SelectItem(g_MainProcess.m_Techniques.m_System.m_iVoltage);

    // 设置通道参数
    m_pComboChannel->SelectItem(g_MainProcess.m_Techniques.m_iChannel);

    UpdateChannelUI(&g_MainProcess.m_Techniques.m_pChannel[g_MainProcess.m_Techniques.m_iChannel]);

    // 设置 扫差参数
    strText.Format(_T("%.1f"), m_pSTD->mDetetionParam2995_200[g_MainProcess.m_Techniques.m_iChannel].fScanGain);
    m_pEditScanGain->SetText(strText);
    strText.Format(_T("%.1f"), m_pSTD->mDetetionParam2995_200[g_MainProcess.m_Techniques.m_iChannel].fScanCompensate);
    m_pEditScanCompensate->SetText(strText);
    strText.Format(_T("%.1f"), m_pSTD->mDetetionParam2995_200[g_MainProcess.m_Techniques.m_iChannel].fScanTrUTGain);
    m_pEditTrUTGain->SetText(strText);
    strText.Format(_T("%.1f"), m_pSTD->mDetetionParam2995_200[g_MainProcess.m_Techniques.m_iChannel].fScanTrUTCompensate);
    m_pEditTrUtcompensate->SetText(strText);

    // 设置工件参数
    // strText.Format(_T("%.2f"), g_MainProcess.m_Techniques.m_iChannel].fScanTrUTCompensate);
    m_pEditWheelName->SetText(g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.szWheelName);
    m_pEditEditWheelModel->SetText(g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.szWheelModel);
    m_pEditDetectionStd->SetText(g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.szDetectionStd);

    // 设置PLC 参数
    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideYPos);
    m_pEditSideYPos->SetText(strText);
    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart);
    m_pEditSideXStart->SetText(strText);
    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXEnd);
    m_pEditSideXEnd->SetText(strText);
    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep);
    m_pEditSideXStep->SetText(strText);

    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadLyXPos);
    m_pEditTreadLyXPos->SetText(strText);
    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadXPos);
    m_pEditTreadXPos->SetText(strText);
    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart);
    m_pEditTreadYStart->SetText(strText);
    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYEnd);
    m_pEditTreadYEnd->SetText(strText);
    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep);
    m_pEditTreadYStep->SetText(strText);
    strText.Format(_T("最佳圈数:%d圈"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.nBestCirSize);
    m_pLabelBestCir->SetText(strText);

    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.mPLCSpeed.fSideXSpeed1);
    m_pEditSideXSpeed1->SetText(strText);
    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.mPLCSpeed.fSideXSpeed2);
    m_pEditSideXSpeed2->SetText(strText);
    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.mPLCSpeed.fSideYSpeed1);
    m_pEditSideYSpeed1->SetText(strText);
    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.mPLCSpeed.fSideYSpeed2);
    m_pEditSideYSpeed2->SetText(strText);

    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.mPLCSpeed.fTreadXSpeed1);
    m_pEditTreadXSpeed1->SetText(strText);
    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.mPLCSpeed.fTreadXSpeed2);
    m_pEditTreadXSpeed2->SetText(strText);
    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.mPLCSpeed.fTreadYSpeed1);
    m_pEditTreadYSpeed1->SetText(strText);
    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.mPLCSpeed.fTreadYSpeed2);
    m_pEditTreadYSpeed2->SetText(strText);
    strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.mPLCSpeed.fRotateSpeed1);
    m_pEditRotateSpeed1->SetText(strText);
}

void SetWnd::UpdateChannelUI(Channel* pChannel) {
    CString strText;
    strText.Format(_T("%.1f"), pChannel->m_fRange);
    m_pEditRange->SetText(strText);
    strText.Format(_T("%.1f"), pChannel->m_fGain);
    m_pEditGain->SetText(strText);
    strText.Format(_T("%d"), pChannel->m_iVelocity);
    m_pEditVelocity->SetText(strText);
    strText.Format(_T("%.1f"), pChannel->m_fDelay);
    m_pEditDelay->SetText(strText);
    strText.Format(_T("%.2f"), pChannel->m_fOffset);
    m_pEditOffset->SetText(strText);

    m_pComboFilter->SelectItem(pChannel->m_iFilter);
    m_pComboDemodu->SelectItem(pChannel->m_iDemodu);
    int iChMode[HD_CHANNEL_NUM]; // 通道设置单晶双晶
    for (int i = 0; i < HD_CHANNEL_NUM; i++) {
        iChMode[i] = int((g_MainProcess.m_Techniques.m_System.m_iChMode >> i) & 0x01);
    }
    m_pComboChFlag->SelectItem(iChMode[g_MainProcess.m_Techniques.m_iChannel]);

    // 波门
    for (int iGate = 0; iGate < MAX_GATE_NUM; iGate++) {
        strText.Format(_T("%.1f"), pChannel->m_pGatePos[iGate] * 100);
        m_pEditGatePos[iGate]->SetText(strText);
        strText.Format(_T("%.1f"), pChannel->m_pGateWidth[iGate] * 100);
        m_pEditGateWidth[iGate]->SetText(strText);
        strText.Format(_T("%.1f"), pChannel->m_pGateHeight[iGate] * 100);
        m_pEditGateHeight[iGate]->SetText(strText);
    }
    // m_pEditScanGain, * m_pEditScanCompensate, * m_pEditTrUTGain, * m_pEditTrUtcompensate;
    UpdateDAC();
}

void SetWnd::OnTimer(int iIdEvent) {
    switch (iIdEvent) {
        case TIME_STATUS: {
            CString strInfo;
            // 测量显示
            strInfo.Format(_Tr("%d"), g_MainProcess.GetChannelIndex() + 1);
            for (int i = 0; i < MESSURE_NUM; i++) {
                m_pLabelChannel[i]->SetText(strInfo);
            }
            if (g_MainProcess.m_Techniques.m_iChannel < 6) {
                UpdateDAC();
            }

            Channel* pChannel = g_MainProcess.GetCurChannel();
            if (pChannel != nullptr) {
                for (int iGate = 0; iGate < MAX_GATE_NUM; iGate++) {
                    strInfo.Format(_Tr("%.1fmm"), pChannel->m_pGateData[iGate].fPos + pChannel->m_fDelay);
                    m_pLabelGatePos[iGate]->SetText(strInfo);
                    strInfo.Format(_Tr("%.1f%%"), pChannel->m_pGateData[iGate].fAmp * 100.0f);
                    m_pLabelGateAmp[iGate]->SetText(strInfo);
                }
                strInfo.Format(_Tr("%.1fmm"), pChannel->m_pGateData[GATE_B].fPos - pChannel->m_pGateData[GATE_A].fPos);
                m_pLabelGatePosDistance->SetText(strInfo);

                strInfo.Format(_Tr("%.1fmm"), pChannel->m_pGatePos[GATE_B] * pChannel->m_fRange);
                m_pLabelGateBDepth->SetText(strInfo);
            }

            // 状态显示
            if (g_MainProcess.m_HDBridge.m_iNetwork == -1) {
                strInfo = _Tr("超声板卡未连接");
            } else {
                //	UCHAR* pVersion = UNION_PORT_GetVersion(g_MainProcess.m_HDBridge.m_iNetwork);
                // if (pVersion != NULL)
                //	strInfo.Format(_Tr("FPGA: %d.%d - %04d.%02d.%02d"), pVersion[0], pVersion[1], pVersion[2] * 100 + pVersion[3], pVersion[4], pVersion[5]);
                // else
                //	strInfo = _Tr("超声板卡未响应");
            }
            m_pLabelStatus->SetText(strInfo);
        } break;
    }
}
void SetWnd::UpdateGain_ScanGain() {
    Channel* pChannel = g_MainProcess.GetCurChannel();
    int      ch       = g_MainProcess.m_Techniques.m_iChannel;
    pChannel->m_fGain;

    m_pSTD->mDetetionParam2995_200[ch].fScanGain = pChannel->m_fGain;
    CString strText;
    strText.Format(_T("%.1f"), pChannel->m_fGain);
    m_pEditGain->SetText(strText);
    strText.Format(_T("%.1f"), m_pSTD->mDetetionParam2995_200[g_MainProcess.m_Techniques.m_iChannel].fScanGain);
    m_pEditScanGain->SetText(strText);
}
void SetWnd::OnBtnSpin(LPCTSTR lpName, float fValue) {
    CEditUI* pEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(lpName));
    if (pEdit == nullptr)
        return;

    CString strText;
    if (!_tcsicmp(lpName, _T("EditRange"))) {
        Channel* pChannel = g_MainProcess.GetCurChannel();
        pChannel->m_fRange += fValue;

        float max_range = MAX_SAMPLEDEPTH * (float)pChannel->m_iVelocity / 2000.0f;
        if (pChannel->m_fRange < MIN_RANGE)
            pChannel->m_fRange = MIN_RANGE;
        if (pChannel->m_fRange > max_range)
            pChannel->m_fRange = max_range;

        strText.Format(_T("%.1f"), pChannel->m_fRange);
    }

    else if (!_tcsicmp(lpName, _T("EditGain"))) {
        Channel* pChannel = g_MainProcess.GetCurChannel();
        pChannel->m_fGain += fValue;

        if (pChannel->m_fGain < MIN_GAIN)
            pChannel->m_fGain = MIN_GAIN;
        if (pChannel->m_fGain > MAX_GAIN)
            pChannel->m_fGain = MAX_GAIN;

        strText.Format(_T("%.1f"), pChannel->m_fGain);
        UpdateGain_ScanGain();
    } else if (!_tcsicmp(lpName, _T("EditVelocity"))) {
        Channel* pChannel     = g_MainProcess.GetCurChannel();
        pChannel->m_iVelocity = int((float)pChannel->m_iVelocity + fValue + 0.5f);

        if (pChannel->m_iVelocity < MIN_VELOCITY)
            pChannel->m_iVelocity = MIN_VELOCITY;
        if (pChannel->m_iVelocity > MAX_VELOCITY)
            pChannel->m_iVelocity = MAX_VELOCITY;

        strText.Format(_T("%d"), pChannel->m_iVelocity);
    } else if (!_tcsicmp(lpName, _T("EditDelay"))) {
        Channel* pChannel = g_MainProcess.GetCurChannel();
        pChannel->m_fDelay += fValue;

        float min_delay = (MIN_DELAY - pChannel->m_fOffset) * (float)pChannel->m_iVelocity / 2000.0f;
        if (min_delay < 0)
            min_delay = 0;
        float max_delay = (MAX_SAMPLEDEPTH - pChannel->m_fOffset) * (float)pChannel->m_iVelocity / 2000.0f;
        if (pChannel->m_fDelay < min_delay)
            pChannel->m_fDelay = min_delay;
        if (pChannel->m_fDelay > max_delay)
            pChannel->m_fDelay = max_delay;

        strText.Format(_T("%.1f"), pChannel->m_fDelay);
    } else if (!_tcsicmp(lpName, _T("EditOffset"))) {
        Channel* pChannel = g_MainProcess.GetCurChannel();
        pChannel->m_fOffset += fValue;

        FLOAT fdelay    = pChannel->m_fDelay * 2000.0f / (float)pChannel->m_iVelocity;
        float min_delay = MIN_DELAY - fdelay;
        float max_delay = MAX_SAMPLEDEPTH - fdelay;

        if (pChannel->m_fOffset < min_delay)
            pChannel->m_fOffset = min_delay;
        if (pChannel->m_fOffset > max_delay)
            pChannel->m_fOffset = max_delay;

        strText.Format(_T("%.2f"), pChannel->m_fOffset);
    } else if (!_tcsicmp(lpName, _T("EditFrequency"))) {
        g_MainProcess.m_Techniques.m_System.m_iFrequency = int(g_MainProcess.m_Techniques.m_System.m_iFrequency + fValue + 0.5F);

        if (g_MainProcess.m_Techniques.m_System.m_iFrequency < MIN_FREQUENCY)
            g_MainProcess.m_Techniques.m_System.m_iFrequency = MIN_FREQUENCY;
        if (g_MainProcess.m_Techniques.m_System.m_iFrequency > MAX_FREQUENCY)
            g_MainProcess.m_Techniques.m_System.m_iFrequency = MAX_FREQUENCY;

        strText.Format(_T("%d"), g_MainProcess.m_Techniques.m_System.m_iFrequency);
    } else if (!_tcsicmp(lpName, _T("EditPulseWidth"))) {
        g_MainProcess.m_Techniques.m_System.m_iPulseWidth = int(g_MainProcess.m_Techniques.m_System.m_iPulseWidth + fValue);

        if (g_MainProcess.m_Techniques.m_System.m_iPulseWidth < MIN_PULSEWIDTH)
            g_MainProcess.m_Techniques.m_System.m_iPulseWidth = MIN_PULSEWIDTH;
        if (g_MainProcess.m_Techniques.m_System.m_iPulseWidth > MAX_PULSEWIDTH)
            g_MainProcess.m_Techniques.m_System.m_iPulseWidth = MAX_PULSEWIDTH;

        strText.Format(_T("%d"), g_MainProcess.m_Techniques.m_System.m_iPulseWidth);
    } else if (!_tcsicmp(lpName, _T("EditGatePos0"))) {
        Channel* pChannel = g_MainProcess.GetCurChannel();
        pChannel->m_pGatePos[GATE_A] += fValue;

        if (pChannel->m_pGatePos[GATE_A] < 0.0f)
            pChannel->m_pGatePos[GATE_A] = 0.0f;
        if (pChannel->m_pGatePos[GATE_A] > 1.0f)
            pChannel->m_pGatePos[GATE_A] = 1.0f;

        strText.Format(_T("%.1f"), pChannel->m_pGatePos[GATE_A] * 100.0f);
    } else if (!_tcsicmp(lpName, _T("EditGatePos1"))) {
        Channel* pChannel = g_MainProcess.GetCurChannel();
        pChannel->m_pGatePos[GATE_B] += fValue;

        if (pChannel->m_pGatePos[GATE_B] < 0.0f)
            pChannel->m_pGatePos[GATE_B] = 0.0f;
        if (pChannel->m_pGatePos[GATE_B] > 1.0f)
            pChannel->m_pGatePos[GATE_B] = 1.0f;

        strText.Format(_T("%.1f"), pChannel->m_pGatePos[GATE_B] * 100.0f);
    } else if (!_tcsicmp(lpName, _T("EditGateWidth0"))) {
        Channel* pChannel = g_MainProcess.GetCurChannel();
        pChannel->m_pGateWidth[GATE_A] += fValue;

        if (pChannel->m_pGateWidth[GATE_A] < 0.0f)
            pChannel->m_pGateWidth[GATE_A] = 0.0f;
        if (pChannel->m_pGateWidth[GATE_A] > 1.0f)
            pChannel->m_pGateWidth[GATE_A] = 1.0f;

        strText.Format(_T("%.1f"), pChannel->m_pGateWidth[GATE_A] * 100.0f);
    } else if (!_tcsicmp(lpName, _T("EditGateWidth1"))) {
        Channel* pChannel = g_MainProcess.GetCurChannel();
        pChannel->m_pGateWidth[GATE_B] += fValue;

        if (pChannel->m_pGateWidth[GATE_B] < 0.0f)
            pChannel->m_pGateWidth[GATE_B] = 0.0f;
        if (pChannel->m_pGateWidth[GATE_B] > 1.0f)
            pChannel->m_pGateWidth[GATE_B] = 1.0f;

        strText.Format(_T("%.1f"), pChannel->m_pGateWidth[GATE_B] * 100.0f);
    } else if (!_tcsicmp(lpName, _T("EditGateHeight0"))) {
        Channel* pChannel = g_MainProcess.GetCurChannel();
        pChannel->m_pGateHeight[GATE_A] += fValue;

        if (pChannel->m_pGateHeight[GATE_A] < 0.0f)
            pChannel->m_pGateHeight[GATE_A] = 0.0f;
        if (pChannel->m_pGateHeight[GATE_A] > 1.0f)
            pChannel->m_pGateHeight[GATE_A] = 1.0f;

        strText.Format(_T("%.1f"), pChannel->m_pGateHeight[GATE_A] * 100.0f);
    } else if (!_tcsicmp(lpName, _T("EditGateHeight1"))) {
        Channel* pChannel = g_MainProcess.GetCurChannel();
        pChannel->m_pGateHeight[GATE_B] += fValue;

        if (pChannel->m_pGateHeight[GATE_B] < 0.0f)
            pChannel->m_pGateHeight[GATE_B] = 0.0f;
        if (pChannel->m_pGateHeight[GATE_B] > 1.0f)
            pChannel->m_pGateHeight[GATE_B] = 1.0f;

        strText.Format(_T("%.1f"), pChannel->m_pGateHeight[GATE_B] * 100.0f);
    } else if (!_tcsicmp(lpName, _T("EditScanGain"))) {
        int      ch       = g_MainProcess.m_Techniques.m_iChannel;
        Channel* pChannel = g_MainProcess.GetCurChannel();

        m_pSTD->mDetetionParam2995_200[ch].fScanGain += fValue;

        if (m_pSTD->mDetetionParam2995_200[ch].fScanGain < MIN_GAIN)
            m_pSTD->mDetetionParam2995_200[ch].fScanGain = MIN_GAIN;
        if (m_pSTD->mDetetionParam2995_200[ch].fScanGain > MAX_GAIN)
            m_pSTD->mDetetionParam2995_200[ch].fScanGain = MAX_GAIN;

        strText.Format(_T("%.1f"), m_pSTD->mDetetionParam2995_200[ch].fScanGain);
        pChannel->m_fGain = m_pSTD->mDetetionParam2995_200[ch].fScanGain;
        UpdateGain_ScanGain();
    } else if (!_tcsicmp(lpName, _T("EditScanCompensate"))) {
        Channel* pChannel = g_MainProcess.GetCurChannel();
        int      ch       = g_MainProcess.m_Techniques.m_iChannel;
        m_pSTD->mDetetionParam2995_200[ch].fScanCompensate += fValue;

        if (m_pSTD->mDetetionParam2995_200[ch].fScanCompensate < MIN_GAIN)
            m_pSTD->mDetetionParam2995_200[ch].fScanCompensate = MIN_GAIN;
        if (m_pSTD->mDetetionParam2995_200[ch].fScanCompensate > MAX_GAIN)
            m_pSTD->mDetetionParam2995_200[ch].fScanCompensate = MAX_GAIN;

        strText.Format(_T("%.1f"), m_pSTD->mDetetionParam2995_200[ch].fScanCompensate);
        //	pChannel->m_fGain = m_pSTD->mDetetionParam2995_200[ch].fScanGain + m_pSTD->mDetetionParam2995_200[ch].fScanCompensate;
        // UpdateGain_ScanGain();
    } else if (!_tcsicmp(lpName, _T("EditTrUTGain"))) {
        int ch = g_MainProcess.m_Techniques.m_iChannel;
        m_pSTD->mDetetionParam2995_200[ch].fScanTrUTGain += fValue;

        if (m_pSTD->mDetetionParam2995_200[ch].fScanTrUTGain < MIN_GAIN)
            m_pSTD->mDetetionParam2995_200[ch].fScanTrUTGain = MIN_GAIN;
        if (m_pSTD->mDetetionParam2995_200[ch].fScanTrUTGain > MAX_GAIN)
            m_pSTD->mDetetionParam2995_200[ch].fScanTrUTGain = MAX_GAIN;

        strText.Format(_T("%.1f"), m_pSTD->mDetetionParam2995_200[ch].fScanTrUTGain);
    } else if (!_tcsicmp(lpName, _T("EditTrUtcompensate"))) {
        int ch = g_MainProcess.m_Techniques.m_iChannel;
        m_pSTD->mDetetionParam2995_200[ch].fScanTrUTCompensate += fValue;

        if (m_pSTD->mDetetionParam2995_200[ch].fScanTrUTCompensate < MIN_GAIN)
            m_pSTD->mDetetionParam2995_200[ch].fScanTrUTCompensate = MIN_GAIN;
        if (m_pSTD->mDetetionParam2995_200[ch].fScanTrUTCompensate > MAX_GAIN)
            m_pSTD->mDetetionParam2995_200[ch].fScanTrUTCompensate = MAX_GAIN;

        strText.Format(_T("%.1f"), m_pSTD->mDetetionParam2995_200[ch].fScanTrUTCompensate);
    } else if (!_tcsicmp(lpName, _T("EditSideYPos"))) {
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideYPos += fValue;
        if (g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideYPos < MIN_Y_POS) {
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideYPos = MIN_Y_POS;
        }
        if (g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideYPos > MAX_Y_POS) {
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideYPos = MAX_Y_POS;
        }
        strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideYPos);
    } else if (!_tcsicmp(lpName, _T("EditSideXStart"))) {
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart += fValue;
        if (g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart < g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXEnd) {
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXEnd;
        }
        // if (fValue < 	g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXEnd || fValue > MAX_X_POS)
        if (g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart > MAX_Y_POS) {
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart = MAX_Y_POS;
        }
        strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart);
    } else if (!_tcsicmp(lpName, _T("EditSideXEnd"))) {
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXEnd += fValue;
        if (g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXEnd < MIN_X_POS) {
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXEnd = MIN_X_POS;
        }
        if (g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXEnd > g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart) {
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXEnd = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart;
        }
        strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXEnd);

    } else if (!_tcsicmp(lpName, _T("EditSideXStep"))) {
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep += fValue;

        strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep);
    } else if (!_tcsicmp(lpName, _T("EditTreadLyXPos"))) {
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadLyXPos += fValue;
        if (g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadLyXPos < MIN_X_POS) {
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadLyXPos = MIN_X_POS;
        }
        if (g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadLyXPos > MAX_X_POS) {
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadLyXPos = MAX_X_POS;
        }
        strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadLyXPos);
    } else if (!_tcsicmp(lpName, _T("EditTreadXPos"))) {
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadXPos += fValue;
        if (g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadXPos < MIN_X_POS) {
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadXPos = MIN_X_POS;
        }
        if (g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadXPos > MAX_X_POS) {
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadXPos = MAX_X_POS;
        }
        strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadXPos);
    } else if (!_tcsicmp(lpName, _T("EditTreadYStart"))) {
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart += fValue;
        if (g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart < g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYEnd) {
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYEnd;
        }
        if (g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart > MAX_Y_POS) {
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart = MAX_Y_POS;
        }
        strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart);
    } else if (!_tcsicmp(lpName, _T("EditTreadYEnd"))) {
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYEnd += fValue;
        if (g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYEnd < MIN_X_POS) {
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYEnd = MIN_X_POS;
        }
        if (g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYEnd > g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart) {
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYEnd = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart;
        }
        strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYEnd);
    } else if (!_tcsicmp(lpName, _T("EditTreadYStep"))) {
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep += fValue;

        strText.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep);
    }

    pEdit->SetText(strText);

    g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
    UpdateDAC();
}
void SetWnd::UpdateDAC() {
    Channel* pChannel = g_MainProcess.GetCurChannel();
    g_MainProcess.m_Techniques.GetDetectionStd()->CountDAC(&(g_MainProcess.m_Techniques), g_MainProcess.m_Techniques.m_iChannel,
                                                           g_MainProcess.m_Techniques.m_pChannel[g_MainProcess.m_Techniques.m_iChannel].m_fGain, pChannel->m_pGateData[0].fPos + pChannel->m_fDelay);
}
void SetWnd::OnBtnSave() {
    if (m_ProbleWheelWnd != NULL) {
        m_ProbleWheelWnd->ShowWindow(SW_HIDE);
    }
    if (m_pDACSetWnd != NULL) {
        m_pDACSetWnd->ShowWindow(SW_HIDE);
    }
    TecListWnd tecListWnd;
    tecListWnd.m_TechType = TECH_SAVE;
    tecListWnd.Create(m_hWnd, _T("TecListWnd"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
    tecListWnd.CenterWindow();
    tecListWnd.ShowModal();

    m_pLabelSelectTech->SetText(g_MainProcess.m_Techniques.m_pCurTechName);
}

void SetWnd::OnBtnLoad() {
    if (m_ProbleWheelWnd != NULL) {
        m_ProbleWheelWnd->ShowWindow(SW_HIDE);
    }

    if (m_pDACSetWnd != NULL) {
        m_pDACSetWnd->ShowWindow(SW_HIDE);
    }
    TecListWnd tecListWnd;
    tecListWnd.m_TechType = TECH_READ;
    tecListWnd.Create(m_hWnd, _T("TecListWnd"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
    tecListWnd.CenterWindow();
    tecListWnd.ShowModal();
    UpdateUI();
    g_MainProcess.m_HDBridge.OnConfig(&g_MainProcess.m_Techniques);
    m_pLabelSelectTech->SetText(g_MainProcess.m_Techniques.m_pCurTechName);
}

void SetWnd::OnSetLocalIp() {
    ControlIP controlIP;
    int       size = controlIP.Size();

    if (size == 1) {

    } else {
        DMessageBox(_T("存在多个网卡，暂不支持修改Ip"), _T("IP设置"));
    }
}

void SetWnd::OnSetBestStep() {
    float fPLCSideStep = 0, fPLCTreadStep = 0;
    int   nPLCSideSize = 0;
    CoutBestStepCir(fPLCSideStep, fPLCTreadStep, nPLCSideSize);
    int iDrawSideSize = (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimOuterDiameter -
                         g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimlInnerDiameter) /
                        2.0f / abs(g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep); // 侧面绘图圈数   根据 检测范围 /步进      轮辋外径（7）-轮辋内径（8）
    int iDrawTreadSize = (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRim -
                          g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelFlangeHeight) /
                         abs(g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep); // 踏面绘图圈数 根据 检测范围/步进        踏面轮辋厚度（2）-轮缘厚度（8）
    if (nPLCSideSize > iDrawSideSize) {
        DMessageBox(_T("PLC侧面检测移动距离过大请重新调整！"), _T("PLC设置异常"));
        return;
    }
    if (nPLCSideSize > iDrawTreadSize) {
        DMessageBox(_T("PLC踏面检测移动距离过大请重新调整！"), _T("PLC设置异常"));
        return;
    }
    CString strTXT;
    strTXT.Format(_T("扫差圈数：%d\n侧面步进：%.1f mm\n踏面步进：%.1f mm\n点击\"是\"应用步进值"), nPLCSideSize, fPLCSideStep, fPLCTreadStep);
    if (DMessageBox(strTXT, _T("扫差优化结果！"), MB_YESNO) == IDYES) {
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep   = fPLCSideStep;
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep  = fPLCTreadStep;
        g_MainProcess.m_ConnectPLC.m_PlcDownParam.nBestCirSize = nPLCSideSize;

        strTXT.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep);
        m_pEditSideXStep->SetText(strTXT);

        strTXT.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep);
        m_pEditTreadYStep->SetText(strTXT);
        strTXT.Format(_T("最佳圈数:%d圈"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.nBestCirSize);
        m_pLabelBestCir->SetText(strTXT);
    }
}

void SetWnd::OnReturn() {
    float fPLCSideStep = 0, fPLCTreadStep = 0;
    int   nPLCSideSize = 0;
    CoutBestStepCir(fPLCSideStep, fPLCTreadStep, nPLCSideSize);

    int iDrawSideSize = (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimOuterDiameter -
                         g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRimlInnerDiameter) /
                        2.0f / abs(g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep); // 侧面绘图圈数   根据 检测范围 /步进      轮辋外径（7）-轮辋内径（8）
    int iDrawTreadSize = (g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelRim -
                          g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fWheelFlangeHeight) /
                         abs(g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep); // 踏面绘图圈数 根据 检测范围/步进        踏面轮辋厚度（2）-轮缘厚度（8）
    if (nPLCSideSize > iDrawSideSize) {
        DMessageBox(_T("PLC侧面检测移动距离过大请重新调整！"), _T("PLC设置异常"));
        return;
    }
    if (nPLCSideSize > iDrawTreadSize) {
        DMessageBox(_T("PLC踏面检测移动距离过大请重新调整！"), _T("PLC设置异常"));
        return;
    }

    if (nPLCSideSize != g_MainProcess.m_ConnectPLC.m_PlcDownParam.nBestCirSize) {
        CString strTXT;
        strTXT.Format(_T("扫差圈数：%d\n侧面步进：%.1f mm\n踏面步进：%.1f mm\n点击\"是\"应用步进值"), nPLCSideSize, fPLCSideStep, fPLCTreadStep);
        if (DMessageBox(strTXT, _T("扫差优化结果！"), MB_YESNO) == IDYES) {
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep   = fPLCSideStep;
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep  = fPLCTreadStep;
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.nBestCirSize = nPLCSideSize;

            strTXT.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep);
            m_pEditSideXStep->SetText(strTXT);

            strTXT.Format(_T("%.1f"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep);
            m_pEditTreadYStep->SetText(strTXT);
            strTXT.Format(_T("最佳圈数:%d圈"), g_MainProcess.m_ConnectPLC.m_PlcDownParam.nBestCirSize);
            m_pLabelBestCir->SetText(strTXT);
        }
    } else {
        Close(IDCANCEL);
    }
}

void SetWnd::CoutBestStepCir(float& fPLCSideStep, float& fPLCTreadStep, int& nPLCSideSize) {
    fPLCSideStep        = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep;
    float fPLCSideMove  = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXEnd - g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart + 0.2f;
    float fPLCSideSize  = fPLCSideMove / fPLCSideStep; // 根据PLC设定计算圈数
    nPLCSideSize        = fPLCSideMove / fPLCSideStep;
    fPLCTreadStep       = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep;
    float fPLCTreadMove = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYEnd - g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart + 0.2f;
    float fPLCTreadSize = fPLCTreadMove / fPLCTreadStep; // 根据PLC设定计算圈数
    int   nPLCTreadSize = fPLCTreadMove / fPLCTreadStep;
    // 根据设定值优化圈数 和步进 根据最多圈数，反算步进
    if (fPLCSideSize > nPLCSideSize)
        nPLCSideSize += 1;
    if (fPLCTreadSize > nPLCTreadSize)
        nPLCTreadSize += 1;
    if (nPLCSideSize > nPLCTreadSize)
        nPLCTreadSize = nPLCSideSize;
    else
        nPLCSideSize = nPLCTreadSize;

    fPLCSideStep  = (fPLCSideMove) / nPLCSideSize;
    fPLCTreadStep = (fPLCTreadMove) / nPLCTreadSize; // 同步扫查圈数 重新计算步进
}

void SetWnd::OnBtnCopy() {
    bool    bCopyTo[HD_CHANNEL_NUM] = {0};
    CString strInfo;
    strInfo.Format(_T("点击'是',将通道%d参数复制给通道"), g_MainProcess.m_Techniques.m_iChannel + 1);
    bool bCopy = false;
    for (int i = 0; i < HD_CHANNEL_NUM; i++) {
        bCopyTo[i] = m_pOptCh[i]->IsSelected();
        if (bCopyTo[i]) {
            bCopy = true;
            CString strCH;
            strCH.Format(_T("%d,"), i + 1);
            strInfo.Append(strCH);
        }
    }

    strInfo.Append(_T("'否'取消复制！\n1,3,5为踏面同类型Ф20直探头,2,4,6为踏面Ф16弧面探头,7,9为侧面Ф20直探头,8,10为侧面双晶探头,\n不建议在不同类型的探头之间复制！"));
    if (bCopy) {
        if (DMessageBox(strInfo, _T("通道拷贝"), MB_YESNO) == IDYES) {
            for (int i = 0; i < HD_CHANNEL_NUM; i++) {
                if (bCopyTo[i]) {
                    g_MainProcess.m_Techniques.m_pChannel[i].CopyeChannel(&g_MainProcess.m_Techniques.m_pChannel[g_MainProcess.m_Techniques.m_iChannel]);

                    memcpy(&g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[i],
                           &g_MainProcess.m_Techniques.GetDetectionStd()->mDetetionParam2995_200[g_MainProcess.m_Techniques.m_iChannel], sizeof(DETECTION_PARAM2995_200));
                }
            }
        }
    }
    int tes = 0;
}