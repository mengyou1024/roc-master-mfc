#include "pch.h"

#include "DMessageBox.h"
#include "MainFrame.h"
#include "MeshBscan.h"
#include "MeshCscan.h"
#include "ModelScan.h"
#include "ModelGroupAScan.h"
#include "ModelGroupCScan.h"
#include "ModelSet.h"
#include "Mutilple.h"
#include "OpenGL.h"

// TODO: 暂时屏蔽警告
#pragma warning(disable :4267 4244 4552 4305 4101)

constexpr auto TIMER_OPENGL_RENDER = 0x00;
#define OpenGL_PTR          _T("OpenGL")

OpenGL::OpenGL(void) : m_hWnd(nullptr), m_hDC(nullptr), m_hRC(nullptr), m_iWidth(1028), m_iHeight(502), m_iColorTexture(0) {
    m_pColor = new DWORD[COLOR_TEX_WIDTH * COLOR_TEX_HEIGHT];
}

OpenGL::~OpenGL(void) {
    Release();

    delete[] m_pColor;
    KillTimer(m_hWnd, TIMER_OPENGL_RENDER);
}

bool OpenGL::Create(HWND hParent) {
    CString strWindow;
    strWindow.Format(_T("OpenglWindow_%lld"), GetTickCount64());
    WNDCLASSEX wc = {sizeof(WNDCLASSEX),
                     CS_CLASSDC | CS_DBLCLKS,
                     __WndProc,
                     0L,
                     0L,
                     NULL,
                     NULL,
                     LoadCursor(NULL, IDC_ARROW),
                     NULL,
                     NULL,
                     strWindow,
                     NULL};
    RegisterClassEx(&wc);

    long lStyle = WS_VISIBLE | WS_CHILD | WS_DISABLED;
    m_hWnd      = ::CreateWindow(strWindow, strWindow, lStyle, 0, 0, m_iWidth, m_iHeight, hParent, NULL, wc.hInstance, NULL);
    if (m_hWnd == nullptr)
        return false;
    m_hDC = GetDC(m_hWnd);
    if (m_hDC == nullptr)
        return false;

    ::SetProp(m_hWnd, OpenGL_PTR, (HANDLE)this);

    static PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), // size of this pfd
        1,                             // version number
        PFD_DRAW_TO_WINDOW |           // support window
            PFD_SUPPORT_OPENGL |       // support OpenGL
            PFD_DOUBLEBUFFER,          // double buffered
        PFD_TYPE_RGBA,                 // RGBA type
        24,                            // 24-bit color depth
        0,
        0,
        0,
        0,
        0,
        0, // color bits ignored
        0, // no alpha buffer
        0, // shift bit ignored
        0, // no accumulation buffer
        0,
        0,
        0,
        0,              // accum bits ignored
        16,             // 16-bit z-buffer
        0,              // no stencil buffer
        0,              // no auxiliary buffer
        PFD_MAIN_PLANE, // main layer
        0,              // reserved
        0,
        0,
        0 // layer masks ignored
    };

    int iPixelFormat = ::ChoosePixelFormat(m_hDC, &pfd);
    if (::SetPixelFormat(m_hDC, iPixelFormat, &pfd) == FALSE)
        return false;

    m_hRC = ::wglCreateContext(m_hDC);
    if (m_hRC == nullptr)
        return false;

    SupportOpenGL();

    Init();

    return true;
}

bool OpenGL::SupportOpenGL() {
    wglMakeCurrent(m_hDC, m_hRC);
    const GLubyte* OpenGLVersion = glGetString(GL_VERSION);

    return true;
}

bool OpenGL::Attach(CWindowUI* pWindow) {
    if (!m_hWnd)
        return false;
    return pWindow->Attach(m_hWnd);
}

void OpenGL::Init() {
    wglMakeCurrent(m_hDC, m_hRC);

    // Init GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    // 加载字体
    m_Font.Create(IDR_MYFONT1, 20);

    SetTimer(m_hWnd, TIMER_OPENGL_RENDER, 15, NULL); // 15FPS刷新速度。

    // 初始化色彩纹理
    InitColorTable();
}

