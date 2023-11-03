#pragma once

#include "Model.h"
#include "MeshAscan.h"

class ModelSet : public Model
{
public:
    ModelSet(Techniques* pTechniques, OpenGL* pOpenGL);
    virtual ~ModelSet();

    virtual void Init();
    virtual void SetSize(int left, int top, int right, int bottom);

    virtual void Setup();
    virtual void Release();

    virtual void UpdateData();

    virtual void RenderBK();
    virtual void Render();
    virtual void RenderFore();

public:
    MeshAscan* m_pMesh;
    int m_iBuffer;
};

