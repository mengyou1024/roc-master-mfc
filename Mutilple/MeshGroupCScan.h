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

    struct CScanLine {
        float     x      = 0.0f;
        float     heifht = 50.0f;
        glm::vec4 color  = {1.0f, 0.0f, 0.0f, 1.0f};
        float     width  = 1.0f;
    };

    /**
     * @brief Ìí¼ÓÏßÌõ
     * @param height 0-100
     * @param width 
     * @param color 
    */
    void AppendLine(float height, float width, glm::vec4 color = {1.0f, 0.0f, 0.0f, 1.0f}) {
        if (m_pLineVertices.size() > 0) {
            const auto& last = *m_pLineVertices.rbegin();
            if (last.x + last.width >= m_rcItem.right)
            {
                m_pLineVertices.clear();
                CScanLine t = {0, height, color, width};
                m_pLineVertices.push_back(t);
                return;
            }
            CScanLine t = {static_cast<float>(static_cast<int>(last.x + last.width) % m_rcItem.right ), height/100.f*(m_rcItem.bottom-m_rcItem.top), color, width};
            m_pLineVertices.push_back(t);
        } else {
            CScanLine t = {0, height, color, width};
            m_pLineVertices.push_back(t);
        }


    }
    std::list<CScanLine> m_pLineVertices = {};
};
