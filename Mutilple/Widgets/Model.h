#pragma once

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

public:
    bool        m_bSetup;
    OpenGL*     m_pOpenGL;

    bool     m_bLButtonDown;
    ::CPoint m_ptLButtonDown;
    RECT     m_rcItem;
};
