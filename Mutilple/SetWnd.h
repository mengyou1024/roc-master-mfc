#pragma once

#include "OpenGL.h"

class DetectionStd_TBT2995_200;
class PLCWnd;
class ProbleWheelWnd;
class DACSetWnd;

class SetWnd : public CDuiWindowBase {
public:
    ~SetWnd();

    virtual LPCTSTR GetWindowClassName() const {
        return _T("SetWnd");
    }
    virtual CDuiString GetSkinFile() {
        return _T("Theme\\UI_SetWnd.xml");
    }

    void InitLabel();
    void InitEdit();
    void InitCombo();
    void InitOpenGL();
    void InitWindow();
    void Notify(TNotifyUI& msg);

    bool OnEdit(void* pv);

    void UpdateUI();
    void UpdateChannelUI(Channel* pChannel);

    void OnTimer(int iIdEvent);

    void OnBtnSpin(LPCTSTR lpName, float fValue);

    void OnBtnSave();
    void OnBtnLoad();
    void OnSetLocalIp();
    //  void OnSetFPGAIp();
    void OnSetBestStep();
    void OnReturn();
    void CoutBestStepCir(float& step1, float& step2, int& cir);
    void OnBtnCopy();
    void UpdateDAC();
    void UpdateGain_ScanGain();

public:
    CWindowUI* m_pWndOpenGL;
    CLabelUI * m_pLabelStatus, *m_pLabelSelectTech, *m_pLabelBestCir;

    // 系统参数
    CEditUI * m_pEditIP_FPGA, *m_pEditPORT_FPGA, *m_pEditIP_PC, *m_pEditPORT_PC;
    CEditUI * m_pEditFrequency, *m_pEditPulseWidth;
    CComboUI *m_pComboPulseWidth, *m_pComboVoltage;

    // 通道参数
    CComboUI *m_pComboChannel, *m_pComboFilter, *m_pComboDemodu, *m_pComboChFlag;
    CEditUI * m_pEditRange, *m_pEditGain, *m_pEditVelocity, *m_pEditDelay, *m_pEditOffset;

    // 测量参数
    CLabelUI* m_pLabelChannel[MESSURE_NUM];
    CLabelUI* m_pLabelGatePos[MAX_GATE_NUM];
    CLabelUI* m_pLabelGateAmp[MAX_GATE_NUM];
    CLabelUI *m_pLabelGatePosDistance, *m_pLabelGateBDepth;

    // 波门参数
    CEditUI *m_pEditGatePos[MAX_GATE_NUM], *m_pEditGateWidth[MAX_GATE_NUM], *m_pEditGateHeight[MAX_GATE_NUM];

    // 探伤参数
    CEditUI *m_pEditScanGain, *m_pEditScanCompensate, *m_pEditTrUTGain, *m_pEditTrUtcompensate;

    // 车轮参数
    CEditUI *m_pEditWheelName, *m_pEditEditWheelModel, *m_pEditDetectionStd;

    // PLC 控制参数
    CEditUI *m_pEditSideYPos, *m_pEditSideXStart, *m_pEditSideXEnd, *m_pEditSideXStep,
        *m_pEditTreadLyXPos, *m_pEditTreadXPos, *m_pEditTreadYStart, *m_pEditTreadYEnd, *m_pEditTreadYStep;

    CEditUI *m_pEditSideXSpeed1, *m_pEditSideXSpeed2, *m_pEditSideYSpeed1, *m_pEditSideYSpeed2,
        *m_pEditTreadXSpeed1, *m_pEditTreadXSpeed2, *m_pEditTreadYSpeed1, *m_pEditTreadYSpeed2,
        *m_pEditRotateSpeed1;
    CHorizontalLayoutUI *m_pLayTrUTGain, *m_pLayTrUTtcompensate;
    // 通道拷贝
    COptionUI* m_pOptCh[HD_CHANNEL_NUM];

public:
    OpenGL          m_OpenGL;
    PLCWnd*         m_PLCWnd;
    ProbleWheelWnd* m_ProbleWheelWnd;
    DACSetWnd*      m_pDACSetWnd;

    DetectionStd_TBT2995_200* m_pSTD; // 工艺里面的 标准指针 便于调用读写参数
};
