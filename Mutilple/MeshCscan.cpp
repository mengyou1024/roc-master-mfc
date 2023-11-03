#include "pch.h"

#include "MeshCscan.h"
#include "OpenGL.h"
#include "Techniques.h"

MeshCscan::MeshCscan(OpenGL* pOpenGL) :
Mesh(pOpenGL),
m_iScanVAO(0),
m_iScanVBO(0),
m_iScanEBO(0),
m_iMeshVAO(0),
m_iMeshVBO(0),
m_iMeshEBO(0),
m_iScanTexture(0),
m_Limits{0},
m_fRadius(1.0f),
m_iPointSize(72),
m_iSideSize(2) {
}

MeshCscan::~MeshCscan() {
    DeleteTexture(m_iScanTexture);
    DeleteVAO(m_iScanVAO, m_iScanVBO, m_iScanEBO);
    DeleteVAO(m_iMeshVAO, m_iMeshVBO);
    DeleteVAO(m_iCirVAO, m_iCirVBO);
}
void MeshCscan::SetWheelParam(float f1, float f2, float f3, float f4) {
    m_fWheelHubInnerDiameter  = f1; // 轮毂内径 车轮内径 4
    m_fWheelHubOuterDiameter  = f2; // 轮毂外径 5
    m_fWheelRimlInnerDiameter = f3; // 轮辋内径 6
    m_fWheelRimOuterDiameter  = f4; // 轮辋外径 7
}
void MeshCscan::SetScan(float fRadius, float* pSideRadius, int iSideSize, int iPointSize) {
    m_fRadius    = fRadius;
    m_iSideSize  = iSideSize;
    m_iPointSize = iPointSize;

    m_pScanBits.resize(iSideSize * iPointSize);

    // 圆环iPointSize + 1
    m_pScanVertices.resize(iSideSize * (iPointSize + 1));
    m_pScanIndex.resize((iSideSize - 1) * iPointSize * 6);

    float fAngleStep = ANG2RAD(360.0f / iPointSize); // 弧度步进

    for (int icir = 0; icir < iSideSize; icir++) {
        for (int i = 0; i <= iPointSize; i++) {
            int iIndex                = icir * (iPointSize + 1) + i;
            m_pScanVertices[iIndex].x = (m_fWheelRimlInnerDiameter + pSideRadius[icir] + (pSideRadius[1] - pSideRadius[0]) / 2) * sin(i * fAngleStep); // 从轮辋外径开始画
            m_pScanVertices[iIndex].y = -(m_fWheelRimlInnerDiameter + pSideRadius[icir] + (pSideRadius[1] - pSideRadius[0]) / 2) * cos(i * fAngleStep);
            m_pScanVertices[iIndex].z = 0.0f;
            //   if(iIndex%4==0)
            {
                m_pScanVertices[iIndex].s = float(i + 0.5f) / float(iPointSize);
                m_pScanVertices[iIndex].t = float(icir + 0.5f) / float(iSideSize);
            }
            if (icir > 0 && i < iPointSize) {
                int iEboIndex = ((icir - 1) * iPointSize + i) * 6;
                int p0        = iIndex - (iPointSize + 1);
                int p1        = p0 + 1;
                int p2        = iIndex;
                int p3        = iIndex + 1;

                m_pScanIndex[iEboIndex]     = p0;
                m_pScanIndex[iEboIndex + 1] = p1;
                m_pScanIndex[iEboIndex + 2] = p2;
                m_pScanIndex[iEboIndex + 3] = p1;
                m_pScanIndex[iEboIndex + 4] = p2;
                m_pScanIndex[iEboIndex + 5] = p3; // 第一圈和第二圈的四个点 组成2个三角形绘图
            }
        }
    }

    int test = 0;
}

