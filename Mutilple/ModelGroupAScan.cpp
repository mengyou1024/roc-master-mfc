#include "pch.h"

#include "DetectionStd_TBT2995_200.h"
#include "MeshAscan.h"
#include "MeshBscan.h"
#include "MeshCscan.h"
#include "ModelGroupAScan.h"
#include "OpenGL.h"
#include "Techniques.h"

ModelGroupAScan::ModelGroupAScan(Techniques* pTechniques, OpenGL* pOpenGL) : Model(pTechniques, pOpenGL), m_iBuffer(0) {}

ModelGroupAScan::~ModelGroupAScan() {
    Release();
}
void ModelGroupAScan::OnLButtonDown(UINT nFlags, ::CPoint pt) {}

void ModelGroupAScan::OnLButtonUp(UINT nFlags, ::CPoint pt) {}

void ModelGroupAScan::OnLButtonDClick(UINT nFlags, ::CPoint pt) {}

void ModelGroupAScan::OnMouseMove(UINT nFlags, ::CPoint pt) {
    printf("nFlags:%x, pt:{%d, %d}\n", nFlags, pt.x, pt.y);
}

void ModelGroupAScan::OnMouseWheel(UINT nFlags, short zDelta, ::CPoint pt) {}

void ModelGroupAScan::Init() {
    Release();

    // `VIEW_ASCAN_NUM`个A扫
    for (size_t view = 0; view < HD_CHANNEL_NUM; view++) {
        if (m_pMesh.count(view) == 0) {
            m_pMesh.insert(std::pair<size_t, Mesh*>(view, new MeshAscan(m_pOpenGL)));
        }
    }
    m_bSetup = false;
}

void ModelGroupAScan::SetSize(int left, int top, int right, int bottom) {
    mSetSizeRect = {left, top, right, bottom};
    // A扫宽度
    int iAscanWidth = ((right - left) - 3) / VIEW_ASCAN_COLUMNS;
    // A扫高度
    int iAscanHeight = ((bottom - top) - 6) / (VIEW_ASCAN_NUM / VIEW_ASCAN_COLUMNS);

    RECT rc{0};
    // 计算A扫显示区域
    for (int i = 0; i < VIEW_ASCAN_NUM; i++) {
        rc.left  = (i % VIEW_ASCAN_COLUMNS) * (iAscanWidth + 1);
        rc.right = rc.left + iAscanWidth;
        rc.top   = (i / VIEW_ASCAN_COLUMNS) * (iAscanHeight + 1);
        if (i / VIEW_ASCAN_COLUMNS == (VIEW_ASCAN_NUM / VIEW_ASCAN_COLUMNS - 1)) {
            rc.bottom = bottom;
        } else {
            rc.bottom = rc.top + iAscanHeight;
        }

        for (int offset = 0; offset < 3; offset++) {
            // OpenGL视图坐标0点在左下角
            size_t iView = offset*4 + static_cast<size_t>(VIEW_TYPE::VIEW_ASCAN_0) +
                           (static_cast<size_t>((VIEW_ASCAN_NUM / VIEW_ASCAN_COLUMNS - 1)) - i / VIEW_ASCAN_COLUMNS) * VIEW_ASCAN_COLUMNS +
                           (i % VIEW_ASCAN_COLUMNS);
            if (m_pMesh.count(iView) != 0) {
                m_pMesh[iView]->SetSize(rc.left, rc.top, rc.right, rc.bottom);
            }
        }

    }
    m_bSetup = false;
}

void ModelGroupAScan::Setup() {
    //
    //SetSize(mSetSizeRect.left, mSetSizeRect.top, mSetSizeRect.right, mSetSizeRect.bottom);

    for (auto& [index, ptr] : m_pMesh) {
        if (ptr  /*&& (index >= m_pTechniques->m_GroupScanOffset &&
                    index < (static_cast<size_t>(m_pTechniques->m_GroupScanOffset) + VIEW_ASCAN_NUM))*/) {
            ptr->Setup();
        }
    }
    m_bSetup = true;
}

