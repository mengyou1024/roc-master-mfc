#include "pch.h"

#include "DetectionStd_TBT2995_200.h"
#include "HDBridge.h"
#include "Mutilple.h"
#include "Techniques.h"

using namespace TOFDPort;
using namespace TOFDPortExtensions;

HDBridge::HDBridge(void) :
m_fFPS(0),
m_fPacketLoss(0),
m_iNetwork(-1),
m_pTechniques(nullptr),
m_pICoder(nullptr) {
    InitializeCriticalSection(&m_csData);
}

HDBridge::~HDBridge(void) {
    Close();

    DeleteCriticalSection(&m_csData);
}

BOOL HDBridge::Open(char *pFPGA_IP, unsigned short iPortFPGA, char *pPC_IP, unsigned short iPortPC) {
    bool res = TOFD_PORT_OpenDevice();
    return TRUE;
}

BOOL HDBridge::IsOpened() {
    return TOFD_PORT_IsOpen();
}

void HDBridge::SetCoder(ICoderAngle *pCoder) {
    m_pICoder = pCoder;
}

BOOL HDBridge::OnConfig(Techniques *pTechniques) {
    if (pTechniques == NULL)
        return FALSE;

    m_pTechniques = pTechniques;
    TOFD_PORT_SetFrequency(pTechniques->m_System.m_iFrequency);
    TOFD_PORT_SetVoltage(pTechniques->m_System.m_iVoltage);
    TOFD_PORT_SetChannelFlag(pTechniques->m_System.m_iChMode); // 9/10  11/12 为双晶配置 0x05ff0aff
    //	TOFD_PORT_SetChannelFlag(0xfffffffff);//9/10  11/12 为双晶配置
    TOFD_PORT_SetLED(0);
    TOFD_PORT_SetScanIncrement(pTechniques->m_System.m_iScanIncrement);
    TOFD_PORT_SetDamperFlag(0x00000fff);
    TOFD_PORT_SetEncoderPulse(1);
    // UNION_PORT_SetWorkType(m_iNetwork, pTechniques->m_System.m_iWorkType);

    for (int iChannel = 0; iChannel < HD_CHANNEL_NUM_12; iChannel++) {
        // 12通道转换上位机10通道转换
        int temiChannel = iChannel;
        if (pTechniques->m_System.m_iChMode == 0x05ff0aff) {
            if (iChannel == 9) {
                temiChannel = 8;
            }
            if (iChannel == 10 || iChannel == 11) {
                temiChannel = 9;
            }

        } else {
            if (iChannel == 10 || iChannel == 11) {
                temiChannel = 9;
            }
        }
        int temDowniChannel = iChannel;

        TOFD_PORT_SetPulseWidth(temDowniChannel, static_cast<float>(pTechniques->m_System.m_iPulseWidth));
        float fDelay = pTechniques->m_pChannel[temiChannel].m_fDelay * 2000.0f / (float)pTechniques->m_pChannel[temiChannel].m_iVelocity;
        fDelay += pTechniques->m_pChannel[temiChannel].m_fOffset;
        TOFD_PORT_SetDelay(temDowniChannel, fDelay);
        FLOAT fSampleDepth =
            pTechniques->m_pChannel[temiChannel].m_fRange * 2000.0f / (float)pTechniques->m_pChannel[temiChannel].m_iVelocity;
        TOFD_PORT_SetSampleDepth(temDowniChannel, fSampleDepth);
        int iAScanSize    = int(fSampleDepth * 100.0F + 0.5F);
        int iSampleFactor = iAScanSize / 512;
        if (iSampleFactor < 1)
            iSampleFactor = 1;
        if (iSampleFactor > 255)
            iSampleFactor = 255;
        pTechniques->m_pChannel[temiChannel].m_iSampleFactor = iSampleFactor;
        TOFD_PORT_SetSampleFactor(temDowniChannel, pTechniques->m_pChannel[temiChannel].m_iSampleFactor);

        // 参数设置时使用 正常增益
        if (g_MainProcess.m_enumMainProType == ROCESS_PARAMSET) {
            TOFD_PORT_SetGain(temDowniChannel, pTechniques->m_pChannel[temiChannel].m_fGain);
        }
        ////扫查时 使用 设定的 探伤灵敏度 +补偿
        else if (g_MainProcess.m_enumMainProType == ROCESS_SCAN) {
            float fScanGain = pTechniques->GetDetectionStd()->mDetetionParam2995_200[temiChannel].fScanGain +
                              pTechniques->GetDetectionStd()->mDetetionParam2995_200[temiChannel].fScanCompensate;
            TOFD_PORT_SetGain(temDowniChannel, fScanGain);
        }
        // TOFD_PORT_SetGain(iChannel, 50);
        TOFD_PORT_SetFilter(temDowniChannel, pTechniques->m_pChannel[temiChannel].m_iFilter);
        TOFD_PORT_SetDemodu(temDowniChannel, pTechniques->m_pChannel[temiChannel].m_iDemodu);
        TOFD_PORT_SetPhaseReverse(temDowniChannel, pTechniques->m_pChannel[temiChannel].m_iPhaseReverse);

        TOFD_PORT_SetGateInfo(temDowniChannel, GATE_A, TRUE, pTechniques->m_pChannel[temiChannel].m_pGateAlarmType[GATE_A],
                              pTechniques->m_pChannel[temiChannel].m_pGatePos[GATE_A],
                              pTechniques->m_pChannel[temiChannel].m_pGateWidth[GATE_A],
                              pTechniques->m_pChannel[temiChannel].m_pGateHeight[GATE_A]);

        // if (pTechniques->m_pChannel[temiChannel].m_iGateBType==0) {
        pTechniques->m_pChannel[temiChannel].m_iGateBType = 1; // 设置波门2跟随
        // }
        TOFD_PORT_SetGate2Type(temDowniChannel, pTechniques->m_pChannel[temiChannel].m_iGateBType);

        TOFD_PORT_SetGateInfo(temDowniChannel, GATE_B, TRUE, pTechniques->m_pChannel[temiChannel].m_pGateAlarmType[GATE_B],
                              pTechniques->m_pChannel[temiChannel].m_pGatePos[GATE_B],
                              pTechniques->m_pChannel[temiChannel].m_pGateWidth[GATE_B],
                              pTechniques->m_pChannel[temiChannel].m_pGateHeight[GATE_B]);
    }
    // TOFD_PORT_SetGain(0, 10);
    int res = TOFD_PORT_FlushSetting();
    return res;
}

