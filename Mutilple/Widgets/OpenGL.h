#pragma once

// �����
#include "FreeFont.h"
#include <type_traits>
#include <assert.h>

const int FRONT_MAX = 3;

const int COLOR_TEX_WIDTH  = 256; // ɫ��������
const int COLOR_TEX_HEIGHT = 32;  // ɫ������߶�

// ����ͼ����λ��
const int BK_TEX_DEF       = 0;  // ��ͼ����
const int BK_TEX_AMP       = 1;  // ��������λ��
const int BK_TEX_RANGE     = 3;  // ��������λ��
const int BK_TEX_UT        = 5;  // UT����λ��
const int BK_TEX_STEP      = 7;  // ��������λ��
const int BK_TEX_SCAN      = 9;  // ɨ������λ��
const int BK_TEX_AXIS_BK   = 11; // �̶ȱ���
const int BK_TEX_AXIS      = 13; // �̶�
const int BK_TEX_AXIS_ZERO = 15; // ��̶�
const int BK_TEX_TOFD_AMP  = 17; // TOFD
const int BK_TEX_MESH      = 19; // ����

class Model;
class Techniques;
class OpenGL {
public:
    OpenGL(void);
    ~OpenGL(void);
    bool         Create(HWND hParent);
    virtual bool SupportOpenGL();

    virtual bool Attach(CWindowUI* pWindow);

    void Init();
    void InitColorTable();
    void InitViewport();

    void AddGroupAScanModel();
    void AddGroupCScanModel();

    template <class T>
    void AddModel() {
        static_assert(std::is_base_of_v<Model, T>, "type must be Model");
        RemoveModel();

        Model* pModel = new T(this);
        pModel->Init();
        pModel->SetSize(1, 1, m_iWidth - 1, m_iHeight - 1);

        m_pModel.push_back(pModel);
    }

    void RemoveModel();

    void  SetPos(int iWidth, int iHeight);
    float FPS();
    void  Render();
    void  Release();
    // ���� ��������
    void OnKeyLeft();
    void OnKeyRight();
    void OnKeyDown();
    void OnKeyUp();

    // �����Ϣ
    void OnLButtonDown(UINT nFlags, ::CPoint pt);
    void OnLButtonUp(UINT nFlags, ::CPoint pt);
    void OnLButtonDClick(UINT nFlags, ::CPoint pt);
    void OnRButtonDown(UINT nFlags, ::CPoint pt);
    void OnRButtonUp(UINT nFlags, ::CPoint pt);
    void OnRButtonDClick(UINT nFlags, ::CPoint pt);
    void OnMouseMove(UINT nFlags, ::CPoint pt);
    void OnMouseWheel(UINT nFlags, short zDelta, ::CPoint pt);
    void OnKey(TCHAR cKey);

    // ���ڹ��̺���
    static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    template <class T>
    T getModel() {
        static_assert(std::is_pointer_v<T>, "type must be a pointer");
        static_assert(std::is_base_of_v<Model, std::remove_pointer_t<T>>, "type must be Model");
        return dynamic_cast<T>(m_pModel.at(0));
    }

    Model* getModel() {
        return m_pModel[0];
    }


public:
    HWND  m_hWnd{};
    HDC   m_hDC{};
    HGLRC m_hRC{};
    int   m_iWidth{};
    int   m_iHeight{};

public:
    std::vector<Model*> m_pModel{};
    CFreeFont           m_Font{};

    GLuint m_iColorTexture{};
    DWORD* m_pColor{};

    float m_fPt_Xscale{}; // ̤�� �����λ���ڻ�ͼx����ı���  ����Ϊ԰�ı���
    int   m_fPt_Cir{};    // �����λ���ڵ�Ȧ�� ���������ݵ�λ�ã�

    int m_fPtC_DrawCir{}; // �����λ���ڵ�Ȧ�� ������Ȧ����λ�ã����Ƕ���̽ͷ��
    int m_nProbleGrope{}; // ����ĵڼ���̽ͷ��̤��0,1,2 ���� 3,4
    int m_nDefectCh{};    // ȱ��ͨ��  1-12 �ͻ�ͼ��ɫһֱ
};
