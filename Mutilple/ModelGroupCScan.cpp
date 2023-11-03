#include "pch.h"

#include "DetectionStd_TBT2995_200.h"
#include "MeshGroupCScan.h"
#include "ModelGroupCScan.h"
#include "OpenGL.h"
#include "Techniques.h"

void ModelGroupCScan::DrawAxis(void) {
    glViewport(mCurrentViewPort.left, mCurrentViewPort.top, std::abs(mCurrentViewPort.right - mCurrentViewPort.left),
               std::abs(mCurrentViewPort.top - mCurrentViewPort.bottom));
    glBindVertexArray(m_iAxisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_iAxisVBO);
    m_pAxisVertices.resize(2);
    for (int i = 0; i < (int)m_pAxis.GetCount(); i++) {
        float x      = (float)(mAxisViewPort.left + 1 + m_pAxis[i]);
        float y      = (float)mAxisViewPort.top + 1;
        float fCoord = (m_pAxis.m_pAxis_value[i] == 0) ? ((15 + 0.5f) / 35) : ((13 + 0.5f) / 35);

        m_pAxisVertices[0].x = x;
        m_pAxisVertices[0].y = y;
        m_pAxisVertices[0].z = 0;
        m_pAxisVertices[0].s = 0.5f;
        m_pAxisVertices[0].t = fCoord;
        if (m_pAxis.m_pAxisType[i] == 2) {
            y += 15;
        } else if (m_pAxis.m_pAxisType[i] == 1) {
            y += 10;
        } else {
            // y += 5;
        }
        m_pAxisVertices[1].x = x;
        m_pAxisVertices[1].y = y;
        m_pAxisVertices[1].z = 0;
        m_pAxisVertices[1].s = 0.5f;
        m_pAxisVertices[1].t = fCoord;
        glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V3F_T2F) * 2, m_pAxisVertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_LINES, 0, 2);
    }

    m_pAxisVertices[0].x = (float)mAxisViewPort.left;
    m_pAxisVertices[0].y = (float)mAxisViewPort.top;
    m_pAxisVertices[0].z = 0;
    m_pAxisVertices[0].s = 0.5F;
    m_pAxisVertices[0].t = ((13 + 0.5f) / 35);
    m_pAxisVertices[1].x = (float)mAxisViewPort.right;
    m_pAxisVertices[1].y = (float)mAxisViewPort.top;
    m_pAxisVertices[1].z = 0;
    m_pAxisVertices[1].s = 0.5F;
    m_pAxisVertices[1].t = ((13 + 0.5f) / 35);

    glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V3F_T2F) * 2, m_pAxisVertices.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINES, 0, 2);

    glBindVertexArray(0);

    // ��������ֵ
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    CString   strInfo;
    glm::vec4 color(1.0f, 0, 0, 1.0f);
    // ����
    glScissor(mAxisViewPort.left, mAxisViewPort.top, std::abs(mAxisViewPort.right - mAxisViewPort.left),
              std::abs(mAxisViewPort.top - mAxisViewPort.bottom));
    glPushMatrix();
    glTranslatef((float)mAxisViewPort.left + 3, (float)mAxisViewPort.top + 5, 0.0F);
    for (int i = 0; i < (int)m_pAxis.GetCount(); ++i) {
        if (m_pAxis.m_pAxisType[i] == 2) {
            // �̶�����
            if (m_pAxis.m_pAxis_value[i] == 0) {
                strInfo.Format(_T("%.1fmm"), m_pAxis.m_pAxis_value[i]);
                color = glm::vec4(1.0f, 0, 0, 1.0f);
            } else {
                strInfo.Format(_T("%.1f"), m_pAxis.m_pAxis_value[i]);
                color = glm::vec4(1.0f, 1.0f, 0, 1.0f);
            }
            m_pOpenGL->m_Font.Text(m_pAxis[i] + 1.0f, 2.0f, strInfo, color, 0.75F);
        }
    }
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
}

ModelGroupCScan::ModelGroupCScan(Techniques *pTechniques, OpenGL *pOpenGL) : Model(pTechniques, pOpenGL), m_iBuffer(0) {}

ModelGroupCScan::~ModelGroupCScan() {
    Release();
}

void ModelGroupCScan::Init() {
    Release();

    // `VIEW_CSCAN_NUM`��Aɨ
    for (size_t view = 0; view < (size_t)(VIEW_CSCAN_NUM); view++) {
        if (m_pMesh.count(view) == 0) {
            m_pMesh.insert(std::pair<size_t, Mesh *>(view, new MeshGroupCScan(m_pOpenGL)));
        }
    }
    m_bSetup = false;
}