BOOL HDBridge::StartSequencer(BOOL bStart) {
    if (!IsOpened())
        return false;

    BOOL bRet = FALSE;

    if (bStart) {
        m_ThreadRead.Create(&HDBridge::_Read, this);
        //	bRet = TOFD_PORT_OpenDevice();
        //		bRet = TOFD_PORT_Device();
    } else {
        m_ThreadRead.Close();
        //	bRet = TOFD_PORT_CloseDevice();
    }

    return bRet;
}

void HDBridge::ResetCoder() {
    TOFD_PORT_ResetCoder_Immediate();
    // UNION_PORT_ResetCoder(m_iNetwork, 1);
    //	UNION_PORT_FlushSetting(m_iNetwork);
}

BOOL HDBridge::ReadAscan() {
    EnterCriticalSection(&m_csData);

    // 获取编码器角度
    float fAngle = 0;
    if (m_pICoder != nullptr) {
        fAngle = m_pICoder->GetCoder();
    }
    m_ReadData.fAngle = fAngle;

    for (int i = 0; i < HD_CHANNEL_NUM_12; i++) {
        NM_DATA *pReadData = TOFD_PORT_ReadDatasFormat();
        if (pReadData != NULL) {
            if (pReadData->iChannel >= 0 && pReadData->iChannel < HD_CHANNEL_NUM_12) // 9 和10   11和 12为双晶 合并一个通道
            {
                int temChannel = pReadData->iChannel;
                // TODO: 待定
                // if (g_MainProcess.m_Techniques.m_System.m_iChMode == 0x05ff0aff) { //
                //    if (pReadData->iChannel == 9 || pReadData->iChannel == 11)
                //        continue; // 双晶以发的通道为准
                //    //	if (pReadData->iChannel == 8 || pReadData->iChannel == 9)temChannel = 8;
                //    if (pReadData->iChannel == 10)
                //        temChannel = 9; //
                //} else {
                //    if (pReadData->iChannel > 9) {
                //        continue;
                //    }
                //}
                m_ReadData.pAscan[temChannel].resize(pReadData->iAScanSize);

                //	m_ReadData.pAxle[pReadData->iChannel] = pReadData->iAxle;
                m_ReadData.pChannel[temChannel] = temChannel;
                m_ReadData.pPackage[temChannel] = pReadData->iPackage;

                memcpy(m_ReadData.pAscan[temChannel].data(), pReadData->pAscan, pReadData->iAScanSize);
                FLOAT fSampleDepth = g_MainProcess.m_Techniques.m_pChannel[temChannel].m_fRange * 2000.0f /
                                     (float)g_MainProcess.m_Techniques.m_pChannel[temChannel].m_iVelocity;

                if (temChannel == 0) {
                    int test = 0;
                }
                if (pReadData->pGatePos[0] == 0) { // pReadData->pGatePos[0]异常为0时 重新寻找A门内最高位置
                    int GateApos = static_cast<int>(g_MainProcess.m_Techniques.m_pChannel[temChannel].m_pGatePos[GATE_A] * pReadData->iAScanSize);
                    int GateAEnd = static_cast<int>((g_MainProcess.m_Techniques.m_pChannel[temChannel].m_pGatePos[GATE_A] +
                                    g_MainProcess.m_Techniques.m_pChannel[temChannel].m_pGateWidth[GATE_A]) *
                                   pReadData->iAScanSize);
                    int nMaxAmp   = 0;
                    int nMaxIndex = GateApos;
                    for (int i = GateApos; i < GateAEnd; i++) {
                        if (m_ReadData.pAscan[temChannel][i] > nMaxAmp) {
                            nMaxAmp   = m_ReadData.pAscan[temChannel][i];
                            nMaxIndex = i;
                        }
                    }
                    pReadData->pGatePos[0] = static_cast<int>(100 * fSampleDepth * nMaxIndex / pReadData->iAScanSize);
                    pReadData->pGateAmp[0] = nMaxAmp;
                }

                // b门跟随重新计算b门最高波

                int GateBPos = static_cast<int>((pReadData->pGatePos[0] / 100.0f / fSampleDepth +
                                g_MainProcess.m_Techniques.m_pChannel[temChannel].m_pGatePos[GATE_B]) *
                               pReadData->iAScanSize);
                int GateBEnd = static_cast<int>((pReadData->pGatePos[0] / 100.0f / fSampleDepth +
                                                 g_MainProcess.m_Techniques.m_pChannel[temChannel].m_pGatePos[GATE_B] +
                                                 g_MainProcess.m_Techniques.m_pChannel[temChannel].m_pGateWidth[GATE_B]) *
                                                pReadData->iAScanSize);
                if (GateBPos < pReadData->iAScanSize) {
                    if (GateBEnd > pReadData->iAScanSize)
                        GateBEnd = pReadData->iAScanSize;
                    int nMaxBmp    = m_ReadData.pAscan[temChannel][GateBPos];
                    int nMaxBIndex = GateBPos;

                    for (int i = GateBPos; i < GateBEnd; i++) {
                        if (m_ReadData.pAscan[temChannel][i] > nMaxBmp) {
                            nMaxBmp    = m_ReadData.pAscan[temChannel][i];
                            nMaxBIndex = i;
                        }
                    }
                    pReadData->pGatePos[1] = static_cast<int>(100 * fSampleDepth * nMaxBIndex / pReadData->iAScanSize);
                    pReadData->pGateAmp[1] = m_ReadData.pAscan[temChannel][nMaxBIndex];
                }
                m_ReadData.pCoder[temChannel][0]   = pReadData->pCoder[0];
                m_ReadData.pCoder[temChannel][1]   = pReadData->pCoder[1];
                m_ReadData.pGatePos[temChannel][0] = pReadData->pGatePos[0];
                m_ReadData.pGatePos[temChannel][1] = pReadData->pGatePos[1];
                m_ReadData.pGateAmp[temChannel][0] = pReadData->pGateAmp[0];
                m_ReadData.pGateAmp[temChannel][1] = pReadData->pGateAmp[1];
                m_ReadData.pAlarm[temChannel][0]   = pReadData->pAlarm[0];
                m_ReadData.pAlarm[temChannel][1]   = pReadData->pAlarm[1];

                 //m_fPacketLoss = UNION_PORT_GetPacketLoss(m_iNetwork, pReadData->iChannel);
            }
            TOFD_PORT_Free_NM_DATA(pReadData);
            m_fFPS = FPS();
        }
    }

    LeaveCriticalSection(&m_csData);

    return TRUE;
}

