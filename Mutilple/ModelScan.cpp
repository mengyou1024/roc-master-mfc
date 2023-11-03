#include "pch.h"

#include "DetectionStd_TBT2995_200.h"
#include "MeshAscan.h"
#include "MeshBscan.h"
#include "MeshCscan.h"
#include "ModelScan.h"
#include "OpenGL.h"
#include "Techniques.h"

ModelScan::ModelScan(Techniques* pTechniques, OpenGL* pOpenGL) :
Model(pTechniques, pOpenGL),
m_iBuffer(0) {
}

ModelScan::~ModelScan() {
    Release();
}
void ModelScan::OnLButtonDown(UINT nFlags, ::CPoint pt) {
}
void ModelScan::Init() {
    Release();

    // 10个A扫
    for (size_t i = 0; i < m_pTechniques->m_pChannel.size(); i++) {
        size_t iView = static_cast<size_t>(VIEW_TYPE::VIEW_ASCAN_0) + i;
        if (m_pMesh.count(iView) == 0) {
            m_pMesh.insert(std::pair<size_t, Mesh*>(iView, new MeshAscan(m_pOpenGL)));
        }
    }

    if (m_pMesh.count(static_cast<size_t>(VIEW_TYPE::VIEW_BSCAN)) == 0) // 踏面
    {
        m_pMesh.insert(std::pair<size_t, Mesh*>(static_cast<size_t>(VIEW_TYPE::VIEW_BSCAN), new MeshBscan(m_pOpenGL)));
        MeshBscan* pBscan = static_cast<MeshBscan*>(m_pMesh[static_cast<size_t>(VIEW_TYPE::VIEW_BSCAN)]);
        // float C = m_pTechniques->m_Specimen.m_WheelParam.fWheelRimOuterDiameter * PI;
        float C = 360;
        //   *2倍（轮辋厚度-轮缘厚度） 缩小绘图宽度
        float Y = (m_pTechniques->m_Specimen.m_WheelParam.fWheelRim - m_pTechniques->m_Specimen.m_WheelParam.fWheelFlangeHeight) * 2; 
        pBscan->SetScan(m_pTechniques->m_Scan.m_iDrawTreadSize, m_pTechniques->m_Scan.GetTreadPoint(), 0, C, 0, Y);
    
    }

    if (m_pMesh.count(static_cast<size_t>(VIEW_TYPE::VIEW_CSCAN)) == 0) // 侧面
    {
        m_pMesh.insert(std::pair<size_t, Mesh*>(static_cast<size_t>(VIEW_TYPE::VIEW_CSCAN), new MeshCscan(m_pOpenGL)));
        MeshCscan* pCscan = static_cast<MeshCscan*>(m_pMesh[static_cast<size_t>(VIEW_TYPE::VIEW_CSCAN)]);
        pCscan->SetWheelParam(m_pTechniques->m_Specimen.m_WheelParam.fWheelHubInnerDiameter / 2.0f,
                              m_pTechniques->m_Specimen.m_WheelParam.fWheelHubOuterDiameter / 2.0f,
                              m_pTechniques->m_Specimen.m_WheelParam.fWheelRimlInnerDiameter / 2.0f,
                              m_pTechniques->m_Specimen.m_WheelParam.fWheelRimOuterDiameter / 2.0f);
        pCscan->SetScan(
            m_pTechniques->m_Specimen.m_WheelParam.fWheelRimOuterDiameter / 2.0f, 
            m_pTechniques->m_Scan.m_pSideRadius.data(), 
            m_pTechniques->m_Scan.m_iDrawSideSize,
            m_pTechniques->m_Scan.GetSidePoint()
        );
    }

    m_bSetup = false;
}