void MeshCscan::CreateBK() {
    Mesh::CreateBK();

    // 计算显示范围
    if (m_rcItem.vWidth() < m_rcItem.vHeight()) {
        float r         = m_fRadius * m_rcItem.vHeight() / m_rcItem.vWidth();
        m_Limits.left   = -m_fRadius;
        m_Limits.right  = m_fRadius;
        m_Limits.top    = -r;
        m_Limits.bottom = r;
    } else {
        float r         = m_fRadius * m_rcItem.vWidth() / m_rcItem.vHeight();
        m_Limits.left   = -r;
        m_Limits.right  = r;
        m_Limits.top    = -m_fRadius;
        m_Limits.bottom = m_fRadius;
    }
    m_pScanMin[AXIS_X] = m_Limits.left;
    m_pScanMin[AXIS_Y] = m_Limits.top;
    m_pScanMax[AXIS_X] = m_Limits.right;
    m_pScanMax[AXIS_Y] = m_Limits.bottom;

    // 中心O点分割线
    m_pMeshVertices.resize(4);
    m_pMeshVertices[0].x = 0.0f;
    m_pMeshVertices[0].y = m_Limits.top;
    m_pMeshVertices[0].a = 0.7f;
    m_pMeshVertices[0].r = 1.0f;
    m_pMeshVertices[0].g = 1.0f;
    m_pMeshVertices[0].b = 0.0f;

    m_pMeshVertices[1].x = 0.0f;
    m_pMeshVertices[1].y = m_Limits.bottom;
    m_pMeshVertices[1].a = 0.7f;
    m_pMeshVertices[1].r = 1.0f;
    m_pMeshVertices[1].g = 1.0f;
    m_pMeshVertices[1].b = 0.0f;

    m_pMeshVertices[2].x = m_Limits.left;
    m_pMeshVertices[2].y = 0.0f;
    m_pMeshVertices[2].a = 0.7f;
    m_pMeshVertices[2].r = 1.0f;
    m_pMeshVertices[2].g = 1.0f;
    m_pMeshVertices[2].b = 0.0f;

    m_pMeshVertices[3].x = m_Limits.right;
    m_pMeshVertices[3].y = 0.0F;
    m_pMeshVertices[3].a = 0.7f;
    m_pMeshVertices[3].r = 1.0f;
    m_pMeshVertices[3].g = 1.0f;
    m_pMeshVertices[3].b = 0.0f;
}

