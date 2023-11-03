#pragma once

#include "Mesh.h"
#include "Model.h"

constexpr auto VIEW_ASCAN_NUM     = 12;
constexpr auto VIEW_ASCAN_COLUMNS = 2;

enum class VIEW_TYPE : size_t {
    VIEW_ASCAN_0 = 0x00,
    VIEW_ASCAN_1,
    VIEW_ASCAN_2,
    VIEW_ASCAN_3,
    VIEW_ASCAN_4,
    VIEW_ASCAN_5,
    VIEW_ASCAN_6,
    VIEW_ASCAN_7,
    VIEW_ASCAN_8,
    VIEW_ASCAN_9,
    VIEW_ASCAN_10,
    VIEW_ASCAN_11,
    VIEW_BSCAN = 0x100,
    VIEW_CSCAN,
};

class ModelScan : public Model {
public:
    ModelScan(Techniques* pTechniques, OpenGL* pOpenGL);
    virtual ~ModelScan();

    virtual void Init();
    virtual void SetSize(int left, int top, int right, int bottom);

    virtual void Setup();
    virtual void Release();

    virtual void UpdateData();

    virtual void RenderBK();
    virtual void Render();
    virtual void RenderFore();
    virtual void OnLButtonDown(UINT nFlags, ::CPoint pt);

public:
    std::map<size_t, Mesh*> m_pMesh;
    int                     m_iBuffer;
};
