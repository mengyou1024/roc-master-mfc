#pragma once

// 字体库
#include "FreeFont.h"

const int FRONT_MAX = 3;

const int COLOR_TEX_WIDTH  = 256; // 色彩纹理宽度
const int COLOR_TEX_HEIGHT = 32;  // 色彩纹理高度

// 背景图纹理位置
const int BK_TEX_DEF       = 0;  // 视图背景
const int BK_TEX_AMP       = 1;  // 波幅纹理位置
const int BK_TEX_RANGE     = 3;  // 声程纹理位置
const int BK_TEX_UT        = 5;  // UT纹理位置
const int BK_TEX_STEP      = 7;  // 步进纹理位置
const int BK_TEX_SCAN      = 9;  // 扫查纹理位置
const int BK_TEX_AXIS_BK   = 11; // 刻度背景
const int BK_TEX_AXIS      = 13; // 刻度
const int BK_TEX_AXIS_ZERO = 15; // 零刻度
const int BK_TEX_TOFD_AMP  = 17; // TOFD
const int BK_TEX_MESH      = 19; // 网格

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

    void AddScanModel(Techniques* pTechniques);
    void AddGroupAScanModel(Techniques* pTechniques);
    void AddGroupCScanModel(Techniques* pTechniques);
    void AddSetModel(Techniques* pTechniques);

    void RemoveModel();

    void  SetPos(int iWidth, int iHeight);
    float FPS();
    void  Render();
    void  Release();
    void  GetDefectPt(); // 重复覆盖时找到有缺陷的圈数
    void  GetClickPt();  // 根据圈数和点数计算坐标
    // 按键 上下左右
    void OnKeyLeft();
    void OnKeyRight();
    void OnKeyDown();
    void OnKeyUp();

    // 鼠标消息
    void OnLButtonDown(UINT nFlags, ::CPoint pt);
    void OnLButtonUp(UINT nFlags, ::CPoint pt);
    void OnLButtonDClick(UINT nFlags, ::CPoint pt);
    void OnRButtonDown(UINT nFlags, ::CPoint pt);
    void OnRButtonUp(UINT nFlags, ::CPoint pt);
    void OnRButtonDClick(UINT nFlags, ::CPoint pt);
    void OnMouseMove(UINT nFlags, ::CPoint pt);
    void OnMouseWheel(UINT nFlags, short zDelta, ::CPoint pt);
    void OnKey(TCHAR cKey);

    // 窗口过程函数
    static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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

    float m_fPt_Xscale{}; // 踏面 点击的位置在绘图x方向的比例  侧面为园的比例
    int   m_fPt_Cir{};    // 点击的位置在的圈数 （保存数据的位置）

    int m_fPtC_DrawCir{}; // 点击的位置在的圈数 （成像圈数的位置，考虑多组探头）
    int m_nProbleGrope{}; // 点击的第几组探头，踏面0,1,2 侧面 3,4
    int m_nDefectCh{};    // 缺陷通道  1-12 和绘图颜色一直
};