void OpenGL::InitColorTable() {
    for (int y = 0; y < COLOR_TEX_HEIGHT; y++) {
        int iIndex = y * COLOR_TEX_WIDTH;

        switch (y) {
                // 波幅
            case BK_TEX_AMP:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = COLOR_TAB[1][x];
                break;
                // A扫
            case BK_TEX_RANGE:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFF306090;
                break;
                // UT
            case BK_TEX_UT:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFFBA55D3;
                break;
                // Step
            case BK_TEX_STEP:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFF6E6B3D;
                break;
                // Scan
            case BK_TEX_SCAN:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFF218868;
                break;
                // 刻度背景
            case BK_TEX_AXIS_BK:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFF000000;
                break;
                // 刻度
            case BK_TEX_AXIS:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFFFFFFFF;
                break;
                // 零刻度
            case BK_TEX_AXIS_ZERO:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFFFF0000;
                break;
                // TOFD
            case BK_TEX_TOFD_AMP:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFF << 24 | x << 16 | x << 8 | x;
                break;
                // 网格
            case BK_TEX_MESH:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0x7FFFFF00;
                break;
                // 默认色
            default:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0x00000000;
                break;
        }
    }

    glGenTextures(1, &m_iColorTexture);
    glBindTexture(GL_TEXTURE_2D, m_iColorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, COLOR_TEX_WIDTH, COLOR_TEX_HEIGHT, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_pColor);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGL::InitViewport() {
    glViewport(0, 0, m_iWidth, m_iHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, m_iWidth, 0, m_iHeight, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void OpenGL::AddScanModel(Techniques* pTechniques) {
    RemoveModel();

    Model* pModel = new ModelScan(pTechniques, this);
    pModel->Init();
    pModel->SetSize(1, 1, m_iWidth - 1, m_iHeight - 1);

    m_pModel.push_back(pModel);
}

void OpenGL::AddGroupAScanModel(Techniques* pTechniques) {
    RemoveModel();

    Model* pModel = new ModelGroupAScan(pTechniques, this);
    pModel->Init();
    pModel->SetSize(1, 1, m_iWidth - 1, m_iHeight - 1);

    m_pModel.push_back(pModel);
}

void OpenGL::AddGroupCScanModel(Techniques* pTechniques) {
    RemoveModel();

    Model* pModel = new ModelGroupCScan(pTechniques, this);
    pModel->Init();
    pModel->SetSize(1, 1, m_iWidth - 1, m_iHeight - 1);

    m_pModel.push_back(pModel);
}

void OpenGL::AddSetModel(Techniques* pTechniques) {
    RemoveModel();

    Model* pModel = new ModelSet(pTechniques, this);
    pModel->Init();
    pModel->SetSize(1, 1, m_iWidth - 1, m_iHeight - 1);

    m_pModel.push_back(pModel);
}

void OpenGL::RemoveModel() {
    for (auto i = 0; i < m_pModel.size(); i++) {
        delete m_pModel[i];
        m_pModel[i] = nullptr;
    }
    m_pModel.clear();
}

void OpenGL::SetPos(int iWidth, int iHeight) {
    m_iWidth  = iWidth;
    m_iHeight = iHeight;

    for (auto i = 0; i < m_pModel.size(); i++) {
        m_pModel[i]->SetSize(1, 1, m_iWidth - 1, m_iHeight - 1);
    }
}

float OpenGL::FPS() {
    static float fps         = 0;    // 我们需要计算的FPS值
    static int   frameCount  = 0;    // 帧数
    static float currentTime = 0.0f; // 当前时间
    static float lastTime    = 0.0f; // 持续时间

    frameCount++; // 每调用一次Get_FPS()函数，帧数自增1
    currentTime =
        GetTickCount64() * 0.001f; // 获取系统时间，其中timeGetTime函数返回的是以毫秒为单位的系统时间，所以需要乘以0.001，得到单位为秒的时间

    // 如果当前时间减去持续时间大于了1秒钟，就进行一次FPS的计算和持续时间的更新，并将帧数值清零
    if (currentTime - lastTime > 1.0f) // 将时间控制在1秒钟
    {
        fps        = (float)frameCount / (currentTime - lastTime); // 计算这1秒钟的FPS值
        lastTime   = currentTime; // 将当前时间currentTime赋给持续时间lastTime，作为下一秒的基准时间
        frameCount = 0;           // 将本次帧数frameCount值清零
    }

    return fps;
}

void OpenGL::Render() {
    // 设定OpenGL当前线程的渲染环境
    wglMakeCurrent(m_hDC, m_hRC);

    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        if (!m_pModel[i]->m_bSetup) {
            m_pModel[i]->Setup();
        }
    }

    // 重置视口
    InitViewport();

    // 当前缓冲区清除
    glClearColor(0.95F, 0.95F, 0.95F, 0.95F);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_iColorTexture);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (size_t i = 0; i < iSize; i++) {
        m_pModel[i]->RenderBK();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    for (size_t i = 0; i < iSize; i++) {
        m_pModel[i]->Render();
    }

    InitViewport();

    for (size_t i = 0; i < iSize; i++) {
        m_pModel[i]->RenderFore(); // 文字
    }

#ifdef _DEBUG
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    CString strInfo;
    if (iSize != 0) {
        // strInfo.Format(_T("FPS:%.1f"), g_MainProcess.m_HDBridge.m_fFPS);
        strInfo.Format(_T("FPS:%.0f "), FPS());
        m_Font.RightText((float)m_iWidth, (float)m_iHeight - 24, strInfo, {0.15f, 0.35f, 0.55f}, 0.75F);
    } else {
        const char* version = (const char*)glGetString(GL_VERSION);
        USES_CONVERSION;
        strInfo.Format(_T("OpenGL: %s"), A2W(version));
        m_Font.RightText((float)m_iWidth, (float)m_iHeight - 24, strInfo, {0.15f, 0.35f, 0.55f}, 1.0F);
    }
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
#endif

    // Swap the screen buffers
    ::SwapBuffers(m_hDC);
}

void OpenGL::Release() {
    RemoveModel();

    DeleteTexture(m_iColorTexture);
}

// 按键 上下左右
void OpenGL::OnKeyUp() { // 上一圈
    if (m_fPtC_DrawCir > 0 && m_fPtC_DrawCir < g_MainProcess.m_Techniques.m_Scan.m_iDrawTreadSize) {
        m_fPtC_DrawCir--;
    }
    GetDefectPt();
}
void OpenGL::OnKeyDown() { // 下一圈
    if (m_fPtC_DrawCir >= 0 && m_fPtC_DrawCir < g_MainProcess.m_Techniques.m_Scan.m_iDrawTreadSize - 1) {
        m_fPtC_DrawCir++;
    }
    GetDefectPt();
}

void OpenGL::OnKeyLeft() { // 前一个点
    // if (m_fPt_Xscale > 0 && m_fPt_Xscale < 1) {
    m_fPt_Xscale = (m_fPt_Xscale * g_MainProcess.m_Techniques.m_iSidePoints - 0.5) / g_MainProcess.m_Techniques.m_iSidePoints;
    if (m_fPt_Xscale < 0)
        m_fPt_Xscale = 1;
    //}
    GetDefectPt();
}
void OpenGL::OnKeyRight() { // 后一个点
    // if (m_fPt_Xscale > 0 && m_fPt_Xscale < 1) {
    m_fPt_Xscale = (m_fPt_Xscale * g_MainProcess.m_Techniques.m_iSidePoints + 0.5) / g_MainProcess.m_Techniques.m_iSidePoints;
    if (m_fPt_Xscale > 1)
        m_fPt_Xscale = 0;
    // }
    GetDefectPt();
}

// 点击缺陷列表
void OpenGL::GetClickPt() {
    size_t iSize     = m_pModel.size();
    int    nDefectCh = 0;
    for (size_t i = 0; i < iSize; i++) {
        MeshBscan* pBscan = static_cast<MeshBscan*>(((ModelScan*)m_pModel[i])->m_pMesh[static_cast<size_t>(VIEW_TYPE::VIEW_BSCAN)]); // 踏面
        MeshCscan* pCscan = static_cast<MeshCscan*>(((ModelScan*)m_pModel[i])->m_pMesh[static_cast<size_t>(VIEW_TYPE::VIEW_CSCAN)]); // 侧面

        if (pBscan != NULL) {
            float fwidth  = pBscan->m_pScanMax[0]; // 实际坐标
            float fHeigth = pBscan->m_pScanMax[1];
            // 每圈检测点数
            int iPointSize = pBscan->m_iPointSize;
            // 检测圈数数
            int iTreadSize = pBscan->m_iTreadSize;
            pBscan->UpdateClickPt(true, m_fPt_Xscale * fwidth,
                                  (m_fPtC_DrawCir + 0.5) * fHeigth / g_MainProcess.m_Techniques.m_Scan.m_iDrawTreadSize, fwidth / 20);
        }

        if (pCscan != NULL) {
            // 每圈检测点数
            int iPointSize = pCscan->m_iPointSize;
            // 检测圈数数
            int   iTreadSize = pCscan->m_iSideSize;
            float fWstep     = (pCscan->m_fWheelRimOuterDiameter - pCscan->m_fWheelRimlInnerDiameter) / iTreadSize; // 轮辋内径 6

            float x = (pCscan->m_fWheelRimlInnerDiameter + (m_fPtC_DrawCir + 0.5) * fWstep) *
                      sin(m_fPt_Xscale * pCscan->m_iPointSize * PI / 180); // 从轮辋外径开始画 弧度=角度*Pi/180;
            float y = -(pCscan->m_fWheelRimlInnerDiameter + (m_fPtC_DrawCir + 0.5) * fWstep) *
                      cos(m_fPt_Xscale * pCscan->m_iPointSize * PI / 180);
            /*
            m_fPt_Xscale = 1.0*ptWangle / 360;
            m_fPt_Cir = (ptWRadio - pCscan->m_fWheelRimlInnerDiameter) / fWstep; //计算点击第几圈
            */
            pCscan->UpdateClickPt(true, x, y, (pCscan->m_fWheelRimOuterDiameter - pCscan->m_fWheelRimlInnerDiameter));
        }
    }
}
void OpenGL::GetDefectPt() {
    size_t iSize     = m_pModel.size();
    int    nDefectCh = 0;
    for (size_t i = 0; i < iSize; i++) {
        MeshBscan* pBscan = static_cast<MeshBscan*>(((ModelScan*)m_pModel[i])->m_pMesh[static_cast<size_t>(VIEW_TYPE::VIEW_BSCAN)]); // 踏面
        MeshCscan* pCscan = static_cast<MeshCscan*>(((ModelScan*)m_pModel[i])->m_pMesh[static_cast<size_t>(VIEW_TYPE::VIEW_CSCAN)]); // 侧面

        if (pBscan != NULL) {
            // 每圈检测点数
            int iPointSize = pBscan->m_iPointSize;
            // 检测圈数数
            int iTreadSize = pBscan->m_iTreadSize;

            if (m_fPtC_DrawCir >= g_MainProcess.m_Techniques.m_Scan.m_iSideSize) {                             // 超出旋转圈数
                m_fPt_Cir = m_fPtC_DrawCir - g_MainProcess.m_Techniques.m_Scan.m_iDrawProbleTreadSize;         // 减一组探头间距
                if (m_fPt_Cir >= g_MainProcess.m_Techniques.m_Scan.m_iSideSize) {                              // 还是超出旋转圈数
                    m_fPt_Cir = m_fPtC_DrawCir - g_MainProcess.m_Techniques.m_Scan.m_iDrawProbleTreadSize * 2; // 再减一组探头间距
                }
            } else {
                m_fPt_Cir = m_fPtC_DrawCir;
            }

            if ((m_fPtC_DrawCir + m_fPt_Xscale) * iPointSize <
                pBscan->m_pScanBits
                    .size()) { // 由于探头区分组成像圈数与旋转圈数不一致，有重复覆盖的情况 存在一个点有两圈的情况 有限显示有缺陷的圈数
                DWORD res = pBscan->m_pScanBits[(m_fPtC_DrawCir + m_fPt_Xscale) * iPointSize]; // 读取点击区域的成像颜色 判断缺陷的通道数
                int ch = 0;
                for (int i = 0; i < 12; i++) {
                    if (res == COLOR_TAB_CH[i]) {
                        nDefectCh   = i;
                        m_nDefectCh = i;
                        ch          = i;
                    }
                }
                if (ch <= 2) { // 第一组探头  成像圈数和 扫差圈数一致

                } else if (ch <= 4) //
                {
                    m_fPt_Cir = m_fPtC_DrawCir - g_MainProcess.m_Techniques.m_Scan.m_iDrawProbleTreadSize; // 减去和第一组探头之间的圈数
                } else if (ch <= 6) {
                    m_fPt_Cir = m_fPtC_DrawCir - g_MainProcess.m_Techniques.m_Scan.m_iDrawProbleTreadSize * 2; // 减去和第一组探头之间的圈数
                } else {                                                                                       // 无缺陷点击

                    if (m_fPt_Cir >= g_MainProcess.m_Techniques.m_Scan.m_iDrawTreadSize) {                     // 超出旋转圈数
                        m_fPt_Cir = m_fPtC_DrawCir - g_MainProcess.m_Techniques.m_Scan.m_iDrawProbleTreadSize; // 减一组探头间距
                        if (m_fPt_Cir >= g_MainProcess.m_Techniques.m_Scan.m_iDrawTreadSize) {                 // 还是超出旋转圈数
                            m_fPt_Cir = m_fPtC_DrawCir - g_MainProcess.m_Techniques.m_Scan.m_iDrawProbleTreadSize * 2; // 再减一组探头间距
                        }
                    }
                }
            }
        }
        if (nDefectCh == 0 || nDefectCh == 11) { // 踏面没有缺陷在 根据侧面是否有缺陷找位置
            if (pCscan != NULL) {
                // 每圈检测点数
                int iPointSize = pCscan->m_iPointSize;
                // 检测圈数数
                int iTreadSize = pCscan->m_iSideSize;
                if (m_fPtC_DrawCir >=
                    g_MainProcess.m_Techniques.m_Scan.m_iSideSize + g_MainProcess.m_Techniques.m_Scan.m_iDrawProbleSideSize) {
                    continue;
                }
                if (m_fPtC_DrawCir >= g_MainProcess.m_Techniques.m_Scan.m_iSideSize) {                    // 还是超出旋转圈数
                    m_fPt_Cir = m_fPtC_DrawCir - g_MainProcess.m_Techniques.m_Scan.m_iDrawProbleSideSize; // 再减一组探头间距
                } else {
                    m_fPt_Cir = m_fPtC_DrawCir;
                }

                if (m_fPtC_DrawCir + m_fPt_Xscale > 0 &&
                    (m_fPtC_DrawCir + m_fPt_Xscale) * iPointSize <
                        pCscan->m_pScanBits.size()) { // 由于探头区分组成像圈数与旋转圈数不一致，有重复覆盖的情况
                                                      // 存在一个点有两圈的情况 有限显示有缺陷的圈数
                    DWORD res =
                        pCscan->m_pScanBits[(m_fPtC_DrawCir + m_fPt_Xscale) * iPointSize]; // 读取点击区域的成像颜色 判断缺陷的通道数
                    int ch = 0;                                                            //
                    for (int i = 0; i < 12; i++) {
                        if (res == COLOR_TAB_CH[i]) {
                            m_nDefectCh = i;
                            ch          = i;
                        }
                    }
                    if (ch == 7 || ch == 9) { // 第一组探头  成像圈数和 扫差圈数一致

                    } else if (ch == 8 || ch == 10) //
                    {
                        m_fPt_Cir = m_fPtC_DrawCir - g_MainProcess.m_Techniques.m_Scan.m_iDrawProbleSideSize; // 减去和第一组探头之间的圈数
                    } else {                                                                                  // 无缺陷点击
                        if (m_fPt_Cir >= g_MainProcess.m_Techniques.m_Scan.m_iSideSize) {                     // 超出旋转圈数
                            m_fPt_Cir = m_fPtC_DrawCir - g_MainProcess.m_Techniques.m_Scan.m_iDrawProbleSideSize; // 减一组探头间距
                        }
                    }
                }
            }
        }
    }

    GetClickPt();
}
void OpenGL::OnLButtonDown(UINT nFlags, ::CPoint pt) {
    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        m_pModel[i]->OnLButtonDown(nFlags, pt);
    }
}

void OpenGL::OnLButtonUp(UINT nFlags, ::CPoint pt) {
    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        m_pModel[i]->OnLButtonUp(nFlags, pt);
    }
}
void OpenGL::OnRButtonDown(UINT nFlags, ::CPoint pt) {
    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        //  m_pModel[i]->OnRButtonDown(nFlags, pt);
    }
}

