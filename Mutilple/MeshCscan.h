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

    void DrawCir(float fRadius); // f1 �����⾶ ��f2 �����ھ� f3 ����⾶ f4 ����ھ��������ھ���
    void SetWheelParam(float f1, float f2, float f3, float f4);

    void UpdateClickPt(bool bEnable, float fClickX, float fClickY, float fWidth);

public:
    float m_fWheelHubInnerDiameter;  // ����ھ� �����ھ� 4
    float m_fWheelHubOuterDiameter;  // ����⾶ 5
    float m_fWheelRimlInnerDiameter; // �����ھ� 6
    float m_fWheelRimOuterDiameter;  // �����⾶ 7
    struct _ClickPt_info {
        bool  bEnable;
        float fXPos;
        float fYPos;
        float fWidth;
    } m_ClickPt;

    // �����뾶
    float m_fRadius;
    // ÿȦ������
    int m_iPointSize;
    // ������
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

    GLuint                  m_iCirVAO, m_iCirVBO; // ��Ȧ
    std::vector<PT_V2F_C4F> m_pCirVertices;
};