void ModelGroupCScan::SetSize(int left, int top, int right, int bottom) {
    // ��ǰ�ӿ�
    mCurrentViewPort.left   = left;
    mCurrentViewPort.right  = right;
    mCurrentViewPort.bottom = bottom;
    mCurrentViewPort.top    = top;
    // ƫ��һ��������ĸ߶�
    bottom -= 26;
    // Cɨ���
    int iViewWidth = ((right - left) - 3) / VIEW_CSCAN_COLUMNS;
    // Cɨ�߶�
    int iViewHeight = ((bottom - top) - 6) / (VIEW_CSCAN_NUM / VIEW_CSCAN_COLUMNS);

    mAxisViewPort.left   = left - 1;
    mAxisViewPort.right  = right - 4;
    mAxisViewPort.bottom = bottom - 25;
    mAxisViewPort.top    = bottom + 1;

    RECT rc{0};
    // ����Aɨ��ʾ����
    for (int i = 0; i < VIEW_CSCAN_NUM; i++) {
        rc.left  = (i % VIEW_CSCAN_COLUMNS) * (iViewWidth + 1);
        rc.right = rc.left + iViewWidth;
        rc.top   = (i / VIEW_CSCAN_COLUMNS) * (iViewHeight + 1) + top;
        if (i / VIEW_CSCAN_COLUMNS == (VIEW_CSCAN_NUM / VIEW_CSCAN_COLUMNS - 1)) {
            rc.bottom = bottom;
        } else {
            rc.bottom = rc.top + iViewHeight;
        }

        // OpenGL��ͼ����0�������½�
        size_t iView = static_cast<size_t>(VIEW_TYPE::VIEW_CSCAN_0) +
                       (static_cast<size_t>((VIEW_CSCAN_NUM / VIEW_CSCAN_COLUMNS - 1)) - i / VIEW_CSCAN_COLUMNS) * VIEW_CSCAN_COLUMNS +
                       (i % VIEW_CSCAN_COLUMNS);
        if (m_pMesh.count(iView) != 0) {
            m_pMesh[iView]->SetSize(rc.left, rc.top, rc.right, rc.bottom);
        }
    }
    m_bSetup = false;
}

void ModelGroupCScan::Setup() {
    for (auto it = m_pMesh.begin(); it != m_pMesh.end(); ++it) {
        if (it->second) {
            it->second->Setup();
        }
    }
    if (m_iAxisVAO == 0) {
        GenVAO(m_iAxisVAO, m_iAxisVBO);
        glBindVertexArray(m_iAxisVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_iAxisVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V3F_T2F), NULL, GL_DYNAMIC_DRAW);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(PT_V3F_T2F), nullptr);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(PT_V3F_T2F), (GLvoid *)(3 * sizeof(GLfloat)));
        glBindVertexArray(0);
    }

    m_bSetup = true;
}

void ModelGroupCScan::Release() {
    for (auto &ptr : m_pMesh) {
        delete ptr.second;
        ptr.second = nullptr;
    }
    m_pMesh.clear();
}

void ModelGroupCScan::UpdateData() {}

void ModelGroupCScan::RenderBK() {
    for (auto &ptr : m_pMesh) {
        if (ptr.second) {
            ptr.second->RenderBK();
        }
    }
}

void ModelGroupCScan::Render() {
    for (auto &ptr : m_pMesh) {
        if (ptr.second) {
            ptr.second->Render();
        }
    }
}

void ModelGroupCScan::RenderFore() {
    for (auto &ptr : m_pMesh) {
        if (ptr.second) {
            ptr.second->RenderFore();
        }
    }
    glViewport(mAxisViewPort.left, mAxisViewPort.top, std::abs(mAxisViewPort.right - mAxisViewPort.left),
               std::abs(mAxisViewPort.top - mAxisViewPort.bottom));
    glEnable(GL_SCISSOR_TEST);
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glScissor(mAxisViewPort.left, mAxisViewPort.top, std::abs(mAxisViewPort.right - mAxisViewPort.left),
              std::abs(mAxisViewPort.top - mAxisViewPort.bottom));
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    m_pAxis.Set(std::abs(mAxisViewPort.right - mAxisViewPort.left - 1), 0.f, 100.f);
    DrawAxis();

    // ��������ֵ
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    glm::vec4 color(0.2f, 0.6f, 0.9f, 1.0f);
    // ��������
    glScissor(mCurrentViewPort.left, mCurrentViewPort.top, std::abs(mCurrentViewPort.right - mCurrentViewPort.left),
              std::abs(mCurrentViewPort.top - mCurrentViewPort.bottom));
    glPushMatrix();
    for (auto &ptr : m_pMesh) {
        glPushMatrix();
        if (ptr.second) {
            glTranslatef((float)ptr.second->m_rcItem.left + 3, (float)ptr.second->m_rcItem.top + 10, 0.0F);
            CString strInfo;
            strInfo.Format(_T("%lld"), ptr.first + 1 + m_pTechniques->m_GroupScanOffset);
            m_pOpenGL->m_Font.RightText(static_cast<float>(mCurrentViewPort.right - 15),
                                        static_cast<float>(ptr.second->m_rcItem.Height() - 50), strInfo, color, 2.F);
        }
        glPopMatrix();
    }
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
}

void ModelGroupCScan::OnLButtonDown(UINT nFlags, ::CPoint pt) {}