void MeshCscan::Setup() {
    Mesh::Setup();

    // C Scan
    if (m_iScanTexture == 0) {
        glGenTextures(1, &m_iScanTexture);
        glBindTexture(GL_TEXTURE_2D, m_iScanTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST /*GL_LINEAR*/);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST /*GL_LINEAR*/);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, m_iPointSize, m_iSideSize, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
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

    // 网格
    if (m_iMeshVAO == 0) {
        GenVAO(m_iMeshVAO, m_iMeshVBO);
        glBindVertexArray(m_iMeshVAO);
        // VBO
        glBindBuffer(GL_ARRAY_BUFFER, m_iMeshVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V2F_C4F) * m_pMeshVertices.size(), &m_pMeshVertices[0], GL_STATIC_DRAW);
        // Position
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)0);
        // Color
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, sizeof(PT_V2F_C4F), (GLvoid*)(2 * sizeof(GLfloat)));
        glBindVertexArray(0);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, m_iMeshVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PT_V2F_C4F) * m_pMeshVertices.size(), &m_pMeshVertices[0], GL_STATIC_DRAW);
    }
}
// pData 波幅数据 pDataCH 缺陷通道数据
void MeshCscan::UpdateData(UCHAR* pData, UCHAR* pDataCH, int iCircle, int iPoint) {
    for (int i = 0; i < m_iSideSize * m_iPointSize; i++) {
        // m_pScanBits[i] = COLOR_TAB[2][pData[i]];
        m_pScanBits[i] = COLOR_TAB_CH[pDataCH[i]];
    }
    glBindTexture(GL_TEXTURE_2D, m_iScanTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_iPointSize, m_iSideSize, GL_BGRA, GL_UNSIGNED_BYTE, m_pScanBits.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void MeshCscan::DrawAixsText() {
    glEnable(GL_SCISSOR_TEST);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    CString   strInfo;
    glm::vec4 color(1.0f, 0, 0, 1.0f);

    // 坐标
    // x
    glScissor(m_rcItem.vleft, m_rcItem.top, m_rcItem.vWidth(), m_rcItem.iAxisWidth);
    glPushMatrix();
    glTranslatef((float)m_rcItem.vleft, (float)m_rcItem.top, 0.0F);
    for (int i = 0; i < (int)m_pAxis[AXIS_X].GetCount(); ++i) {
        if (m_pAxis[AXIS_X].m_pAxisType[i] == 2) {
            if (m_pAxis[AXIS_X].m_pAxis_value[i] == 0) {
                strInfo.Format(_T("%.1fmm"), m_pAxis[AXIS_X].m_pAxis_value[i]);
                color = glm::vec4(1.0f, 0, 0, 1.0f);
            } else {
                strInfo.Format(_T("%.1f"), m_pAxis[AXIS_X].m_pAxis_value[i]);
                color = glm::vec4(1.0f, 1.0f, 0, 1.0f);
            }
            m_pOpenGL->m_Font.Text(m_pAxis[AXIS_X][i] + 1.0f, 2.0f, strInfo, color, 0.75F);
        }
    }
    glPopMatrix();

    // y
    glScissor(m_rcItem.left, m_rcItem.vtop, m_rcItem.iAxisWidth, m_rcItem.vHeight());
    glPushMatrix();
    glTranslatef((float)m_rcItem.left, (float)m_rcItem.bottom, 0.0F);
    glRotatef(-90, 0.0F, 0.0F, 1.0F);
    for (int i = 0; i < (int)m_pAxis[AXIS_Y].GetCount(); ++i) {
        if (m_pAxis[AXIS_Y].m_pAxisType[i] == 2) {
            if (m_pAxis[AXIS_Y].m_pAxis_value[i] == 0) {
                strInfo.Format(_T("%.1fmm"), m_pAxis[AXIS_Y].m_pAxis_value[i]);
                color = glm::vec4(1.0f, 0, 0, 1.0f);
            } else {
                strInfo.Format(_T("%.1f"), m_pAxis[AXIS_Y].m_pAxis_value[i]);
                color = glm::vec4(1.0f, 1.0f, 0, 1.0f);
            }
            m_pOpenGL->m_Font.Text(m_pAxis[AXIS_Y][i] + 1.0f, 2.0f, strInfo, color, 0.75F);
        }
    }
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
}

void MeshCscan::Render() {
    /**/
    glViewport(m_rcItem.vleft, m_rcItem.vtop, m_rcItem.vWidth(), m_rcItem.vHeight());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(m_Limits.left, m_Limits.right, m_Limits.bottom, m_Limits.top, -1, 1);
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

    // 中心O点分割线
    glEnable(GL_BLEND);
    glLineStipple(1, 0x0101);
    glEnable(GL_LINE_STIPPLE);
    glBindVertexArray(m_iMeshVAO);
    glDrawArrays(GL_LINES, 0, (GLsizei)m_pMeshVertices.size());
    glBindVertexArray(0);
    glDisable(GL_LINE_STIPPLE);
    glDisable(GL_BLEND);

    DrawCir(m_fWheelHubInnerDiameter);
    DrawCir(m_fWheelHubOuterDiameter);
    DrawCir(m_fWheelRimlInnerDiameter);
    DrawCir(m_fWheelRimOuterDiameter);

    if (m_ClickPt.bEnable) {
        glm::vec4 color;
        float     fLineWidth = 1.0f;
        color                = glm::vec4(1.0f, 0, 0, 1.0f);
        DrawLineX(m_ClickPt.fXPos - m_ClickPt.fWidth, m_ClickPt.fXPos + m_ClickPt.fWidth, m_ClickPt.fYPos, color, fLineWidth);
        DrawLineY(m_ClickPt.fXPos, m_ClickPt.fYPos - m_ClickPt.fWidth, m_ClickPt.fYPos + m_ClickPt.fWidth, color, fLineWidth);
    }
}
void MeshCscan::UpdateClickPt(bool bEnable, float fClickX, float fClickY, float fWidth) {
    m_ClickPt.bEnable = true;
    m_ClickPt.fXPos   = fClickX;
    m_ClickPt.fYPos   = fClickY;
    m_ClickPt.fWidth  = fWidth;
}
void MeshCscan::DrawCir(float fRadius) {
    // 测试圆的绘制
    const float center_x = 0.0f; // 半圆的中心点坐标
    const float center_y = 0.0f;
    const int   segments = 36;   // 圆上的点的数量，可以根据需要调整
    glColor3f(1.0f, 1.0f, 0.0f); // 设置圆的颜色，
    glLineStipple(1, 0x0101);    // 设置线条样式为虚线，重复因子为1，模式为0x0101
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINE_LOOP);

    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * PI * float(i) / float(segments);
        float x     = fRadius * cosf(theta);
        float y     = fRadius * sinf(theta);
        glVertex2f(x, y);
    }
    glEnd();
    glFlush();
    glDisable(GL_LINE_STIPPLE);
}
