#pragma once

#include "Mesh.h"
#include "Model.h"
#include <atltypes.h>
#include <map>

class ModelGroupCScan : public Model {
private:
    enum class VIEW_TYPE : size_t {
        VIEW_CSCAN_0 = 0x00,
        VIEW_CSCAN_1,
        VIEW_CSCAN_2,
        VIEW_CSCAN_3,
        VIEW_CSCAN_MAX,

    };
    constexpr static auto VIEW_CSCAN_NUM     = static_cast<int>(VIEW_TYPE::VIEW_CSCAN_MAX);
    constexpr static auto VIEW_CSCAN_COLUMNS = 1;

    void DrawAxis(void);

public:
    ModelGroupCScan(Techniques* pTechniques, OpenGL* pOpenGL);
    virtual ~ModelGroupCScan();

    virtual void Init() override;
    virtual void SetSize(int left, int top, int right, int bottom) override;

    virtual void Setup() override;
    virtual void Release() override;

    virtual void RenderBK() override;
    virtual void Render() override;
    virtual void RenderFore() override;

    virtual void OnLButtonDown(UINT nFlags, ::CPoint pt) override;

    virtual void UpdateData();

public:
    std::map<size_t, Mesh*> m_pMesh;
    int                     m_iBuffer;
    CAxis                   m_pAxis;

    void SetAxisRange(float min, float max) {
        mAxisMin = min;
        mAxisMax = max;
    }

private:
    float                   mAxisMin         = {0.0f};
    float                   mAxisMax         = {100.f};
    RECT                    mCurrentViewPort = {};
    RECT                    mAxisViewPort   = {};
    GLuint                  m_iAxisVAO      = {};
    GLuint                  m_iAxisVBO      = {};
    std::vector<PT_V3F_T2F> m_pAxisVertices = {};
};