void ModelGroupAScan::Release() {
    for (auto& ptr : m_pMesh) {
        delete ptr.second;
        ptr.second = nullptr;
    }
    m_pMesh.clear();
}

void ModelGroupAScan::UpdateData() {
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

    for (size_t iChannel = 0; iChannel < HD_CHANNEL_NUM; iChannel++) {
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
                pAscan->UpdateData(m_pTechniques->m_pDraw[m_iBuffer].pData[iChannel].data(),
                                   m_pTechniques->m_pDraw[m_iBuffer].pData[iChannel].size());

                for (int iGate = 0; iGate < MAX_GATE_NUM; iGate++) {
                    if (iGate == 1) {
                        float fGateAamp = pChannel->m_pGateData[GATE_A].fPos / pChannel->m_fRange;
                        pAscan->UpdateGate(iGate, true, pChannel->m_pGatePos[iGate] + fGateAamp, pChannel->m_pGateWidth[iGate],
                                           pChannel->m_pGateHeight[iGate]);
                    } else {
                        pAscan->UpdateGate(iGate, true, pChannel->m_pGatePos[iGate], pChannel->m_pGateWidth[iGate],
                                           pChannel->m_pGateHeight[iGate]);
                    }
                }
                // 母线 正常不显示
                // pAscan->UpdateDACData(m_pTechniques->GetDetectionStd()->m_DAC_MiChannel].data(),
                // m_pTechniques->GetDetectionStd()->m_DAC_M[iChannel].size());

                // 判废线
                pAscan->UpdateDac(0, m_pTechniques->GetDetectionStd()->m_DAC_RL[iChannel].data(),
                                  m_pTechniques->GetDetectionStd()->m_DAC_RL[iChannel].size());

                // 评定线
                pAscan->UpdateDac(1, m_pTechniques->GetDetectionStd()->m_DAC_ED[iChannel].data(),
                                  m_pTechniques->GetDetectionStd()->m_DAC_ED[iChannel].size());
            }
        }
    }

    ResetEvent(m_pTechniques->m_pDraw[m_iBuffer].hReady);
}

void ModelGroupAScan::RenderBK() {
    for (auto& [index, ptr] : m_pMesh) {
        if (ptr && (index >= m_pTechniques->m_GroupScanOffset &&
                    index < (static_cast<size_t>(m_pTechniques->m_GroupScanOffset) + VIEW_ASCAN_NUM))) {
            ptr->RenderBK();
        }
    }
}

void ModelGroupAScan::Render() {
    UpdateData();

    for (auto& [index, ptr] : m_pMesh) {
        if (ptr && (index >= m_pTechniques->m_GroupScanOffset &&
                    index < (static_cast<size_t>(m_pTechniques->m_GroupScanOffset) + VIEW_ASCAN_NUM))) {
            ptr->Render();
        }
    }
}

void ModelGroupAScan::RenderFore() {
    for (auto& [index, ptr] : m_pMesh) {
        if (ptr && (index >= m_pTechniques->m_GroupScanOffset &&
                    index < (static_cast<size_t>(m_pTechniques->m_GroupScanOffset) + VIEW_ASCAN_NUM))) {
            ptr->RenderFore();
        }
    }
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    CString strInfo;
    for (size_t iChannel = 0; iChannel < VIEW_ASCAN_NUM; iChannel++) {
        size_t iView = static_cast<size_t>(VIEW_TYPE::VIEW_ASCAN_0) + iChannel;
        if (m_pMesh.count(iView) != 0) {
            strInfo.Format(_T("%lld"), iChannel + 1 + m_pTechniques->m_GroupScanOffset);
            float l = (iChannel + 1 + m_pTechniques->m_GroupScanOffset) > 9 ? m_pMesh[iView]->m_rcItem.left :
                                                                              m_pMesh[iView]->m_rcItem.left + 5.0f;
            float t = m_pMesh[iView]->m_rcItem.top + 3.0f;
            m_pOpenGL->m_Font.Text(l, t, strInfo, {0.2f, 0.6f, 0.9f}, 1.0f);
        }
    }
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}
