#include "pch.h"

#include "MeshGroupCScan.h"
#include "OpenGL.h"
#include "Techniques.h"

void MeshGroupCScan::DrawIndex() {}

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

void MeshGroupCScan::Setup() {}
// pData 波幅数据 pDataCH 缺陷通道数据
void MeshGroupCScan::UpdateData(UCHAR* pData, UCHAR* pDataCH, int iCircle, int iPoint) {}

void MeshGroupCScan::DrawAixs() {}

void MeshGroupCScan::DrawAixsText() {}

void MeshGroupCScan::Render() {
    glViewport(m_rcItem.vleft, m_rcItem.vtop, m_rcItem.vWidth(), m_rcItem.vHeight());
    glEnable(GL_SCISSOR_TEST);
    glClearColor(0.0f, 0.01f, 0.01f, 1.0f);
    glScissor(m_rcItem.vleft, m_rcItem.vtop, m_rcItem.vWidth(), m_rcItem.vHeight());
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    DrawIndex();
}

void MeshGroupCScan::RenderBK() {}

void MeshGroupCScan::UpdateClickPt(bool bEnable, float fClickX, float fClickY, float fWidth) {}