void HDBridge::Close() {
    m_ThreadRead.Close();
    TOFD_PORT_CloseDevice();
    //	UNION_PORT_StopSequencer(m_iNetwork);
    // UNION_PORT_Close(m_iNetwork);
}

float HDBridge::FPS() {
    static float fps = 0;

    static int   iFrame      = 0;
    static float currentTime = 0.0f;
    static float lastTime    = 0.0f;
    currentTime              = GetTickCount64() * 0.001f;
    iFrame++;

    if (currentTime - lastTime > 1.0f) {
        fps      = (float)iFrame / (currentTime - lastTime);
        lastTime = currentTime;
        iFrame   = 0;
    }

    return fps;
}

void HDBridge::_Read() {
    INT iBuffer = 0;
    while (m_ThreadRead.m_bWorking) {
        if (g_MainProcess.m_Techniques.m_ScanType == SCAN_REPLAY) {
            continue;
        }
        if (m_pTechniques != nullptr) {
            if (ReadAscan()) {
                m_pTechniques->Dealwith(m_ReadData); //

                bool bNoData = false;
                for (int i = 0; i < 2; i++) {
                    iBuffer = (iBuffer + 1) % 2;
                    if (WaitForSingleObject(m_pTechniques->m_pDraw[iBuffer].hReady, 0) == WAIT_TIMEOUT) {
                        bNoData = true;
                        break;
                    }
                }

                if (bNoData) {
                    for (int iChannel = 0; iChannel < HD_CHANNEL_NUM; iChannel++) {
                        size_t iSize = m_ReadData.pAscan[iChannel].size();
                        if (iSize != 0) {
                            m_pTechniques->m_pDraw[iBuffer].pData[iChannel].resize(iSize);
                            for (size_t i = 0; i < iSize; i++) {
                                m_pTechniques->m_pDraw[iBuffer].pData[iChannel][i] = float(m_ReadData.pAscan[iChannel][i]) / MAX_AMP;
                            }

                            // 编码器值
                            m_pTechniques->m_pDraw[iBuffer].pCoder[iChannel][0] = m_ReadData.pCoder[iChannel][0];
                            m_pTechniques->m_pDraw[iBuffer].pCoder[iChannel][1] = m_ReadData.pCoder[iChannel][1];
                            // 波门位置
                            m_pTechniques->m_pDraw[iBuffer].pGatePos[iChannel][0] = m_ReadData.pGatePos[iChannel][0];
                            m_pTechniques->m_pDraw[iBuffer].pGatePos[iChannel][1] = m_ReadData.pGatePos[iChannel][1];
                            // 波门波幅
                            m_pTechniques->m_pDraw[iBuffer].pGateAmp[iChannel][0] = m_ReadData.pGateAmp[iChannel][0];
                            m_pTechniques->m_pDraw[iBuffer].pGateAmp[iChannel][1] = m_ReadData.pGateAmp[iChannel][1];
                        }
                    }
                    SetEvent(m_pTechniques->m_pDraw[iBuffer].hReady);
                }
            } else {
                Sleep(10);
            }
        } else {
            Sleep(100);
        }
    }

    if (m_pTechniques != nullptr) {
        ResetEvent(m_pTechniques->m_pDraw[0].hReady);
        ResetEvent(m_pTechniques->m_pDraw[1].hReady);
    }
}
