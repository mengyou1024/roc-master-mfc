#include "pch.h"

#include "MeshGroupCScan.h"
#include "OpenGL.h"
#include "Techniques.h"

MeshGroupCScan::MeshGroupCScan(OpenGL* pOpenGL) :
Mesh(pOpenGL),
m_iMeshVAO(0),
m_iMeshVBO(0),
m_iMeshEBO(0),
m_iScanTexture(0),
m_Limits{0} {}

MeshGroupCScan::~MeshGroupCScan() {
    DeleteTexture(m_iScanTexture);
}

void MeshGroupCScan::SetSize(int left, int top, int right, int bottom) {
    m_rcItem.set(left, right, top, bottom, 0);
}

void MeshGroupCScan::CreateBK() {}

void MeshGroupCScan::Setup() {
    if (m_iQuadVAO == 0) {
        GLuint pIndex[] = {0, 1, 2, 1, 2, 3};
        GenVAO(m_iQuadVAO, m_iQuadVBO, m_iQuadEBO);
        glBindVertexArray(m_iQuadVAO);
        // VBO
        glBindBuffer(GL_ARRAY_BUFFER, m_iQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_pQuadVertices), m_pQuadVertices, GL_STREAM_DRAW);
        // IBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iQuadEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pIndex), pIndex, GL_STATIC_DRAW);
        // Position
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)0);
        // tex
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)(2 * sizeof(GLfloat)));
        glBindVertexArray(0);
    }

}
// pData 波幅数据 pDataCH 缺陷通道数据
void MeshGroupCScan::UpdateData(UCHAR* pData, UCHAR* pDataCH, int iCircle, int iPoint) {}

void MeshGroupCScan::DrawAixs() {}

void MeshGroupCScan::DrawAixsText() {}

void MeshGroupCScan::Render() {
    glEnable(GL_SCISSOR_TEST);
    glClearColor(0.0f, 0.01f, 0.01f, 1.0f);
    glScissor(m_rcItem.vleft, m_rcItem.vtop, m_rcItem.vWidth(), m_rcItem.vHeight());
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
    // 绘制线条
    for (const auto& it : m_pLineVertices) {
        DrawLineY(it.x + m_rcItem.left, m_rcItem.top, it.heifht + m_rcItem.top, it.color, it.width);
    }
}

void MeshGroupCScan::RenderBK() {}

void MeshGroupCScan::UpdateClickPt(bool bEnable, float fClickX, float fClickY, float fWidth) {}
