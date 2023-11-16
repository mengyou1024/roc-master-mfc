#pragma once

#include "Mesh.h"
#include <memory>
#include <thread>

const int MAX_DAC_LINES_NUM = 2; //      瑞铁 使用2根      更加调整RL SL EL

class Channel;
class MeshAscan : public Mesh {
public:
    MeshAscan(OpenGL* pOpenGL);
    virtual ~MeshAscan();

    virtual void SetLimits(float fMin, float fMax) override;

    virtual void CreateBK() override;
    virtual void Setup() override;

    virtual void RenderBK();
    virtual void RenderFore() override;
    virtual void Render();

    virtual int   GetPixel(int iAxis, float fValue);
    virtual float GetValue(int iAxis, int iPixel);

    void UpdateGate(int iGate, bool bEnable, float fPos, float fWidth, float fHeight);
    void UpdateAScanData();

    void hookAScanData(const std::shared_ptr<std::vector<uint8_t>> data);

private:
    void    DrawGate();
    void    DrawAixsText();
    FLOAT   m_fScanMin;
    FLOAT   m_fScanMax;
    GLsizei m_iAScanSize;
    GLsizei m_iDACSize;                        // 母线DAC
    GLsizei m_iDACLineSize[MAX_DAC_LINES_NUM]; // RL SL
    struct _gate_info {
        bool  bEnable;
        float fPos;
        float fWidth;
        float fHeight;
    } m_Gate[MAX_GATE_NUM];

    GLuint                  m_iAscanVAO, m_iAscanVBO;
    std::vector<PT_V2F_C4F> m_pAscanVertices;
    GLuint                  m_iMeshVAO, m_iMeshVBO, m_iMeshEBO;
    std::vector<PT_V2F_C4F> m_pMeshVertices;
    GLuint                  m_iDACVAO, m_iDACVBO;
    std::vector<PT_V2F_C4F> m_pDACVertices;
    GLuint                  m_iDACLineVAO[MAX_DAC_LINES_NUM], m_iDACLineVBO[MAX_DAC_LINES_NUM];
    std::vector<PT_V2F_C4F> m_pDACLineVertices[MAX_DAC_LINES_NUM];

    std::mutex                            mGMutex       = {};
    std::shared_ptr<std::vector<uint8_t>> mRawAScanData = nullptr;
};