void ModelScan::SetSize(int left, int top, int right, int bottom) {
    int iAscanWidth  = ((right - left) * 2 / 3 - 3) / 2; //
    int iAscanHeight = ((bottom - top) - 6) / 6;

    RECT rc{0};
    // 计算A扫显示区域
    for (int i = 0; i < VIEW_ASCAN_NUM; i++) {
        rc.left  = (i % 2) * (iAscanWidth + 1);
        rc.right = rc.left + iAscanWidth;
        rc.top   = (i / 2) * (iAscanHeight + 1);
        if (i / VIEW_ASCAN_COLUMNS == (VIEW_ASCAN_NUM / VIEW_ASCAN_COLUMNS - 1))
            rc.bottom = bottom;
        else
            rc.bottom = rc.top + iAscanHeight;

        // OpenGL视图坐标0点在左下角
        size_t iView = static_cast<size_t>(VIEW_TYPE::VIEW_ASCAN_0) +
                       (static_cast<size_t>((VIEW_ASCAN_NUM / 2 - 1)) - i / 2) * 2 + (i % 2);
        if (m_pMesh.count(iView) != 0) {
            m_pMesh[iView]->SetSize(rc.left, rc.top, rc.right, rc.bottom);
        }
    }

    if (m_pMesh.count(static_cast<size_t>(VIEW_TYPE::VIEW_BSCAN)) != 0) {
        rc.left   = 2 * (iAscanWidth + 1);
        rc.right  = right;
        rc.top    = top;
        rc.bottom = top + 2 * iAscanHeight + 1;
        m_pMesh[static_cast<size_t>(VIEW_TYPE::VIEW_BSCAN)]->SetSize(rc.left, rc.top, rc.right, rc.bottom);
    }

    if (m_pMesh.count(static_cast<size_t>(VIEW_TYPE::VIEW_CSCAN)) != 0) {
        rc.left   = 2 * (iAscanWidth + 1);
        rc.right  = right;
        rc.top    = top + 2 * iAscanHeight + 2;
        rc.bottom = bottom;
        m_pMesh[static_cast<size_t>(VIEW_TYPE::VIEW_CSCAN)]->SetSize(rc.left, rc.top, rc.right, rc.bottom);
    }

    m_bSetup = false;
}

void ModelScan::Setup() {
    for (auto it = m_pMesh.begin(); it != m_pMesh.end(); ++it) {
        it->second->Setup();
    }
    m_bSetup = true;
}

void ModelScan::Release() {
    for (auto& ptr : m_pMesh) {
        delete ptr.second;
        ptr.second = nullptr;
    }
    m_pMesh.clear();
}

