#pragma once

#include "Mesh.h"
#include "Model.h"
#include <atltypes.h>
#include <map>

class ModelGroupAScan : public Model {
private:
    RECT mSetSizeRect = {};

    enum class VIEW_TYPE : size_t {
        VIEW_ASCAN_0 = 0x00,
        VIEW_ASCAN_1,
        VIEW_ASCAN_2,
        VIEW_ASCAN_3,
        VIEW_ASCAN_MAX,

    };
    constexpr static auto VIEW_ASCAN_NUM     = static_cast<int>(VIEW_TYPE::VIEW_ASCAN_MAX);
    constexpr static auto VIEW_ASCAN_COLUMNS = 2;

public:
    ModelGroupAScan(Techniques* pTechniques, OpenGL* pOpenGL);
    virtual ~ModelGroupAScan();

    virtual void Init() override;
    virtual void SetSize(int left, int top, int right, int bottom) override;

    virtual void Setup() override;
    virtual void Release() override;

    virtual void RenderBK() override;
    virtual void Render() override;
    virtual void RenderFore() override;

    virtual void OnLButtonDown(UINT nFlags, ::CPoint pt) override;
    virtual void OnLButtonUp(UINT nFlags, ::CPoint pt) override;
    virtual void OnLButtonDClick(UINT nFlags, ::CPoint pt) override;
    virtual void OnMouseMove(UINT nFlags, ::CPoint pt) override;
    virtual void OnMouseWheel(UINT nFlags, short zDelta, ::CPoint pt) override;

    virtual void UpdateData();



public:
    std::map<size_t, Mesh*> m_pMesh;
    int                     m_iBuffer;
    
};
