#pragma once

#include "Mesh.h"

class Channel;
class MeshBscan : public Mesh
{
public:
    MeshBscan(OpenGL* pOpenGL);
    virtual ~MeshBscan();

    virtual void SetScan(int iTreadSize, int iPointSize, float fMinX, float fMaxX, float fMinY, float fMaxY);

    virtual void CreateBK();
    virtual void Setup();

    virtual void UpdateData(UCHAR* pData, UCHAR* pDataCH,  int iTread, int iPoint);

    virtual void Render();
    void  UpdateClickPt(bool bEnable, float fClickX, float fClickY, float fWidth);
public:
    //每圈检测点数
    int   m_iPointSize;
    //检测次数
    int   m_iTreadSize;

    struct _ClickPt_info
    {
        bool bEnable;
        float fXPos;
        float fYPos;
        float fWidth;
    }m_ClickPt;
public:
    GLuint m_iScanTexture;
    std::vector<DWORD> m_pScanBits;

    GLuint m_iScanVAO, m_iScanVBO, m_iScanEBO;
    std::vector<PT_V3F_T2F> m_pScanVertices;
    std::vector<GLuint> m_pScanIndex;
};


