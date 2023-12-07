#pragma once

#include <Mesh.h>
class OpenGL;
class Techniques;

class Model {
public:
    Model(OpenGL* pOpenGL);
    virtual ~Model();

    virtual void Init()                                            = 0;
    virtual void SetSize(int left, int top, int right, int bottom) = 0;

    virtual void Setup()   = 0;
    virtual void Release() = 0;

    virtual void RenderBK()   = 0;
    virtual void Render()     = 0;
    virtual void RenderFore() = 0;

    virtual void OnLButtonDown(UINT nFlags, ::CPoint pt);
    virtual void OnLButtonUp(UINT nFlags, ::CPoint pt);
    virtual void OnLButtonDClick(UINT nFlags, ::CPoint pt);
    virtual void OnMouseMove(UINT nFlags, ::CPoint pt);
    virtual void OnMouseWheel(UINT nFlags, short zDelta, ::CPoint pt);

    template <class T>
    T getMesh(size_t index) {
        static_assert(std::is_pointer_v<T>, "类型必须是指针");
        static_assert(std::is_base_of_v<Mesh, std::remove_pointer_t<T>>, "类型必须是Mesh或其子类");
        return dynamic_cast<T>(m_pMesh[index]);
    }

    template <>
    Mesh* getMesh(size_t index) {
        return getMesh<Mesh*>(index);
    }

public:
    std::map<size_t, Mesh*> m_pMesh;
    bool                    m_bSetup;
    OpenGL*                 m_pOpenGL;

    bool     m_bLButtonDown;
    ::CPoint m_ptLButtonDown;
    RECT     m_rcItem;
};
