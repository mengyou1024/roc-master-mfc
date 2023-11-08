#pragma once

#include "Mesh.h"

class Channel;
class MeshCscan : public Mesh {
public:
    MeshCscan(OpenGL* pOpenGL);
    virtual ~MeshCscan();

    virtual void SetScan(float fRadius, float* pSideRadius, int iSideSize, int iPointSize);

    virtual void CreateBK();
    virtual void Setup();

    virtual void UpdateData(UCHAR* pData, UCHAR* pDataCH, int iCircle, int iPoint);

    virtual void DrawAixsText();

    virtual void Render();

    void DrawCir(float fRadius); // f1 轮辋外径 ，f2 轮辋内径 f3 轮毂外径 f4 轮毂内径（车轮内径）
    void SetWheelParam(float f1, float f2, float f3, float f4);

    void UpdateClickPt(bool bEnable, float fClickX, float fClickY, float fWidth);

public:
    float m_fWheelHubInnerDiameter;  // 轮毂内径 车轮内径 4
    float m_fWheelHubOuterDiameter;  // 轮毂外径 5
    float m_fWheelRimlInnerDiameter; // 轮辋内径 6
    float m_fWheelRimOuterDiameter;  // 轮辋外径 7
    struct _ClickPt_info {
        bool  bEnable;
        float fXPos;
        float fYPos;
        float fWidth;
    } m_ClickPt;

    // 工件半径
    float m_fRadius;
    // 每圈检测点数
    int m_iPointSize;
    // 检测次数
    int m_iSideSize;

    RECTF m_Limits;

public:
    GLuint             m_iScanTexture;
    std::vector<DWORD> m_pScanBits;

    GLuint                  m_iScanVAO, m_iScanVBO, m_iScanEBO;
    std::vector<PT_V3F_T2F> m_pScanVertices;
    std::vector<GLuint>     m_pScanIndex;

    GLuint                  m_iMeshVAO, m_iMeshVBO, m_iMeshEBO;
    std::vector<PT_V2F_C4F> m_pMeshVertices;

    GLuint                  m_iCirVAO, m_iCirVBO; // 画圈
    std::vector<PT_V2F_C4F> m_pCirVertices;
};
