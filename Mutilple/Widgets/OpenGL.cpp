#include "pch.h"

#include "DMessageBox.h"
#include "ModelGroupAScan.h"
#include "ModelGroupCScan.h"
#include "Mutilple.h"
#include "OpenGL.h"

// TODO: ��ʱ���ξ���
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

    // ��������
    m_Font.Create(IDR_MYFONT1, 20);

    SetTimer(m_hWnd, TIMER_OPENGL_RENDER, 15, NULL); // 15FPSˢ���ٶȡ�

    // ��ʼ��ɫ������
    InitColorTable();
}

void OpenGL::InitColorTable() {
    for (int y = 0; y < COLOR_TEX_HEIGHT; y++) {
        int iIndex = y * COLOR_TEX_WIDTH;

        switch (y) {
                // ����
            case BK_TEX_AMP:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = COLOR_TAB[1][x];
                break;
                // Aɨ
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
                // �̶ȱ���
            case BK_TEX_AXIS_BK:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFF000000;
                break;
                // �̶�
            case BK_TEX_AXIS:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFFFFFFFF;
                break;
                // ��̶�
            case BK_TEX_AXIS_ZERO:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFFFF0000;
                break;
                // TOFD
            case BK_TEX_TOFD_AMP:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0xFF << 24 | x << 16 | x << 8 | x;
                break;
                // ����
            case BK_TEX_MESH:
                for (int x = 0; x < COLOR_TEX_WIDTH; x++)
                    m_pColor[iIndex + x] = 0x7FFFFF00;
                break;
                // Ĭ��ɫ
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


void OpenGL::AddGroupAScanModel() {
    RemoveModel();

    Model* pModel = new ModelGroupAScan(this);
    pModel->Init();
    pModel->SetSize(1, 1, m_iWidth - 1, m_iHeight - 1);

    m_pModel.push_back(pModel);
}

void OpenGL::AddGroupCScanModel() {
    RemoveModel();

    Model* pModel = new ModelGroupCScan(this);
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
    static float fps         = 0;    // ������Ҫ�����FPSֵ
    static int   frameCount  = 0;    // ֡��
    static float currentTime = 0.0f; // ��ǰʱ��
    static float lastTime    = 0.0f; // ����ʱ��

    frameCount++; // ÿ����һ��Get_FPS()������֡������1
    currentTime =
        GetTickCount64() * 0.001f; // ��ȡϵͳʱ�䣬����timeGetTime�������ص����Ժ���Ϊ��λ��ϵͳʱ�䣬������Ҫ����0.001���õ���λΪ���ʱ��

    // �����ǰʱ���ȥ����ʱ�������1���ӣ��ͽ���һ��FPS�ļ���ͳ���ʱ��ĸ��£�����֡��ֵ����
    if (currentTime - lastTime > 1.0f) // ��ʱ�������1����
    {
        fps        = (float)frameCount / (currentTime - lastTime); // ������1���ӵ�FPSֵ
        lastTime   = currentTime; // ����ǰʱ��currentTime��������ʱ��lastTime����Ϊ��һ��Ļ�׼ʱ��
        frameCount = 0;           // ������֡��frameCountֵ����
    }

    return fps;
}

void OpenGL::Render() {
    // �趨OpenGL��ǰ�̵߳���Ⱦ����
    wglMakeCurrent(m_hDC, m_hRC);

    size_t iSize = m_pModel.size();
    for (size_t i = 0; i < iSize; i++) {
        if (!m_pModel[i]->m_bSetup) {
            m_pModel[i]->Setup();
        }
    }

    // �����ӿ�
    InitViewport();

    // ��ǰ���������
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
        m_pModel[i]->RenderFore(); // ����
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

// ���� ��������
void OpenGL::OnKeyUp() { // ��һȦ

}
void OpenGL::OnKeyDown() { // ��һȦ

}

void OpenGL::OnKeyLeft() { // ǰһ����

}
void OpenGL::OnKeyRight() { // ��һ����

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