void OpenGL::OnRButtonUp(UINT nFlags, ::CPoint pt) {
    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        // m_pModel[i]->OnRButtonUp(nFlags, pt);
    }
}
void OpenGL::OnLButtonDClick(UINT nFlags, ::CPoint pt) {
    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        m_pModel[i]->OnLButtonDClick(nFlags, pt);
    }
}
void OpenGL::OnRButtonDClick(UINT nFlags, ::CPoint pt) {
    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        // m_pModel[i]->OnRButtonDClick(nFlags, pt);
    }
}

void OpenGL::OnMouseMove(UINT nFlags, ::CPoint pt) {
    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        m_pModel[i]->OnMouseMove(nFlags, pt);
    }
}

void OpenGL::OnMouseWheel(UINT nFlags, short zDelta, ::CPoint pt) {
    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        m_pModel[i]->OnMouseWheel(nFlags, zDelta, pt);
    }
}

void OpenGL::OnKey(TCHAR cKey) {}

LRESULT CALLBACK OpenGL::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    OpenGL* pthis = reinterpret_cast<OpenGL*>(::GetProp(hWnd, OpenGL_PTR));
    if (uMsg >= WM_LBUTTONDOWN && uMsg <= WM_MBUTTONDBLCLK) {
        TRACE("%d-%d", WM_RBUTTONDOWN, uMsg);
    }
    switch (uMsg) {
        case WM_SIZE: {
            if (pthis)
                pthis->SetPos(LOWORD(lParam), HIWORD(lParam));
        } break;
        case WM_TIMER: {
            switch (wParam) {
                case TIMER_OPENGL_RENDER: pthis->Render(); break;
            }
        } break;
        case WM_LBUTTONDOWN: {
            pthis->OnLButtonDown(LOWORD(wParam), (::CPoint)lParam);
        } break;
        case WM_LBUTTONUP: {
            pthis->OnLButtonUp(LOWORD(wParam), (::CPoint)lParam);
        } break;
        case WM_RBUTTONDOWN: {
            pthis->OnRButtonDown(LOWORD(wParam), (::CPoint)lParam);
        } break;
        case WM_RBUTTONUP: {
            pthis->OnRButtonUp(LOWORD(wParam), (::CPoint)lParam);
        } break;
        case WM_LBUTTONDBLCLK: {
            pthis->OnLButtonDClick(LOWORD(wParam), (::CPoint)lParam);
        } break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