void ModelScan::UpdateData() {
    // Bscan
    if (m_pMesh.count(static_cast<size_t>(VIEW_TYPE::VIEW_BSCAN)) != 0) {
        MeshBscan* pBscan = static_cast<MeshBscan*>(m_pMesh[static_cast<size_t>(VIEW_TYPE::VIEW_BSCAN)]);

        if (m_pTechniques->m_iTreadCirIndex >= 0 && m_pTechniques->m_iTreadCirIndex < m_pTechniques->m_Scan.m_iSideSize) {
            UCHAR* pData   = m_pTechniques->m_Scan.m_pBscanData.data();
            UCHAR* pDataCH = m_pTechniques->m_Scan.m_pBscanCH.data();
            pBscan->UpdateData(pData, pDataCH, m_pTechniques->m_iTreadCirIndex, m_pTechniques->m_iTreadPointIndex);
        }
    }

    // Cscan
    if (m_pMesh.count(static_cast<size_t>(VIEW_TYPE::VIEW_CSCAN)) != 0) {
        MeshCscan* pCscan = static_cast<MeshCscan*>(m_pMesh[static_cast<size_t>(VIEW_TYPE::VIEW_CSCAN)]);

        if (m_pTechniques->m_iSideCirIndex >= 0 && m_pTechniques->m_iSideCirIndex < m_pTechniques->m_Scan.m_iSideSize) {
            UCHAR* pData   = m_pTechniques->m_Scan.m_pCscanData.data();
            UCHAR* pDataCH = m_pTechniques->m_Scan.m_pCscanCH.data();
            pCscan->UpdateData(pData, pDataCH, m_pTechniques->m_iSideCirIndex, m_pTechniques->m_iSidePointIndex);
        }
    }

    bool bData = false;
    for (int i = 0; i < 2; i++) {
        m_iBuffer = (m_iBuffer + 1) % 2;
        if (WaitForSingleObject(m_pTechniques->m_pDraw[m_iBuffer].hReady, 0) != WAIT_TIMEOUT) {
            bData = true;
            break;
        }
    }
    if (m_pTechniques->m_ScanType != SCAN_REPLAY) {
        if (!bData)
            return;
    }
    P_DRAW_BUFFER pDraw = &m_pTechniques->m_pDraw[m_iBuffer];

    for (size_t iChannel = 0; iChannel < m_pTechniques->m_pChannel.size(); iChannel++) {
        Channel* pChannel = &m_pTechniques->m_pChannel[iChannel];
        if (pChannel) {
            pChannel->m_pGateData[GATE_A].fAmp = (float)pDraw->pGateAmp[iChannel][GATE_A] / MAX_AMP;
            pChannel->m_pGateData[GATE_B].fAmp = (float)pDraw->pGateAmp[iChannel][GATE_B] / MAX_AMP;
            pChannel->m_pGateData[GATE_A].fPos = (float)pDraw->pGatePos[iChannel][GATE_A] / 100.0f * (float)pChannel->m_iVelocity / 2000.0f;
            pChannel->m_pGateData[GATE_B].fPos = (float)pDraw->pGatePos[iChannel][GATE_B] / 100.0f * (float)pChannel->m_iVelocity / 2000.0f;

            size_t iView = static_cast<size_t>(VIEW_TYPE::VIEW_ASCAN_0) + iChannel;
            if (m_pMesh.count(iView) != 0) {
                MeshAscan* pAscan = static_cast<MeshAscan*>(m_pMesh[iView]);
                pAscan->SetLimits(pChannel->m_fDelay, pChannel->m_fDelay + pChannel->m_fRange);
                pAscan->UpdateData(m_pTechniques->m_pDraw[m_iBuffer].pData[iChannel].data(), m_pTechniques->m_pDraw[m_iBuffer].pData[iChannel].size());

                for (int iGate = 0; iGate < MAX_GATE_NUM; iGate++) {
                    if (iGate == 1) {
                        float fGateAamp = pChannel->m_pGateData[GATE_A].fPos / pChannel->m_fRange;
                        pAscan->UpdateGate(iGate, true, pChannel->m_pGatePos[iGate] + fGateAamp, pChannel->m_pGateWidth[iGate], pChannel->m_pGateHeight[iGate]);
                    } else {
                        pAscan->UpdateGate(iGate, true, pChannel->m_pGatePos[iGate], pChannel->m_pGateWidth[iGate], pChannel->m_pGateHeight[iGate]);
                    }
                }
                // 母线 正常不显示
                // pAscan->UpdateDACData(m_pTechniques->GetDetectionStd()->m_DAC_MiChannel].data(), m_pTechniques->GetDetectionStd()->m_DAC_M[iChannel].size());

                // 判废线
                pAscan->UpdateDac(0, m_pTechniques->GetDetectionStd()->m_DAC_RL[iChannel].data(), m_pTechniques->GetDetectionStd()->m_DAC_RL[iChannel].size());

                // 评定线
                pAscan->UpdateDac(1, m_pTechniques->GetDetectionStd()->m_DAC_ED[iChannel].data(), m_pTechniques->GetDetectionStd()->m_DAC_ED[iChannel].size());
            }
        }
    }

    ResetEvent(m_pTechniques->m_pDraw[m_iBuffer].hReady);
}

void ModelScan::RenderBK() {
    for (auto& ptr : m_pMesh) {
        ptr.second->RenderBK();
    }
}

void ModelScan::Render() {
    UpdateData();

    for (auto& ptr : m_pMesh) {
        ptr.second->Render();
    }
}

void ModelScan::RenderFore() {
    for (auto& ptr : m_pMesh) {
        ptr.second->RenderFore();
    }

#ifdef _DEBUG
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    CString strInfo;
    for (size_t iChannel = 0; iChannel < m_pTechniques->m_pChannel.size(); iChannel++) {
        size_t iView = static_cast<size_t>(VIEW_TYPE::VIEW_ASCAN_0) + iChannel;
        if (m_pMesh.count(iView) != 0) {
            strInfo.Format(_T("%lld"), iChannel + 1);
            float l = iChannel == 9 ? m_pMesh[iView]->m_rcItem.left : m_pMesh[iView]->m_rcItem.left + 5.0f;
            float t = m_pMesh[iView]->m_rcItem.top + 3.0f;
            m_pOpenGL->m_Font.Text(l, t, strInfo, {0.2f, 0.6f, 0.9f}, 1.0f);
        }
    }
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
#endif
}