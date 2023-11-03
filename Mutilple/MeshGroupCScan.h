#pragma once

#include "Mesh.h"
#include <Define.h>
#include <Windows.h>

#include <gl/glew.h>

#include <vector>

using std::vector;

class Channel;
class MeshGroupCScan : public Mesh {
private:
    void DrawIndex();

public:
    int mCurrentIndex = 0;
    MeshGroupCScan(OpenGL* pOpenGL);
    virtual ~MeshGroupCScan();

    virtual void SetSize(int left, int top, int right, int bottom) override;

    virtual void CreateBK() override;
    virtual void Setup() override;

    virtual void UpdateData(UCHAR* pData, UCHAR* pDataCH, int iCircle, int iPoint);
    virtual void DrawAixs() override;
    virtual void DrawAixsText() override;
    virtual void Render() override;
    virtual void RenderBK() override;
    void         UpdateClickPt(bool bEnable, float fClickX, float fClickY, float fWidth);

public:
    struct _ClickPt_info {
        bool  bEnable;
        float fXPos;
        float fYPos;
        float fWidth;
    } m_ClickPt;

    RECTF m_Limits;

public:
    GLuint             m_iScanTexture;
    std::vector<DWORD> m_pScanBits;

    GLuint                  m_iMeshVAO, m_iMeshVBO, m_iMeshEBO;
    std::vector<PT_V2F_C4F> m_pMeshVertices;
};
