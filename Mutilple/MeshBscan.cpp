#include "pch.h"

#include "MeshBscan.h"
#include "OpenGL.h"
#include "Techniques.h"

MeshBscan::MeshBscan(OpenGL* pOpenGL) :
Mesh(pOpenGL),
m_iScanVAO(0),
m_iScanVBO(0),
m_iScanEBO(0),
m_iScanTexture(0),
m_iPointSize(72),
m_iTreadSize(2) {
}

MeshBscan::~MeshBscan() {
    DeleteTexture(m_iScanTexture);
    DeleteVAO(m_iScanVAO, m_iScanVBO, m_iScanEBO);
}

void MeshBscan::SetScan(int iTreadSize, int iPointSize, float fMinX, float fMaxX, float fMinY, float fMaxY) {
    m_iTreadSize = iTreadSize;
    m_iPointSize = iPointSize;

    m_pScanMin[AXIS_X] = fMinX;
    m_pScanMax[AXIS_X] = fMaxX;
    m_pScanMin[AXIS_Y] = fMinY;
    m_pScanMax[AXIS_Y] = fMaxY;

    m_pScanBits.resize(iTreadSize * iPointSize);

    m_pScanVertices.resize(4);
    m_pScanIndex.resize(6);

    m_pScanVertices[0].x = fMinX;
    m_pScanVertices[0].y = fMinY;
    m_pScanVertices[0].z = 0.0f;
    m_pScanVertices[0].s = float(0.00f) / float(iPointSize);
    m_pScanVertices[0].t = float(0.00f) / float(iTreadSize);

    m_pScanVertices[1].x = fMaxX;
    m_pScanVertices[1].y = fMinY;
    m_pScanVertices[1].z = 0.0f;
    m_pScanVertices[1].s = float(iPointSize - 0.00f) / float(iPointSize);
    m_pScanVertices[1].t = float(0.00f) / float(iTreadSize);

    m_pScanVertices[2].x = fMinX;
    m_pScanVertices[2].y = fMaxY;
    m_pScanVertices[2].z = 0.0f;
    m_pScanVertices[2].s = float(0.00f) / float(iPointSize);
    m_pScanVertices[2].t = float(iTreadSize - 0.00f) / float(iTreadSize);

    m_pScanVertices[3].x = fMaxX;
    m_pScanVertices[3].y = fMaxY;
    m_pScanVertices[3].z = 0.0f;
    m_pScanVertices[3].s = float(iPointSize - 0.00f) / float(iPointSize);
    m_pScanVertices[3].t = float(iTreadSize - 0.00f) / float(iTreadSize);

    m_pScanIndex[0] = 0;
    m_pScanIndex[1] = 1;
    m_pScanIndex[2] = 2;
    m_pScanIndex[3] = 1;
    m_pScanIndex[4] = 2;
    m_pScanIndex[5] = 3;
}

void MeshBscan::CreateBK() {
    Mesh::CreateBK();
}

void MeshBscan::Setup() {
    Mesh::Setup();

    // B Scan
    if (m_iScanTexture == 0) {
        glGenTextures(1, &m_iScanTexture);
        glBindTexture(GL_TEXTURE_2D, m_iScanTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, m_iPointSize, m_iTreadSize, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (m_iScanVAO == 0) {
        GenVAO(m_iScanVAO, m_iScanVBO, m_iScanEBO);
        glBindVertexArray(m_iScanVAO);
        // VBO
        glBindBuffer(GL_ARRAY_BUFFER, m_iScanVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V3F_T2F) * m_pScanVertices.size(), m_pScanVertices.data(), GL_DYNAMIC_DRAW);
        // IBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iScanEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_pScanIndex.size(), m_pScanIndex.data(), GL_STATIC_DRAW);
        // Position
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(PT_V3F_T2F), (GLvoid*)0);
        // tex
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(PT_V3F_T2F), (GLvoid*)(3 * sizeof(GLfloat)));
        glBindVertexArray(0);
    }
}

void MeshBscan::UpdateData(UCHAR* pData, UCHAR* pDataCH, int iCircle, int iPoint) {
    for (int i = 0; i < m_iPointSize * m_iTreadSize; i++) {
        //      m_pScanBits[i] = COLOR_TAB[2][pData[i]];
        m_pScanBits[i] = COLOR_TAB_CH[pDataCH[i]];
    }
    glBindTexture(GL_TEXTURE_2D, m_iScanTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_iPointSize, m_iTreadSize, GL_BGRA, GL_UNSIGNED_BYTE, m_pScanBits.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void MeshBscan::Render() {
    glViewport(m_rcItem.vleft, m_rcItem.vtop, m_rcItem.vWidth(), m_rcItem.vHeight());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(m_pScanMin[AXIS_X], m_pScanMax[AXIS_X], m_pScanMax[AXIS_Y] * 2, m_pScanMin[AXIS_Y], -1, 1); //
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_iScanTexture);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindVertexArray(m_iScanVAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)m_pScanIndex.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    // m_ClickPt.

    //  float ptWidth = (m_pScanMax[AXIS_X]- m_pScanMin[AXIS_X] )/ 20;

    if (m_ClickPt.bEnable) {
        glm::vec4 color;
        float     fLineWidth = 1.0f;
        color                = glm::vec4(1.0f, 0, 0, 1.0f);
        DrawLineX(m_ClickPt.fXPos - m_ClickPt.fWidth, m_ClickPt.fXPos + m_ClickPt.fWidth, m_ClickPt.fYPos, color, fLineWidth);
        DrawLineY(m_ClickPt.fXPos, m_ClickPt.fYPos - m_ClickPt.fWidth, m_ClickPt.fYPos + m_ClickPt.fWidth, color, fLineWidth);
    }
}
void MeshBscan::UpdateClickPt(bool bEnable, float fClickX, float fClickY, float fWidth) {
    m_ClickPt.bEnable = true;
    m_ClickPt.fXPos   = fClickX;
    m_ClickPt.fYPos   = fClickY;
    m_ClickPt.fWidth  = fWidth;
}
