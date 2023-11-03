#include "pch.h"

#include "DetectionStd_TBT2995_200.h"
#include "MeshAscan.h"
#include "ModelSet.h"
#include "OpenGL.h"
#include "Techniques.h"

ModelSet::ModelSet(Techniques* pTechniques, OpenGL* pOpenGL) :
Model(pTechniques, pOpenGL),
m_pMesh(nullptr),
m_iBuffer(0) {
}

ModelSet::~ModelSet() {
    Release();
}

void ModelSet::Init() {
    Release();

    if (m_pMesh == nullptr) {
        m_pMesh = new MeshAscan(m_pOpenGL);
    }
    m_bSetup = false;
}

void ModelSet::SetSize(int left, int top, int right, int bottom) {
    if (m_pMesh) {
        m_pMesh->SetSize(left, top, right, bottom - 30);
    }
}

void ModelSet::Setup() {
    if (m_pMesh) {
        m_pMesh->Setup();
    }

    m_bSetup = true;
}

void ModelSet::Release() {
    if (m_pMesh != nullptr) {
        delete m_pMesh;
        m_pMesh = nullptr;
    }
}

void ModelSet::UpdateData() {
    bool bData = false;
    for (int i = 0; i < 2; i++) {
        m_iBuffer = (m_iBuffer + 1) % 2;
        if (WaitForSingleObject(m_pTechniques->m_pDraw[m_iBuffer].hReady, 0) != WAIT_TIMEOUT) {
            bData = true;
            break;
        }
    }

    if (!bData)
        return;

    P_DRAW_BUFFER pDraw    = &m_pTechniques->m_pDraw[m_iBuffer];
    Channel*      pChannel = &m_pTechniques->m_pChannel[m_pTechniques->m_iChannel];
    if (pChannel) {
        // 编码器值
        pChannel->m_pCoder[0] = pDraw->pCoder[m_pTechniques->m_iChannel][0];
        pChannel->m_pCoder[1] = pDraw->pCoder[m_pTechniques->m_iChannel][1];

        pChannel->m_pGateData[GATE_A].fAmp = (float)pDraw->pGateAmp[m_pTechniques->m_iChannel][GATE_A] / MAX_AMP;
        pChannel->m_pGateData[GATE_B].fAmp = (float)pDraw->pGateAmp[m_pTechniques->m_iChannel][GATE_B] / MAX_AMP;
        pChannel->m_pGateData[GATE_A].fPos = (float)pDraw->pGatePos[m_pTechniques->m_iChannel][GATE_A] / 100.0f * (float)pChannel->m_iVelocity / 2000.0f;
        pChannel->m_pGateData[GATE_B].fPos = (float)pDraw->pGatePos[m_pTechniques->m_iChannel][GATE_B] / 100.0f * (float)pChannel->m_iVelocity / 2000.0f;

        if (m_pMesh) {
            m_pMesh->SetLimits(pChannel->m_fDelay, pChannel->m_fDelay + pChannel->m_fRange);
            m_pMesh->UpdateData(m_pTechniques->m_pDraw[m_iBuffer].pData[m_pTechniques->m_iChannel].data(), m_pTechniques->m_pDraw[m_iBuffer].pData[m_pTechniques->m_iChannel].size());

            for (int iGate = 0; iGate < MAX_GATE_NUM; iGate++) {
                if (iGate == 1) {
                    float fGateAamp = pChannel->m_pGateData[GATE_A].fPos / pChannel->m_fRange;
                    m_pMesh->UpdateGate(iGate, true, pChannel->m_pGatePos[iGate] + fGateAamp, pChannel->m_pGateWidth[iGate], pChannel->m_pGateHeight[iGate]);
                } else {
                    m_pMesh->UpdateGate(iGate, true, pChannel->m_pGatePos[iGate], pChannel->m_pGateWidth[iGate], pChannel->m_pGateHeight[iGate]);
                }
            }
            // 母线 正常不显示
            // m_pMesh->UpdateDACData(m_pTechniques->GetDetectionStd()->m_DAC_MiChannel].data(), m_pTechniques->GetDetectionStd()->m_DAC_M[iChannel].size());

            // 判废线
            m_pMesh->UpdateDac(0, m_pTechniques->GetDetectionStd()->m_DAC_RL[m_pTechniques->m_iChannel].data(), m_pTechniques->GetDetectionStd()->m_DAC_RL[m_pTechniques->m_iChannel].size());

            // 评定线
            m_pMesh->UpdateDac(1, m_pTechniques->GetDetectionStd()->m_DAC_ED[m_pTechniques->m_iChannel].data(), m_pTechniques->GetDetectionStd()->m_DAC_ED[m_pTechniques->m_iChannel].size());
        }
    }

    ResetEvent(m_pTechniques->m_pDraw[m_iBuffer].hReady);
}

void ModelSet::RenderBK() {
    if (m_pMesh) {
        m_pMesh->RenderBK();
    }
}

void ModelSet::Render() {
    UpdateData();

    if (m_pMesh) {
        m_pMesh->Render();
    }
}

void ModelSet::RenderFore() {
    if (m_pMesh) {
        m_pMesh->RenderFore();
    }

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    CString  strInfo;
    Channel* pChannel = &m_pTechniques->m_pChannel[m_pTechniques->m_iChannel];
    strInfo.Format(_T("Coder1: %d, Coder2: %d"), pChannel->m_pCoder[0], pChannel->m_pCoder[0]);
    m_pOpenGL->m_Font.Text(1.0f, (float)m_pOpenGL->m_iHeight - 24, strInfo, {0.10f, 0.20f, 0.45f}, 0.75F);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}